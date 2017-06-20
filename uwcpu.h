#ifndef UWCPU_H
#define UWCPU_H

#include <vector>
#include <string>
#include <map>
#include <tuple>
#include <limits>
#include <bitset>

#include "ram.h"

class UWCPU
{
public:


    UWCPU(int _wordsize, int _uregs, int _regs, RAM *_ram, int _verbose);

    //for gui (might not happen ever)
    std::vector<std::vector<unsigned int> > *getUREGs();
    std::vector<unsigned int> *getREGs();

    //for assembler
    //map instruction name to a tuple with # of parameters and a vector with a param descriptor
    std::map<std::string,std::string> *getInstructionDesc();

    //instructions
    std::tuple<int,std::string> ADD(int rd, int r1, int r2);
    std::tuple<int,std::string> ADDI(int rd, int r1, int imm);
    std::tuple<int,std::string> ADDU(int rd, int r1, int r2);
    std::tuple<int,std::string> AND(int rd, int r1, int r2);
    std::tuple<int,std::string> ANDI(int rd, int r1, int imm);
    std::tuple<int,std::string> BEQ(int r1, int r2, int add);
    std::tuple<int,std::string> BNE(int r1, int r2, int add);
    std::tuple<int,std::string> J(int add);
    std::tuple<int,std::string> JAL(int add);
    std::tuple<int,std::string> JR(int r1);
    std::tuple<int,std::string> LI(int rd, int imm);

    std::tuple<int,std::string> LUW(int rd, int u1);
    std::tuple<int,std::string> LW(int rd, int r1, int off, RAM *_ram);
    std::tuple<int,std::string> NOT(int rd, int r1);
    std::tuple<int,std::string> OR(int rd, int r1, int r2);
    std::tuple<int,std::string> ORI(int rd, int r1, int imm);
    std::tuple<int,std::string> SL(int rd, int r1, int r2);
    std::tuple<int,std::string> SLI(int rd, int r1, int imm);
    std::tuple<int,std::string> SLT(int rd, int r1, int r2);
    std::tuple<int,std::string> SLTI(int rd, int r1, int imm);
    std::tuple<int,std::string> SR(int rd, int r1, int r2);

    std::tuple<int,std::string> SRI(int rd, int r1, int imm);
    std::tuple<int,std::string> SUB(int rd, int r1, int r2);
    std::tuple<int,std::string> SUBU(int rd, int r1, int r2);
    std::tuple<int,std::string> SW(int r2, int r1, int off, RAM *_ram);
    std::tuple<int,std::string> XOR(int rd, int r1, int r2);
    std::tuple<int,std::string> XORI(int rd, int r1, int imm);

    //UW INSTRUCTIONS
    std::tuple<int,std::string> UWADD(int ud, int u1, int u2);
    std::tuple<int,std::string> UWAND(int ud, int u1, int u2);
    std::tuple<int,std::string> UWBEQ(int u1, int u2, int add);
    std::tuple<int,std::string> UWBNE(int u1, int u2, int add);
    std::tuple<int,std::string> UWCOM(int ud, int u1);
    std::tuple<int,std::string> UWEXP(int ud, int u1);
    std::tuple<int,std::string> UWLB(int ud, int j, int base, RAM *_ram);
    std::tuple<int,std::string> UWLC(int ud, int r1, RAM *_ram);
    std::tuple<int,std::string> UWLRR(int rd, int u1);
    std::tuple<int,std::string> UWLUW(int ud, int r1, RAM *_ram);

    std::tuple<int,std::string> UWNOT(int ud, int u1);
    std::tuple<int,std::string> UWOR(int ud, int u1, int u2);
    std::tuple<int,std::string> UWSB(int ud, int j, int base, RAM *_ram);
    std::tuple<int,std::string> UWSC(int u1, int u2, int base, RAM *_ram);
    std::tuple<int,std::string> UWSL(int ud, int u1, int r1);
    std::tuple<int,std::string> UWSLI(int ud, int u1, int imm);

    std::tuple<int,std::string> UWSPR(int ud, int r1);

    std::tuple<int,std::string> UWSR(int ud, int u1, int imm);
    std::tuple<int,std::string> UWSRI(int ud, int u1, int r1);
    std::tuple<int,std::string> UWSRR(int ud, int r1);
    std::tuple<int,std::string> UWSSQ(int ud, int imm);
    std::tuple<int,std::string> UWSUB(int ud, int u1, int u2);

    std::tuple<int,std::string> UWSUW(int u1, int r1, RAM *_ram);
    std::tuple<int,std::string> UWXOR(int ud, int u1, int u2);


    //EXECUTION FUNCTIONS

    std::vector<std::string> tokenize(std::string str, std::string separator);
    std::tuple<int,std::string> handleInstruction(std::string instructionline);
    int executeProgram(int start);
    void printRegister(char c, int i);


private:
    //CONSTANTS
    int WORD_SIZE; //Ultraword will be WORD_SIZE^2
    int NUM_BLOCKS;
    int NUM_UREGS;
    int NUM_REGS;

    //REGISTERS
    std::vector<std::vector<unsigned int>> UREG;
    std::vector<unsigned int> REG;
    int PC= 0;
    std::string IR;
    int OVF;
    int CAR;
    int CLOCK;

    //OTHER
    std::map<std::string,std::string> INSTRUCTIONS;
    RAM *ramm;
    bool halt;
    int verbose;
    void fillInstructionDescriptions();
    int validr(int r);
    int validur(int ur);

};

#endif // UWCPU_H
