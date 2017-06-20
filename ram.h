#ifndef RAM_H
#define RAM_H

#include <iostream>
#include <map>
#include <string>
#include <cmath>
#include <tuple>
#include <bitset>
class RAM
{
 public:
    //number of words
    int SIZE;

    //the RAM (stores strings for rep purposes)
    std::map<int,std::string> ram;

    RAM(int _size);
    std::tuple<int, std::string> get(int memloc);
    int set(int memloc, std::string content);
    void RAM_info();
    void print_range(int from, int to);
};

#endif // RAM_H
