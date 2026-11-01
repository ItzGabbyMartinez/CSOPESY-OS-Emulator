#include "config.h"
#include <fstream>
#include <iostream>
#include <set>

using namespace std;

Config systemConfig;
bool isInitialized = false;

bool loadConfig(const string& filename) {
    ifstream file(filename);

    if (!file.is_open()) {
        cout << "Error: Could not open " << filename << "." << endl;
        return false;
    }

    // Store values here first so systemConfig is only changed
    // after the entire configuration has been validated.
    Config tempConfig{};

    string key;
    set<string> foundKeys;

    while (file >> key) {

        // Reject duplicate parameters
        if (foundKeys.count(key) > 0) {
            cout << "Error: Duplicate configuration parameter: "
                 << key << "." << endl;
            return false;
        }

        foundKeys.insert(key);

        if (key == "num-cpu") {
            if (!(file >> tempConfig.numCpu)) {
                cout << "Error: Invalid value for num-cpu." << endl;
                return false;
            }
        }
        else if (key == "scheduler") {
            if (!(file >> tempConfig.scheduler)) {
                cout << "Error: Invalid value for scheduler." << endl;
                return false;
            }
        }
        else if (key == "quantum-cycles") {
            if (!(file >> tempConfig.quantumCycles)) {
                cout << "Error: Invalid value for quantum-cycles." << endl;
                return false;
            }
        }
        else if (key == "batch-process-freq") {
            if (!(file >> tempConfig.batchProcessFreq)) {
                cout << "Error: Invalid value for batch-process-freq." << endl;
                return false;
            }
        }
        else if (key == "min-ins") {
            if (!(file >> tempConfig.minIns)) {
                cout << "Error: Invalid value for min-ins." << endl;
                return false;
            }
        }
        else if (key == "max-ins") {
            if (!(file >> tempConfig.maxIns)) {
                cout << "Error: Invalid value for max-ins." << endl;
                return false;
            }
        }
        else if (key == "delays-per-exec") {
            if (!(file >> tempConfig.delaysPerExec)) {
                cout << "Error: Invalid value for delays-per-exec." << endl;
                return false;
            }
        }
        else {
            cout << "Error: Unknown configuration parameter: "
                 << key << "." << endl;
            return false;
        }
    }

    file.close();

    // All seven configuration parameters are required
    if (foundKeys.size() != 7) {
        cout << "Error: Missing configuration parameter(s)." << endl;
        return false;
    }

    // Validate CPU count
    if (tempConfig.numCpu < 1 || tempConfig.numCpu > 128) {
        cout << "Error: num-cpu must be between 1 and 128." << endl;
        return false;
    }

    // Validate scheduler
    if (tempConfig.scheduler != "fcfs" &&
        tempConfig.scheduler != "rr") {
        cout << "Error: scheduler must be either fcfs or rr." << endl;
        return false;
    }

    // Validate positive configuration values
    if (tempConfig.quantumCycles == 0) {
        cout << "Error: quantum-cycles must be greater than 0." << endl;
        return false;
    }

    if (tempConfig.batchProcessFreq == 0) {
        cout << "Error: batch-process-freq must be greater than 0." << endl;
        return false;
    }

    if (tempConfig.minIns == 0) {
        cout << "Error: min-ins must be greater than 0." << endl;
        return false;
    }

    if (tempConfig.maxIns == 0) {
        cout << "Error: max-ins must be greater than 0." << endl;
        return false;
    }

    // Minimum cannot be greater than maximum
    if (tempConfig.minIns > tempConfig.maxIns) {
        cout << "Error: min-ins cannot be greater than max-ins." << endl;
        return false;
    }

    // Only update the real configuration after everything passes
    systemConfig = tempConfig;

    return true;
}