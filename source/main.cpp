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
    startscheduler();        // starts the scheduler and core threads
    runcli();
    stopscheduler();         // stops the scheduler and core threads

    return 0;
}

