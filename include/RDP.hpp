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
    void prog();
    void declarativePart(int & size);
    void identifierList(IdList* & idList);
    void moreIdentifiers(IdList* & idList);
    void typeMark(bool & constType, VarConstType & theType, int & value, float & valueR);
    void value(VarConstType & theType, int & value, float & valueR);
    void procedures();
    void args(Param* & paramList);
    void argList(Param* & paramList);
    void moreArgs(Param* & paramList);
    void mode(ParamMode & mode);
    void seqOfStatements();
    
    //Statement Grammar
    void statTail();
    void statement();
    void assignStat();
    void IOStat();
    void expr();
    void relation();
    void simpleExpr();
    void moreTerm();
    void term();
    void moreFactor();
    void factor();


    //Actions
    void a1_CheckDup(std::string lexeme);
    SymTblEntry* a2_InsertProc(std::string lexeme, TokenT token);
    void a3_AssignArgListToProc(Param* start, SymTblEntry* entry);
    void a4_CheckClosingID(std::string lexeme, std::string procName);
    void a5_InsertVarsAndConsts(int & curSize, IdList* idList, bool isConst, VarConstType type, int value, float valueR);
    void a6_AddModeAndType(Param* & cur, ParamMode mode, VarType varType, IdList* idList);
    void a7_DeleteScope();
    void a8_AddIDToList(IdList* & idList, std::string idToAdd);
    void a9_DeallocateIdentiferList(IdList* & idList);
    void a10_CheckDefined(std::string lexeme);
public:
    void Parse();
    RDP(std::string fileName);
};

#endif