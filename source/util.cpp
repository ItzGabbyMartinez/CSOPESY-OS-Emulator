#include "util.h"
#include <iostream>
#include <string>
#include <ctime>

using namespace std;

// returns the current date and time
string getTimestamp() {
    time_t now = time(nullptr);                 // get the current time
    tm* local = localtime(&now);                // kenn: had issues on my machine with localtime_s, 
                                                // so using localtime instead. If you have issues, try changing it back/uncommenting to localtime_s
    
    // tm local;
    // localtime_s(&local, &now);                  // convert it to the local date and time

    char buffer[30];
    strftime(buffer, sizeof(buffer), "%m/%d/%Y %I:%M:%S%p", local);    // followed format: MM/DD/YYYY HH:MM:SSAM
    return string(buffer);
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