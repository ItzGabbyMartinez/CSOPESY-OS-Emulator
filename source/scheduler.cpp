#include "scheduler.h"
#include "util.h"
#include "process.h"
#include "config.h"
#include <iostream>
#include <fstream>
#include <string>
#include <mutex>
#include <queue> // used for ready queue
#include <vector>
#include <thread>
#include <chrono>
#include <atomic>
#include <iomanip>
#include <sstream>

using namespace std;

extern mutex inputMutex;

void createProcesses();
void printProcessList();
void startcores();              // cpu workers, not yet properly defined, for testing only ^^ 
                                // it gets called before it exists so I put the prototype
void stopcores();


// ============================================================
// SCHEDULER: READY QUEUE & CORE TRACKING
// ============================================================
         
queue<Process*> readyQueue;             // processes waiting for a core, FCFS!

// 2 mutex below are used by different threads, so lock them before reading or changing the queue / the process info
mutex readyQueueMutex;
mutex processMutex;

atomic<bool> runScheduler(false);           // flag that tells the scheduler (and the cores) to keep going or stop
thread schedulerThread;                     // the one thread that hands out processes to the cores

// Core tracking vectors declared here so functions below can access them
vector<Process*> coreProcess;               // dynamic array based on systemConfig.numCpu
vector<int> coreQuantumUsed;                // tracks RR quantum ticks per core
vector<thread> coreThreads;                 // dynamic worker threads

// helper for screen -s <name> (and scheduler-start) to push a new process into readyQueue
void enqueueProcess(Process* p) {
    if (p == nullptr) return;
    {
        lock_guard<mutex> lock(processMutex);
        p->state = "Waiting";
        p->coreId = -1;
    }
    lock_guard<mutex> qLock(readyQueueMutex);
    readyQueue.push(p);
}

// ============================================================
// BATCH GENERATOR (scheduler-start / scheduler-stop) & REPORTING
// ============================================================
atomic<bool> runBatchGen(false);
thread batchThread;
static atomic<int> processCounter(1);

static string generateNextProcessName() {
    ostringstream ss;
    ss << "p" << setfill('0') << setw(2) << processCounter++;
    return ss.str();
}

void batchworker() {
    while (runBatchGen) {
        {
            string name = generateNextProcessName();
            Process* p = createProcess(name);
            if (p != nullptr) {
                generateRandomInstructions(*p);
                enqueueProcess(p);
            }
        }
        int delayMs = systemConfig.batchProcessFreq * 10;
        this_thread::sleep_for(chrono::milliseconds(max(1, delayMs)));
    }
}

void startbatchgenerator() {
    if (runBatchGen) return;
    runBatchGen = true;
    batchThread = thread(batchworker);
}

void stopbatchgenerator() {
    if (!runBatchGen) return;
    runBatchGen = false;
    if (batchThread.joinable()) batchThread.join();
}


 // ============================================================
 // CPU REPORTING
 // ============================================================

 // Builds one consistent report for screen -ls and report-util
 static string buildCPUReport() {
     ostringstream report;

     // Protect process and core information while reading
     scoped_lock lock(processMutex, processListMutex);

     int totalCores = systemConfig.numCpu;
     int usedCores = 0;

     // Count cores that currently have assigned processes
     for (const Process* p : coreProcess) {
         if (p != nullptr) {
             usedCores++;
         }
     }

     int availableCores = totalCores - usedCores;

     int utilization = (totalCores > 0)
         ? (usedCores * 100) / totalCores
         : 0;

     report << "CPU utilization: " << utilization << "%\n";
     report << "Cores used: " << usedCores << "\n";
     report << "Cores available: " << availableCores << "\n";
     report << "-----------------------------------------\n";

     report << "Active processes (Running / Waiting / Sleeping):\n";

     for (const Process& p : allProcesses) {
         if (p.state == "Running" ||
             p.state == "Waiting" ||
             p.state == "Sleeping") {

             string coreStr = (p.coreId >= 0)
                 ? to_string(p.coreId)
                 : "N/A";

             report << left << setw(12) << p.name
                 << " (" << p.startTime << ")"
                 << "   State: " << setw(10) << p.state
                 << "   Core: " << setw(5) << coreStr
                 << "   " << p.pc
                 << " / " << p.instructions.size()
                 << "\n";
        }
    }

     report << "\nFinished processes:\n";

     for (const Process& p : allProcesses) {
         if (p.state == "Finished") {
             report << left << setw(12) << p.name
                    << " (" << p.startTime << ")"
                    << "   Finished   "
                    << p.pc
                    << " / " << p.instructions.size()
                    << "\n";
         }
     }

     return report.str();
 }

 // Display the CPU report in the console
 void printProcessList() {
     cout << buildCPUReport() << endl;
 }

 // Save the same CPU report to a text file
 void generateReportUtil(const string& path) {
     string report = buildCPUReport();

     ofstream out(path);

     if (!out.is_open()) {
         cout << "Error: Could not open "
              << path << " for writing." << endl << endl;
         return;
     }

     out << report;

     if (!out.good()) {
         cout << "Error: Could not write report to "
              << path << "." << endl << endl;
         return;
     }

     out.close();

     if (out.fail()) {
         cout << "Error: Could not finish writing report to "
              << path << "." << endl << endl;
         return;
     }

     cout << "Report generated at "
          << path << "!" << endl << endl;
 }


// ============================================================
// SCHEDULER THREAD
// ============================================================

// the scheduler thread: takes processes from the ready queue in FCFS order and gives them to free cores
void runscheduler() {
    while (runScheduler) {                      // keep looping until stopscheduler() turns the flag off
        {
            lock_guard<mutex> lock(processMutex);       // lock the process info
            
            for (int core = 0; core < systemConfig.numCpu; core++) {      // go through each core
                if (coreProcess[core] == nullptr) {
                    lock_guard<mutex> q(readyQueueMutex);       // lock ready queue independently
                    if (!readyQueue.empty()) {
                        Process* p = readyQueue.front();    // front of the queue
                        readyQueue.pop();                   // take it out of the queue

                        p->state = "Running";               // mark as running
                        p->coreId = core;                   // remember which core has it
                        coreProcess[core] = p;              // the core is now busy
                        coreQuantumUsed[core] = 0;          // reset quantum counter
                    }
                }
            }
        }                                       // locks release here

        this_thread::sleep_for(chrono::milliseconds(1));    // rest a bit so we don't hog the CPU
    }
}

// called by a core worker when its process is done, frees the core
void releaseCore(int coreId) {
    // Note: Caller must already hold processMutex, or we lock it here safely
    Process* p = coreProcess[coreId];           // the process that just finished on this core
    if (p != nullptr) {
        p->state = "Finished";                  // screen -ls will now list it under finished
        p->coreId = -1;                         // not on a core anymore
    }
    coreProcess[coreId] = nullptr;              // the core is free again
    coreQuantumUsed[coreId] = 0;
}

void startscheduler() {
    if (runScheduler) return;                   // already running, do nothing
    
    // Allocate dynamic core structures based on config.txt
    coreProcess.assign(systemConfig.numCpu, nullptr);
    coreQuantumUsed.assign(systemConfig.numCpu, 0);

    runScheduler = true;                        // turn the flag on before starting the threads
    schedulerThread = thread(runscheduler);     // start the scheduler in its own thread
    startcores();                               // start core threads
}

void stopscheduler() {
    if (!runScheduler) return;                  // not running, nothing to stop
    stopbatchgenerator();
    runScheduler = false;                       // tell every thread to stop
    if (schedulerThread.joinable()) schedulerThread.join();     // wait for the scheduler thread to finish
    stopcores();                                // then wait for the core threads
}

// ============================================================
// CORE THREADS
// ============================================================

// helper to write timestamp and core ID to the process's individual text file
void executePrintToFiles(Process* p, int coreId) {
    if (p == nullptr) return;

    string filename = p->name + ".txt";
    ofstream outFile(filename, ios::app);

    if (outFile.is_open()) {
        if (p->pc == 0) { 
            outFile << "Process name: " << p->name << endl;
            outFile << "Logs:" << endl << endl;
        }

        outFile << "(" << getTimestamp() << ") Core:" << coreId 
                << " \"Hello world from " << p->name << "!\"" << endl;
        outFile.close();
    }
}

void coreworker(int coreId) {
    while (runScheduler) {
        Process* p = nullptr;
        {
            lock_guard<mutex> lock(processMutex);
            p = coreProcess[coreId];
        }

        if (p == nullptr) {
            this_thread::sleep_for(chrono::milliseconds(5));
            continue;
        }

        // Execute step if instructions remain
        bool isDone = false;
        if (p->pc < (int)p->instructions.size()) {
            if (p->instructions[p->pc].type == PRINT) {
                executePrintToFiles(p, coreId); 
            }

            {
                lock_guard<mutex> lock(processMutex);
                isDone = executeNext(*p);
                coreQuantumUsed[coreId]++;
            }

            // Apply delay setting from config.txt
            if (systemConfig.delaysPerExec > 0) {
                this_thread::sleep_for(chrono::milliseconds(systemConfig.delaysPerExec * 5));
            } else {
                this_thread::sleep_for(chrono::milliseconds(1));
            }

            // Post-execution state checks
            lock_guard<mutex> lock(processMutex);
            if (isDone) {
                releaseCore(coreId);
            } 
            else if (systemConfig.scheduler == "rr" && 
                     coreQuantumUsed[coreId] >= (int)systemConfig.quantumCycles) {
                
                // Preempt process
                p->state = "Waiting";
                p->coreId = -1;
                coreProcess[coreId] = nullptr;
                coreQuantumUsed[coreId] = 0;

                // Lock readyQueue without holding processMutex across queue ops
                {
                    lock_guard<mutex> qLock(readyQueueMutex);
                    readyQueue.push(p);
                }
            }
        } else {
            lock_guard<mutex> lock(processMutex);
            releaseCore(coreId);
        }
    }
}

void startcores() {
    coreThreads.clear();
    for (int i = 0; i < systemConfig.numCpu; i++) {
        coreThreads.emplace_back(coreworker, i);             // start worker per dynamic core
    }
}

void stopcores() {
    for (int i = 0; i < systemConfig.numCpu; i++) {
        if (coreThreads[i].joinable()) coreThreads[i].join();   // wait for each core thread to finish
    }
    coreThreads.clear();
}