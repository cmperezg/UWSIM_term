#include "uwsim.h"

UWCPU *cpu;
RAM *ram;
UWassembler *assem;
std::string simfile;
int firstadd;

UWsim::UWsim(std::string filename)
{
    ram = new RAM(3600);
    cpu = new UWCPU(32,32,32,ram,1);
    assem = new UWassembler(cpu->getInstructionDesc());
    simfile = filename;
    firstadd = 100;
}

void UWsim::assembleAndLoad(){
    std::vector<std::tuple<int,std::string>> assembled =  assem->assemble(simfile,firstadd);
    if(assembled.size() == 0){
        std::cout << "UWsim: Error assembling file \n";
    }else{
        for(unsigned int i = 0; i < assembled.size(); i++){
            ram->set(std::get<0>(assembled[i]), std::get<1>(assembled[i]));
        }
    }
}

void UWsim::runSimulation(){
    assembleAndLoad();
    int clock = cpu->executeProgram(firstadd);
    std::cout<<"Number of instructions: " <<clock<<"\n";

}

void UWsim::printFromRAM(int from, int to){
    ram->print_range(from,to);
}

void UWsim::printRegister(char c, int i){
    cpu->printRegister(c,i);
}

UWsim::~UWsim(){
    delete cpu;
    delete ram;
    delete assem;
}

