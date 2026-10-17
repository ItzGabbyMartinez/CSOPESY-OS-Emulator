#pragma once

const int NUM_CORES = 4;

void createProcesses();
void printProcessList();

void startscheduler();          // starts the scheduler thread and the core threads
void stopscheduler();           // stops and joins all of them