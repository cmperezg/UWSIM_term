#ifndef UWASSEMBLER_H
#define UWASSEMBLER_H

#include <map>
#include <string>
#include <vector>
#include <iostream>     // std::cout
#include <fstream>
#include <sstream>
#include <tuple>
#include <regex>
#include <boost/algorithm/string.hpp>



class UWassembler
{
public:
    UWassembler(std::map<std::string, std::string>* inst);
    //public only for testing now
    std::vector<std::string> tokenize(std::string str, std::string separator);
    std::vector<std::string> fileToList(std::string filename);
    std::string fileToString(std::string filename);
    void getDataAndCode(std::string &text, std::string &data, std::string &code);
    std::vector<std::tuple<int,std::string,std::vector<std::string>,bool>> first_pass(std::string &code);
    std::vector<std::tuple<int, std::string> > handleDataSection(std::string &data);
    std::vector<std::tuple<int,std::string>> second_pass( std::vector<std::tuple<int,std::string,std::vector<std::string>,bool>> &fp);
    std::vector<std::tuple<int,std::string>> assemble(std::string filename, int memstart);
};

#endif // UWASSEMBLER_H*
