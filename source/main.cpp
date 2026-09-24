#include <iostream>
#include <string>
#include <cstdlib>
#include <thread>
#include <chrono>

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



// ============================================================
// FEATURE 1: CONSOLE UI & COMMAND INTERPRETER
// ============================================================
void showheader();
void runcli();
void readcommand(const string& command);
void showhelp();

// Helpers
void clearscreen();   


// ============================================================
// FEATURE 2: MARQUEE DISPLAY & ANIMATION
// ============================================================
void startmarquee();
void stopmarquee();
void runmarquee();


// ============================================================
// FEATURE 3: MARQUEE SETTINGS
// ============================================================
void settext(const string& text);
void setspeed(int milliseconds);


// ============================================================
// FEATURE 4: PROGRAM INTEGRATION & PERFORMANCE
// ============================================================
void initializemarquee();
void shutdownmarquee();


//Main Function
int main(){

    //Let's keep main() limited to function calls so we can stay modular
    //this will make it easier for us to expand the project later for MO3
    showheader();
    runcli();

    return 0;
}

void showheader() {
    cout << R"(
             _     ____   _____              
            | |   / __ \ / ____|       
  _ __   ___| | _| |  | | (___      /\
 | '_ \ / _ \ |/ / |  | |\___ \    (. . 7      
 | | | |  __/   <| |__| |____) |    |  ~\
 |_| |_|\___|_|\_\\____/|_____/     |_f_,)/

 Type a command to begin (or 'exit' to quit)
)" << endl;
}

void runcli(){
    string command;

    while (true){
        cout << "Enter a command: ";
        getline(cin, command);

        if (command == "clear") {
            clearscreen();
            showheader();
        }
        else if (command == "exit") {
            exit(0);          
        }
        else {
            readcommand(command);   
        }
    }
}

//Function responsible for handling the 5 required commands 
void readcommand(const string& command){
    if (command == "initialize") {
        cout << "Initialize command recognized. Doing something." << endl;
    }
    else if (command == "screen") {
        cout << "Screen command recognized. Doing something." << endl;
    }
    else if (command == "scheduler-start") {
        cout << "Scheduler-start command recognized. Doing something." << endl;
    }
    else if (command == "scheduler-stop") {
        cout << "Scheduler-stop command recognized. Doing something." << endl;
    }
    else if (command == "report-util") {
        cout << "Report-util command recognized. Doing something." << endl;
    }
    else {
        cout << command << " is not recognized as a command." << endl;
    }
}

void clearscreen() {
    // used escape characters to clean, and \033[1;1H moves the cursor to the top left corner of the screen
    std::cout << "\033[2J\033[1;1H";
}