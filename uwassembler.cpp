#include "uwassembler.h"

int LC;
std::map<std::string,int> labels;
std::map<std::string,std::string>* instructions;

int errcode = 0;
std::string errmessage = "";

UWassembler::UWassembler( std::map<std::string,std::string> *inst)
{

    instructions = inst;
}

std::vector<std::string> UWassembler::tokenize(std::string str, std::string separator){
    std::vector<std::string> ret;
    int index = 0;
    while(index!=-1){
        if(str.size()==0){
            return ret;
        }
        index = str.find(separator);
        if(index==-1){
            ret.push_back(str);
            return ret;
        }else{
            if(index==0){
                str = str.substr(index+separator.size());
                continue;
            }else{
                std::string seg = str.substr(0,index);
                str = str.substr(index+separator.size());
                ret.push_back(seg);
            }
        }
    }
    return ret;
}

std::vector<std::string> UWassembler::fileToList(std::string filename){
    std::ifstream input;
    std::vector<std::string> lines;
    std::string line;

    input.open(filename);
        while(input)
        {
          std::getline(input, line);
          lines.push_back(line);
        }
        input.close();
    return lines;
}

std::string UWassembler::fileToString(std::string filename){
    std::ifstream in(filename);
    std::string s((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());

    return s;
}

// !!!! ERRORS NOT HANDLED YET !!!!
void UWassembler::getDataAndCode(std::string &text,std::string &data, std::string &code){

    int iofdata = 0;
    int iofcode = 0;

    std::string sdata = ".data";
    std::string scode = ".code";

    iofdata = text.find(sdata);
    iofcode = text.find(scode);

    std::cout << "iofdata: " <<iofdata <<"\n";
    std::cout << "iofcode: " <<iofcode <<"\n";

    if(iofcode==-1){
        //error in code.
        std::cout << "ERROR: \n";
        return;
    }

    if(iofdata>iofcode){
        //error in code layout
        std::cout << "ERROR: \n";
        return;
    }

    if(iofdata!=-1){
        int start = iofdata+sdata.length();
        data = text.substr(start,(iofcode-start));
    }else{
        data = "";
    }

    code = text.substr(iofcode+scode.length());

}


std::vector<std::tuple<int,std::string,std::vector<std::string>,bool>> UWassembler::first_pass(std::string &code){
    std::vector<std::tuple<int,std::string,std::vector<std::string>,bool>> res;
    std::map<char,std::regex> paramform;

    paramform['u'] = std::regex("\\$u[0-9]+");
    paramform['r'] = std::regex("\\$r[0-9]+");
    paramform['i'] = std::regex("[0-9]+");
    std::regex label("[a-zA-Z][a-zA-Z0-9]*");

    labels.clear();
    int tindex = -1;
    std::istringstream iss(code);
    for (std::string line; std::getline(iss,line);){
        //std::cout<<"Original line: "<<line<<"\n";
        boost::trim(line);
        if(line.size()==0){//empty line
            continue;
        }

        //HANDLE COMMENTS

        tindex = line.find("#");
        if(tindex!=-1){ //there are comments
            //std::cout<<"Comments found "<<"\n";
            if(tindex == 0){//whole line is a comment, ignore
                //std::cout<<"Line is a comment"<<"\n";
                continue;
            }else{//ignore everything after comment symbol
                line = line.substr(0,line.size()-(line.size()-tindex));
                //std::cout<<"Comments removed: "<<line<<"\n";
            }
        }

        //HANDLE LEADING LABELS
        //labels have to be at the beginning of the line, there can be an instruction after.
        boost::trim(line);

        tindex = line.find(":");
        if(tindex!=-1){//there's a label
            std::vector<std::string> split = tokenize(line,":");
            if(split.size()>2){
                errcode = 1;
                errmessage = line.append(" , too many labels in a line");
                return res;
            }
            if(regex_match(split[0],label)==false){
                errcode = 1;
                errmessage = line.append(" , bad label");
                return res;
            }
            if(instructions->find(split[0])!=instructions->end()){
                errcode = 1;
                errmessage = line.append(" , bad label");
                return res;
            }

            auto insert = labels.insert(std::pair<std::string,int>(split[0],LC));
            if(insert.second==false){
                errcode = 1;
                errmessage = line.append(" , repeated label");
                return res;
            }else{
                //std::cout<<"Label inserted!!\n";
            }

            if(split.size()==1){
                continue;
            }else{
                line = split[1];
            }
        }

        //HANDLE INSTRUCTION
        std::vector<std::string> empty;
        bool labelinparams = false;
        boost::trim(line);
        std::string spaces = " \t";
        tindex = line.find_first_of(spaces);
        if(tindex == -1){ //instruction with no params or bad syntax
           auto it = instructions->find(line);
            if(it != instructions->end()){ //instruction exists
                std::string sec = it->second;
                if(sec.size() !=0){
                    errcode = 1;
                    errmessage = line.append(" , missing parameters");
                    return res;
                }else{//0 parameters
                    //std::tuple<int,std::string,std::vector<std::string>,bool>
                    res.push_back(std::make_tuple(LC,line,empty,false));
                    LC++;
                    continue;
                }
            }else{//instruction doesn't exist
                errcode = 1;
                errmessage = line.append(" , instruction does not exist");
                return res;
            }
        }else{//there is a space!
            std::string first = line.substr(0,tindex);
            //std::cout<<"first: "<<first<<"\n";
            std::string second = line.substr(tindex+1);
            //std::cout<<"second: "<<second<<"\n";

            boost::trim(first);
            auto it = instructions->find(first);
            if(it != instructions->end()){ //instruction exists
                //param descriptor string
                std::string parstr = it->second;
                std::vector<std::string> params = tokenize(second,",");

                if(parstr.size() != params.size()){
                    errcode = 1;
                    errmessage = line.append(" , parameter mismatch");
                    return res;
                }else{// number of parameters match

                    for(unsigned int i = 0; i<parstr.size(); i++){ //will return in any error case, if it passes this stage, params are fine
                        boost::trim(params[i]);
                        if(parstr[i]=='i'){
                            if(regex_match(params[i],paramform[parstr[i]])==false){
                                if(regex_match(params[i],label)==false){
                                    errcode = 1;
                                    errmessage = line.append(" , bad parameter format");
                                    return res;
                                }else{
                                    labelinparams = true;
                                    //std::cout<<"There is a label in the parameters!\n";
                                    continue;
                                }
                            }else continue;
                        }else{
                            if(regex_match(params[i],paramform[parstr[i]])==false){
                                errcode = 1;
                                errmessage = line.append(" , bad parameter format");
                                return res;
                            }else{
                                //remove dollar signs and register type
                                params[i] = params[i].substr(2);
                                continue;
                            }
                        }
                    }// params are fine

                    //std::tuple<int,std::string,std::vector<std::string>,bool>
                    res.push_back(std::make_tuple(LC,first,params,labelinparams));
                    LC++;
                }

            }else{//instruction doesn't exist
                errcode = 1;
                errmessage = line.append(" , instruction does not exist");
                return res;
            }
        }

    }//line finished
    return res;
}

//FOR NOW, ENTIRE DATA DEFINITION MUST BE IN A SINGLE LINE
std::vector<std::tuple<int,std::string>> UWassembler::handleDataSection(std::string &data){

    LC++;
    std::vector<std::tuple<int,std::string>> res; // line number, word data

    if(data.size()==0){
        std::cout<<"No data section\n";
        return res;
    }
    std::istringstream iss(data);
    std::regex label("[a-zA-Z][a-zA-Z0-9]*");
    int tindex; //general purpose index
    for (std::string line; std::getline(iss,line);){
        //std::cout<<"Original line: "<<line<<"\n";
        boost::trim(line);
        //HANDLE COMMENTS

        if(line.size()==0){//empty line
            continue;
        }
        tindex = line.find("#");
        if(tindex!=-1){ //there are comments
            //std::cout<<"Comments found "<<"\n";
            if(tindex == 0){//whole line is a comment, ignore
                continue;
            }else{//ignore everything after comment symbol
                line = line.substr(0,line.size()-(line.size()-tindex));
            }
        }

        tindex = line.find(":");
        if(tindex == -1){
            errcode = 1;
            errmessage = line.append(" , label missing");
            return res;
        }else{//label found
            std::vector<std::string> split = tokenize(line,":");
            if(split.size()>2){
                errcode = 1;
                errmessage = line.append(" , too many labels in a line");
                return res;
            }
            //data definition should be on the same line as the label
            if(split.size() == 1){
                errcode = 1;
                errmessage = line.append(" , data definition should be on the same line as the label");
                return res;
            }
            if(regex_match(split[0],label)==false){
                errcode = 1;
                errmessage = line.append(" , bad label");
                return res;
            }
            if(instructions->find(split[0])!=instructions->end()){
                errcode = 1;
                errmessage = line.append(" , bad label");
                return res;
            }

            auto insert = labels.insert(std::pair<std::string,int>(split[0],LC));
            if(insert.second==false){
                errcode = 1;
                errmessage = line.append(" , repeated label");
                return res;
            }else{//label inserted
                //HANDLE DATA
                line = split[1];
                boost::trim(line);
                std::string spaces = " \t";
                tindex = line.find_first_of(spaces);
                if(tindex == -1){
                    errcode = 1;
                    errmessage = line.append(" , format error");
                    return res;
                }else{
                    std::string first = line.substr(0,tindex);
                    std::string second = line.substr(tindex+1);

                    boost::trim(first);
                    boost::trim(second);


                    if(first==".word"){
                        std::regex number("[0-9]+");
                        std::vector<std::string> nums = tokenize(second,",");
                        for(unsigned int j = 0; j<nums.size(); j++){
                            boost::trim(nums[j]);
                            if(regex_match(nums[j],number)==false){
                                errcode = 1;
                                errmessage = line.append(" , bad data");
                                return res;
                            }
                            res.push_back(std::make_tuple(LC,nums[j]));
                            LC++;
                        }
                    }else if(first==".ascii"){
                        //each word will store a single character
                        std::regex str("\".*\"");
                        boost::trim(second);
                        if(regex_match(second,str)){
                            second.pop_back();
                            second = second.substr(1);
                            for(unsigned int j = 0; j<second.size(); j++){
                                int t = (int)second[j];
                                res.push_back(std::make_tuple(LC,std::to_string(t)));
                                LC++;
                            }
                        }else{
                            errcode = 1;
                            errmessage = line.append(" , bad data format");
                            return res;
                        }

                    }else if(first==".space"){
                        std::regex number("[0-9]+");
                        boost::trim(second);
                        if(regex_match(second,number)){
                            for(int j = 0; j<atoi(second.c_str());j++){
                                res.push_back(std::make_tuple(LC,"0"));
                                LC++;
                            }
                        }else{
                            errcode = 1;
                            errmessage = line.append(" , bad data format");
                            return res;
                        }
                    }else{

                    errcode = 1;
                    errmessage = line.append(" , unknown data type");
                    return res;
                    }
                }
            }
        }
    }//finished all lines

    labels["stack"] = LC+1000;
    return res;
}

std::vector<std::tuple<int,std::string>> UWassembler::second_pass( std::vector<std::tuple<int,std::string,std::vector<std::string>,bool>> &fp){
    std::vector<std::tuple<int,std::string>> res;
    //std::vector<std::tuple<int,std::string,std::vector<std::string>,bool>> fp

    for(unsigned int i = 0; i<fp.size(); i++){
        std::regex label("[a-zA-Z][a-zA-Z0-9]*");
        auto line = fp[i];
        std::vector<std::string> params = std::get<2>(line);
        if(std::get<3>(line) == true){
            std::string nln = std::get<1>(line);
            nln.append(" ");
            for(unsigned int j = 0; j<params.size();j++){
                if(regex_match(params[j],label)){//at this point, param is either a valid label or a valid number
                    if(labels.find(params[j])==labels.end()){//label does not exist
                        errcode = 1;
                        errmessage = params[j].append(" , invalid label");
                        return res;
                    }else{
                        int mapped = labels[params[j]];
                        std::string str = std::to_string(mapped);
                        nln.append(str);
                    }
                }else{
                    nln.append(params[j]);
                }

                nln.append(",");
            }
            nln.pop_back();
            res.push_back(std::make_tuple(std::get<0>(line),nln));

        }else{
            std::string nln = std::get<1>(line);
            nln.append(" ");

            for(unsigned int j = 0; j<params.size();j++){
                nln.append(params[j]);
                nln.append(",");
            }
            //remove trailing comma
            nln.pop_back();

            res.push_back(std::make_tuple(std::get<0>(line),nln));
        }
    }

    return res;
}

std::vector<std::tuple<int,std::string>> UWassembler::assemble(std::string filename, int memstart){

    LC = memstart;
    std::vector<std::tuple<int,std::string>> final;
    //first get file as a string
    std::string text = fileToString(filename);
    //separate code and data sections
    std::string data = " ";
    std::string code = " ";
    getDataAndCode(text,data,code);

    //first pass code section
    std::vector<std::tuple<int,std::string,std::vector<std::string>,bool>> linesfp = first_pass(code);
    if(errcode!=0){
        std::cout<<"ASSEM: "<<errmessage<<"\n";
        return final;
    }else{/*
        std::cout<<"FIRST PASS: \n";
        for(unsigned int i = 0; i<linesfp.size(); i++){
            std::cout << "LINE: " << std::get<0>(linesfp[i]) << "\n";
            std::cout << "INSTRUCTION: " << std::get<1>(linesfp[i]) << "\n";
            std::vector<std::string> params = std::get<2>(linesfp[i]);
            std::cout<< "PARAMS: ";
            for(unsigned int j = 0; j<params.size(); j++){
                std::cout << params[j] <<", ";
            }
            std::cout<<"\n";
            std::cout << "HAS LABEL: " << std::get<3>(linesfp[i]) << "\n";
        }*/
    }
    //handle data section
    //std::vector<std::tuple<int,std::string>>handleDataSection(std::string &data)

    std::vector<std::tuple<int,std::string>> linesds = handleDataSection(data);
    if(errcode!=0){
        std::cout<<"ASSEM: "<<errmessage<<"\n";
        return final;
    }else{
        //print out label content
        for(auto it = labels.cbegin(); it !=labels.cend(); ++it){
            std::cout << it->first << " , " <<it->second << "\n";
        }

    }


    //second pass code section
    std::vector<std::tuple<int,std::string>> linessp = second_pass(linesfp);
    if(errcode!=0){
        std::cout<<"ASSEM: "<<errmessage<<"\n";
        return final;
    }else{
        for(unsigned int i =0; i<linessp.size(); i++){
            std::cout << std::get<0>(linessp[i]) << " : "<< std::get<1>(linessp[i]) << "\n";
        }
    }



    final.reserve(linessp.size() + linesds.size());
    final.insert(final.end(),linessp.begin(),linessp.end());
    final.insert(final.end(),linesds.begin(),linesds.end());

    return final;
}











































