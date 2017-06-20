#ifndef UWSIM_H
#define UWSIM_H

#include <map>
#include <string>
#include <vector>
#include <iostream>     // std::cout
#include <fstream>
#include <sstream>
#include <tuple>

#include "uwcpu.h"
#include "ram.h"
#include "uwassembler.h"

class UWsim
{
public:
    UWsim(std::string filename);

    void assembleAndLoad();
    void runSimulation();

    void printFromRAM(int from, int to);
    void printRegister(char c, int i);

    ~UWsim();

};

#endif // UWSIM_H
