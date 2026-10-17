NEKOS OS EMULATOR
CSOPESY - FCFS CPU Scheduler
============================================================

NekOS is a command-line OS emulator developed in C++ for
CSOPESY. The program implements a multithreaded
First-Come-First-Serve (FCFS) CPU scheduler together with the
previously developed marquee console features.

Upon startup, the emulator automatically creates 10 processes
with 100 print commands each. The processes are placed in a
ready queue and assigned to four available CPU cores according
to FCFS scheduling.


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

Each process generates its own text file containing the output
of its 100 print commands.

The following files are generated:

process01.txt
process02.txt
process03.txt
process04.txt
process05.txt
process06.txt
process07.txt
process08.txt
process09.txt
process10.txt

Each file identifies the corresponding process and contains
100 log entries. Each print command records the timestamp of
execution, the CPU core that executed it, and a message from
the process.

Example:

Process name: process02
Logs:

(10/17/2026 07:19:23PM) Core:1 "Hello world from process02!"

The generation of process text files is temporary and is used
for the FCFS scheduler homework. It will be disabled for the
final machine project submission as required.


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

help
    Displays the available commands and their descriptions.

screen -ls
    Displays the currently running and finished processes.

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


FCFS TEST PROCEDURE
============================================================

1. Run the NekOS emulator using the IDE's Run/Debug function.

2. Upon startup, 10 processes with 100 print commands each are
   automatically created and scheduled.

3. Enter the following command every 1 to 2 seconds:

   screen -ls

4. Observe the processes moving from the running processes list
   to the finished processes list.

5. Continue until all 10 processes are shown as finished with
   100 / 100 instructions completed.

6. Enter:

   exit

7. Verify that 10 process text files were generated.


EXAMPLE
============================================================

Command> screen -ls

Running processes:
process05   Core: 3   84 / 100
process06   Core: 0   83 / 100
process07   Core: 1   81 / 100
process08   Core: 2   82 / 100

Finished processes:
process01   Finished   100 / 100
process02   Finished   100 / 100
process03   Finished   100 / 100
process04   Finished   100 / 100

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