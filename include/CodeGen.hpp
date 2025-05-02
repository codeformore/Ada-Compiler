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
* class CodeGen (pure virtual base class to be inherited by TACGen and Intel8086Gen)
********************************************************************/

#ifndef CODEGEN_HPP
#define CODEGEN_HPP

#include <string>
#include <map>
#include <stdexcept>

enum BinOp { addBOP, subBOP, orBOP, mulBOP, divBOP, modBOP, remBOP, andBOP };
const std::map<BinOp, std::string> BINOP_NAMES = {
    {addBOP, "addition"}, {subBOP, "subtraction"}, {orBOP, "or"}, {mulBOP, "multiplication"},
    {divBOP, "division"}, {modBOP, "modulus"}, {remBOP, "remainder"}, {andBOP, "and"}
};

enum UnOp { posUOP, negUOP, notUOP };
const std::map<UnOp, std::string> UNOP_NAMES = {
    {posUOP, "positive"}, {negUOP, "negative"}, {notUOP, "not"}
};

enum TACArgType { StackTAC, ConstTAC, GlobalTAC, LitTAC };
const std::map<TACArgType, std::string> TACARGTYPE_NAMES = {
    {StackTAC, "Stack"}, {ConstTAC, "Constant"}, {GlobalTAC, "Global"}, {LitTAC, "Literal"}
};

struct TACArg
{
    TACArgType type;
    bool ref;
    union
    {
        int offset;
        std::string value;
        std::string name;
        int literalNum;
    };
    TACArg& operator=(const TACArg& toCopy) {
        if (this != &toCopy) {
            this->~TACArg();
            new (this) TACArg(toCopy);  // placement new copy
        }
        return *this;
    }
    TACArg(const TACArg& toCopy) : type(toCopy.type), ref(toCopy.ref) {
        if (type == StackTAC) {
            offset = toCopy.offset;
        } else if (type == ConstTAC) {
            new (&value) std::string(toCopy.value);
        } else if (type == GlobalTAC) {
            new (&name) std::string(toCopy.name);
        } else {
            literalNum = toCopy.literalNum;
        }
    }
    TACArg(TACArgType type = StackTAC, bool ref = false, int data = 0) : type(type), ref(ref) { 
        if (type == StackTAC) {
            this->offset = data;
        } else if (type == LitTAC) {
            this->literalNum = data;
        } else {
            throw std::logic_error("Invalid TACArgType for int constructor");
        }
        this->offset = data; 
    }
    TACArg(TACArgType type, bool ref, std::string data) : type(type), ref(ref) {
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

class CodeGen {
public:
    virtual ~CodeGen() = default;
    virtual void EmitBinaryAssign(const TACArg & x, const TACArg & y, const TACArg & z, BinOp binOp) = 0;
    virtual void EmitUnaryAssign(const TACArg & x, const TACArg & y, UnOp unaryOp) = 0;
    virtual void EmitCopy(const TACArg & x, const TACArg & y) = 0;
    virtual void EmitProcCall(std::string P) = 0;
    virtual void EmitPush(const TACArg & a, bool ref) = 0;
    virtual void EmitProcHead(std::string P) = 0;
    virtual void EmitProcEnd(std::string P) = 0;
    virtual void EmitIO(bool write, bool string, bool line, const TACArg &x) = 0;
    virtual void EmitProgStart(std::string P) = 0;
    virtual void EmitProgEnd(std::string P) = 0;
};

#endif