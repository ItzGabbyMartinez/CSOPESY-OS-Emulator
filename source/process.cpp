#include "process.h"
#include "config.h"
#include "util.h"
#include <cctype>
#include <random>

using namespace std;

deque<Process> allProcesses;
mutex processListMutex;
static int nextProcessId = 1;

// makes a new process with an empty instruction list, the generator fills it in later
Process* createProcess(const string& name) {
    lock_guard<mutex> lock(processListMutex);

    for (Process& p : allProcesses) {
        if (p.name == name) return nullptr;
    }

    allProcesses.emplace_back();
    Process& p = allProcesses.back();
    p.name = name;
    p.id = nextProcessId++;
    p.pc = 0;
    p.sleepTicksLeft = 0;
    p.state = "Waiting";
    p.coreId = -1;
    p.startTime = getTimestamp();
    return &p;
}

// finds an existing process by name in a thread-safe way
Process* findProcess(const string& name) {
    lock_guard<mutex> lock(processListMutex);
    for (Process& p : allProcesses) {
        if (p.name == name) return &p;
    }
    return nullptr;
}

// turns FOR loops into plain repeated instructions so the worker only walks a flat list
void flattenInstructions(const vector<Instruction>& in, vector<Instruction>& out) {
    for (const Instruction& ins : in) {
        if (ins.type == FOR) {
            for (int i = 0; i < ins.value; i++)     // repeat the body 'value' times
                flattenInstructions(ins.body, out); // recursive, so nested FORs work too
        } else {
            out.push_back(ins);
        }
    }
}

/*

resolve: Evaluates an argument string, determines if it is a literal number
or a variable name, and returns its corresponding integer value.
- If arg is empty, returns 0.
- If arg starts with a digit, it converts the numeric string to an int using stoi().
- Otherwise, it treats arg as a variable name and looks it up in p.vars. 
   Note: std::map::operator[] automatically creates the variable with a default 
   value of 0 if it has not been declared yet.

*/
static int resolve(Process& p, const string& arg) {
    if (arg.empty()) return 0;
    if (isdigit(arg[0])) return stoi(arg);
    return p.vars[arg];
}

// keeps the value inside uint16 range, done in int so 3 - 5 doesn't wrap to 65534
static uint16_t clamp16(int v) {
    if (v < 0) return 0;
    if (v > 65535) return 65535;
    return (uint16_t)v;
}

// runs one instruction (one CPU tick). The caller (core worker) must hold processMutex
bool executeNext(Process& p) {
    if (p.pc >= (int)p.instructions.size()) return true;    // nothing left to run

    Instruction& ins = p.instructions[p.pc];

    switch (ins.type) {
        case DECLARE:
            p.vars[ins.var1] = clamp16(ins.value);
            break;
        case ADD:
            p.vars[ins.var1] = clamp16(resolve(p, ins.arg2) + resolve(p, ins.arg3));
            break;
        case SUBTRACT:
            p.vars[ins.var1] = clamp16(resolve(p, ins.arg2) - resolve(p, ins.arg3));
            break;
        case PRINT: {
            string out = ins.msg;
            if (!ins.arg2.empty()) out += to_string(resolve(p, ins.arg2));
            p.logs.push_back("(" + getTimestamp() + ") Core:" + to_string(p.coreId) + " \"" + out + "\"");
            break;
        }
        case SLEEP:
            p.sleepTicksLeft = ins.value;
            p.state = "Sleeping";
            break;
        case FOR:                                   // already flattened at creation, nothing to do
            break;
    }

    p.pc++;                                         // move to the next instruction
    return p.pc >= (int)p.instructions.size();
}

// Generates random instructions for a process based on config.txt parameters (min-ins / max-ins)
void generateRandomInstructions(Process& p) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> targetDist(systemConfig.minIns, systemConfig.maxIns);
    uniform_int_distribution<int> typeDist(0, 4); // PRINT, DECLARE, ADD, SUBTRACT, SLEEP (FOR handled separately or via flattening)

    int targetCount = targetDist(gen);
    vector<Instruction> rawInstructions;

    while ((int)rawInstructions.size() < targetCount) {
        int t = typeDist(gen);
        Instruction ins;
        if (t == 0) {
            ins.type = PRINT;
            ins.msg = "Hello world from " + p.name + "!";
        } else if (t == 1) {
            ins.type = DECLARE;
            ins.var1 = "x" + to_string(rawInstructions.size());
            uniform_int_distribution<int> valDist(0, 100);
            ins.value = valDist(gen);
        } else if (t == 2) {
            ins.type = ADD;
            ins.var1 = "y" + to_string(rawInstructions.size());
            ins.arg2 = "5";
            ins.arg3 = "10";
        } else if (t == 3) {
            ins.type = SUBTRACT;
            ins.var1 = "z" + to_string(rawInstructions.size());
            ins.arg2 = "20";
            ins.arg3 = "5";
        } else {
            ins.type = SLEEP;
            uniform_int_distribution<int> sleepDist(1, 10); // uint8 ticks
            ins.value = sleepDist(gen);
        }
        rawInstructions.push_back(ins);
    }

    flattenInstructions(rawInstructions, p.instructions);
}