#include "cli.h"
#include "util.h"
#include "marquee.h"
#include "scheduler.h"
#include "process.h"
#include <iostream>
#include <string>
#include <sstream>
#include <cctype>
#include <conio.h>
#include <mutex>
#include "config.h"

using namespace std;

// shared command input, ownership synchronization
string inputBuffer = "";
mutex inputMutex;

// the process whose screen the user is currently inside (nullptr = main menu)
static Process* currentScreen = nullptr;

// ============================================================
// FEATURE 1: CONSOLE UI & COMMAND INTERPRETER
// ============================================================
void showheader();
void runcli();
void readcommand(const string& command);
void showhelp();


// ============================================================
// FEATURE 1: CONSOLE UI & COMMAND INTERPRETER
// ============================================================

void showheader() {
    cout << R"(
             _     ____   _____              
            | |   / __ \ / ____|       
  _ __   ___| | _| |  | | (___      /\
 | '_ \ / _ \ |/ / |  | |\___ \    (. . 7      
 | | | |  __/   <| |__| |____) |    |  ~\
 |_| |_|\___|_|\_\\____/|_____/     |_f_,)/

Group developer:
    - BANCORO, Maria Fides
    - DIMAUNAHAN, Chelsea Jei
    - MANALANG, Kennese Ross
    - MARTINEZ, Gabrielle

 Version date: November 01, 2026
)" << endl;
}


#include <thread>
#include <chrono>

void runcli() {
    string command;
    std::cout << "\nCommand> " << flush;

    while (true) {
        if (_kbhit()) {
            char ch = _getch();

            if (ch == '\r' || ch == '\n') {     // if user pressed Enter
                {
                    lock_guard<mutex> lock(inputMutex);
                    cout << endl;
                    command = trim(inputBuffer);
                    inputBuffer = "";
                }
                
                if (!command.empty()) {
                    bool shouldQuit = (command == "exit" && currentScreen == nullptr);
                    readcommand(command);

                    if (shouldQuit) { // only exit if user typed "exit" in the main menu, not inside a process screen
                        break;
                    }
                }

                cout << "Command> " << flush;
            }
            else if (ch == '\b') {              // backspace
                lock_guard<mutex> lock(inputMutex);
                if (!inputBuffer.empty()) {
                    inputBuffer.pop_back();
                    cout << "\b \b" << flush;
                }
            }
            else {                              // regular character
                lock_guard<mutex> lock(inputMutex);
                inputBuffer += ch;
                cout << ch << flush;
            }
        } else {
            // prevent 100% CPU lock in the input loop
            this_thread::sleep_for(chrono::milliseconds(10));
        }
    }
}

// ============================================================
// SCREEN MULTIPLEXER & PROCESS CLI
// ============================================================

// > process-smi: prints the process name, ID and logs, then either
// > "Current instruction line / Lines of code" or "Finished!" when the process is done.
static void printProcessSmi(Process* p) {
    lock_guard<mutex> consoleLock(inputMutex);  // lock the console so the output doesn't get mixed with user input
    lock_guard<mutex> dataLock(processMutex);   // lock the process data so it doesn't change while we're printing it

    cout << "Process name: " << p->name << endl;
    cout << "ID: " << p->id << endl;
    cout << "Logs:" << endl;
    for (const string& line : p->logs) {
        cout << line << endl;
    }
    cout << endl;

    if (p->state == "Finished") {
        cout << "Finished!" << endl;
    } else {
        cout << "Current instruction line: " << p->pc << endl; // uses the index of the nect instruction to be executed
        cout << "Lines of code: " << p->instructions.size() << endl; 
    }
    cout << endl;
}

// handles: screen -s <name> and screen -r <name> and screen -ls
static void handleScreenCommand(const string& arg) {
    istringstream iss(arg);
    string opt, name, extra;
    iss >> opt >> name >> extra;

    // 1) screen -ls
    if (opt == "-ls" && name.empty()) {
        printProcessList(); // lists all processes, both running and finished
        return;
    }

    // 2) screen -s / screen -r
    if ((opt == "-s" || opt == "-r") && !name.empty() && extra.empty()) { // needs one and only one argument after the option

        // check for valid input first
        for (char c : name) {
            if (!isalnum(static_cast<unsigned char>(c)) && c != '_' && c != '-') {
                cout << "Input Error: process name may only contain letters, digits, '_' and '-'." << endl;
                cout << endl;
                return;
            }
        }

        if (opt == "-s") {
            // create a new process, give it random instructions, and put it in the ready queue
            Process* p = createProcess(name);
            if (p == nullptr) {
                cout << "Error: a process named '" << name << "' already exists." << endl;
                cout << endl;
                return;
            }

            {
                lock_guard<mutex> lock(processMutex);
                generateRandomInstructions(*p);}
            enqueueProcess(p);

            currentScreen = p; // move into the new process screen
        }
        else { 
            // reopen a process 
            Process* p = findProcess(name);
            bool finished = false;

            if (p != nullptr) { // process found, check if it's finished
                lock_guard<mutex> lock(processMutex);
                finished = (p->state == "Finished");
            }

            if (p == nullptr || finished) { // process not found or already finished
                cout << "Process " << name << " not found." << endl;
                cout << endl;
                return;}

            currentScreen = p;
        }

        // move into the process screen: clear the console, then show the process info
        clearscreen();
        printProcessSmi(currentScreen);
        return;
    }

    // anything else BESIDES -s, -r, and -ls is not a valid screen command
    cout << "Input Error: invalid screen command." << endl;
    cout << "Usage: screen -s <process name> | screen -r <process name> | screen -ls" << endl;
    cout << endl;
}

void readcommand(const string& command) {
 
    // First, split into the command keyword and the rest as its argument.
    size_t spacePos = command.find(' ');
    string cmd = (spacePos == string::npos) ? command : command.substr(0, spacePos);
    string arg  = (spacePos == string::npos) ? "" : trim(command.substr(spacePos + 1));

    // only process-smi and exit are recognized (inside a process screen)
    if (currentScreen != nullptr) {
        if (cmd == "process-smi") {
            printProcessSmi(currentScreen);
        }
        else if (cmd == "exit") {
            currentScreen = nullptr;                
            clearscreen();
            showheader();
        }
        else {
            cout << "Error: '" << cmd << "' is not recognized in a process screen. "
                 << "Available commands: process-smi, exit" << endl;
            cout << endl;
        }
        return;
    }

    // Initialize the system using the values from config.txt
    if (cmd == "initialize") {
        if (isInitialized) {
            cout << "System is already initialized." << endl << endl;
            return;
        }

        if (loadConfig("config.txt")) {
            isInitialized = true;
            startscheduler(); // Automatically start CPU worker threads after initializing

            cout << "System initialized successfully." << endl;
            cout << "CPU cores: " << systemConfig.numCpu << endl;
            cout << "Scheduler: " << systemConfig.scheduler << endl;
            cout << "Quantum cycles: " << systemConfig.quantumCycles << endl;
            cout << "Batch process frequency: " << systemConfig.batchProcessFreq << endl;
            cout << "Min instructions: " << systemConfig.minIns << endl;
            cout << "Max instructions: " << systemConfig.maxIns << endl;
            cout << "Delay per execution: " << systemConfig.delaysPerExec << endl;
            cout << endl;
        }

        return;
    }

    // Only initialize and exit are allowed before system initialization
    if (!isInitialized && cmd != "exit") {
        cout << "Error: Please initialize the system first." << endl << endl;
        return;
    }
 
    if (cmd == "help") {
        showhelp();
    }
    else if (cmd == "clear") {
        clearscreen();
        showheader();
    }
    else if (cmd == "start_marquee") {
        startmarquee();
    }
    else if (cmd == "stop_marquee") {
        stopmarquee();
    }
    else if (cmd == "set_text") {
        if (arg.empty()) {
            cout << "Input Error: set_text requires a text argument. Usage: set_text <text>" << endl;

            cout << endl; // addtl line break for readability 

        } else {
            settext(arg);
        }
    }
    else if (cmd == "set_speed") {
        if (arg.empty()) {
            cout << "Input Error: set_speed requires a number in milliseconds. Usage: set_speed <ms>" << endl;

            cout << endl; // addtl line break for readability
        } else {
            // error checking of speed input!
            try {
                size_t pos;
                int ms = stoi(arg, &pos);
                if (pos != arg.size()) {
                    cout << "Input Error: set_speed argument must be a whole number." << endl;

                    cout << endl;
                } else {
                    setspeed(ms);
                }
            } catch (...) {
                cout << "Input Error: set_speed argument must be a whole number." << endl;

                cout << endl;
            }
        }
    }
    else if (cmd == "scheduler-start") {
        startbatchgenerator();
        cout << "Batch process generator started." << endl << endl;
    }
    else if (cmd == "scheduler-stop") {
        stopbatchgenerator();
        cout << "Batch process generator stopped." << endl << endl;
    }
    else if (cmd == "report-util") {
        generateReportUtil("csopesy-log.txt");
    }
    else if (cmd == "screen") {
        handleScreenCommand(arg);
    }
    else if (cmd == "exit") {
        cout << "Shutting down NekOS..." << endl;
        stopscheduler();
        shutdownmarquee();
    }
    else {
        cout << "Error: '" << cmd
             << "' is not recognized as a command." << endl;

        cout << endl;
    }
}

void showhelp() {
    cout << "\n========== NekOS Commands ==========\n";
    cout << "initialize       - Initializes the system using config.txt\n";
    cout << "scheduler-start  - Starts batch dummy process generation\n";
    cout << "scheduler-stop   - Stops batch dummy process generation\n";
    cout << "report-util      - Generates CPU utilization report in csopesy-log.txt\n";
    cout << "help             - Displays the commands and their descriptions\n";
    cout << "start_marquee    - Starts the marquee animation\n";
    cout << "stop_marquee     - Stops the marquee animation\n";
    cout << "set_text <text>  - Sets the marquee text\n";
    cout << "set_speed <ms>   - Sets the marquee refresh rate in milliseconds\n";
    cout << "screen -s <name> - Creates a new process and opens its screen\n";
    cout << "screen -r <name> - Reopens the screen of a running process\n";
    cout << "screen -ls       - Lists the running and finished processes\n";
    cout << "process-smi      - (inside a process screen) Shows the process info and logs\n";
    cout << "clear            - Clears the console\n";
    cout << "exit             - Terminates the console\n";
    cout << "====================================\n\n";
}