#pragma once
#include <string>
#include <vector>
#include <map>
#include <deque>
#include <mutex>
#include <cstdint>

enum InstrType { PRINT, DECLARE, ADD, SUBTRACT, SLEEP, FOR };

struct Instruction {
    InstrType type = PRINT;
    std::string var1;               // DECLARE/ADD/SUBTRACT: the variable being set
    std::string arg2;               // a variable name or a number (PRINT: optional variable to append)
    std::string arg3;               // a variable name or a number
    std::string msg;                // PRINT text
    int value = 0;                  // DECLARE: default value, SLEEP: ticks, FOR: repeat count
    std::vector<Instruction> body;  // FOR: the instructions inside the loop
};

struct Process {
    std::string name;                       // dynamic, from screen -s <name> or auto-generated
    int id;
    std::vector<Instruction> instructions;  // the flat list the CPU steps through
    int pc;                                 // index of the next instruction to run
    std::map<std::string, uint16_t> vars;   // this process's variables
    std::vector<std::string> logs;          // PRINT output goes here, never to cout
    int sleepTicksLeft;                     // ticks remaining while Sleeping
    std::string state;                      // "Waiting", "Running", "Sleeping" or "Finished"
    int coreId;                             // -1 if not on a core
    std::string startTime;
};

extern std::deque<Process> allProcesses;    // deque so Process* pointers stay valid as it grows
extern std::mutex processListMutex;         // lock when adding to or looping over allProcesses

Process* createProcess(const std::string& name);    // returns nullptr if the name is already taken
bool executeNext(Process& p);                       // runs ONE instruction, returns true when none are left
void flattenInstructions(const std::vector<Instruction>& in, std::vector<Instruction>& out);

Process* findProcess(const std::string& name);      // returns nullptr if not found
void generateRandomInstructions(Process& p);        // populates p.instructions based on config