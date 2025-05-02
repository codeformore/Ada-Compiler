/********************************************************************
*** NAME : Caden Fischer ***
*** CLASS : CSC 446 ***
*** INSTRUCTOR : Dr. George Hamer ***
*********************************************************************
*** FILE: TACGen.hpp ***
*** DESCRIPTION : An Intel 8086 assembly Generator ***
*** EXPORTED FUNCTIONALITY : 
* class Intel8086 : CodeGen
    void EmitBinaryAssign(TACArg x, TACArg y, TACArg z, BinOp binOp)
    void EmitUnaryAssign(TACArg x, TACArg y, UnOp unaryOp)
    void EmitCopy(TACArg x, TACArg y)
    void EmitProcCall(std::string P)
    void EmitPush(TACArg a, bool ref)
    void EmitProcHead(std::string P)
    void EmitProcEnd(std::string P)
********************************************************************/

#include <Intel8086Gen.hpp>
#include <iostream>
#include <iomanip>

Intel8086Gen::Intel8086Gen(std::string theFileName)
{
    //Get extension and replace with .tac
    theFileName.replace(theFileName.length() - 4, 4, ".asm"); //Replaces the extension
    fileName = theFileName;

    //Open file
    outFile.open(fileName);
    if (!outFile.is_open()) 
    {
        throw std::runtime_error("Unable to open " + fileName + ".");
    }
    outFile << std::setfill(' '); //Ensure fill character is ' '
    outFile << std::left; //Ensure left align is used
}

Intel8086Gen::~Intel8086Gen()
{
    outFile.close();
}

std::string Intel8086Gen::tacArgToString(const TACArg & arg)
{
    return std::string();
}

void Intel8086Gen::EmitBinaryAssign(const TACArg & x, const TACArg & y, const TACArg & z, BinOp binOp)
{
    //Convert Args to string
    std::string xSTR = tacArgToString(x);
    std::string ySTR = tacArgToString(y);
    std::string zSTR = tacArgToString(z);

    //Always will move y into AX register
    outFile << std::setw(8) << ""; //No label
    outFile << std::setw(25) << "MOV AX, " << ySTR;
    outFile << std::endl;

    switch (binOp)
    {
    case addBOP:
        //ADD AX, z
        outFile << std::setw(8) << ""; //No label
        outFile << std::setw(25) << "ADD AX, " << zSTR;
        outFile << std::endl;
        break;

    case subBOP:
        //SUB AX, z
        outFile << std::setw(8) << ""; //No label
        outFile << std::setw(25) << "SUB AX, " << zSTR;
        outFile << std::endl;
        break;

    case mulBOP:
        //MOV BX, z
        outFile << std::setw(8) << ""; //No label
        outFile << std::setw(25) << "MOV BX, " << zSTR;
        outFile << std::endl;
        //IMUL BX
        outFile << std::setw(8) << ""; //No label
        outFile << std::setw(25) << "IMUL BX";
        outFile << std::endl;
        break;
    
    case divBOP:
        //MOV BX, z
        outFile << std::setw(8) << ""; //No label
        outFile << std::setw(25) << "MOV BX, " << zSTR;
        outFile << std::endl;
        //IDIV BX
        outFile << std::setw(8) << ""; //No label
        outFile << std::setw(25) << "IDIV BX";
        outFile << std::endl;
        break;
    
    default:
        std::cerr << BINOP_NAMES.at(binOp) << " has not been implemented for TAC Code" << std::endl;
        outFile << BINOP_NAMES.at(binOp) << " has not been implemented for TAC Code" << std::endl;
        return;
        break;
    }

    //Always will move result from AX to x
    outFile << std::setw(8) << ""; //No label
    outFile << std::setw(25) << "MOV " << xSTR << ", AX";
    outFile << std::endl;
}

void Intel8086Gen::EmitUnaryAssign(const TACArg & x, const TACArg & y, UnOp unaryOp)
{

}

void Intel8086Gen::EmitCopy(const TACArg & x, const TACArg & y)
{
    
}

void Intel8086Gen::EmitProcCall(std::string P)
{

}

void Intel8086Gen::EmitPush(const TACArg & a, bool ref)
{

}

void Intel8086Gen::EmitProcHead(std::string P)
{

}

void Intel8086Gen::EmitProcEnd(std::string P)
{

}

void Intel8086Gen::EmitIO(bool write, bool string, bool line, const TACArg &x)
{

}

void Intel8086Gen::EmitProgStart(std::string P)
{



}

void Intel8086Gen::EmitProgEnd(std::string P)
{

}