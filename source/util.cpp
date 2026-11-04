#include "util.h"
#include <iostream>
#include <string>
#include <ctime>
#include <mutex>

using namespace std;

string getTimestamp() {
    static mutex timeMutex;
    time_t now = time(nullptr);    // get current time
    tm local;

    {
        lock_guard<mutex> lock(timeMutex);
        tm* shared = localtime(&now);           // points to the shared buffer
        local = *shared;                        // copy it before unlocking
    }

    char buffer[30];
    strftime(buffer, sizeof(buffer), "%m/%d/%Y %I:%M:%S%p", &local);    // followed format: MM/DD/YYYY HH:MM:SSAM
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