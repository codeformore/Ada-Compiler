/********************************************************************
*** NAME : Caden Fischer ***
*** CLASS : CSC 446 ***
*** INSTRUCTOR : Dr. George Hamer ***
*********************************************************************
*** FILE: RDP.cpp ***
*** DESCRIPTION : A recursive descent parser for the Ada language subset. ***
*** EXPORTED FUNCTIONALITY : 
* class RDP
*   Parse(string fileName);
********************************************************************/

#include <RDP.hpp>
#include <iostream>
#include <Intel8086Gen.hpp>

void RDP::Parse()
{
    scanner.GetNextToken();
    codeGen->EmitProgStart(""); //TODO: Remove ""
    std::string mainProcName = prog();
    codeGen->EmitProgEnd(mainProcName);
    symTbl.WriteTable(depth); //For assignment 5
    if (scanner.Token != eoft)
    {
        throw std::runtime_error(scanner.FileName + ":" + std::to_string(scanner.LineNum) + ": Unused Tokens");
    }
}

RDP::RDP(std::string fileName) : scanner(fileName) 
{
    codeGen = new Intel8086Gen(fileName);
}

RDP::~RDP()
{
    delete codeGen;
}

void RDP::match(TokenT expected)
{
    if (scanner.Token == expected)
    {
        scanner.GetNextToken();
    }
    else
    {
        throw std::runtime_error(scanner.FileName + ":" + std::to_string(scanner.LineNum) + ": Expected " + TOKEN_NAMES.at(expected) + ", but got " + TOKEN_NAMES.at(scanner.Token) + " instead.");
    }
}

//Implements: 
// Prog -> proct idt {1} {2} Args {3} is DeclarativePart Procedures begint SeqOfStatements {7} endt idt {4} semit
std::string RDP::prog()
{
    match(proct);
    a1_CheckDup(scanner.Lexeme);
    SymTblEntry* newEntry = a2_InsertProc(scanner.Lexeme, scanner.Token);
    match(idt);
    depth++;
    Param* paramList = nullptr;
    args(paramList);
    int sizeOfParams;
    a3_AssignArgListToProc(paramList, newEntry, sizeOfParams);
    match(ist);
    declarativePart(newEntry->procedure.size);
    procedures();
    codeGen->EmitProcHead(newEntry->lexeme);
    match(begint);
    seqOfStatements(newEntry->procedure.size);
    symTbl.WriteTable(depth); //For Assignment 5
    a7_DeleteScope();
    codeGen->EmitProcEnd(newEntry->lexeme, newEntry->procedure.size, sizeOfParams);
    match(endt);
    a4_CheckClosingID(scanner.Lexeme, newEntry->lexeme);
    match(idt);
    match(semit);
    return newEntry->lexeme;
}

//Implements:
// DeclarativePart -> IdentifierList colont TypeMark {5} {9} semit DeclarativePart | o
void RDP::declarativePart(int & size)
{
    if (scanner.Token == idt)
    {
        IdList* idList = nullptr;
        identifierList(idList, false);
        match(colont);
        bool isConst;
        VarConstType theType;
        int value;
        float valueR;
        typeMark(isConst, theType, value, valueR);
        a5_InsertVarsAndConsts(size, idList, isConst, theType, value, valueR, false, false);
        if (depth == 1)
        {
            a14_WriteGlobalVars(idList, isConst, theType, value);
        }
        a9_DeallocateIdentiferList(idList);
        match(semit);
        declarativePart(size);
    }
    //Otherwise, nullable
}

//Implements: 
// IdentifierList -> {8} idt MoreIdentifiers
void RDP::identifierList(IdList* & idList, bool reverse)
{
    std::string idLex = scanner.Lexeme;
    if (!reverse)
    {
        a8_AddIDToList(idList, idLex);
    }
    match(idt);
    moreIdentifiers(idList);
    if (reverse)
    {
        a8_AddIDToList(idList, idLex);
    }
}

//Implements:
// MoreIdentifiers -> commat {8} idt MoreIdentifiers | o
void RDP::moreIdentifiers(IdList* & idList)
{
    if (scanner.Token == commat)
    {
        match(commat);
        a8_AddIDToList(idList, scanner.Lexeme);
        match(idt);
        moreIdentifiers(idList);
    }
    //Otherwise, nullable
}

//Implements:
// TypeMark -> integert | realt | chart | constt assignt Value
void RDP::typeMark(bool & constType, VarConstType & theType, int & value, float & valueR)
{
    //Check each case
    constType = false;
    switch (scanner.Token)
    {
    case integert:
        match(integert);
        theType.varType = IntVar;
        break;
    
    case realt:
        match(realt);
        theType.varType = RealVar;
        break;
    
    case chart:
        match(chart);
        theType.varType = CharVar;
        break;
    
    case constt:
        match(constt);
        match(assignt);
        constType = true;
        this->value(theType, value, valueR);
        break;
    
    //If we didn't find anything, then throw an error
    default:
        throw std::runtime_error(scanner.FileName + ":" + std::to_string(scanner.LineNum) + ": Got " + TOKEN_NAMES.at(scanner.Token) + " but expected one of integert, realt, chart, or constt.");
        break;
    }
}

//Implements:
// Value -> numt
void RDP::value(VarConstType & theType, int & value, float & valueR)
{
    value = scanner.Value;
    valueR = scanner.ValueR;
    if (scanner.Lexeme.find('.') != std::string::npos)
    {
        theType.constType = RealConst;
    }
    else
    {
        theType.constType = IntConst;
    }
    match(numt);
}

//Implements:
// Procedures -> Prog Procedures | o
void RDP::procedures()
{
    if (scanner.Token == proct)
    {
        prog();
        procedures();
    }
    //Otherwise, nullable
}

//Implements:
// Args -> lpart ArgList rpart | o
void RDP::args(Param* & paramList)
{
    if (scanner.Token == lpart)
    {
        match(lpart);
        int size = 0;
        argList(paramList, size);
        match(rpart);
    }
    //Otherwise, nullable
}

//Implements:
// ArgList -> Mode IdentifierList colont TypeMark {5} {6} {9} MoreArgs
void RDP::argList(Param* & paramList, int & size)
{
    ParamMode theMode;
    mode(theMode);
    IdList* idList = nullptr;
    identifierList(idList, true);
    match(colont);
    bool isConst;
    VarConstType theType;
    int value;
    float valueR;
    typeMark(isConst, theType, value, valueR);
    if (isConst)
    {
        throw std::runtime_error(scanner.FileName + ":" + std::to_string(scanner.LineNum) + ": Constant Typed Arguments are not allowed.");
    }
    moreArgs(paramList, size);
    a5_InsertVarsAndConsts(size, idList, isConst, theType, value, valueR, true, (theMode != InMode));
    a6_AddModeAndType(paramList, theMode, theType.varType, idList);
    a9_DeallocateIdentiferList(idList);
}

//Implements:
// MoreArgs -> semit ArgList | o
void RDP::moreArgs(Param* & paramList, int & size)
{
    if (scanner.Token == semit)
    {
        match(semit);
        argList(paramList, size);
    }
    //Otherwise, nullable
}

//Implements:
// Mode -> intok | outt | inoutt | o
void RDP::mode(ParamMode & mode)
{
    switch (scanner.Token)
    {
    case intok:
        match(intok);
        mode = InMode;
        break;
    
    case outt:
        match(outt);
        mode = OutMode;
        break;
    
    case inoutt:
        match(inoutt);
        mode = InoutMode;
        break;
    
    default:
        mode = InMode;
        break;
    }
    //Otherwise, nullable
}

//Implements:
// SeqOfStatements -> Statement semit StatTail | o
void RDP::seqOfStatements(int & size)
{
    if (scanner.Token == idt || scanner.Token == gett || scanner.Token == putt || scanner.Token == putlnt || scanner.Token == semit)
    {
        statement(size);
        match(semit);
        statTail(size);
    }
    //Otherwise, nullable
}

//Implements:
// StatTail -> Statement ; StatTail | o
void RDP::statTail(int & size)
{
    if (scanner.Token == idt || scanner.Token == gett || scanner.Token == putt || scanner.Token == putlnt || scanner.Token == semit)
    {
        statement(size);
        match(semit);
        statTail(size);
    }
    //Otherwise, nullable
}

//Implements
//??????
void RDP::statement(int & size)
{
    if (scanner.Token == idt)
    {
        assignStat(size);
    }
    else if (scanner.Token == gett || scanner.Token == putt || scanner.Token == putlnt)
    {
        IOStat();
    }
    //Otherwise, nullable
}

//Implements:
// AssignStat -> {10} idt assignt Expr | ProcCall
void RDP::assignStat(int & size)
{
    std::string idLexeme = scanner.Lexeme;
    TACArg idArg;
    a10_CheckDefined(idLexeme);
    a11_CreateTACArg(idLexeme, idArg);
    match(idt);
    if (scanner.Token == assignt)
    {
        match(assignt);
        TACArg exprArg;
        expr(size, exprArg);
        codeGen->EmitCopy(idArg, exprArg);
    }
    else if (scanner.Token == lpart)
    {
        procCall(idLexeme);
    }
    else
    {
        throw std::runtime_error(scanner.FileName + ":" + std::to_string(scanner.LineNum) + ": expected one of assignt or lpart, but got " + TOKEN_NAMES.at(scanner.Token) + " instead.");
    }
}

//Implements:
// IOStat -> In_Stat | Out_Stat
void RDP::IOStat()
{
    if (scanner.Token == gett)
    {
        In_Stat();
    }
    else if (scanner.Token == putt || scanner.Token == putlnt)
    {
        Out_Stat();
    }
    else
    {
        throw std::runtime_error(scanner.FileName + ":" + std::to_string(scanner.LineNum) + ": expected one of gett, putt, or putlnt, but got " + TOKEN_NAMES.at(scanner.Token) + " instead.");
    }
}

//Implements:
// Expr -> Relation
void RDP::expr(int & size, TACArg & outArg)
{
    relation(size, outArg);
}

//Implements:
// Relation -> SimpleExpr
void RDP::relation(int & size, TACArg & outArg)
{
    simpleExpr(size, outArg);
}

//Implements:
// SimpleExpr -> Term MoreTerm
void RDP::simpleExpr(int & size, TACArg & outArg)
{
    TACArg outTerm;
    term(size, outTerm);
    moreTerm(outTerm, size, outArg);
}

//Implements:
// MoreTerm -> addopt Term MoreTerm | o
void RDP::moreTerm(TACArg & inArg, int & size, TACArg & outArg)
{
    if (scanner.Token == addopt)
    {
        //Get type of operation
        BinOp theOp;
        a12_GetBinaryOp(scanner.Lexeme, theOp);
        match(addopt);
        TACArg outTerm;
        term(size, outTerm);
        //Create a new temp
        std::string tempName = symTbl.CreateTemp(depth, size);
        //Convert to TACArg
        TACArg tempArg;
        a11_CreateTACArg(tempName, tempArg);
        //Emit operation
        codeGen->EmitBinaryAssign(tempArg, inArg, outTerm, theOp);
        //If new temp is at depth 1, put in data segment
        if (depth == 1)
        {
            codeGen->EmitGlobalVar(false, tempArg, "?");
        }
        //More terms
        moreTerm(tempArg, size, outArg);
    }
    //Otherwise, nullable
    else
    {
        outArg = inArg;
    }
}

//Implements:
// Term -> Factor MoreFactor
void RDP::term(int & size, TACArg & outArg)
{
    TACArg inArg;
    factor(size, inArg);
    moreFactor(inArg, size, outArg);
}

//Implements:
// MoreFactor -> mulopt Factor MoreFactor | o
void RDP::moreFactor(TACArg inArg, int & size, TACArg & outArg)
{
    if (scanner.Token == mulopt)
    {
        //Get the operation
        BinOp theOp;
        a12_GetBinaryOp(scanner.Lexeme, theOp);
        match(mulopt);
        //Get the second factor
        TACArg outFactor;
        factor(size, outFactor);
        //Create a new temp
        std::string tempName = symTbl.CreateTemp(depth, size);
        //Convert to TACArg
        TACArg tempArg;
        a11_CreateTACArg(tempName, tempArg);
        //Emit operation
        codeGen->EmitBinaryAssign(tempArg, inArg, outFactor, theOp);
        //If new temp is at depth 1, put in data segment
        if (depth == 1)
        {
            codeGen->EmitGlobalVar(false, tempArg, "?");
        }
        //More Factor
        moreFactor(tempArg, size, outArg);
    }
    //Otherwise, nullable
    else
    {
        outArg = inArg;
    }
}

//Implements:
// Factor -> idt | numt | lpart Expr rpart | nott Factor | signopt Factor
void RDP::factor(int & size, TACArg & outArg)
{
    TACArg outFactor;
    UnOp theOp;
    std::string tempName;
    switch (scanner.Token)
    {
    case idt:
        a10_CheckDefined(scanner.Lexeme);
        a11_CreateTACArg(scanner.Lexeme, outArg);
        match(idt);
        break;

    case numt:
        outArg = TACArg(ConstTAC, false, scanner.Lexeme);
        match(numt);
        break;

    case lpart:
        match(lpart);
        expr(size, outArg);
        match(rpart);
        break;
    
    case nott:
        throw std::runtime_error(scanner.FileName + ":" + std::to_string(scanner.LineNum) + ": The not operation has not been implemented");
        match(nott);
        factor(size, outArg);
        break;

    case addopt:
        if (scanner.Lexeme != "+" && scanner.Lexeme != "-")
        {
            throw std::runtime_error(scanner.FileName + ":" + std::to_string(scanner.LineNum) + ": " + scanner.Lexeme + " is not allowed as a sign operator.");
        }
        //Get the operation
        a13_GetUnaryOp(scanner.Lexeme, theOp);
        match(addopt);
        //Get the factor
        factor(size, outFactor);
        //Create a new temp
        tempName = symTbl.CreateTemp(depth, size);
        //Convert to TACArg
        a11_CreateTACArg(tempName, outArg);
        //Emit operation
        codeGen->EmitUnaryAssign(outArg, outFactor, theOp);
        //If new temp is at depth 1, put in data segment
        if (depth == 1)
        {
            codeGen->EmitGlobalVar(false, outArg, "?");
        }
        break;
    
    default:
        throw std::runtime_error(scanner.FileName + ":" + std::to_string(scanner.LineNum) + ": Got " + TOKEN_NAMES.at(scanner.Token) + ", but expected one of idt, numt, lpart, nott, addopt.");
        break;
    }
}

//Implements:
// ProcCall -> lpart Params rpart
void RDP::procCall(std::string idLexeme)
{
    //Get entry and check if it is a procedure
    SymTblEntry* entry = symTbl.Lookup(idLexeme);
    if (entry->entryType != Procedure)
    {
        throw std::runtime_error(scanner.FileName + ":" + std::to_string(scanner.LineNum) + ": " + idLexeme + " is not a procedure.");
    }
    match(lpart);
    params(entry->procedure.params);
    match(rpart);

    //Emit the call
    codeGen->EmitProcCall(entry->lexeme);
}

//Implements:
// Params -> idt ParamsTail | num ParamsTail | o
void RDP::params(Param *curParam)
{
    if (scanner.Token == idt)
    {
        if (curParam == nullptr)
        {
            throw std::runtime_error(scanner.FileName + ":" + std::to_string(scanner.LineNum) + ": Too many arguments provided.");
        }
        a10_CheckDefined(scanner.Lexeme);
        //NO TYPE CHECKING OCCURRING
        //Determine passing mode
        bool ref = (curParam->mode != InMode);
        //Create TACArg from param
        TACArg paramArg;
        a11_CreateTACArg(scanner.Lexeme, paramArg);
        //Emit Push
        codeGen->EmitPush(paramArg, ref);
        match(idt);
        paramsTail(curParam->next);
    }
    else if (scanner.Token == numt)
    {
        if (curParam == nullptr)
        {
            throw std::runtime_error(scanner.FileName + ":" + std::to_string(scanner.LineNum) + ": Too many arguments provided.");
        }
        //NO TYPE CHECKING OCCURRING
        //Determine passing mode
        if (curParam->mode != InMode)
        {
            throw std::runtime_error(scanner.FileName + ":" + std::to_string(scanner.LineNum) + ": Cannot use a constant as a inout or out parameter.");
        }
        //Create TACArg from param
        TACArg paramArg = TACArg(ConstTAC, false, scanner.Lexeme);
        //Emit Push
        codeGen->EmitPush(paramArg, false);
        match(numt);
        paramsTail(curParam->next);
    }
    //Otherwise, nullable
}

//Implements:
// ParamsTail -> , idt ParamsTail | , numt ParamsTail | o
void RDP::paramsTail(Param *curParam)
{

    if (scanner.Token != commat)
    {
        return;
    }
    
    match(commat);

    if (scanner.Token == idt)
    {
        if (curParam == nullptr)
        {
            throw std::runtime_error(scanner.FileName + ":" + std::to_string(scanner.LineNum) + ": Too many arguments provided.");
        }
        a10_CheckDefined(scanner.Lexeme);
        //NO TYPE CHECKING OCCURRING
        //Determine passing mode
        bool ref = (curParam->mode != InMode);
        //Create TACArg from param
        TACArg paramArg;
        a11_CreateTACArg(scanner.Lexeme, paramArg);
        //Emit Push
        codeGen->EmitPush(paramArg, ref);
        match(idt);
        paramsTail(curParam->next);
    }
    else if (scanner.Token == numt)
    {
        if (curParam == nullptr)
        {
            throw std::runtime_error(scanner.FileName + ":" + std::to_string(scanner.LineNum) + ": Too many arguments provided.");
        }
        //NO TYPE CHECKING OCCURRING
        //Determine passing mode
        if (curParam->mode != InMode)
        {
            throw std::runtime_error(scanner.FileName + ":" + std::to_string(scanner.LineNum) + ": Cannot use a constant as a inout or out parameter.");
        }
        //Create TACArg from param
        TACArg paramArg = TACArg(ConstTAC, false, scanner.Lexeme);
        //Emit Push
        codeGen->EmitPush(paramArg, false);
        match(numt);
        paramsTail(curParam->next);
    }
}

//Implements:
// In_Stat -> gett lpart IdList rpart
void RDP::In_Stat()
{
    match(gett);
    match(lpart);
    IdList* idList = nullptr;
    identifierList(idList, false);
    TACArg curTacArg;
    IdList* tmp = idList;
    while (tmp != nullptr)
    {
        a11_CreateTACArg(idList->id, curTacArg);
        codeGen->EmitIO(false, false, false, curTacArg);
        tmp = tmp->next;
    }
    a9_DeallocateIdentiferList(idList);
    match(rpart);
}

//Implements:
// Out_Stat -> putt lpart Write_List rpart | putlnt lpart Write_List rpart
void RDP::Out_Stat()
{
    bool line;
    if (scanner.Token == putt)
    {
        line = false;
        match(putt);
    }
    else
    {
        line = true;
        match(putlnt);
    }
    match(lpart);
    Write_List();
    if (line)
    {
        codeGen->EmitIO(true, false, true, TACArg());
    }
    match(rpart);
}

//Implements:
// Write_List -> Write_Token Write_List_Tail
void RDP::Write_List()
{
    Write_Token();
    Write_List_Tail();
}

//Implements:
// Write_List_Tail -> commat Write_Token Write_List_Tail | o
void RDP::Write_List_Tail()
{
    if (scanner.Token == commat)
    {
        match(commat);
        Write_Token();
        Write_List_Tail();
    }
    //Otherwise, nullable
}

//Implements:
// Write_Token -> idt | numt | strt
void RDP::Write_Token()
{
    TACArg outputTACArg;
    if (scanner.Token == idt)
    {
        a11_CreateTACArg(scanner.Lexeme, outputTACArg);
        codeGen->EmitIO(true, false, false, outputTACArg);
        match(idt);
    }
    else if (scanner.Token == numt)
    {
        outputTACArg = TACArg(ConstTAC, false, scanner.Value);
        match(numt);
    }
    else if (scanner.Token == strt)
    {
        int literalNum;
        symTbl.InsertLiteral(scanner.Literal, literalNum);
        outputTACArg = TACArg(LitTAC, false, literalNum);
        codeGen->EmitIO(true, true, false, outputTACArg);
        codeGen->EmitGlobalVar(true, outputTACArg, symTbl.LookupLiteral(literalNum));
        match(strt);
    }
}

void RDP::a1_CheckDup(std::string lexeme)
{
    SymTblEntry* entry = symTbl.Lookup(lexeme);
    if (entry != nullptr && entry->depth == depth)
    {
        throw std::runtime_error(scanner.FileName + ":" + std::to_string(scanner.LineNum) + ": " + lexeme + " is already defined.");
    }
}

SymTblEntry *RDP::a2_InsertProc(std::string lexeme, TokenT token)
{
    symTbl.Insert(lexeme, token, depth);
    SymTblEntry* theNewProc = symTbl.Lookup(lexeme);
    theNewProc->entryType = Procedure;
    return theNewProc;
}

void RDP::a3_AssignArgListToProc(Param *start, SymTblEntry* entry, int & sizeOfParams)
{
    entry->procedure.params = start;
    Param* tmp = entry->procedure.params;
    int numParams = 0;
    sizeOfParams = 0;
    while (tmp != nullptr)
    {
        numParams++;
        if (tmp->type == IntVar)
        {
            sizeOfParams += 2;
        }
        else if (tmp->type == RealVar)
        {
            sizeOfParams += 4;
        }
        else if (tmp->type == CharVar)
        {
            sizeOfParams += 1;
        }
        tmp = tmp->next;
    }
    entry->procedure.numParams = numParams;
}

void RDP::a4_CheckClosingID(std::string lexeme, std::string procName)
{
    if (lexeme != procName)
    {
        throw std::runtime_error(scanner.FileName + ":" + std::to_string(scanner.LineNum) + ": " + lexeme + " did not match the procedure name " + procName + ".");
    }
}

void RDP::a5_InsertVarsAndConsts(int &curSize, IdList *idList, bool isConst, VarConstType type, int value, float valueR, bool posOffset, bool ref)
{
    while (idList != nullptr)
    {
        a1_CheckDup(idList->id);
        symTbl.Insert(idList->id, idt, depth);
        SymTblEntry* theNewEntry = symTbl.Lookup(idList->id);
        if (isConst)
        {
            theNewEntry->entryType = Constant;
            theNewEntry->constant.type = type.constType;
            if (type.constType == IntConst)
            {
                theNewEntry->constant.value = value;
            }
            else
            {
                theNewEntry->constant.valueR = valueR;
            }
        }
        else
        {
            theNewEntry->entryType = Variable;
            theNewEntry->variable.type = type.varType;
            theNewEntry->variable.ref = ref;
            switch (type.varType)
            {
            case IntVar:
                theNewEntry->variable.size = 2;
                break;

            case RealVar:
                theNewEntry->variable.size = 4;
                break;
            
            case CharVar:
                theNewEntry->variable.size = 1;
                break;
            }
            if (posOffset)
            {
                theNewEntry->variable.offset = curSize + theNewEntry->variable.size;
            }
            else
            {
                theNewEntry->variable.offset = -curSize;
            }
            curSize += theNewEntry->variable.size;
        }
        idList = idList->next;
    }
}

void RDP::a6_AddModeAndType(Param* & cur, ParamMode mode, VarType varType, IdList *idList)
{
    while (idList != nullptr)
    {
        Param* theNewNode = new Param;
        theNewNode->mode = mode;
        theNewNode->type = varType;
        if (cur == nullptr)
        {
            cur = theNewNode;
        }
        else
        {
            Param* tmp = cur;
            while (tmp->next != nullptr)
            {
                tmp = tmp->next;
            }
            tmp->next = theNewNode;
        }
        idList = idList->next;
    }
}

void RDP::a7_DeleteScope()
{
    symTbl.DeleteDepth(depth);
    depth--;
}

void RDP::a8_AddIDToList(IdList* & idList, std::string idToAdd)
{
    IdList* theNewItem = new IdList;
    theNewItem->id = idToAdd;
    theNewItem->next = nullptr;

    //Add to the end of the list
    if (idList == nullptr)
    {
        idList = theNewItem;
    }
    else
    {
        IdList* tmp = idList;
        while (tmp->next != nullptr)
        {
            tmp = tmp->next;
        }
        tmp->next = theNewItem;
    }
}

void RDP::a9_DeallocateIdentiferList(IdList* & idList)
{
    IdList* tmp;
    while (idList != nullptr)
    {
        tmp = idList;
        idList = idList->next;
        delete tmp;
    }
}

void RDP::a10_CheckDefined(std::string lexeme)
{
    SymTblEntry* entry = symTbl.Lookup(lexeme);
    if (entry == nullptr)
    {
        throw std::runtime_error(scanner.FileName + ":" + std::to_string(scanner.LineNum) + ": " + lexeme + " is not defined.");
    }
}

void RDP::a11_CreateTACArg(std::string lexeme, TACArg & retTACArg)
{
    SymTblEntry* entry = symTbl.Lookup(lexeme);
    if (entry->entryType == Constant)
    {
        std::string value;
        if (entry->constant.type == RealConst)
        {
            value = std::to_string(entry->constant.valueR);
        }
        else
        {
            value = std::to_string(entry->constant.value);
        }
        retTACArg = TACArg(ConstTAC, false, value);
    }
    else if (entry->entryType == Variable)
    {
        if (entry->depth == 1) //Then it is a global
        {
            retTACArg = TACArg(GlobalTAC, entry->variable.ref, lexeme);
        }
        else //Then it is on the stack
        {
            retTACArg = TACArg(StackTAC, entry->variable.ref, entry->variable.offset);
        }
    }
}

void RDP::a12_GetBinaryOp(std::string opString, BinOp & op)
{
    if (opString == "+")
    {
        op = addBOP;
    }
    else if (opString == "-")
    {
        op = subBOP;
    }
    else if (opString == "*")
    {
        op = mulBOP;
    }
    else if (opString == "/")
    {
        op = divBOP;
    }
    else if (opString == "mod")
    {
        op = modBOP;
    }
    else if (opString == "rem")
    {
        op = remBOP;
    }
    else
    {
        throw std::runtime_error(scanner.FileName + ":" + std::to_string(scanner.LineNum) + ": " + opString + " is not an implemented binary operation.");
    }
}

void RDP::a13_GetUnaryOp(std::string opString, UnOp &op)
{
    if (opString == "+")
    {
        op = posUOP;
    }
    else if (opString == "-")
    {
        op = negUOP;
    }
    else
    {
        throw std::runtime_error(scanner.FileName + ":" + std::to_string(scanner.LineNum) + ": " + opString + " is not an implemented unary operation.");
    }
}

void RDP::a14_WriteGlobalVars(IdList *idList, bool isConst, VarConstType type, int value)
{
    TACArg curArg;
    while (idList != nullptr)
    {
        if (isConst)
        {
            switch (type.constType)
            {
            case IntConst:
                curArg = TACArg(GlobalTAC, false, idList->id);
                codeGen->EmitGlobalVar(false, curArg, std::to_string(value));
                break;
            
            default:
                throw std::runtime_error(scanner.FileName + ":" + std::to_string(scanner.LineNum) + ": Real constants have not been implemented.");
                break;
            }
        }
        else
        {
            switch (type.varType)
            {
            case IntVar:
                curArg = TACArg(GlobalTAC, false, idList->id);
                codeGen->EmitGlobalVar(false, curArg, "?");
                break;
            
            default:
                throw std::runtime_error(scanner.FileName + ":" + std::to_string(scanner.LineNum) + ": Real and Char variables have not been implemented.");
                break;
            }
        }
        idList = idList->next;
    }
}
