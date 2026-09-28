#include <iostream>
#include <string>
#include <cstdlib>
#include <thread>
#include <chrono>
#include <conio.h>
#include <mutex>

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
// SHARED MARQUEE SETTINGS
// ============================================================
// Marquee display/animation will use this later on
string marqueeText = "Hello World!";
int marqueeSpeed = 100;

// For the stop/start functionality
bool runMarquee = false;
thread marqueeThread;

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

// Helpers
void clearscreen();
string trim(const string& s);


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


// ============================================================
// MAIN FUNCTION
// ============================================================
int main(){

    //Let's keep main() limited to function calls so we can stay modular
    //this will make it easier for us to expand the project later for MO3
    showheader();

    // Temporary tests for Feature 3
    settext("NekOS Marquee Test");
    setspeed(250);

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

 Version date: 
)" << endl;
}


void runcli() {
    string command;
    std::cout << "\nCommand> " << flush;

    while (true) {
        if (_kbhit()) {
            char ch = _getch();

            if (ch == '\r' || ch == '\n') {     // if the user pressed Enter/Return key

                lock_guard<mutex> lock(inputMutex); // lock the input buffer for thread safety

                cout << endl;                   // print a new line

                command = trim(inputBuffer);    // trim whitespace from the typed command
                inputBuffer = "";               // clear the buffer for the next command

                if (!command.empty()) {         // if the command is not empty, process, else exit
                    readcommand(command);

                    if (command == "exit") {
                        break;
                    }
                }

                {
                    lock_guard<mutex> lock(inputMutex);
                    cout << "Command> " << flush;       // print the command prompt again for the next input
                }   
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
        } else {
            settext(arg);
        }
    }
    else if (cmd == "set_speed") {
        if (arg.empty()) {
            cout << "Input Error: set_speed requires a number in milliseconds. Usage: set_speed <ms>" << endl;
        } else {
            // error checking of speed input!
            try {
                size_t pos;
                int ms = stoi(arg, &pos);
                if (pos != arg.size()) {
                    cout << "Input Error: set_speed argument must be a whole number." << endl;
                } else {
                    setspeed(ms);
                }
            } catch (...) {
                cout << "Input Error: set_speed argument must be a whole number." << endl;
            }
        }
    }
    else if (cmd == "exit") {
        cout << "Exiting NekOS Marquee..." << endl;
        shutdownmarquee();
    }
    else {
        cout << "Error: '" << cmd
             << "' is not recognized as a command." << endl;
    }
}

void showhelp() {
    cout << "\n========== NekOS Commands ==========\n";
    cout << "help             - Displays the commands and their descriptions\n";
    cout << "start_marquee    - Starts the marquee animation\n";
    cout << "stop_marquee     - Stops the marquee animation\n";
    cout << "set_text <text>  - Sets the marquee text\n";
    cout << "set_speed <ms>   - Sets the marquee refresh rate in milliseconds\n";
    cout << "clear            - Clears the console\n";
    cout << "exit             - Terminates the console\n";
    cout << "====================================\n\n";
}

void clearscreen() {
    // used escape characters to clean, and \033[1;1H moves the cursor to the top left corner of the screen
    std::cout << "\033[2J\033[1;1H";
}

string trim(const string& s) {
    // Removes whitespace before and after the command.
    size_t start = s.find_first_not_of(" \t\r\n");
    if (start == string::npos) return "";

    size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

// ============================================================
// FEATURE 2: MARQUEE DISPLAY & ANIMATION
// ============================================================
void startmarquee() {
    if (runMarquee) return;                 // if the marquee is already running, do nothing.

    runMarquee = true;                      // set the flag to true to indicate that the marquee is running.

    cout << endl;                           // print a new line to separate marquee from the command prompt.
    marqueeThread = thread(runmarquee);     // start the marquee animation in a separate thread

}

void stopmarquee() {
    if (!runMarquee)                                // if the marquee is not running,
    {
        cout << "Marquee is not running!" << endl;  // print a message to inform the user
        return;                                     // and return early 
    } 

    runMarquee = false;                             // signal animation loop to stop

    if (marqueeThread.joinable())                   // join only if the thread is still active
        marqueeThread.join();                       // wait for the marquee thread to finish before continuing, ensuring that the marquee has stopped before we proceed.    
}

void runmarquee() {

    const int WIDTH = 70;           // the width of the marquee display area. 
                                    // or, the number of characters that will be visible

    int position = -marqueeText.length();   // start the text off-screen to the left.

    while (runMarquee) {

        // using ansi escape sequences/characters:
        cout << "\033[s";     // save cursor position.
        cout << "\033[A";     // move up to the marquee line
        cout << "\r\033[2K";  // return to start and clear that line.

        // draw marquee
        if (position > 0)           // add leading spaces as it moves to the right (scrolling effect !)
            cout << string(position, ' ');

        if (position < 0)           // show only the visible part while entering the screen.
            cout << marqueeText.substr(-position);
        else
            cout << marqueeText;    // show the full text once it is inside the display area.

        cout.flush();          // flush the output to ensure it appears immediately.

        cout << "\033[u";     // put back cursor to the command> line.

        position++;           // move the text to the right for the next frame.

        if (position > WIDTH)                   // if the text has completely scrolled out of view, reset position to start over.
            position = -marqueeText.length(); 

        this_thread::sleep_for(chrono::milliseconds(marqueeSpeed));
        // this_thread::sleep_for --> C++ standard library function that pauses the
        // current thread for the specified duration, so we can control the speed of the 
        // marquee animation.
    }
}

// ============================================================
// FEATURE 3: MARQUEE SETTINGS
// ============================================================
void settext(const string& text){
    if (text.empty()){
        cout << "Error: Marquee text can't be empty." << endl;
        return;
    }
    marqueeText = text;
    cout << "Marquee text set to: " << marqueeText << endl;

}


void setspeed(int milliseconds){
    if (milliseconds <= 0){
        cout << "Error: Marquee speed must be greater than 0 milliseconds" << endl;
        return;
    }
    marqueeSpeed = milliseconds;
    cout << "Marquee speed set to: " << marqueeSpeed << "ms." << endl;
}

// ============================================================
// FEATURE 4: PROGRAM INTEGRATION & PERFORMANCE
// ============================================================
void initializemarquee(){
    cout << "Initializing Marquee Subsystem..." << endl;

    runMarquee = false; 
}

void shutdownmarquee(){
    if (runMarquee) {
        stopmarquee();
    }
};