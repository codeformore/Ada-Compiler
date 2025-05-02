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
    int offset;
    std::string retVal = "";
    switch (arg.type)
    {
    case StackTAC:
        retVal += "[BP";
        if (arg.offset > 0)
        {
            offset = arg.offset + 2;
            retVal += "+";
        }
        else
        {
            offset = arg.offset - 2;
        }
        retVal += std::to_string(offset);
        retVal += "]";
        break;

    case LitTAC:
        retVal += "_S";
        retVal += std::to_string(arg.literalNum);
        break;
    
    case ConstTAC:
        retVal = arg.value;
        break;

    case GlobalTAC:
        retVal = arg.name;
        break;
    }
    return retVal;
}

void Intel8086Gen::storeLoadHelper(bool load, bool ref, std::string regis, std::string argSTR)
{
    std::string retSTR = "";
    if (load)
    {
        if (ref)
        {
            outFile << std::setw(8) << ""; //No label
            outFile << std::setw(25) << "MOV DX, " + argSTR;
            outFile << std::endl;
            outFile << std::setw(8) << ""; //No label
            outFile << std::setw(25) << "MOV " + regis + ", [DX]";
            outFile << std::endl;
        }
        else
        {
            outFile << std::setw(8) << ""; //No label
            outFile << std::setw(25) << "MOV " + regis + ", " + argSTR;
            outFile << std::endl;
        }
    }
    else
    {
        if (ref)
        {
            outFile << std::setw(8) << ""; //No label
            outFile << std::setw(25) << "MOV DX, " + argSTR;
            outFile << std::endl;
            outFile << std::setw(8) << ""; //No label
            outFile << std::setw(25) << "MOV [DX], " + regis;
            outFile << std::endl;
        }
        else
        {
            outFile << std::setw(8) << ""; //No label
            outFile << std::setw(25) << "MOV " + argSTR + ", " + regis;
            outFile << std::endl;
        }
    }
}

void Intel8086Gen::EmitBinaryAssign(const TACArg & x, const TACArg & y, const TACArg & z, BinOp binOp)
{
    //Convert Args to string
    std::string xSTR = tacArgToString(x);
    std::string ySTR = tacArgToString(y);
    std::string zSTR = tacArgToString(z);

    //Always will move y into AX register
    storeLoadHelper(true, y.ref, "AX", ySTR);

    switch (binOp)
    {
    case addBOP:
        //ADD AX, z
        outFile << std::setw(8) << ""; //No label
        outFile << std::setw(25) << "ADD AX, " + zSTR;
        outFile << std::endl;
        break;

    case subBOP:
        //SUB AX, z
        outFile << std::setw(8) << ""; //No label
        outFile << std::setw(25) << "SUB AX, " + zSTR;
        outFile << std::endl;
        break;

    case mulBOP:
        //MOV BX, z
        storeLoadHelper(true, z.ref, "BX", zSTR);
        //IMUL BX
        outFile << std::setw(8) << ""; //No label
        outFile << std::setw(25) << "IMUL BX";
        outFile << std::endl;
        break;
    
    case divBOP:
        //MOV BX, z
        storeLoadHelper(true, z.ref, "BX", zSTR);
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
    storeLoadHelper(false, x.ref, "AX", xSTR);
    outFile << std::endl;
}

void Intel8086Gen::EmitUnaryAssign(const TACArg & x, const TACArg & y, UnOp unaryOp)
{
    //Convert Args to string
    std::string xSTR = tacArgToString(x);
    std::string ySTR = tacArgToString(y);

    //Always will move y into AX register
    storeLoadHelper(true, y.ref, "AX", xSTR);

    //Convert unaryOp to string
    std::string unaryOpSTR;
    switch (unaryOp)
    {
    case posUOP:
        break;
    
    case negUOP:
        //NEG AX
        outFile << std::setw(8) << ""; //No label
        outFile << std::setw(25) << "NEG AX";
        outFile << std::endl;
        break;
    
    default:
        std::cerr << UNOP_NAMES.at(unaryOp) << " has not been implemented for TAC Code" << std::endl;
        outFile << UNOP_NAMES.at(unaryOp) << " has not been implemented for TAC Code" << std::endl;
        return;
        break;
    }

    //Always will move result from AX to x
    storeLoadHelper(false, x.ref, "AX", xSTR);
    outFile << std::endl;
}

void Intel8086Gen::EmitCopy(const TACArg & x, const TACArg & y)
{
    //Convert Args to string
    std::string xSTR = tacArgToString(x);
    std::string ySTR = tacArgToString(y);

    //Always will move y into AX register
    storeLoadHelper(true, y.ref, "AX", ySTR);

    //Always will move result from AX to x
    storeLoadHelper(false, x.ref, "AX", xSTR);
    outFile << std::endl;
}

void Intel8086Gen::EmitProcCall(std::string P)
{
    outFile << std::setw(8) << ""; //No label
    outFile << std::setw(25) << "CALL " + P;
    outFile << std::endl << std::endl;
}

void Intel8086Gen::EmitPush(const TACArg & a, bool ref)
{
    std::string aSTR = tacArgToString(a);

    outFile << std::setw(8) << ""; //No label
    outFile << std::setw(25) << "PUSH " << aSTR;
    outFile << std::endl;
}

void Intel8086Gen::EmitProcHead(std::string P)
{
    outFile << std::setw(8) << P + " ";
    outFile << std::setw(25) << "PROC ";
    outFile << std::endl;

    outFile << std::setw(8) << "";
    outFile << std::setw(25) << "PUSH BP";
    outFile << std::endl;

    outFile << std::setw(8) << "";
    outFile << std::setw(25) << "MOV BP, SP";
    outFile << std::endl;

    outFile << std::setw(8) << "";
    outFile << std::setw(0) << "SUB SP, ";
    localVarSizeLocations.push_back(outFile.tellp());
    outFile << std::setw(0) << "$$$"; //Placeholder for size of local variables
    outFile << std::endl << std::endl;
}

void Intel8086Gen::EmitProcEnd(std::string P, int sizeOfLocals, int sizeOfParams)
{
    //Overwrite the placeholder for sizeOfLocals set at begining
    std::streampos curLocation = outFile.tellp();
    outFile.seekp(localVarSizeLocations[localVarSizeLocations.size() - 1]);
    localVarSizeLocations.pop_back();
    outFile << std::setw(3) << sizeOfLocals;
    outFile.seekp(curLocation);

    outFile << std::setw(8) << "";
    outFile << std::setw(25) << "ADD SP, " + std::to_string(sizeOfLocals); //TODO: Size of Locals Logic
    outFile << std::endl;

    outFile << std::setw(8) << "";
    outFile << std::setw(25) << "POP BP";
    outFile << std::endl;

    outFile << std::setw(8) << "";
    outFile << std::setw(25) << "RET " + std::to_string(sizeOfParams); //TODO: Size of Params Logic
    outFile << std::endl;

    outFile << std::setw(8) << P + " ";
    outFile << std::setw(25) << "ENDP ";
    outFile << std::endl << std::endl;
}

void Intel8086Gen::EmitIO(bool write, bool string, bool line, const TACArg &x)
{
    std::string xSTR = tacArgToString(x);

    if (write)
    {
        if (line)
        {
            outFile << std::setw(8) << "";
            outFile << std::setw(25) << "CALL writeln";
            outFile << std::endl;
        }
        else
        {
            if (string)
            {
                outFile << std::setw(8) << "";
                outFile << std::setw(25) << "MOV DX, offset " + xSTR;
                outFile << std::endl;

                outFile << std::setw(8) << "";
                outFile << std::setw(25) << "CALL writestr";
                outFile << std::endl;
            }
            else
            {
                outFile << std::setw(8) << "";
                outFile << std::setw(25) << "MOV AX, " + xSTR;
                outFile << std::endl;

                outFile << std::setw(8) << "";
                outFile << std::setw(25) << "CALL writeint";
                outFile << std::endl;
            }
        }
    }
    else
    {
        outFile << std::setw(8) << "";
        outFile << std::setw(25) << "CALL readint";
        outFile << std::endl;

        outFile << std::setw(8) << "";
        outFile << std::setw(25) << "MOV " + xSTR + ", BX";
        outFile << std::endl;
    }

    outFile << std::endl;
}

void Intel8086Gen::EmitProgStart(std::string P)
{
    outFile << std::setw(8) << "";
    outFile << std::setw(25) << ".model small";
    outFile << std::endl;

    outFile << std::setw(8) << "";
    outFile << std::setw(25) << ".586";
    outFile << std::endl;

    outFile << std::setw(8) << "";
    outFile << std::setw(25) << ".stack 100h";
    outFile << std::endl;

    outFile << std::setw(8) << "";
    outFile << std::setw(25) << ".data";
    outFile << std::endl;

    outFile << std::setw(8) << "";
    outFile << std::setw(25) << ".code";
    outFile << std::endl;

    outFile << std::setw(8) << "";
    outFile << std::setw(25) << "include io.asm";
    outFile << std::endl << std::endl;
}

void Intel8086Gen::EmitProgEnd(std::string P)
{
    outFile << std::setw(8) << "main";
    outFile << std::setw(25) << "PROC";
    outFile << std::endl;

    outFile << std::setw(8) << "";
    outFile << std::setw(25) << "MOV AX, @data";
    outFile << std::endl;

    outFile << std::setw(8) << "";
    outFile << std::setw(25) << "MOV DS, AX";
    outFile << std::endl;

    outFile << std::setw(8) << "";
    outFile << std::setw(25) << "CALL " + P;
    outFile << std::endl;

    outFile << std::setw(8) << "";
    outFile << std::setw(25) << "MOV AH, 04ch";
    outFile << std::endl;

    outFile << std::setw(8) << "";
    outFile << std::setw(25) << "int 21h";
    outFile << std::endl;

    outFile << std::setw(8) << "main";
    outFile << std::setw(25) << "ENDP";
    outFile << std::endl;

    outFile << std::setw(8) << "";
    outFile << std::setw(25) << "END main";
    outFile << std::endl;
}