/********************************************************************
*** NAME : Caden Fischer ***
*** CLASS : CSC 446 ***
*** INSTRUCTOR : Dr. George Hamer ***
*********************************************************************
*** FILE: TACGen.cpp ***
*** DESCRIPTION : A Three Address Code (TAC) Generator with TACArg Type ***
*** EXPORTED FUNCTIONALITY : 
* class TACGen : CodeGen
    void EmitBinaryAssign(TACArg x, TACArg y, TACArg z, BinOp binOp)
    void EmitUnaryAssign(TACArg x, TACArg y, UnOp unaryOp)
    void EmitCopy(TACArg x, TACArg y)
    void EmitProcCall(std::string P)
    void EmitPush(TACArg a, bool ref)
    void EmitProcHead(std::string P)
    void EmitProcEnd(std::string P)
********************************************************************/

#include <TACGen.hpp>
#include <iostream>
#include <iomanip>

std::string TACGen::tacArgToString(const TACArg & arg)
{
    std::string retVal = "";
    switch (arg.type)
    {
    case StackTAC:
        if (arg.ref)
        {
            retVal += "*";
        }
        retVal += "_BP";
        if (arg.offset >= 0)
        {
            retVal += "+";
        }
        retVal += std::to_string(arg.offset);
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

TACGen::TACGen(std::string theFileName)
{
    //Get extension and replace with .tac
    theFileName.replace(theFileName.length() - 4, 4, ".tac"); //Replaces the extension
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

TACGen::~TACGen()
{
    outFile.close();
}

void TACGen::EmitBinaryAssign(const TACArg & x, const TACArg & y, const TACArg & z, BinOp binOp)
{
    //Convert Args to string
    std::string xSTR = tacArgToString(x);
    std::string ySTR = tacArgToString(y);
    std::string zSTR = tacArgToString(z);

    //Convert BinOp to string
    std::string binOpSTR;
    switch (binOp)
    {
    case addBOP:
        binOpSTR = "  +  ";
        break;

    case subBOP:
        binOpSTR = "  -  ";
        break;

    case mulBOP:
        binOpSTR = "  *  ";
        break;
    
    case divBOP:
        binOpSTR = "  /  ";
        break;
    
    case modBOP:
        binOpSTR = " mod ";
        break;
    
    case remBOP:
        binOpSTR = " rem ";
        break;
    
    default:
        std::cerr << BINOP_NAMES.at(binOp) << " has not been implemented for TAC Code" << std::endl;
        outFile << BINOP_NAMES.at(binOp) << " has not been implemented for TAC Code" << std::endl;
        return;
        break;
    }

    //Output line: X = Y binOp Z
    outFile << std::setw(8) << ""; //Nothing in column 1
    outFile << std::setw(16) << xSTR; //X in column 2
    outFile << std::setw(5) << "  =  "; //= in column 3
    outFile << std::setw(16) << ySTR; //Y in column 4
    outFile << std::setw(5) << binOpSTR; //binOp in column 5
    outFile << std::setw(16) << zSTR; //z in column 6
    outFile << std::endl; //Finish Line
}

void TACGen::EmitUnaryAssign(const TACArg &x, const TACArg &y, UnOp unaryOp)
{
    //Convert Args to string
    std::string xSTR = tacArgToString(x);
    std::string ySTR = tacArgToString(y);

    //Convert unaryOp to string
    std::string unaryOpSTR;
    switch (unaryOp)
    {
    case posUOP:
        unaryOpSTR = "  +";
        break;
    
    case negUOP:
        unaryOpSTR = "  -";
        break;
    
    default:
        std::cerr << UNOP_NAMES.at(unaryOp) << " has not been implemented for TAC Code" << std::endl;
        outFile << UNOP_NAMES.at(unaryOp) << " has not been implemented for TAC Code" << std::endl;
        return;
        break;
    }

    //Output line X = unaryOp Y
    outFile << std::setw(8) << ""; //Nothing in column 1
    outFile << std::setw(16) << xSTR; //X in column 2
    outFile << std::setw(5) << "  =  "; //= in column 3
    outFile << std::setw(3) << unaryOpSTR; //unaryOp in column 4
    outFile << std::setw(16) << ySTR; //Y in column 5
    outFile << std::endl; //Finish Line
}

void TACGen::EmitCopy(const TACArg &x, const TACArg &y)
{
    //Convert Args to string
    std::string xSTR = tacArgToString(x);
    std::string ySTR = tacArgToString(y);

    //Output line X = Y
    outFile << std::setw(8) << ""; //Nothing in column 1
    outFile << std::setw(16) << xSTR; //X in column 2
    outFile << std::setw(5) << "  =  "; //= in column 3
    outFile << std::setw(16) << ySTR; //Y in column 4
    outFile << std::endl; //Finish Line
}

void TACGen::EmitProcCall(std::string P)
{
    //Output line call P
    outFile << std::setw(8) << ""; //Nothing in column 1
    outFile << std::setw(8) << "call "; //Keyword call in column 2
    outFile << std::setw(16) << P; //P in column 3
    outFile << std::endl; //Finish Line
}

void TACGen::EmitPush(const TACArg &a, bool ref)
{
    //Convert Args to string
    std::string aSTR = tacArgToString(a);

    //Add & if bool ref is true
    if (ref)
    {
        aSTR.insert(aSTR.begin(), '&');
    }

    //Output line push a or push &a
    outFile << std::setw(8) << ""; //Nothing in column 1
    outFile << std::setw(8) << "push "; //Keyword push in column 2
    outFile << std::setw(16) << aSTR; //a or &a in column 3
    outFile << std::endl; //Finish Line
}

void TACGen::EmitProcHead(std::string P)
{
    //Output line proc P
    outFile << std::setw(8) << "PROC"; //Keyword proc in column 1
    outFile << std::setw(16) << P; //P in column 2
    outFile << std::endl; //Finish Line
}

void TACGen::EmitProcEnd(std::string P)
{
    //Output line end P
    outFile << std::setw(8) << "ENDP"; //Keyword call in column 1
    outFile << std::setw(16) << P; //P in column 2
    outFile << std::endl; //Finish Line
}

void TACGen::EmitIO(bool write, bool string, bool line, const TACArg &x)
{
    std::string tacArgString = tacArgToString(x);
    std::string instruction = "";
    if (write)
    {
        instruction += "wr";
    }
    else
    {
        instruction += "rd";
    }

    if (line)
    {
        instruction += "ln";
    }
    else
    {
        if (string)
        {
            instruction += "s";
        }
        else
        {
            instruction += "i";
        }
    }

    outFile << std::setw(8) << ""; //Nothing in column 1
    outFile << std::setw(8) << instruction; //IO instruction in column 2
    outFile << std::setw(16) << tacArgString; //emit tacArg in column 3
    outFile << std::endl; //Finish Line
}

void TACGen::EmitProgEnd(std::string P)
{
    //Output line start proc P
    outFile << std::setw(8) << "START";
    outFile << std::setw(8) << "PROC";
    outFile << std::setw(16) << P;
    outFile << std::endl;
}
