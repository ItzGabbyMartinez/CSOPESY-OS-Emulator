#pragma once
#include <string>

// updated to support dynamic core allocation from config.txt
extern int NUM_CORES; 

void createProcesses();
void printProcessList();

void startscheduler();          // starts the scheduler thread and the core threads
void stopscheduler();           // stops and joins all of them

// new function prototypes for batch generation & reporting
void startbatchgenerator();
void stopbatchgenerator();
void generateReportUtil(const std::string& path = "csopesy-log.txt");