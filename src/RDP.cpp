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

void RDP::Parse()
{
    scanner.GetNextToken();
    prog();
    symTbl.WriteTable(depth); //For assignment 5
    if (scanner.Token != eoft)
    {
        throw std::runtime_error(scanner.FileName + ":" + std::to_string(scanner.LineNum) + ": Unused Tokens");
    }
}

RDP::RDP(std::string fileName) : scanner(fileName) {}

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
void RDP::prog()
{
    match(proct);
    a1_CheckDup(scanner.Lexeme);
    SymTblEntry* newEntry = a2_InsertProc(scanner.Lexeme, scanner.Token);
    match(idt);
    depth++;
    Param* paramList = nullptr;
    args(paramList);
    a3_AssignArgListToProc(paramList, newEntry);
    match(ist);
    declarativePart(newEntry->procedure.size);
    procedures();
    match(begint);
    seqOfStatements();
    symTbl.WriteTable(depth); //For Assignment 5
    a7_DeleteScope();
    match(endt);
    a4_CheckClosingID(scanner.Lexeme, newEntry->lexeme);
    match(idt);
    match(semit);
}

//Implements:
// DeclarativePart -> IdentifierList colont TypeMark {5} {9} semit DeclarativePart | o
void RDP::declarativePart(int & size)
{
    if (scanner.Token == idt)
    {
        IdList* idList = nullptr;
        identifierList(idList);

        //Debug
        // IdList* tmp = idList;
        // while (tmp != nullptr)
        // {
        //     std::cout << tmp->id << std::endl;
        //     tmp = tmp->next;
        // }
        // if (idList == nullptr)
        // {
        //     std::cout << "PROBLEM" << std::endl;
        // }

        match(colont);
        bool isConst;
        VarConstType theType;
        int value;
        float valueR;
        typeMark(isConst, theType, value, valueR);
        a5_InsertVarsAndConsts(size, idList, isConst, theType, value, valueR);
        a9_DeallocateIdentiferList(idList);
        match(semit);
        declarativePart(size);
    }
    //Otherwise, nullable
}

//Implements: 
// IdentifierList -> {8} idt MoreIdentifiers
void RDP::identifierList(IdList* & idList)
{
    a8_AddIDToList(idList, scanner.Lexeme);
    match(idt);
    moreIdentifiers(idList);
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
        argList(paramList);
        match(rpart);
    }
    //Otherwise, nullable
}

//Implements:
// ArgList -> Mode IdentifierList colont TypeMark {5} {6} {9} MoreArgs
void RDP::argList(Param* & paramList)
{
    ParamMode theMode;
    mode(theMode);
    IdList* idList = nullptr;
    identifierList(idList);
    match(colont);
    int size = 0;
    bool isConst;
    VarConstType theType;
    int value;
    float valueR;
    typeMark(isConst, theType, value, valueR);
    a5_InsertVarsAndConsts(size, idList, isConst, theType, value, valueR);
    if (isConst)
    {
        throw std::runtime_error(scanner.FileName + ":" + std::to_string(scanner.LineNum) + ": Constant Typed Arguments are not allowed.");
    }
    a6_AddModeAndType(paramList, theMode, theType.varType, idList);
    a9_DeallocateIdentiferList(idList);
    moreArgs(paramList);
}

//Implements:
// MoreArgs -> semit ArgList | o
void RDP::moreArgs(Param* & paramList)
{
    if (scanner.Token == semit)
    {
        match(semit);
        argList(paramList);
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
void RDP::seqOfStatements()
{
    if (scanner.Token == idt || scanner.Token == semit)
    {
        statement();
        match(semit);
        statTail();
    }
    //Otherwise, nullable
}

//Implements:
// StatTail -> Statement ; StatTail | o
void RDP::statTail()
{
    if (scanner.Token == idt || scanner.Token == semit)
    {
        statement();
        match(semit);
        statTail();
    }
    //Otherwise, nullable
}

void RDP::statement()
{
    if (scanner.Token == idt)
    {
        assignStat();
    }
    else 
    {
        IOStat();
    }
}

//Implements:
// AssignStat -> {10} idt assignt Expr
void RDP::assignStat()
{
    a10_CheckDefined(scanner.Lexeme);
    match(idt);
    match(assignt);
    expr();
}

//Implements:
// IOStat -> o
void RDP::IOStat()
{
    //Nullable
}

//Implements:
// Expr -> Relation
void RDP::expr()
{
    relation();
}

//Implements:
// Relation -> SimpleExpr
void RDP::relation()
{
    simpleExpr();
}

//Implements:
// SimpleExpr -> Term MoreTerm
void RDP::simpleExpr()
{
    term();
    moreTerm();
}

//Implements:
// MoreTerm -> addopt Term MoreTerm | o
void RDP::moreTerm()
{
    if (scanner.Token == addopt)
    {
        match(addopt);
        term();
        moreTerm();
    }
    //Otherwise, nullable
}

//Implements:
// Term -> Factor MoreFactor
void RDP::term()
{
    factor();
    moreFactor();
}

//Implements:
// MoreFactor -> mulopt Factor MoreFactor | o
void RDP::moreFactor()
{
    if (scanner.Token == mulopt)
    {
        match(mulopt);
        factor();
        moreFactor();
    }
    //Otherwise, nullable
}

void RDP::factor()
{
    switch (scanner.Token)
    {
    case idt:
        a10_CheckDefined(scanner.Lexeme);
        match(idt);
        break;

    case numt:
        match(numt);
        break;

    case lpart:
        match(lpart);
        expr();
        match(rpart);
        break;
    
    case nott:
        match(nott);
        factor();
        break;

    case addopt:
        if (scanner.Lexeme != "+" || scanner.Lexeme != "-")
        {
            throw std::runtime_error(scanner.FileName + ":" + std::to_string(scanner.LineNum) + ": " + scanner.Lexeme + " is not allowed as a sign operator.");
        }
        match(addopt);
        factor();
    
    default:
        throw std::runtime_error(scanner.FileName + ":" + std::to_string(scanner.LineNum) + ": Got " + TOKEN_NAMES.at(scanner.Token) + ", but expected one of idt, numt, lpart, nott, addopt.");
        break;
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

void RDP::a3_AssignArgListToProc(Param *start, SymTblEntry* entry)
{
    entry->procedure.params = start;
    Param* tmp = entry->procedure.params;
    int numParams = 0;
    while (tmp != nullptr)
    {
        numParams++;
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

void RDP::a5_InsertVarsAndConsts(int &curSize, IdList *idList, bool isConst, VarConstType type, int value, float valueR)
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
            theNewEntry->variable.offset = curSize;
            theNewEntry->variable.type = type.varType;
            switch (type.varType)
            {
            case IntVar:
                theNewEntry->variable.size = 2;
                break;

            case RealVar:
                theNewEntry->variable.size = 4;
            
            case CharVar:
                theNewEntry->variable.size = 1;
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
