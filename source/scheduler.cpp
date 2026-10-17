#include "scheduler.h"
#include "util.h"
#include <iostream>
#include <string>
#include <mutex>
#include <queue> // used for ready queue

using namespace std;

extern mutex inputMutex;

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

void createProcesses();
void printProcessList();

// ============================================================
// SCHEDULER: PROCESSES AND READY QUEUE
// ============================================================


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

