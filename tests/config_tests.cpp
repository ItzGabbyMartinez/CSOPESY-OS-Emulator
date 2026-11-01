#include "../source/config.h"

#include <fstream>
#include <iostream>
#include <string>
#include <cstdio>

using namespace std;

int passed = 0;
int failed = 0;

const string TEST_FILE = "test_config.txt";

void expect(bool condition, const string& testName)
{
    if (condition) {
        cout << "[PASS] " << testName << endl;
        passed++;
    }
    else {
        cout << "[FAIL] " << testName << endl;
        failed++;
    }
}

void writeConfig(const string& content)
{
    ofstream file(TEST_FILE);
    file << content;
    file.close();
}

string validConfig(
    int cpu = 4,
    const string& scheduler = "fcfs",
    unsigned int quantum = 5,
    unsigned int batch = 1,
    unsigned int minIns = 1000,
    unsigned int maxIns = 2000,
    unsigned int delay = 0)
{
    return
        "num-cpu " + to_string(cpu) + "\n" +
        "scheduler " + scheduler + "\n" +
        "quantum-cycles " + to_string(quantum) + "\n" +
        "batch-process-freq " + to_string(batch) + "\n" +
        "min-ins " + to_string(minIns) + "\n" +
        "max-ins " + to_string(maxIns) + "\n" +
        "delays-per-exec " + to_string(delay) + "\n";
}


// ============================================================
// VALID CONFIGURATION TESTS
// ============================================================

void testValidFCFS()
{
    writeConfig(validConfig(4, "fcfs"));

    expect(
        loadConfig(TEST_FILE),
        "Valid FCFS configuration"
    );
}

void testValidRR()
{
    writeConfig(validConfig(4, "rr"));

    expect(
        loadConfig(TEST_FILE),
        "Valid Round Robin configuration"
    );
}

void testMinimumCPU()
{
    writeConfig(validConfig(1));

    expect(
        loadConfig(TEST_FILE),
        "Minimum CPU count (1)"
    );
}

void testMaximumCPU()
{
    writeConfig(validConfig(128));

    expect(
        loadConfig(TEST_FILE),
        "Maximum CPU count (128)"
    );
}

void testZeroDelay()
{
    writeConfig(validConfig(4, "fcfs", 5, 1, 1000, 2000, 0));

    expect(
        loadConfig(TEST_FILE),
        "Zero delays-per-exec is valid"
    );
}


// ============================================================
// INVALID VALUE TESTS
// ============================================================

void testCPUTooLow()
{
    writeConfig(validConfig(0));

    expect(
        !loadConfig(TEST_FILE),
        "Reject num-cpu below minimum"
    );
}

void testCPUTooHigh()
{
    writeConfig(validConfig(129));

    expect(
        !loadConfig(TEST_FILE),
        "Reject num-cpu above maximum"
    );
}

void testInvalidScheduler()
{
    writeConfig(validConfig(4, "invalid"));

    expect(
        !loadConfig(TEST_FILE),
        "Reject invalid scheduler"
    );
}

void testZeroQuantum()
{
    writeConfig(validConfig(4, "rr", 0));

    expect(
        !loadConfig(TEST_FILE),
        "Reject zero quantum-cycles"
    );
}

void testZeroBatchFrequency()
{
    writeConfig(validConfig(4, "fcfs", 5, 0));

    expect(
        !loadConfig(TEST_FILE),
        "Reject zero batch-process-freq"
    );
}

void testZeroMinInstructions()
{
    writeConfig(validConfig(4, "fcfs", 5, 1, 0, 2000));

    expect(
        !loadConfig(TEST_FILE),
        "Reject zero min-ins"
    );
}

void testZeroMaxInstructions()
{
    writeConfig(validConfig(4, "fcfs", 5, 1, 1000, 0));

    expect(
        !loadConfig(TEST_FILE),
        "Reject zero max-ins"
    );
}

void testInvalidInstructionRange()
{
    writeConfig(validConfig(4, "fcfs", 5, 1, 2000, 1000));

    expect(
        !loadConfig(TEST_FILE),
        "Reject min-ins greater than max-ins"
    );
}


// ============================================================
// MALFORMED CONFIGURATION TESTS
// ============================================================

void testMissingParameter()
{
    writeConfig(
        "num-cpu 4\n"
        "scheduler fcfs\n"
        "quantum-cycles 5\n"
        "batch-process-freq 1\n"
        "min-ins 1000\n"
        "max-ins 2000\n"
        // delays-per-exec intentionally missing
    );

    expect(
        !loadConfig(TEST_FILE),
        "Reject missing configuration parameter"
    );
}

void testEmptyConfig()
{
    writeConfig("");

    expect(
        !loadConfig(TEST_FILE),
        "Reject empty configuration file"
    );
}

void testMissingFile()
{
    remove("file_that_does_not_exist.txt");

    expect(
        !loadConfig("file_that_does_not_exist.txt"),
        "Reject missing configuration file"
    );
}

void testUnknownParameter()
{
    writeConfig(
        validConfig() +
        "random-setting 123\n"
    );

    expect(
        !loadConfig(TEST_FILE),
        "Reject unknown configuration parameter"
    );
}

void testNonNumericCPU()
{
    writeConfig(
        "num-cpu hello\n"
        "scheduler fcfs\n"
        "quantum-cycles 5\n"
        "batch-process-freq 1\n"
        "min-ins 1000\n"
        "max-ins 2000\n"
        "delays-per-exec 0\n"
    );

    expect(
        !loadConfig(TEST_FILE),
        "Reject non-numeric num-cpu"
    );
}

void testDuplicateParameter()
{
    writeConfig(
        validConfig() +
        "num-cpu 8\n"
    );

    expect(
        !loadConfig(TEST_FILE),
        "Reject duplicate configuration parameter"
    );
}


// ============================================================
// VALUE VERIFICATION
// ============================================================

void testLoadedValues()
{
    writeConfig(
        validConfig(8, "rr", 10, 3, 500, 1500, 2)
    );

    bool loaded = loadConfig(TEST_FILE);

    bool valuesCorrect =
        loaded &&
        systemConfig.numCpu == 8 &&
        systemConfig.scheduler == "rr" &&
        systemConfig.quantumCycles == 10 &&
        systemConfig.batchProcessFreq == 3 &&
        systemConfig.minIns == 500 &&
        systemConfig.maxIns == 1500 &&
        systemConfig.delaysPerExec == 2;

    expect(
        valuesCorrect,
        "Configuration values are stored correctly"
    );
}


// ============================================================
// MAIN TEST RUNNER
// ============================================================

int main()
{
    cout << "============================================" << endl;
    cout << "       NekOS Configuration Test Suite       " << endl;
    cout << "============================================" << endl << endl;

    cout << "--- Valid Configurations ---" << endl;
    testValidFCFS();
    testValidRR();
    testMinimumCPU();
    testMaximumCPU();
    testZeroDelay();

    cout << endl << "--- Invalid Values ---" << endl;
    testCPUTooLow();
    testCPUTooHigh();
    testInvalidScheduler();
    testZeroQuantum();
    testZeroBatchFrequency();
    testZeroMinInstructions();
    testZeroMaxInstructions();
    testInvalidInstructionRange();

    cout << endl << "--- Malformed Configurations ---" << endl;
    testMissingParameter();
    testEmptyConfig();
    testMissingFile();
    testUnknownParameter();
    testNonNumericCPU();
    testDuplicateParameter();

    cout << endl << "--- Value Verification ---" << endl;
    testLoadedValues();

    cout << endl;
    cout << "============================================" << endl;
    cout << "RESULT: " << passed << "/" << (passed + failed)
         << " tests passed" << endl;

    if (failed == 0) {
        cout << "All configuration tests passed!" << endl;
    }
    else {
        cout << failed << " test(s) failed." << endl;
    }

    cout << "============================================" << endl;

    remove(TEST_FILE.c_str());

    return failed == 0 ? 0 : 1;
}