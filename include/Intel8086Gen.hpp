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

#ifndef INTEL8086GEN_HPP
#define INTEL8086GEN_HPP

#include <CodeGen.hpp>
#include <fstream>

class Intel8086Gen : public CodeGen
{
private:
    std::ofstream outFile;
    std::string fileName; 

    std::string tacArgToString(const TACArg & arg);
public:
    Intel8086Gen(std::string theFileName);
    ~Intel8086Gen();
    void EmitBinaryAssign(const TACArg & x, const TACArg & y, const TACArg & z, BinOp binOp) override;
    void EmitUnaryAssign(const TACArg & x, const TACArg & y, UnOp unaryOp) override;
    void EmitCopy(const TACArg & x, const TACArg & y) override;
    void EmitProcCall(std::string P) override;
    void EmitPush(const TACArg & a, bool ref) override;
    void EmitProcHead(std::string P) override;
    void EmitProcEnd(std::string P) override;
    void EmitIO(bool write, bool string, bool line, const TACArg &x) override;
    void EmitProgStart(std::string P) override;
    void EmitProgEnd(std::string P) override;
};

#endif