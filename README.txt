NEKOS MARQUEE CONSOLE
CSOPESY MO3 - Marquee Console
============================================================

NekOS is a command-line OS emulator developed in C++ for the
CSOPESY MO3 Marquee Console. It provides a command interpreter
and an animated text marquee whose text and refresh interval
can be changed while the program is running.


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


HOW TO RUN
============================================================

Using Visual Studio Code:

1. Open the CSOPESY-OS-Emulator folder in Visual Studio Code.
2. Open source/main.cpp.
3. Run the program using the IDE's Run/Debug function.
4. The NekOS console will appear with the Command> prompt.


Using the Terminal:

1. Open a terminal in the project root directory.
2. Compile the program:
   g++ source\main.cpp -o csopesy.exe
3. Run the program:
   csopesy.exe


AVAILABLE COMMANDS
============================================================

help
    Displays the available commands and their descriptions.

start_marquee
    Starts the marquee animation.

stop_marquee
    Stops the marquee animation.

set_text <text>
    Changes the text displayed by the marquee.

set_speed <ms>
    Changes the marquee refresh interval in milliseconds.

exit
    Terminates the program.


EXAMPLE
============================================================

Command> set_text Hello world!
Command> set_speed 300
Command> start_marquee


NOTES
============================================================

The value supplied to set_speed represents the refresh interval
in milliseconds.

Lower millisecond value = shorter delay = faster movement
Higher millisecond value = longer delay = slower movement