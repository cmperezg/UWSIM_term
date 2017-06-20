#include "ram.h"


int first_ram_add;
int last_ram_add;

RAM::RAM(int _size)
{
    SIZE = _size;

    first_ram_add = 100;
    last_ram_add = first_ram_add + SIZE;

    for(int i=first_ram_add; i<last_ram_add; i++){
        ram.insert(std::make_pair(i,""));
    }
}

//UTILITIES
std::string _intToBits(int x){
    std::bitset<32> _x(x);
    return _x.to_string();
}

std::tuple<int,std::string> RAM::get(int memloc){
    if((memloc>=first_ram_add) & (memloc<=last_ram_add)){
        return std::make_tuple(0,ram.at(memloc));
    }

    std::cout<<"memloc out of range: "<<memloc<<"\n";
    return std::make_tuple(1,"ERROR");
}

int RAM::set(int memloc, std::string content){
    if((memloc>=first_ram_add) & (memloc<=last_ram_add)){
        ram.at(memloc) = content;
        return 0;
    }
    std::cout<<"memloc out of range: "<<memloc<<"\n";
    return 1;
}

void RAM::RAM_info(){
    std::cout << "SIZE: " << SIZE << "\n";

    std::cout << "RAM: " << first_ram_add << " - " << last_ram_add << "\n";


}

void RAM::print_range(int from, int to){
    if(from >= first_ram_add & to <= last_ram_add & from <= to){
        for(int i = from; i<= to; i++){
            std::cout << i << " : " << _intToBits(atoi((ram[i]).c_str()))<<" : " <<ram[i]<< "\n";
        }
    }else{
        std::cout<<"print_range: invalid range provided \n";
    }
}
