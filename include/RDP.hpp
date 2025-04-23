/********************************************************************
*** NAME : Caden Fischer ***
*** CLASS : CSC 446 ***
*** INSTRUCTOR : Dr. George Hamer ***
*********************************************************************
*** FILE: RDP.hpp ***
*** DESCRIPTION : A recursive descent parser for the Ada language subset. ***
*** EXPORTED FUNCTIONALITY : 
* class RDP
*   Parse(string fileName);
********************************************************************/

#ifndef RDP_HPP
#define RDP_HPP

#include <Scanner.hpp>
#include <SymTbl.hpp>
#include <TACGen.hpp>

class RDP
{
private:
    Scanner scanner;
    SymTbl symTbl;
    TACGen codeGen;
    int depth = 0;

    void match(TokenT expected);

    //RDP Parsing
    //Proc Grammar
    std::string prog();
    void declarativePart(int & size);
    void identifierList(IdList* & idList);
    void moreIdentifiers(IdList* & idList);
    void typeMark(bool & constType, VarConstType & theType, int & value, float & valueR);
    void value(VarConstType & theType, int & value, float & valueR);
    void procedures();
    void args(Param* & paramList);
    void argList(Param* & paramList, int & size);
    void moreArgs(Param* & paramList, int & size);
    void mode(ParamMode & mode);
    void seqOfStatements(int & size);
    
    //Statement Grammar
    void statTail(int & size);
    void statement(int & size);
    void assignStat(int & size);
    void IOStat();
    void expr(int & size, TACArg & outArg);
    void relation(int & size, TACArg & outArg);
    void simpleExpr(int & size, TACArg & outArg);
    void moreTerm(TACArg & inArg, int & size, TACArg & outArg);
    void term(int & size, TACArg & outArg);
    void moreFactor(TACArg inArg, int & size, TACArg & outArg);
    void factor(int & size, TACArg & outArg);
    void procCall(std::string idLexeme);
    void params(Param* curParam);
    void paramsTail(Param* curParam);

    //Actions
    void a1_CheckDup(std::string lexeme);
    SymTblEntry* a2_InsertProc(std::string lexeme, TokenT token);
    void a3_AssignArgListToProc(Param* start, SymTblEntry* entry);
    void a4_CheckClosingID(std::string lexeme, std::string procName);
    void a5_InsertVarsAndConsts(int &curSize, IdList *idList, bool isConst, VarConstType type, int value, float valueR, bool posOffset);
    void a6_AddModeAndType(Param* & cur, ParamMode mode, VarType varType, IdList* idList);
    void a7_DeleteScope();
    void a8_AddIDToList(IdList* & idList, std::string idToAdd);
    void a9_DeallocateIdentiferList(IdList* & idList);
    void a10_CheckDefined(std::string lexeme);
    void a11_CreateTACArg(std::string lexeme, TACArg & retTACArg);
    void a12_GetBinaryOp(std::string opString, BinOp & op);
    void a13_GetUnaryOp(std::string opString, UnOp & op);
public:
    void Parse();
    RDP(std::string fileName);
};

#endif