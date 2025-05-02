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

    //Set fill and alignment for codeSegment and dataSegment buffers
    codeSegment << std::setfill(' '); //Ensure fill character is ' '
    codeSegment << std::left; //Ensure left align is used
    dataSegment<< std::setfill(' '); //Ensure fill character is ' '
    dataSegment << std::left; //Ensure left align is used
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
            codeSegment << std::setw(8) << ""; //No label
            codeSegment << std::setw(25) << "MOV BX, " + argSTR;
            codeSegment << std::endl;
            codeSegment << std::setw(8) << ""; //No label
            codeSegment << std::setw(25) << "MOV " + regis + ", [BX]";
            codeSegment << std::endl;
        }
        else
        {
            codeSegment << std::setw(8) << ""; //No label
            codeSegment << std::setw(25) << "MOV " + regis + ", " + argSTR;
            codeSegment << std::endl;
        }
    }
    else
    {
        if (ref)
        {
            codeSegment << std::setw(8) << ""; //No label
            codeSegment << std::setw(25) << "MOV BX, " + argSTR;
            codeSegment << std::endl;
            codeSegment << std::setw(8) << ""; //No label
            codeSegment << std::setw(25) << "MOV [BX], " + regis;
            codeSegment << std::endl;
        }
        else
        {
            codeSegment << std::setw(8) << ""; //No label
            codeSegment << std::setw(25) << "MOV " + argSTR + ", " + regis;
            codeSegment << std::endl;
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
        codeSegment << std::setw(8) << ""; //No label
        codeSegment << std::setw(25) << "ADD AX, " + zSTR;
        codeSegment << std::endl;
        break;

    case subBOP:
        //SUB AX, z
        codeSegment << std::setw(8) << ""; //No label
        codeSegment << std::setw(25) << "SUB AX, " + zSTR;
        codeSegment << std::endl;
        break;

    case mulBOP:
        //MOV BX, z
        storeLoadHelper(true, z.ref, "BX", zSTR);
        //IMUL BX
        codeSegment << std::setw(8) << ""; //No label
        codeSegment << std::setw(25) << "IMUL BX";
        codeSegment << std::endl;
        break;
    
    case divBOP:
        //MOV BX, z
        storeLoadHelper(true, z.ref, "BX", zSTR);
        //IDIV BX
        codeSegment << std::setw(8) << ""; //No label
        codeSegment << std::setw(25) << "IDIV BX";
        codeSegment << std::endl;
        break;
    
    default:
        std::cerr << BINOP_NAMES.at(binOp) << " has not been implemented for TAC Code" << std::endl;
        codeSegment << BINOP_NAMES.at(binOp) << " has not been implemented for TAC Code" << std::endl;
        return;
        break;
    }

    //Always will move result from AX to x
    storeLoadHelper(false, x.ref, "AX", xSTR);
    codeSegment << std::endl;
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
        codeSegment << std::setw(8) << ""; //No label
        codeSegment << std::setw(25) << "NEG AX";
        codeSegment << std::endl;
        break;
    
    default:
        std::cerr << UNOP_NAMES.at(unaryOp) << " has not been implemented for TAC Code" << std::endl;
        codeSegment << UNOP_NAMES.at(unaryOp) << " has not been implemented for TAC Code" << std::endl;
        return;
        break;
    }

    //Always will move result from AX to x
    storeLoadHelper(false, x.ref, "AX", xSTR);
    codeSegment << std::endl;
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
    codeSegment << std::endl;
}

void Intel8086Gen::EmitProcCall(std::string P)
{
    codeSegment << std::setw(8) << ""; //No label
    codeSegment << std::setw(25) << "CALL " + P;
    codeSegment << std::endl << std::endl;
}

void Intel8086Gen::EmitPush(const TACArg & a, bool ref)
{
    std::string aSTR = tacArgToString(a);
std::string offsetSTR = ref ? "offset " : "";

    codeSegment << std::setw(8) << ""; //No label
    codeSegment << std::setw(25) << "PUSH " + offsetSTR + aSTR;
    codeSegment << std::endl;
}

void Intel8086Gen::EmitProcHead(std::string P)
{
    codeSegment << std::setw(8) << P + " ";
    codeSegment << std::setw(25) << "PROC ";
    codeSegment << std::endl;

    codeSegment << std::setw(8) << "";
    codeSegment << std::setw(25) << "PUSH BP";
    codeSegment << std::endl;

    codeSegment << std::setw(8) << "";
    codeSegment << std::setw(25) << "MOV BP, SP";
    codeSegment << std::endl;

    codeSegment << std::setw(8) << "";
    codeSegment << std::setw(0) << "SUB SP, ";
    localVarSizeLocations.push_back(codeSegment.tellp());
    codeSegment << std::setw(0) << "$$$"; //Placeholder for size of local variables
    codeSegment << std::endl << std::endl;
}

void Intel8086Gen::EmitProcEnd(std::string P, int sizeOfLocals, int sizeOfParams)
{
    //Overwrite the placeholder for sizeOfLocals set at begining
    std::streampos curLocation = codeSegment.tellp();
    codeSegment.seekp(localVarSizeLocations[localVarSizeLocations.size() - 1]);
    localVarSizeLocations.pop_back();
    codeSegment << std::setw(3) << sizeOfLocals;
    codeSegment.seekp(curLocation);

    codeSegment << std::setw(8) << "";
    codeSegment << std::setw(25) << "ADD SP, " + std::to_string(sizeOfLocals); //TODO: Size of Locals Logic
    codeSegment << std::endl;

    codeSegment << std::setw(8) << "";
    codeSegment << std::setw(25) << "POP BP";
    codeSegment << std::endl;

    codeSegment << std::setw(8) << "";
    codeSegment << std::setw(25) << "RET " + std::to_string(sizeOfParams); //TODO: Size of Params Logic
    codeSegment << std::endl;

    codeSegment << std::setw(8) << P + " ";
    codeSegment << std::setw(25) << "ENDP ";
    codeSegment << std::endl << std::endl;
}

void Intel8086Gen::EmitIO(bool write, bool string, bool line, const TACArg &x)
{
    std::string xSTR = tacArgToString(x);

    if (write)
    {
        if (line)
        {
            codeSegment << std::setw(8) << "";
            codeSegment << std::setw(25) << "CALL writeln";
            codeSegment << std::endl;
        }
        else
        {
            if (string)
            {
                codeSegment << std::setw(8) << "";
                codeSegment << std::setw(25) << "MOV DX, offset " + xSTR;
                codeSegment << std::endl;

                codeSegment << std::setw(8) << "";
                codeSegment << std::setw(25) << "CALL writestr";
                codeSegment << std::endl;
            }
            else
            {
                codeSegment << std::setw(8) << "";
                codeSegment << std::setw(25) << "MOV AX, " + xSTR;
                codeSegment << std::endl;

                codeSegment << std::setw(8) << "";
                codeSegment << std::setw(25) << "CALL writeint";
                codeSegment << std::endl;
            }
        }
    }
    else
    {
        codeSegment << std::setw(8) << "";
        codeSegment << std::setw(25) << "CALL readint";
        codeSegment << std::endl;

        codeSegment << std::setw(8) << "";
        codeSegment << std::setw(25) << "MOV " + xSTR + ", BX";
        codeSegment << std::endl;
    }

    codeSegment << std::endl;
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

    dataSegment << std::setw(8) << "";
    dataSegment << std::setw(25) << ".data";
    dataSegment << std::endl;

    codeSegment << std::setw(8) << "";
    codeSegment << std::setw(25) << ".code";
    codeSegment << std::endl;

    codeSegment << std::setw(8) << "";
    codeSegment << std::setw(25) << "include io.asm";
    codeSegment << std::endl << std::endl;
}

void Intel8086Gen::EmitProgEnd(std::string P)
{
    codeSegment << std::setw(8) << "main";
    codeSegment << std::setw(25) << "PROC";
    codeSegment << std::endl;

    codeSegment << std::setw(8) << "";
    codeSegment << std::setw(25) << "MOV AX, @data";
    codeSegment << std::endl;

    codeSegment << std::setw(8) << "";
    codeSegment << std::setw(25) << "MOV DS, AX";
    codeSegment << std::endl;

    codeSegment << std::setw(8) << "";
    codeSegment << std::setw(25) << "CALL " + P;
    codeSegment << std::endl;

    codeSegment << std::setw(8) << "";
    codeSegment << std::setw(25) << "MOV AH, 04ch";
    codeSegment << std::endl;

    codeSegment << std::setw(8) << "";
    codeSegment << std::setw(25) << "int 21h";
    codeSegment << std::endl;

    codeSegment << std::setw(8) << "main";
    codeSegment << std::setw(25) << "ENDP";
    codeSegment << std::endl;

    codeSegment << std::setw(8) << "";
    codeSegment << std::setw(25) << "END main";
    codeSegment << std::endl;

    //Write code and data segments into outFile
    outFile << dataSegment.str();
    outFile << codeSegment.str(); 
}

void Intel8086Gen::EmitGlobalVar(bool string, const TACArg & label, std::string data)
{
    std::string labelSTR = tacArgToString(label);
    if (string)
    {
        dataSegment << std::setw(8) << labelSTR + " ";
        dataSegment << std::setw(5) << "DB";
        dataSegment << "\"" + data + "\",\"$\"";
        dataSegment << std::endl;
    }
    else //Assuming int
    {
        dataSegment << std::setw(8) << labelSTR + " ";
        dataSegment << std::setw(5) << "DW";
        dataSegment << data;
        dataSegment << std::endl;
    }
}
