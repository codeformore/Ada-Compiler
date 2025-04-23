/********************************************************************
*** NAME : Caden Fischer ***
*** CLASS : CSC 446 ***
*** INSTRUCTOR : Dr. George Hamer ***
*********************************************************************
*** FILE: TACGen.hpp ***
*** DESCRIPTION : A Three Address Code (TAC) Generator with TACArg Type ***
*** EXPORTED FUNCTIONALITY : 
* enum BinOp
* enum UnOp
* enum TACArgType
* struct TACArg
    TACArgType type
    union { int offset; std::string value; std::string name }
* class TACGen
    void EmitBinaryAssign(TACArg x, TACArg y, TACArg z, BinOp binOp)
    void EmitUnaryAssign(TACArg x, TACArg y, UnOp unaryOp)
    void EmitCopy(TACArg x, TACArg y)
    void EmitProcCall(std::string P)
    void EmitPush(TACArg a, bool ref)
    void EmitProcHead(std::string P)
    void EmitProcEnd(std::string P)
********************************************************************/

#ifndef TACGEN_HPP
#define TACGEN_HPP

#include <string>
#include <map>
#include <fstream>

enum BinOp { addBOP, subBOP, orBOP, mulBOP, divBOP, modBOP, remBOP, andBOP };
const std::map<BinOp, std::string> BINOP_NAMES = {
    {addBOP, "addition"}, {subBOP, "subtraction"}, {orBOP, "or"}, {mulBOP, "multiplication"},
    {divBOP, "division"}, {modBOP, "modulus"}, {remBOP, "remainder"}, {andBOP, "and"}
};

enum UnOp { posUOP, negUOP, notUOP };
const std::map<UnOp, std::string> UNOP_NAMES = {
    {posUOP, "positive"}, {negUOP, "negative"}, {notUOP, "not"}
};

enum TACArgType { StackTAC, ConstTAC, GlobalTAC };
const std::map<TACArgType, std::string> TACARGTYPE_NAMES = {
    {StackTAC, "Stack"}, {ConstTAC, "Constant"}, {GlobalTAC, "Global"}
};

struct TACArg
{
    TACArgType type;
    union
    {
        int offset;
        std::string value;
        std::string name;
    };
    TACArg& operator=(const TACArg& toCopy) {
        if (this != &toCopy) {
            this->~TACArg();
            new (this) TACArg(toCopy);  // placement new copy
        }
        return *this;
    }
    TACArg(const TACArg& toCopy) : type(toCopy.type) {
        if (type == StackTAC) {
            offset = toCopy.offset;
        } else if (type == ConstTAC) {
            new (&value) std::string(toCopy.value);
        } else {
            new (&name) std::string(toCopy.name);
        }
    }
    TACArg(TACArgType type = StackTAC, int data = 0) : type(type) { 
        if (type != StackTAC)
        {
            throw std::logic_error("Invalid TACArgType for int constructor");
        }
        this->offset = data; 
    }
    TACArg(TACArgType type, std::string data) : type(type) {
        if (type == ConstTAC) 
        {
            new (&value) std::string(std::move(data));
        } 
        else if (type == GlobalTAC) 
        {
            new (&name) std::string(std::move(data));
        } 
        else 
        {
            throw std::logic_error("Invalid TACArgType for string constructor");
        }
    }
    ~TACArg() {
        // Manually destroy the active member if it's a string
        if (type == ConstTAC)
        {
            this->value.~basic_string();
        }
        else if (type == GlobalTAC)
        {
            this->name.~basic_string();
        }
    }
};

class TACGen
{
private:
    std::ofstream outFile;
    std::string fileName; 

    std::string tacArgToString(const TACArg & arg);
public:
    TACGen(std::string theFileName);
    ~TACGen();
    void EmitBinaryAssign(const TACArg & x, const TACArg & y, const TACArg & z, BinOp binOp);
    void EmitUnaryAssign(const TACArg & x, const TACArg & y, UnOp unaryOp);
    void EmitCopy(const TACArg & x, const TACArg & y);
    void EmitProcCall(std::string P);
    void EmitPush(const TACArg & a, bool ref);
    void EmitProcHead(std::string P);
    void EmitProcEnd(std::string P);
};

#endif