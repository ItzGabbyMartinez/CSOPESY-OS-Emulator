NEKOS OS EMULATOR
CSOPESY - FCFS CPU Scheduler
============================================================

NekOS is a command-line OS emulator developed in C++ for
CSOPESY. The program implements a multithreaded CPU scheduler
(FCFS or Round Robin), a process multiplexer with a screen
command, and a small set of process instructions. It also 
supports marquee console features

The emulator must be initialized first using the initialize
command. It reads its settings from config.txt, then starts the
scheduler thread and the CPU worker threads. Processes can be
created by the user or generated automatically in batches.


GROUP MEMBERS
============================================================

BANCORO, Maria Fides
DIMAUNAHAN, Chelsea Jei
MANALANG, Kennese Ross
MARTINEZ, Gabrielle


ENTRY FILE
============================================================

Entry File:
source/main.cpp

The main() function is located in source/main.cpp.


REQUIREMENTS
============================================================

- C++ compiler with C++11 support or later
- Windows
- Visual Studio Code or another C++ IDE
- MSYS2 UCRT64 GCC/G++ was used during development


FCFS SCHEDULER
============================================================

The emulator uses a multithreaded approach for CPU scheduling.

- 1 scheduler thread
- 4 CPU worker threads
- 10 processes
- 100 print commands per process
- First-Come-First-Serve (FCFS) ready queue

Each available CPU core receives the next process from the
ready queue. A process executes until all 100 of its print
commands are completed. Once a process finishes, the CPU core
becomes available for the next waiting process.

The screen -ls command can be used to observe processes moving
from the running state to the finished state.


PROCESS OUTPUT
============================================================

report-util

generates a report containing CPU utilization details: CPU 
utilization rate, number of cores used, number of cores available,
and a list of detailing active processes. 

example:

CPU utilization: 100%
Cores used: 4
Cores available: 0
-----------------------------------------
Active processes (Running / Waiting / Sleeping):
p01          (11/05/2026 06:56:12PM)   State: Waiting      Core: N/A     50 / 1544
p02          (11/05/2026 06:56:12PM)   State: Waiting      Core: N/A     45 / 1854
p03          (11/05/2026 06:56:12PM)   State: Waiting      Core: N/A     45 / 1792
p04          (11/05/2026 06:56:12PM)   State: Waiting      Core: N/A     45 / 1367
...

process-smi

screen -ls





HOW TO RUN
============================================================

Using Visual Studio Code:

1. Open the CSOPESY-OS-Emulator folder in Visual Studio Code.
2. Open source/main.cpp.
3. Run the program using the IDE's Run/Debug function.
4. The NekOS console will appear with the Command> prompt.


Using the Terminal:

1. Open a terminal in the source directory.
2. Compile the program:

   g++ -std=c++11 main.cpp scheduler.cpp util.cpp cli.cpp marquee.cpp -o main.exe

3. Run the program:

   .\main.exe


AVAILABLE COMMANDS
============================================================
initialize
    Reads config.txt and starts the scheduler. This must be
    done before any other command, except exit.

help
    Displays the available commands and their descriptions.

screen -s <process name>
    Creates a new process with random instructions and opens
    its screen.

screen -r <process name>
    Reopens the screen of a running process. Prints
    "Process <process name> not found." if the process does not
    exist or has already finished.

screen -ls
    Displays the CPU utilization, the active processes and the
    finished processes

scheduler-start
    Starts generating dummy processes continuously.

scheduler-stop
    Stops generating dummy processes.

report-util
    Saves the CPU utilization report to csopesy-log.txt.

start_marquee
    Starts the marquee animation.

stop_marquee
    Stops the marquee animation.

set_text <text>
    Changes the text displayed by the marquee.

set_speed <ms>
    Changes the marquee refresh interval in milliseconds.

clear
    Clears the console.

exit
    Shuts down the NekOS emulator.

Inside a process screen:

process-smi
    Displays the process name, ID and logs. It also shows the
    current instruction line and the total lines of code, or
    "Finished!" if the process is done.


TEST PROCEDURE
============================================================

1. Run the NekOS emulator using the IDE's Run/Debug function.

2. Enter:

   initialize

3. Enter:

   scheduler-start

4. Enter the following command every 1 to 2 seconds:

   screen -ls

5. Observe the processes moving from the active list to the
   finished list.

6. Enter:

   scheduler-stop

7. To check a single process, enter:

   screen -r <process name>

   Then enter process-smi to see its logs, and exit to go back
   to the main menu.

8. Enter:

   report-util

   and check that csopesy-log.txt was created.

9. Enter:

   exit


EXAMPLE
============================================================

Command> screen -ls

...
p559         (11/05/2026 06:56:22PM)   State: Waiting      Core: N/A     5 / 1594
p560         (11/05/2026 06:56:22PM)   State: Waiting      Core: N/A     5 / 1714
p561         (11/05/2026 06:56:22PM)   State: Waiting      Core: N/A     5 / 1991
p562         (11/05/2026 06:56:22PM)   State: Waiting      Core: N/A     5 / 1332
p563         (11/05/2026 06:56:22PM)   State: Waiting      Core: N/A     5 / 1754
...

Command> exit
Shutting down NekOS...


NOTES
============================================================

The FCFS scheduler automatically executes processes without
requiring user intervention. The screen -ls command only
displays the current state of the processes and does not control
their execution.

The value supplied to set_speed represents the marquee refresh
interval in milliseconds.

Lower millisecond value = shorter delay = faster movement
Higher millisecond value = longer delay = slower movement
