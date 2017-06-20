#include <iostream>
#include <vector>
#include <string>
#include <regex>
#include <boost/algorithm/string.hpp>
#include "uwcpu.h"
#include "ram.h"
#include "uwassembler.h"
#include "uwsim.h"

std::vector<std::vector<int>> *uregs = NULL;

void printvector(std::vector<std::string> &vec){
    for(unsigned int i = 0; i<vec.size(); i++){
        std::cout << vec.at(i) << "\n";
    }
}

int main(int argc, char *argv[])
{


    std::string pack = "C://Users//cmpgt//OneDrive//Documentos//code//uwsim_term//"
                        "packuw.uwasm";


    std::string simplest = "C://Users//cmpgt//OneDrive//Documentos//code//uwsim_term//"
                        "simplest.uwasm";

    std::string subsetsum = "C://Users//cmpgt//OneDrive//Documentos//code//uwsim_term//"
                            "subsetsum.uwasm";
    UWsim sim(subsetsum);
    sim.runSimulation();

   /* sim.printRegister('r',29);
    sim.printRegister('r',31);
    sim.printRegister('u',20);
    std::cout<<"ANS: \n";
    sim.printRegister('r',2);
    sim.printRegister('r',4);
    sim.printRegister('r',10);
    sim.printRegister('r',11);
*/


/*sim.printRegister('r',4);
sim.printRegister('r',29);
sim.printRegister('r',31);



sim.printRegister('r',17);
*/

    int table = 256;

    //sim.printFromRAM(table,table+256-1);



//sim.printRegister('u',1);
    sim.printRegister('r',2);

    return 0;
}


































