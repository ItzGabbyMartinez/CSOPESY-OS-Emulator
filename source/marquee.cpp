#include "marquee.h"
#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <mutex>

using namespace std;

extern mutex inputMutex;

// ============================================================
// SHARED MARQUEE SETTINGS
// ============================================================
// Marquee display/animation will use this later on
string marqueeText = "Hello World!";
int marqueeSpeed = 100;

// For the stop/start functionality
bool runMarquee = false;
thread marqueeThread;


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

        {
            lock_guard<mutex> lock(inputMutex);

            // Save current cursor position
            cout << "\033[s";

            // Move up to the marquee line
            cout << "\033[A";

            // Clear marquee line
            cout << "\r\033[2K";

            // Draw marquee
            if (position > 0)
                cout << string(position, ' ');

            if (position < 0)
                cout << marqueeText.substr(-position);
            else
                cout << marqueeText;

            // Restore cursor to command line
            cout << "\033[u";

            cout.flush();
        }

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

    {
        lock_guard<mutex> lock(inputMutex); 
        marqueeText = text;
        cout << "Marquee text set to: " << marqueeText << endl;
    }
}


void setspeed(int milliseconds){
    if (milliseconds <= 0){
        cout << "Error: Marquee speed must be greater than 0 milliseconds" << endl;
        return;
    }

    {
        lock_guard<mutex> lock(inputMutex);
        marqueeSpeed = milliseconds;
        cout << "Marquee speed set to: " << marqueeSpeed << "ms." << endl;  
    }
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