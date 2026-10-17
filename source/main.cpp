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
#include "cli.h"

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

