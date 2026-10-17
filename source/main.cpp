#include <iostream>
#include <string>
#include <cstdlib>
#include <thread>
#include <chrono>
#include <conio.h>
#include <mutex>
#include <queue> // used for ready queue
#include <ctime> // used for timestamp

#include "util.h"
#include "marquee.h"
#include "scheduler.h"

using namespace std; //For the Standard Library

/*
    ============================================================
                        NEKOS MARQUEE CONSOLE
    ============================================================

    DEVELOPMENT NOTE:
    Functions are grouped according to their assigned feature.

    Feel free to add any helper functions, variables, or other
    components needed for your feature. Please place them under
    the appropriate feature section to keep the code organized.
*/

// shared command input, ownership synchronization
string inputBuffer = "";
mutex inputMutex;


// ============================================================
// FEATURE 1: CONSOLE UI & COMMAND INTERPRETER
// ============================================================
void showheader();
void runcli();
void readcommand(const string& command);
void showhelp();


// ============================================================
// MAIN FUNCTION
// ============================================================
int main(){

    //Let's keep main() limited to function calls so we can stay modular
    //this will make it easier for us to expand the project later for MO3
    showheader();
    createProcesses();      // creates the 10 processes as soon as NekOS starts
    runcli();

    return 0;
}

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

 Version date: September 28, 2026
)" << endl;
}


void runcli() {
    string command;
    std::cout << "\nCommand> " << flush;

    while (true) {
        if (_kbhit()) {
            char ch = _getch();

            if (ch == '\r' || ch == '\n') {     // if the user pressed Enter/Return key

                {
                    lock_guard<mutex> lock(inputMutex); // lock the input buffer for thread safety

                    cout << endl;                   // print a new line

                    command = trim(inputBuffer);    // trim whitespace from the typed command
                    inputBuffer = "";               // clear the buffer for the next command
                }
                
                if (!command.empty()) {         // if the command is not empty, process, else exit
                    readcommand(command);

                    if (command == "exit") {
                        break;
                    }
                }

                
                cout << "Command> " << flush;       // print the command prompt again for the next input
                   
            }

            else if (ch == '\b') {              // if the user pressed Backspace
                lock_guard<mutex> lock(inputMutex); // lock the input buffer for thread safety

                if (!inputBuffer.empty()) {     // and is not empty, remove the last character, erase from console
                    inputBuffer.pop_back();

                    cout << "\b \b" << flush;   // Erase on screen
                }
            }
            else {                              // for any other regular typed character
                lock_guard<mutex> lock(inputMutex); 

                inputBuffer += ch;              // append character to the input buffer
                cout << ch << flush;        
            }
        }
    }
}

void readcommand(const string& command) {
 
    // First, split into the command keyword and the rest as its argument.
    size_t spacePos = command.find(' ');
    string cmd = (spacePos == string::npos) ? command : command.substr(0, spacePos);
    string arg  = (spacePos == string::npos) ? "" : trim(command.substr(spacePos + 1));
 
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
    else if (cmd == "screen") {
        if (arg == "-ls") {
            printProcessList();         // show running and finished processes
        } else {
            cout << "Input Error: unknown screen option. Usage: screen -ls" << endl;
            cout << endl;
        }
    }
    else if (cmd == "exit") {
        cout << "Exiting NekOS Marquee..." << endl;
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
    cout << "help             - Displays the commands and their descriptions\n";
    cout << "start_marquee    - Starts the marquee animation\n";
    cout << "stop_marquee     - Stops the marquee animation\n";
    cout << "set_text <text>  - Sets the marquee text\n";
    cout << "set_speed <ms>   - Sets the marquee refresh rate in milliseconds\n";
    cout << "screen -ls       - Lists the running and finished processes\n";
    cout << "clear            - Clears the console\n";
    cout << "exit             - Terminates the console\n";
    cout << "====================================\n\n";
}

