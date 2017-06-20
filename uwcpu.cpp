#include "uwcpu.h"



void UWCPU::fillInstructionDescriptions(){
    /*
     * u: ultrawide register
     * r: w sized register
     * i: immediate value, offset, address or base. can be replaced by a label.
     */

    INSTRUCTIONS["add"] = "rrr";
    INSTRUCTIONS["addi"] = "rri";
    INSTRUCTIONS["addu"] = "rrr";
    INSTRUCTIONS["and"] = "rrr";
    INSTRUCTIONS["andi"] = "rri";

    INSTRUCTIONS["beq"] = "rri";
    INSTRUCTIONS["bne"] = "rri";

    INSTRUCTIONS["j"] = "i";
    INSTRUCTIONS["jal"] = "i";
    INSTRUCTIONS["jr"] = "r";

    INSTRUCTIONS["li"] = "ri";
    INSTRUCTIONS["luw"] = "ru";
    INSTRUCTIONS["lw"] = "rri";

    INSTRUCTIONS["not"] = "rr";

    INSTRUCTIONS["or"] = "rrr";
    INSTRUCTIONS["ori"] = "rri";

    INSTRUCTIONS["sl"] = "rrr";
    INSTRUCTIONS["sli"] = "rri";
    INSTRUCTIONS["slt"] = "rrr";
    INSTRUCTIONS["slti"] = "rri";
    INSTRUCTIONS["sr"] = "rrr";
    INSTRUCTIONS["sri"] = "rri";
    INSTRUCTIONS["sub"] = "rrr";
    INSTRUCTIONS["subu"] = "rrr";
    INSTRUCTIONS["sw"] = "rri";

    INSTRUCTIONS["xor"] = "rrr";
    INSTRUCTIONS["xori"] = "rri";

    INSTRUCTIONS["uwadd"] = "uuu";
    INSTRUCTIONS["uwand"] = "uuu";

    INSTRUCTIONS["uwbeq"] = "uui";
    INSTRUCTIONS["uwbne"] = "uui";

    INSTRUCTIONS["uwcom"] = "uu";

    INSTRUCTIONS["uwexp"] = "uu";

    INSTRUCTIONS["uwlb"] = "uii";
    INSTRUCTIONS["uwlc"] = "ur";
    INSTRUCTIONS["uwlrr"] = "ur";
    INSTRUCTIONS["uwluw"] = "ur";

    INSTRUCTIONS["uwnot"] = "uu";

    INSTRUCTIONS["uwor"] = "uuu";

    INSTRUCTIONS["uwsb"] = "uii";
    INSTRUCTIONS["uwsc"] = "uui";
    INSTRUCTIONS["uwsl"] = "uur";
    INSTRUCTIONS["uwsli"] = "uui";
    INSTRUCTIONS["uwspr"] = "ur";
    INSTRUCTIONS["uwsr"] = "uur";
    INSTRUCTIONS["uwsri"] = "uui";
    INSTRUCTIONS["uwsrr"] = "ru";
    INSTRUCTIONS["uwssq"] = "ui";
    INSTRUCTIONS["uwsub"] = "uuu";
    INSTRUCTIONS["uwsuw"] = "ur";
    INSTRUCTIONS["uwxor"] = "uuu";

    INSTRUCTIONS["halt"] = "";
}


//verbose = 0 prints nothing
//verbose = 1 prints instructions
//verbose = 2 prints changes
UWCPU::UWCPU(int _wordsize, int _uregs, int _regs, RAM *_ram, int _verbose){

    CLOCK = 0;
    halt = false;
    verbose = _verbose;
    WORD_SIZE = _wordsize;
    NUM_BLOCKS = _wordsize;//Ultraword will be WORD_SIZE^2
    NUM_UREGS = _uregs;
    NUM_REGS = _regs;

    ramm = _ram;

    fillInstructionDescriptions();

    UREG.resize(NUM_UREGS,std::vector<unsigned int>(WORD_SIZE,0));

       for(int i =0; i<NUM_UREGS; i++){
           for(int j = 0; j<WORD_SIZE; j++){
               UREG[i][j] = 0;
           }
       }

    for(int i = 0; i<NUM_REGS; i++){
        REG.push_back(0);
    }
}

std::map<std::string, std::string> *UWCPU::getInstructionDesc(){
    return &INSTRUCTIONS;
}

std::vector<std::vector<unsigned int>>* UWCPU::getUREGs(){
    return &UREG;
}

std::vector<unsigned int>* UWCPU::getREGs(){
    return &REG;
}

//1 if register number is valid
int UWCPU::validr(int r){
    if((r>=0) & (r<NUM_REGS)){
        return 1;
    }
    return 0;
}

int UWCPU::validur(int ur){
    if((ur>=0) & (ur<NUM_UREGS)){
        return 1;
    }
    return 0;
}

//UTILITIES
std::string intToBits(int x){
    std::bitset<32> _x(x);
    return _x.to_string();
}

//BASIC REGISTER INSTRUCTIONS
//tuple<error,msg> error = 1 if error occurrs

//ADD $rd, $r1, $r2 : $rd = $r1 + $r2;
std::tuple<int,std::string> UWCPU::ADD(int rd, int r1, int r2){
    std::string msg = "";
    if((validr(rd)==0) | (validr(r1)==0) | (validr(r2)==0)){
        msg = "(ADD): Invalid register(s)";
        return std::make_tuple(1,msg);
    }

    //overflow
    if(((REG[r2]>0) & (REG[r1] > (std::numeric_limits<float>::max()-REG[r2]))) |
          ((REG[r1]>0) & (REG[r2] > (std::numeric_limits<float>::max()-REG[r1])))){
        OVF = 1;
        msg = "(ADD): Overflow detected";
    }

    //underflow
    if(((REG[r2]<0) & (REG[r1] < (std::numeric_limits<float>::min()-REG[r2]))) |
          ((REG[r1]<0) & (REG[r2] < (std::numeric_limits<float>::max()-REG[r1])))){
        OVF = 1;
        msg = "(ADD): Underflow detected";
    }


    REG[rd] = REG[r1] + REG[r2];

    if(verbose==2){
        printRegister('r',rd);
    }

    return std::make_tuple(0,msg);
}

//ADDI $rd, $r1, imm : $rd = $r1 + imm;
std::tuple<int,std::string> UWCPU::ADDI(int rd, int r1, int imm){
    std::string msg = "";
    if((validr(rd)==0) | (validr(r1)==0)){
        msg = "(ADDI): Invalid register(s)";
        return std::make_tuple(1,msg);
    }

    //overflow
    if(((imm>0) & (REG[r1] > (std::numeric_limits<float>::max()-imm))) |
          ((REG[r1]>0) & (imm > (std::numeric_limits<float>::max()-REG[r1])))){
        OVF = 1;
        msg = "(ADDI): Overflow detected";
    }

    //underflow
    if(((imm<0) & (REG[r1] < (std::numeric_limits<float>::min()-imm))) |
          ((REG[r1]<0) & (imm < (std::numeric_limits<float>::max()-REG[r1])))){
        OVF = 1;
        msg = "(ADDI): Underflow detected";
    }
    REG[rd] = REG[r1] + imm;


    if(verbose==2){
        printRegister('r',rd);
    }
    return std::make_tuple(0,msg);
}

//ADDU $rd, $r1, $r2 : $rd = $r1 + $r2; (unsigned, no overflow)
std::tuple<int,std::string> UWCPU::ADDU(int rd, int r1, int r2){
    std::string msg = "";

    if((validr(rd)==0) | (validr(r1)==0) | (validr(r2)==0)){
        msg = "(ADDU): Invalid register(s)";
        return std::make_tuple(1,msg);
    }


    REG[rd] = (unsigned int)REG[r1] + (unsigned int)REG[r2];

    if(verbose==2){
        printRegister('r',rd);
    }

    return std::make_tuple(0,msg);
}

//AND $rd, $r1, $r2 : $rd = $r1 & $r2;
std::tuple<int,std::string> UWCPU::AND(int rd, int r1, int r2){
    std::string msg = "";
    if((validr(rd)==0) | (validr(r1)==0) | (validr(r2)==0)){
        msg = "(AND): Invalid register(s)";
        return std::make_tuple(1,msg);
    }

    REG[rd] = REG[r1] & REG[r2];

    if(verbose==2){
        printRegister('r',rd);
    }

    return std::make_tuple(0,msg);
}

//ANDI $rd, $r1, imm : $rd = $r1 & imm;
std::tuple<int,std::string> UWCPU::ANDI(int rd, int r1, int imm){
    std::string msg = "";
    if((validr(rd)==0) | (validr(r1)==0)){
        msg = "(ANDI): Invalid register(s)";
        return std::make_tuple(1,msg);
    }

    REG[rd] = REG[r1] & imm;

    if(verbose==2){
        printRegister('r',rd);
    }

    return std::make_tuple(0,msg);
}

//BEQ $r1, $r2, off : if($r1 == $r2) then branch to off;
std::tuple<int,std::string> UWCPU::BEQ(int r1, int r2, int add){
    std::string msg = "";
    if((validr(r1)==0) | (validr(r2)==0)){
        msg = "(BEQ): Invalid register(s)";
        return std::make_tuple(1,msg);
    }

    if(REG[r1] == REG[r2]){
        PC = add;
    }

    if(verbose==2){
        std::cout<<"PC: "<<PC<<"\n";
    }

    return std::make_tuple(0,msg);
}


//BNE $r1, $r2, off : if($r1 != $r2) then branch to off;
std::tuple<int,std::string> UWCPU::BNE(int r1, int r2, int add){
    std::string msg = "";
    if((validr(r1)==0) | (validr(r2)==0)){
        msg = "(BNE): Invalid register(s)";
        return std::make_tuple(1,msg);
    }

    if(REG[r1] != REG[r2]){
        PC = add;
    }

    if(verbose==2){
        std::cout<<"PC: "<<add<<"\n";
    }

    return std::make_tuple(0,msg);
}

//J add: PC = add
std::tuple<int,std::string> UWCPU::J(int add){
    std::string msg = "";

    PC = add;

    if(verbose==2){
        std::cout<<"PC: "<<add<<"\n";
    }

    return std::make_tuple(0,msg);
}

//JAL add: $r29 = PC, PC = add.
std::tuple<int,std::string> UWCPU::JAL(int add){
    std::string msg = "";
    //$r31 = $ra
    REG[31] = PC;
    PC = add;

    if(verbose==2){
        std::cout<<"PC: "<<add<<"\n";
        printRegister('r',31);
    }

    return std::make_tuple(0,msg);
}

//JR add: PC = $r1
std::tuple<int,std::string> UWCPU::JR(int r1){
    std::string msg = "";
    if(validr(r1)==0){
        msg = "(JR): Invalid register(s)";
        return std::make_tuple(1,msg);
    }
    PC = REG[r1];
    if(verbose==2){
        std::cout<<"PC: "<<REG[r1]<<"\n";
    }

    return std::make_tuple(0,msg);
}

//LI $rd, imm: $rd = imm
std::tuple<int,std::string> UWCPU::LI(int rd, int imm){
    std::string msg = "";
    if(validr(rd)==0){
        msg = "(LI): Invalid register(s)";
        return std::make_tuple(1,msg);
    }

    REG[rd] = imm;

    if(verbose==2){
        printRegister('r',rd);
    }
    return std::make_tuple(0,msg);
}

//LUW $rd, $u1: $rd = $u1[0]
std::tuple<int,std::string> UWCPU::LUW(int rd, int u1){
    std::string msg = "";
    if(validr(rd)==0){
        msg = "(LUW): Invalid register(s)";
        return std::make_tuple(1,msg);
    }
    if(validur(u1)==0){
        msg = "(LUW): Invalid register(s)";
        return std::make_tuple(1,msg);
    }

    REG[rd] = UREG[u1][0];

    if(verbose==2){
        printRegister('r',rd);
    }

    return std::make_tuple(0,msg);
}

//LW $rd, $r1, off :   $rd = MEM[$r1 + off];
std::tuple<int,std::string> UWCPU::LW(int rd, int r1, int off, RAM *_ram){
    std::string msg = "";
    if((validr(rd)==0) | (validr(r1)==0)){
        msg = "(LW): Invalid register(s)";
        return std::make_tuple(1,msg);
    }
    auto res = _ram->get(REG[r1]+off);

    int i = std::get<0>(res);
    if(i==0){
        REG[rd] = atoi((std::get<1>(res)).c_str());

        if(verbose==2){
            printRegister('r',rd);
        }

        return std::make_tuple(0,msg);
    }


    return std::make_tuple(1,msg);
}

//NOT $rd,$r1: $rd = ~$r1
std::tuple<int,std::string> UWCPU::NOT(int rd, int r1){
    std::string msg = "";
    if((validr(rd)==0) | (validr(r1)==0)){
        msg = "(UWNOT): Invalid register(s)";
        return std::make_tuple(1,msg);
    }

    REG[rd] = ~(REG[r1]);

    if(verbose==2){
        printRegister('r',rd);
    }

    return std::make_tuple(0,msg);
}

//OR $rd, $r1, $r2 : $rd = $r1 | $r2;
std::tuple<int,std::string> UWCPU::OR(int rd, int r1, int r2){
    std::string msg = "";
    if((validr(rd)==0) | (validr(r1)==0) | (validr(r2)==0)){
        msg = "(OR): Invalid register(s)";
        return std::make_tuple(1,msg);
    }

    REG[rd] = REG[r1] | REG[r2];

    if(verbose==2){
        printRegister('r',rd);
    }

    return std::make_tuple(0,msg);
}

//ORI $rd, $r1, imm : $rd = $r1 | imm;
std::tuple<int,std::string> UWCPU::ORI(int rd, int r1, int imm){
    std::string msg = "";
    if((validr(rd)==0) | (validr(r1)==0)){
        msg = "(ORI): Invalid register(s)";
        return std::make_tuple(1,msg);
    }

    REG[rd] = REG[r1] | imm;

    if(verbose==2){
        printRegister('r',rd);
    }

    return std::make_tuple(0,msg);
}


//SL $rd, $r1, $r2 : $rd = $r1 << $r2;
std::tuple<int,std::string> UWCPU::SL(int rd, int r1, int r2){
    std::string msg = "";
    if((validr(rd)==0) | (validr(r1)==0) | (validr(r2)==0)){
        msg = "(SL): Invalid register(s)";
        return std::make_tuple(1,msg);
    }

    REG[rd] = REG[r1] << REG[r2];

    if(verbose==2){
        printRegister('r',rd);
    }

    return std::make_tuple(0,msg);
}

//SLI $rd, $r1, imm : $rd = $r1 << imm;
std::tuple<int,std::string> UWCPU::SLI(int rd, int r1, int imm){
    std::string msg = "";
    if((validr(rd)==0) | (validr(r1)==0)){
        msg = "(SLI): Invalid register(s)";
        return std::make_tuple(1,msg);
    }
    if(imm >= WORD_SIZE){
        msg = "(SLI): Shift larger than WORD_SIZE is invalid";
        return std::make_tuple(1,msg);
    }

    REG[rd] = REG[r1] << imm;

    if(verbose==2){
        printRegister('r',rd);
    }

    return std::make_tuple(0,msg);
}

//SLT $rd, $r1, $r2 : $rd = ($r1 < $r2);
std::tuple<int,std::string> UWCPU::SLT(int rd, int r1, int r2){
    std::string msg = "";
    if((validr(rd)==0) | (validr(r1)==0) | (validr(r2)==0)){
        msg = "(SLT): Invalid register(s)";
        return std::make_tuple(1,msg);
    }
    if(REG[r1] < REG[r2]){
        REG[rd] = 1;
    }else{
        REG[rd] = 0;
    }

    if(verbose==2){
        printRegister('r',rd);
    }

    return std::make_tuple(0,msg);
}

//SLTI $rd, $r1, imm : $rd = ($r1 < imm);
std::tuple<int,std::string> UWCPU::SLTI(int rd, int r1, int imm){
    std::string msg = "";
    if((validr(rd)==0) | (validr(r1)==0)){
        msg = "(SLTI): Invalid register(s)";
        return std::make_tuple(1,msg);
    }
    if(REG[r1] < imm){
        REG[rd] = 1;
    }else{
        REG[rd] = 0;
    }

    if(verbose==2){
        printRegister('r',rd);
    }

    return std::make_tuple(0,msg);
}

//SRV $rd, $r1, $r2 : $rd = $r1 >> $r2;
std::tuple<int,std::string> UWCPU::SR(int rd, int r1, int r2){
    std::string msg = "";
    if((validr(rd)==0) | (validr(r1)==0) | (validr(r2)==0)){
        msg = "(SR): Invalid register(s)";
        return std::make_tuple(1,msg);
    }

    REG[rd] = REG[r1] >> REG[r2];

    if(verbose==2){
        printRegister('r',rd);
    }

    return std::make_tuple(0,msg);
}

//SRI $rd, $r1, imm : $rd = $r1 >> imm;
std::tuple<int,std::string> UWCPU::SRI(int rd, int r1, int imm){
    std::string msg = "";
    if((validr(rd)==0) | (validr(r1)==0)){
        msg = "(SRI): Invalid register(s)";
        return std::make_tuple(1,msg);
    }
    if(imm >= WORD_SIZE){
        msg = "(SRI): Shift larger than WORD_SIZE is invalid";
        return std::make_tuple(1,msg);
    }

    REG[rd] = REG[r1] >> imm;

    if(verbose==2){
        printRegister('r',rd);
    }

    return std::make_tuple(0,msg);
}

//SUB $rd, $r1, $r2 : $rd = $r1 - $r2;
std::tuple<int,std::string> UWCPU::SUB(int rd, int r1, int r2){
    std::string msg = "";
    if((validr(rd)==0) | (validr(r1)==0) | (validr(r2)==0)){
        msg = "(SUB): Invalid register(s)";
        return std::make_tuple(1,msg);
    }

    //overflow
    if(((REG[r2]<0) & (REG[r1] > (std::numeric_limits<float>::max()+REG[r2]))) |
          ((REG[r1]<0) & (REG[r2] > (std::numeric_limits<float>::max()+REG[r1])))){
        OVF = 1;
        msg = "(SUB): Overflow detected";
    }

    //underflow
    if(((REG[r2]>0) & (REG[r1] < (std::numeric_limits<float>::min()+REG[r2]))) |
          ((REG[r1]>0) & (REG[r2] < (std::numeric_limits<float>::min()+REG[r1])))){
        OVF = 1;
        msg = "(SUB): Underflow detected";
    }


    REG[rd] = REG[r1] - REG[r2];

    if(verbose==2){
        printRegister('r',rd);
    }

    return std::make_tuple(0,msg);
}


//SUBU $rd, $r1, $r2 : $rd = $r1 - $r2; (unsigned, no overflow)
std::tuple<int,std::string> UWCPU::SUBU(int rd, int r1, int r2){
    std::string msg = "";

    if((validr(rd)==0) | (validr(r1)==0) | (validr(r2)==0)){
        msg = "(SUBU): Invalid register(s)";
        return std::make_tuple(1,msg);
    }


    REG[rd] = REG[r1] - REG[r2];

    if(verbose==2){
        printRegister('r',rd);
    }

    return std::make_tuple(0,msg);
}

//SW $r1, $r2, off : MEM[$r2 + off] = $r1;
std::tuple<int,std::string> UWCPU::SW(int r1, int r2, int off, RAM *_ram){
    std::string msg = "";
    if((validr(r1)==0) | (validr(r2)==0)){
        msg = "(SW): Invalid register(s)";
        return std::make_tuple(1,msg);
    }

    int i = _ram->set(REG[r2]+off, std::to_string(REG[r1]));

    if(verbose==2){
        std::cout<<"RAM["<<REG[r2]+off<<"]: ";
        printRegister('r',r1);
    }

    return std::make_tuple(i,msg);
}

//XOR $rd, $r1, $r2 : $rd = $r1 ^ $r2;
std::tuple<int,std::string> UWCPU::XOR(int rd, int r1, int r2){
    std::string msg = "";
    if((validr(rd)==0) | (validr(r1)==0) | (validr(r2)==0)){
        msg = "(XOR): Invalid register(s)";
        return std::make_tuple(1,msg);
    }

    REG[rd] = REG[r1] ^ REG[r2];

    if(verbose==2){
        printRegister('r',rd);
    }

    return std::make_tuple(0,msg);
}

//XORI $rd, $r1, imm : $rd = $r1 ^ imm;
std::tuple<int,std::string> UWCPU::XORI(int rd, int r1, int imm){
    std::string msg = "";
    if((validr(rd)==0) | (validr(r1)==0)){
        msg = "(ORI): Invalid register(s)";
        return std::make_tuple(1,msg);
    }

    REG[rd] = REG[r1] ^ imm;

    if(verbose==2){
        printRegister('r',rd);
    }

    return std::make_tuple(0,msg);
}



//UW REGISTER INSTRUCTIONS
//tuple<error,msg> error = 1 if error occurrs
//No UW instruction considers overflow or carry

//UWADD $ud, $u1, $rs : $rd = $r1 + $rs;
std::tuple<int,std::string> UWCPU::UWADD(int ud, int u1, int u2){
    std::string msg = "";
    if((validur(ud)==0) | (validur(u1)==0) | (validur(u2)==0)){
        msg = "(UWADD): Invalid register(s)";
        return std::make_tuple(1,msg);
    }
    int carry = 0;
    for(int i = NUM_BLOCKS-1; i>=0; i--){
        //UREG[ud][i] = UREG[u1][i] + UREG[u2][i];

        int tempa = UREG[u1][i];
        int tempb = UREG[u2][i];
        std::string blockres(WORD_SIZE,'0');


        for(int j=WORD_SIZE-1; j>=0; j--){
            int a = tempa&1;
            int b = tempb&1;
            int c = a^b;
            c = c^carry;
            int tempcarry = a&b;
            if(tempcarry==0){
                tempcarry = c&carry;
            }
            carry = tempcarry;
            tempa = tempa>>1;
            tempb = tempb>>1;
            if(c==1){
               blockres.at(j) = '1';
            }

        }

        UREG[ud][i] = unsigned(std::bitset<32>(blockres).to_ulong());
    }

    if(verbose==2){
        printRegister('u',ud);
    }
    return std::make_tuple(0,msg);
}

//UWAND $ud, $u1, $rs : $ud = $u1 & $us;
std::tuple<int,std::string> UWCPU::UWAND(int ud, int u1, int u2){
    std::string msg = "";
    if((validur(ud)==0) | (validur(u1)==0) | (validur(u2)==0)){
        msg = "(UWAND): Invalid register(s)";
        return std::make_tuple(1,msg);
    }

    for(int i = NUM_BLOCKS-1; i>=0; i--){
        UREG[ud][i] = UREG[u1][i] & UREG[u2][i];
    }

    if(verbose==2){
        printRegister('u',ud);
    }
    return std::make_tuple(0,msg);
}

//UWBEQ $u1, $u2, off : if($r1 == $r2) then branch to off;
std::tuple<int,std::string> UWCPU::UWBEQ(int u1, int u2, int add){
    std::string msg = "";
    if((validur(u1)==0) | (validur(u2)==0)){
        msg = "(UWBEQ): Invalid register(s)";
        return std::make_tuple(1,msg);
    }
    if(add==0){
        msg = "(UWBEQ): Offset on branch cannot be zero";
        return std::make_tuple(1,msg);
    }
    bool allequal = false;
    for(int i =0; i<NUM_BLOCKS; i++){
        if(UREG[u1][i] == UREG[u2][i]){
            allequal = true;
        }else{
            allequal = false;
            break;
        }
    }

    if(allequal){
        PC = add; //-1 since PC is updated before instruction is executed
    }

    if(verbose==2){
        std::cout<<"PC: "<<add<<"\n";
    }

    return std::make_tuple(0,msg);
}


//UWBNE $u1, $u2, off : if($u1 != $u2) then branch to off;
std::tuple<int,std::string> UWCPU::UWBNE(int u1, int u2, int add){
    std::string msg = "";
    if((validur(u1)==0) | (validur(u2)==0)){
        msg = "(UWBNE): Invalid register(s)";
        return std::make_tuple(1,msg);
    }
    if(add==0){
        msg = "(UWBNE): Offset on branch cannot be zero";
        return std::make_tuple(1,msg);
    }
    bool allequal = false;
    for(int i =0; i<NUM_BLOCKS; i++){
        if(UREG[u1][i] == UREG[u2][i]){
            allequal = true;
        }else{
            allequal = false;
            break;
        }
    }

    if(!allequal){
        PC = add; //-1 since PC is updated before instruction is executed
    }

    if(verbose==2){
        std::cout<<"PC: "<<add<<"\n";
    }

    return std::make_tuple(0,msg);
}


//UWCOM $ud, $u1 : packs first bit of each block of $u1 into the first block of $ud
std::tuple<int,std::string> UWCPU::UWCOM(int ud, int u1){
    std::string msg = "";
    if((validur(ud)==0) | (validr(u1)==0)){
        msg = "(UWCOM): Invalid register(s)";
        return std::make_tuple(1,msg);
    }
    if(ud==u1){
        msg = "(UWCOM): Cannot compress onto same register";
        return std::make_tuple(1,msg);
    }
    unsigned int firstmask = 1<<(WORD_SIZE-1);
    for(int i=0; i<NUM_BLOCKS; i++){
        unsigned int maskedn = UREG[u1][i]&firstmask;
        unsigned int thebit = maskedn >> (WORD_SIZE-1);
        UREG[ud][0] = UREG[ud][0] << 1;
        UREG[ud][0] = UREG[ud][0] | thebit;
    }

    if(verbose==2){
        printRegister('u',ud);
    }

    return std::make_tuple(0,msg);

}

//UWEXP $ud, $u1 : spreads first block of $u1 into the first bit of each block of $ud
std::tuple<int,std::string> UWCPU::UWEXP(int ud, int u1){
    std::string msg = "";
    if((validur(ud)==0) | (validr(u1)==0)){
        msg = "(UWEXP): Invalid register(s)";
        return std::make_tuple(1,msg);
    }
    if(ud==u1){
        msg = "(UWEXP): Cannot expand onto same register";
        return std::make_tuple(1,msg);
    }
    unsigned int shiftcount = 0;
    for(int i=NUM_BLOCKS-1; i>=0; i--){
        unsigned int mask = (unsigned int)1<<shiftcount;
        unsigned int maskedn = UREG[u1][0]&mask;
        unsigned int thebit = maskedn >> shiftcount;
        UREG[ud][i] = thebit<<(WORD_SIZE-1);
    }

    if(verbose==2){
        printRegister('u',ud);
    }

    return std::make_tuple(0,msg);

}

//UWLB $ud,j,base:  $ud[j] = MEM[base+j]
std::tuple<int,std::string> UWCPU::UWLB(int ud, int j, int base, RAM *_ram){
    std::string msg = "";
    if(validur(ud)==0){
        msg = "(UWLB): Invalid register(s)";
        return std::make_tuple(1,msg);
    }
    if((j<0) | (j>= NUM_BLOCKS)){
       msg =  "(UWLB): Invalid block index";
       return std::make_tuple(1,msg);
    }

    auto res = _ram->get(base+j);
    int i = std::get<0>(res);

    if(i==0){
        UREG[ud][j] = atoi((std::get<1>(res)).c_str());
        return std::make_tuple(0,msg);
    }

    if(verbose==2){
        printRegister('u',ud);
    }

    return std::make_tuple(1,msg);

}

//UWLC $ud,base $ud[j] = MEM[base+$ud[j]] (for each j in NUM_BLOCKS in parallel)
std::tuple<int,std::string> UWCPU::UWLC(int ud, int r1, RAM *_ram){
    std::string msg = "";
    if(validur(ud)==0){
        msg = "(UWLC): Invalid register(s)";
        return std::make_tuple(1,msg);
    }
    if(validr(r1)==0){
        msg = "(UWLC): Invalid register(s)";
        return std::make_tuple(1,msg);
    }

    for(int i = 0; i<NUM_BLOCKS; i++){

        auto res = _ram->get(REG[r1]+UREG[ud][i]);
        if(std::get<0>(res)==0){
            UREG[ud][i] = atoi((std::get<1>(res)).c_str());
        }else{
            return std::make_tuple(1,msg);
        }

    }

    if(verbose==2){
        printRegister('u',ud);
    }

    return std::make_tuple(0,msg);
}

//UWLRR  $ud, $r1: $ud[0] = $r1
std::tuple<int,std::string> UWCPU::UWLRR(int ud, int r1){
    std::string msg = "";
    if(validur(ud)==0){
        msg = "(UWLR): Invalid register(s)";
        return std::make_tuple(1,msg);
    }
    if(validr(r1)==0){
        msg = "(UWLR): Invalid register(s)";
        return std::make_tuple(1,msg);
    }

    UREG[ud][0] = REG[r1];

    if(verbose==2){
        printRegister('u',ud);
    }
    return std::make_tuple(0,msg);
}

//UWLUW $ud,base: $ud,base:  $ud[j] = MEM[base+j] (for each j in NUM_BLOCKS in parallel)
std::tuple<int,std::string> UWCPU::UWLUW(int ud, int r1, RAM *_ram){
    std::string msg = "";
    if(validur(ud)==0){
        msg = "(UWLUW): Invalid register(s)";
        return std::make_tuple(1,msg);
    }if(validr(r1)==0){
        msg = "(UWLUW): Invalid register(s)";
        return std::make_tuple(1,msg);
    }
    int base = REG[r1];

    for(int i = 0; i<NUM_BLOCKS; i++){

        auto res = _ram->get(base+i);
        if(std::get<0>(res)==0){
            UREG[ud][i] = atoi((std::get<1>(res)).c_str());
        }else{
            return std::make_tuple(1,msg);
        }

    }

    if(verbose==2){
        printRegister('u',ud);
    }



    return std::make_tuple(0,msg);
}

//UWNOT $ud,$u1: $ud = ~$u1
std::tuple<int,std::string> UWCPU::UWNOT(int ud, int u1){
    std::string msg = "";
    if((validur(ud)==0) | (validur(u1)==0)){
        msg = "(UWNOT): Invalid register(s)";
        return std::make_tuple(1,msg);
    }

    for(int i = NUM_BLOCKS-1; i>=0; i--){
        UREG[ud][i] = ~(UREG[u1][i]);
    }

    if(verbose==2){
        printRegister('u',ud);
    }

    return std::make_tuple(0,msg);
}

//UWOR $ud, $u1, $rs : $ud = $u1 | $u2;
std::tuple<int,std::string> UWCPU::UWOR(int ud, int u1, int u2){
    std::string msg = "";
    if((validur(ud)==0) | (validur(u1)==0) | (validur(u2)==0)){
        msg = "(UWOR): Invalid register(s)";
        return std::make_tuple(1,msg);
    }

    for(int i = NUM_BLOCKS-1; i>=0; i--){
        UREG[ud][i] = UREG[u1][i] | UREG[u2][i];
    }

    if(verbose==2){
        printRegister('u',ud);
    }

    return std::make_tuple(0,msg);
}

//UWSB $ud,j,base:  MEM[base+j] = $ud[j]
std::tuple<int,std::string> UWCPU::UWSB(int u1, int j, int base, RAM *_ram){
    std::string msg = "";
    if(validur(u1)==0){
        msg = "(UWSB): Invalid register(s)";
        return std::make_tuple(1,msg);
    }
    if((j<0) | (j>= NUM_BLOCKS)){
       msg =  "(UWSB): Invalid block index";
       return std::make_tuple(1,msg);
    }

    int i =_ram->set(base+j, std::to_string(UREG[u1][j]));

    if(verbose==2){
        std::cout<<"RAM["<<base+j<<"]: ";
        printRegister('u',1);
    }

    return std::make_tuple(i,msg);
}

//UWSC $ud,$u1,base:  MEM[base+$u2[j]] = $u1[j] (for all j in NUM_BLOCKS in parallel)
std::tuple<int,std::string> UWCPU::UWSC(int u1, int u2, int base, RAM *_ram){
    std::string msg = "";
    if((validur(u1)==0) | (validur(u2)==0)){
        msg = "(UWSC): Invalid register(s)";
        return std::make_tuple(1,msg);
    }

    for(int i =0; i<NUM_BLOCKS; i++){
        int k = _ram->set(base+UREG[u2][i], std::to_string(UREG[u1][i]));
        if(k!=0){
            return std::make_tuple(1,msg);
        }
    }

    if(verbose==2){
        std::cout<<"base: "<<base<<"\n";
        printRegister('u',1);
        printRegister('u',2);
    }

    return std::make_tuple(0,msg);
}

//UWSL $ud, $u1, $r1 : $ud = $u1 << $r1;
std::tuple<int,std::string> UWCPU::UWSL(int ud, int u1, int r1){
    std::string msg = "";
    if((validur(ud)==0) | (validur(u1)==0)){
        msg = "(UWSL): Invalid register(s)";
        return std::make_tuple(1,msg);
    }
    if(validr(r1)==0){
        msg = "(UWSL): Invalid register(s)";
        return std::make_tuple(1,msg);
    }

    int imm = REG[r1];

    int bts = floor((float)imm/float(WORD_SIZE));
    int split = imm%WORD_SIZE;
    unsigned int aux1 = 0;
    unsigned int aux2 = 0;
    int i = bts;
    int j = 0;

    std::vector<int> vec(UREG[u1].size());
    for(int k = 0; k<vec.size(); k++){
        vec[k] = 0;
    }

    while(i<NUM_BLOCKS){
        aux1 = 0;
        aux1 = UREG[u1][i]<<split;
        if(((i+1)<NUM_BLOCKS)&&(imm!=WORD_SIZE)){
            aux2 = UREG[u1][i+1]>>(WORD_SIZE-split);
        }else{
            aux2 = 0;
        }
         vec[j]= aux1|aux2;
        j++;i++;
    }

    for(int k = 0; k<vec.size(); k++){
        UREG[ud][k] = vec[k];
    }

    if(verbose==2){
        printRegister('u',ud);
    }

    return std::make_tuple(0,msg);
}

//UWSLI $ud, $u1, imm : $ud = $u1 << imm;
std::tuple<int,std::string> UWCPU::UWSLI(int ud, int u1, int imm){
    std::string msg = "";
    if((validur(ud)==0) | (validur(u1)==0)){
        msg = "(UWSLI): Invalid register(s)";
        return std::make_tuple(1,msg);
    }
    if(imm >= (WORD_SIZE*NUM_BLOCKS)){
        msg = "(UWSLI): Shift larger than register size is invalid";
        return std::make_tuple(1,msg);
    }

    int bts = floor((float)imm/float(WORD_SIZE));
    int split = imm%WORD_SIZE;
    unsigned int aux1 = 0;
    unsigned int aux2 = 0;
    int i = bts;
    int j = 0;

    while(i<NUM_BLOCKS){
        aux1 = 0;
        aux1 = UREG[u1][i]<<split;
        if(((i+1)<NUM_BLOCKS)&&(imm!=WORD_SIZE)){
            aux2 = UREG[u1][i]>>(WORD_SIZE-split);
        }else{
            aux2 = 0;
        }
        UREG[ud][j] = aux1|aux2;
        j++;i++;
    }

    if(verbose==2){
        printRegister('u',ud);
    }

    return std::make_tuple(0,msg);
}

//UWSPR $ud, $u1 : $ud[i] = $u1 for all i
std::tuple<int,std::string> UWCPU::UWSPR(int ud, int r1){
    std::string msg = "";
    if(validur(ud)==0){
        msg = "(UWSPR): Invalid register(s)";
        return std::make_tuple(1,msg);
    }
    if(validr(r1)==0){
        msg = "(UWSPR): Invalid register(s)";
        return std::make_tuple(1,msg);
    }

    for(int i = 0; i<NUM_BLOCKS; i++){
        UREG[ud][i] = REG[r1];
    }

    if(verbose==2){
        printRegister('u',ud);
    }

    return std::make_tuple(0,msg);
}

//UWSR $ud, $u1, $r1 : $ud = $u1 >> $r1;
std::tuple<int,std::string> UWCPU::UWSR(int ud, int u1, int r1){
    std::string msg = "";
    if((validur(ud)==0) | (validur(u1)==0)){
        msg = "(UWSR): Invalid register(s)";
        return std::make_tuple(1,msg);
    }
    if(validr(r1)==0){
        msg = "(UWSR): Invalid register(s)";
        return std::make_tuple(1,msg);
    }

    unsigned int x = REG[r1];

    int bts = floor((float)x/(float)WORD_SIZE);
    int split = x%WORD_SIZE;
    unsigned int aux1 = 0;
    unsigned int aux2 = 0;
    int i = bts;
    int j = 0;

    std::vector<int> shifted (WORD_SIZE);
    std::fill(shifted.begin(),shifted.end(),0);

    while(i<NUM_BLOCKS){
        aux1 = UREG[u1][j]>>split;
         shifted[i] = aux1|aux2;
        if(split!=0){
            aux2 = UREG[u1][j]<<(WORD_SIZE-split);
        }
        i++;j++;
    }
    for(int k = 0; k<WORD_SIZE; k++){
        UREG[ud][k] = shifted[k];
    }

    if(verbose==2){
        printRegister('u',ud);
    }

    return std::make_tuple(0,msg);
}


//UWSRI $ud, $u1, imm : $ud = $u1 >> imm;
std::tuple<int,std::string> UWCPU::UWSRI(int ud, int u1, int imm){
    std::string msg = "";
    if((validur(ud)==0) | (validur(u1)==0)){
        msg = "(UWSRI): Invalid register(s)";
        return std::make_tuple(1,msg);
    }
    if(imm >= (WORD_SIZE*NUM_BLOCKS)){
        msg = "(UWSRI): Shift larger than register size is invalid";
        return std::make_tuple(1,msg);
    }

    int bts = floor((float)imm/(float)WORD_SIZE);
    int split = imm%WORD_SIZE;
    unsigned int aux1 = 0;
    unsigned int aux2 = 0;
    int i = bts;
    int j = 0;

    while(i<NUM_BLOCKS){
        aux1 = UREG[u1][j]>>split;
        UREG[ud][i] = aux1|aux2;
        if(split!=0){
            aux2 = UREG[u1][j]<<(WORD_SIZE-split);
        }
        i++;j++;
    }

    if(verbose==2){
        printRegister('u',ud);
    }

    return std::make_tuple(0,msg);
}

//UWSRR  $rd, $u1: $rd = $u1[0]
std::tuple<int,std::string> UWCPU::UWSRR(int rd, int u1){
    std::string msg = "";
    if(validur(u1)==0){
        msg = "(UWSRR): Invalid register(s)";
        return std::make_tuple(1,msg);
    }
    if(validr(rd)==0){
        msg = "(UWSRR): Invalid register(s)";
        return std::make_tuple(1,msg);
    }

    REG[rd] = UREG[u1][0];
    return std::make_tuple(0,msg);
}


//UWSSQ $ud, base, imm1 : spreads sequence of imm and adds it to address in first block of $ud. stores in $ud.
//ex: UWSSQ $ud,4
//$ud[0] = 2, $ud[1] = 6, $ud[2] = 10, $ud[3] = 14...
std::tuple<int,std::string> UWCPU::UWSSQ(int ud,  int imm){
    std::string msg = "";
    if(validur(ud)==0){
        msg = "(UWSSQ): Invalid register(s)";
        return std::make_tuple(1,msg);
    }

    //int base = UREG[ud][0];
    for(int i=0; i<NUM_BLOCKS; i++){
        UREG[ud][i] =  (i*imm);
    }

    if(verbose==2){
        printRegister('u',ud);
    }

    return std::make_tuple(0,msg);
}


//UWSUB $ud, $u1, $rs : $rd = $r1 - $rs;
std::tuple<int,std::string> UWCPU::UWSUB(int ud, int u1, int u2){
    std::string msg = "";
    if((validur(ud)==0) | (validur(u1)==0) | (validur(u2)==0)){
        msg = "(UWADD): Invalid register(s)";
        return std::make_tuple(1,msg);
    }
    //missing carry management (not for flags, but because UW should know no block boundaries)
    for(int i = NUM_BLOCKS-1; i>=0; i--){
        UREG[ud][i] = UREG[u1][i] - UREG[u2][i];
    }

    if(verbose==2){
        printRegister('u',ud);
    }
    return std::make_tuple(0,msg);
}

//UWSUW $ud,base:  MEM[base+j] = $u1[j] (for all j in NUM_BLOCKS in parallel)
std::tuple<int,std::string> UWCPU::UWSUW(int u1,int r1, RAM *_ram){
    //base address taken from a register
    std::string msg = "";
    if(validur(u1)==0){
        msg = "(UWSUW): Invalid register(s)";
        return std::make_tuple(1,msg);
    }else if(validr(r1)==0){
        msg = "(UWSUW): Invalid register(s)";
        return std::make_tuple(1,msg);
    }
    int base = REG[r1];

    for(int i =0; i<NUM_BLOCKS; i++){
        int k = _ram->set(base+i, std::to_string(UREG[u1][i]));
        if(k!=0){
            return std::make_tuple(1,msg);
        }
    }

    if(verbose==2){
        std::cout<<"RAM["<<base<<"]: ";
        printRegister('u',u1);
    }

    return std::make_tuple(0,msg);
}

//UWXOR $ud, $u1, $u2 : $ud = $u1 | $u2;
std::tuple<int,std::string> UWCPU::UWXOR(int ud, int u1, int u2){
    std::string msg = "";
    if((validur(ud)==0) | (validur(u1)==0) | (validur(u2)==0)){
        msg = "(UWXOR): Invalid register(s)";
        return std::make_tuple(1,msg);
    }

    for(int i = NUM_BLOCKS-1; i>=0; i--){
        UREG[ud][i] = UREG[u1][i] ^ UREG[u2][i];
    }

    if(verbose==2){
        printRegister('u',ud);
    }
    return std::make_tuple(0,msg);
}






// PROGRAM EXECUTION AND INSTRUCTION HANDLING

std::vector<std::string> UWCPU::tokenize(std::string str, std::string separator){
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

//EXECUTE SINGLE INSTRUCTION
std::tuple<int,std::string> UWCPU::handleInstruction(std::string instructionline){
    //at this point we are sure instructions exist and parameters are the correct format
    if(instructionline.empty()){
        halt = true;
        return std::make_tuple(0,"");
    }

    std::vector<std::string> sepinst = tokenize(instructionline," ");
    std::string instruction = sepinst[0];
    std::vector<std::string> params;

    if (sepinst.size() > 1){
        params = tokenize(sepinst[1],",");
    }
     std::tuple<int,std::string> ret;
    //Giant if-else to find instruction
    if(instruction == "add"){
        ret = ADD(atoi(params[0].c_str()),atoi(params[1].c_str()),atoi(params[2].c_str()));
        return ret;
    }else if(instruction == "addi"){
        ret = ADDI(atoi(params[0].c_str()),atoi(params[1].c_str()),atoi(params[2].c_str()));
        return ret;
    }else if(instruction == "addu"){
        ret = ADDU(atoi(params[0].c_str()),atoi(params[1].c_str()),atoi(params[2].c_str()));
        return ret;
    }else if(instruction == "and"){
        ret = AND(atoi(params[0].c_str()),atoi(params[1].c_str()),atoi(params[2].c_str()));
        return ret;
    }else if(instruction == "andi"){
        ret = ANDI(atoi(params[0].c_str()),atoi(params[1].c_str()),atoi(params[2].c_str()));
        return ret;
    }else if(instruction == "beq"){
        ret = BEQ(atoi(params[0].c_str()),atoi(params[1].c_str()),atoi(params[2].c_str()));
        return ret;
    }else if(instruction == "bne"){
        ret = BNE(atoi(params[0].c_str()),atoi(params[1].c_str()),atoi(params[2].c_str()));
        return ret;
    }else if(instruction == "j"){
        ret = J(atoi(params[0].c_str()));
        return ret;
    }else if(instruction == "jal"){
        ret = JAL(atoi(params[0].c_str()));
        return ret;
    }else if(instruction == "jr"){
        ret = JR(atoi(params[0].c_str()));
        return ret;
    }else if(instruction == "li"){
        ret = LI(atoi(params[0].c_str()),atoi(params[1].c_str()));
        return ret;
    }else if(instruction == "luw"){
        ret = LUW(atoi(params[0].c_str()),atoi(params[1].c_str()));
        return ret;
    }else if(instruction == "lw"){
        ret = LW(atoi(params[0].c_str()),atoi(params[1].c_str()),atoi(params[2].c_str()),ramm);
        return ret;
    }else if(instruction == "not"){
        ret = NOT(atoi(params[0].c_str()),atoi(params[1].c_str()));
        return ret;
    }else if(instruction == "or"){
        ret = OR(atoi(params[0].c_str()),atoi(params[1].c_str()),atoi(params[2].c_str()));
        return ret;
    }else if(instruction == "ori"){
        ret = ORI(atoi(params[0].c_str()),atoi(params[1].c_str()),atoi(params[2].c_str()));
        return ret;
    }else if(instruction == "sl"){
        ret = SL(atoi(params[0].c_str()),atoi(params[1].c_str()),atoi(params[2].c_str()));
        return ret;
    }else if(instruction == "sli"){
        ret = SLI(atoi(params[0].c_str()),atoi(params[1].c_str()),atoi(params[2].c_str()));
        return ret;
    }else if(instruction == "slt"){
        ret = SLT(atoi(params[0].c_str()),atoi(params[1].c_str()),atoi(params[2].c_str()));
        return ret;
    }else if(instruction == "slti"){
        ret = SLTI(atoi(params[0].c_str()),atoi(params[1].c_str()),atoi(params[2].c_str()));
        return ret;
    }else if(instruction == "sr"){
        ret = SR(atoi(params[0].c_str()),atoi(params[1].c_str()),atoi(params[2].c_str()));
        return ret;
    }else if(instruction == "sri"){
        ret = SRI(atoi(params[0].c_str()),atoi(params[1].c_str()),atoi(params[2].c_str()));
        return ret;
    }else if(instruction == "sub"){
        ret = SUB(atoi(params[0].c_str()),atoi(params[1].c_str()),atoi(params[2].c_str()));
        return ret;
    }else if(instruction == "subu"){
        ret = SUBU(atoi(params[0].c_str()),atoi(params[1].c_str()),atoi(params[2].c_str()));
        return ret;
    }else if(instruction == "sw"){
        ret = SW  (atoi(params[0].c_str()),atoi(params[1].c_str()),atoi(params[2].c_str()),ramm);
        return ret;
    }else if(instruction == "xor"){
        ret = XOR(atoi(params[0].c_str()),atoi(params[1].c_str()),atoi(params[2].c_str()));
        return ret;
    }else if(instruction == "xori"){
        ret = XORI(atoi(params[0].c_str()),atoi(params[1].c_str()),atoi(params[2].c_str()));
        return ret;
    }else if(instruction == "uwadd"){
        ret = UWADD(atoi(params[0].c_str()),atoi(params[1].c_str()),atoi(params[2].c_str()));
        return ret;
    }else if(instruction == "uwand"){
        ret = UWAND (atoi(params[0].c_str()),atoi(params[1].c_str()),atoi(params[2].c_str()));
        return ret;
    }else if(instruction == "uwbeq"){
        ret = UWBEQ(atoi(params[0].c_str()),atoi(params[1].c_str()),atoi(params[2].c_str()));
        return ret;
    }else if(instruction == "uwbne"){
        ret = UWBNE(atoi(params[0].c_str()),atoi(params[1].c_str()),atoi(params[2].c_str()));
        return ret;
    }else if(instruction == "uwcom"){
        ret = UWCOM(atoi(params[0].c_str()),atoi(params[1].c_str()));
        return ret;
    }else if(instruction == "uwexp"){
        ret = UWEXP(atoi(params[0].c_str()),atoi(params[1].c_str()));
        return ret;
    }else if(instruction == "uwlb"){
        ret = UWLB(atoi(params[0].c_str()),atoi(params[1].c_str()),atoi(params[2].c_str()),ramm);
        return ret;
    }else if(instruction == "uwlc"){
        ret = UWLC(atoi(params[0].c_str()),atoi(params[1].c_str()),ramm);
        return ret;
    }else if(instruction == "uwlrr"){
        ret = UWLRR(atoi(params[0].c_str()),atoi(params[1].c_str()));
        return ret;
    }else if(instruction == "uwluw"){
        ret = UWLUW(atoi(params[0].c_str()),atoi(params[1].c_str()),ramm);
        return ret;
    }else if(instruction == "uwnot"){
        ret = UWNOT(atoi(params[0].c_str()),atoi(params[1].c_str()));
        return ret;
    }else if(instruction == "uwor"){
        ret = UWOR(atoi(params[0].c_str()),atoi(params[1].c_str()),atoi(params[2].c_str()));
        return ret;
    }else if(instruction == "uwsb"){
        ret = UWSB(atoi(params[0].c_str()),atoi(params[1].c_str()),atoi(params[2].c_str()),ramm);
        return ret;
    }else if(instruction == "uwsc"){
        ret = UWSC(atoi(params[0].c_str()),atoi(params[1].c_str()),atoi(params[2].c_str()),ramm);
        return ret;
    }else if(instruction == "uwsl"){
        ret = UWSL(atoi(params[0].c_str()),atoi(params[1].c_str()),atoi(params[2].c_str()));
        return ret;
    }else if(instruction == "uwsli"){
        ret = UWSLI(atoi(params[0].c_str()),atoi(params[1].c_str()),atoi(params[2].c_str()));
        return ret;
    }else if(instruction == "uwspr"){
        ret = UWSPR(atoi(params[0].c_str()),atoi(params[1].c_str()));
        return ret;
    }else if(instruction == "uwsr"){
        ret = UWSR(atoi(params[0].c_str()),atoi(params[1].c_str()),atoi(params[2].c_str()));
        return ret;
    }else if(instruction == "uwsri"){
        ret = UWSRI(atoi(params[0].c_str()),atoi(params[1].c_str()),atoi(params[2].c_str()));
        return ret;
    }else if(instruction == "uwsrr"){
        ret = UWSRR(atoi(params[0].c_str()),atoi(params[1].c_str()));
        return ret;
    }else if(instruction == "uwssq"){
        ret = UWSSQ(atoi(params[0].c_str()),atoi(params[1].c_str()));
        return ret;
    }else if(instruction == "uwsub"){
        ret = UWSUB(atoi(params[0].c_str()),atoi(params[1].c_str()),atoi(params[2].c_str()));
        return ret;
    }else if(instruction == "uwsuw"){
        ret = UWSUW(atoi(params[0].c_str()),atoi(params[1].c_str()),ramm);
        return ret;
    }else if(instruction == "uwxor"){
        ret = UWXOR(atoi(params[0].c_str()),atoi(params[1].c_str()),atoi(params[2].c_str()));
        return ret;
    }else if(instruction == "halt"){
        halt = true;
        return std::make_tuple(0,"");
    }
    return ret;
}

//CLOCK really just counts the number of instructions executed.

int UWCPU::executeProgram(int start){
    PC = start;
    std::cout <<"INSTRUCTIONS EXECUTED \n";

    std::tuple<int,std::string> check;
    while(halt==false){
        auto res = ramm->get(PC);
        IR = std::get<1>(res);
        if(verbose==1 | verbose==2){
            std::cout<<PC<<" : "<<IR<<"\n";
        }

        PC = PC+1;
        check = handleInstruction(IR);

        if(std::get<0>(check) != 0){
            std::cout<< "ERROR: " << PC << " , " << std::get<1>(check)<< "\n";
            return CLOCK;
        }

        CLOCK++;
    }
    return CLOCK;
}


void UWCPU::printRegister(char c, int i){
    if(c=='r'){
        if(validr(i)==0){
            std::cout<< "(printRegister): Invalid register number";
            return;
        }
        std::cout << "R"<<i<<": "<<"\n";
        std::cout << intToBits(REG[i]) << ": "<<REG[i]<< "\n";
        return;
    }else if(c=='u'){
        if(validur(i)==0){
            std::cout<<"(printRegister): Invalid register number";
            return;
        }
        std::cout << "U"<<i<<": "<<"\n";
        for(int j = 0; j<UREG[i].size(); j++){
            std::cout<<intToBits(UREG[i][j])<< ": "<<UREG[i][j]<< "\n";
        }
        return;
    }
    std::cout<< "printRegister:  bad register type \n";

}
