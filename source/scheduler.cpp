#include "scheduler.h"
#include "util.h"
#include <iostream>
#include <fstream>
#include <string>
#include <mutex>
#include <queue> // used for ready queue
#include <thread>
#include <chrono>
#include <atomic>

using namespace std;

extern mutex inputMutex;

void createProcesses();
void printProcessList();
void startcores();              // cpu workers, not yet properly defined, for testing only ^^ 
                                // it gets called before it exists so I put the prototype
void stopcores();


// ============================================================
// SCHEDULER: PROCESSES AND READY QUEUE
// ============================================================
const int NUM_PROCESSES = 10;                   
const int INSTRUCTIONS_PER_PROCESS = 100;       

struct Process {
    string name;                // name of the process (eg: process01, process02, etc.)
    int id;                     // process number, 1 to 10
    int totalInstructions;      // total number of print commands to run
    int executedInstructions;   // number of print commands already run
    string state;               // "Waiting", "Running" or "Finished"
    int coreId;                 // core running the process, -1 if it is not on a core
    string startTime;           // time when the process was created
};

Process allProcesses[NUM_PROCESSES];    // all the processes, in the order they were created
queue<Process*> readyQueue;             // processes waiting for a core, FCFS!

// 2 mutex below are used by different threads, so lock them before reading or changing the queue / the process info
mutex readyQueueMutex;
mutex processMutex;

// makes process01 to process10 and puts them in the ready queue in order
void createProcesses() {
    for (int i = 0; i < NUM_PROCESSES; i++) {   // repeat for each of the 10 processes
        string name = "process";
        if (i + 1 < 10) name += "0";           
        name += to_string(i + 1);

        allProcesses[i].name = name;
        allProcesses[i].id = i + 1;
        allProcesses[i].totalInstructions = INSTRUCTIONS_PER_PROCESS;
        allProcesses[i].executedInstructions = 0;         // no print commands run yet
        allProcesses[i].state = "Waiting";                  // not running yet
        allProcesses[i].coreId = -1;                      // not on a core yet
        allProcesses[i].startTime = getTimestamp();         // save the time process is created

        lock_guard<mutex> lock(readyQueueMutex);            // lock the queue for thread safety
        readyQueue.push(&allProcesses[i]);                  // add to the back of the queue, so the order stays first come first serve
    }
}

// screen -ls, shows the running and finished processes
void printProcessList() {
    lock_guard<mutex> consoleLock(inputMutex);  // lock the console so it doesn't mix with the marquee
    lock_guard<mutex> dataLock(processMutex);  // lock the process info so it doesn't change while printing

    cout << "-----------------------------------------" << endl;
    cout << "Running processes:" << endl;
    for (int i = 0; i < NUM_PROCESSES; i++) {
        Process& p = allProcesses[i];

        if (p.state == "Running") {                 // only show the processes that are running
            cout << p.name << "   (" << p.startTime << ")   Core: " << p.coreId
                 << "   " << p.executedInstructions << " / " << p.totalInstructions << endl;
        }
    }

    cout << endl << "Finished processes:" << endl;
    for (int i = 0; i < NUM_PROCESSES; i++) {
        Process& p = allProcesses[i];
        if (p.state == "Finished") {                // only show the processes that are finished
            cout << p.name << "   (" << p.startTime << ")   Finished   "
                 << p.executedInstructions << " / " << p.totalInstructions << endl;
        }
    }
    cout << "--------------------------------------------" << endl << endl;
}

// ============================================================
// SCHEDULER THREAD AND CORE TRACKING
// ============================================================

atomic<bool> runScheduler(false);           // flag that tells the scheduler (and the cores) to keep going or stop
thread schedulerThread;                     // the one thread that hands out processes to the cores

Process* coreProcess[NUM_CORES];            // process on each core, nullptr means the core is free
                                            // (protected by processMutex)

// Lock order: processMutex first, then readyQueueMutex
// (if two threads lock them in different orders they can get stuck waiting on each other forever)

// the scheduler thread: takes processes from the ready queue in FCFS order and gives them to free cores
void runscheduler() {
    while (runScheduler) {                      // keep looping until stopscheduler() turns the flag off
        {
            lock_guard<mutex> lock(processMutex);       // lock the process info
            lock_guard<mutex> q(readyQueueMutex);       // lock the queue

            for (int core = 0; core < NUM_CORES; core++) {      // go through each core, core 0 to 3
                if (coreProcess[core] == nullptr && !readyQueue.empty()) {  // core is free and someone is waiting
                    Process* p = readyQueue.front();    // front of the queue = first come, first served
                    readyQueue.pop();                   // take it out of the queue

                    p->state = "Running";               // mark as running (screen -ls reads this)
                    p->coreId = core;                   // remember which core has it
                    coreProcess[core] = p;              // the core is now busy
                }
            }
        }                                       // locks release here

        this_thread::sleep_for(chrono::milliseconds(1));    // rest a bit so we don't hog the CPU, then check again
    }
}

// called by a core worker when its process is done, frees the core
void releaseCore(int coreId) {
    lock_guard<mutex> lock(processMutex);       // lock the process info before changing it
    Process* p = coreProcess[coreId];           // the process that just finished on this core
    p->state = "Finished";                      // screen -ls will now list it under finished
    p->coreId = -1;                             // not on a core anymore
    coreProcess[coreId] = nullptr;              // the core is free again, the scheduler will notice on its next loop
}

void startscheduler() {
    if (runScheduler) return;                   // already running, do nothing
    runScheduler = true;                        // turn the flag on before starting the threads
    schedulerThread = thread(runscheduler);     // start the scheduler in its own thread
    startcores();                               // start the 4 core threads
}

void stopscheduler() {
    if (!runScheduler) return;                  // not running, nothing to stop
    runScheduler = false;                       // tell every thread to stop
    if (schedulerThread.joinable()) schedulerThread.join();     // wait for the scheduler thread to finish
    stopcores();                                // then wait for the core threads
}

// ============================================================
// TEMP: CORE THREADS (for testing, will be replaced/completed)
// ============================================================

thread coreThreads[NUM_CORES];                  // one thread per core

// helper to write timestamp and core ID to the process's individual text file
void executePrintToFiles(Process* p, int coreId) {
    if (p == nullptr) return;

    // make the print command write to each process's own text file (10 files total)
    string filename = p->name + ".txt";
    ofstream outFile(filename, ios::app);

    if (outFile.is_open()) {
        // output format:
        // (MM/DD/YYYY HH:MM:SSAM/PM)
        // Core:X
        outFile << getTimestamp() << endl;
        outFile << "Core:" << coreId << endl;
        outFile.close();
    }
}

void coreworker(int coreId) {
    while (runScheduler) {                      // a core keeps working until the program stops
        Process* p;                             // the process this core is going to run
        {
            lock_guard<mutex> lock(processMutex);   // lock before reading what the scheduler gave us
            p = coreProcess[coreId];                // nullptr if nothing was assigned yet
        }

        if (p == nullptr) {                     // nothing to run on this core right now
            this_thread::sleep_for(chrono::milliseconds(1));    // rest a bit, then check again
            continue;
        }

        while (runScheduler && p->executedInstructions < p->totalInstructions) {
            executePrintToFiles(p, coreId); // calls to write the files

            {
                lock_guard<mutex> lock(processMutex);
                p->executedInstructions++;
            }

            this_thread::sleep_for(chrono::milliseconds(10)); // short execution delay
        }

        if (runScheduler) releaseCore(coreId);  // done, free the core (skip if we were told to stop midway)
    }
}

void startcores() {
    for (int i = 0; i < NUM_CORES; i++) coreThreads[i] = thread(coreworker, i);     // start one worker per core
}

void stopcores() {
    for (int i = 0; i < NUM_CORES; i++) {
        if (coreThreads[i].joinable()) coreThreads[i].join();   // wait for each core thread to finish
    }
}

