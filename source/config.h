#pragma once

#include <string>

struct Config {
    int numCpu;
    std::string scheduler;
    unsigned int quantumCycles;
    unsigned int batchProcessFreq;
    unsigned int minIns;
    unsigned int maxIns;
    unsigned int delaysPerExec;
};

extern Config systemConfig;
extern bool isInitialized;

bool loadConfig(const std::string& filename);