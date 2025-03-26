/********************************************************************
*** NAME : Caden Fischer ***
*** CLASS : CSC 446 ***
*** INSTRUCTOR : Dr. George Hamer ***
*********************************************************************
*** FILE: Scanner.hpp ***
*** DESCRIPTION : A scanner for the Ada language subset. ***
*** EXPORTED FUNCTIONALITY : 
* enum EntryType
* enum VarType
* enum ConstType
* enum ParamMode
* struct Param
*   VarType type
*   ParamMode mode
*   Param* next
* struct VarEntry
*   VarType type
*   int offset
*   int size
* struct ConstEntry
*   ConstType type
*   union { int value; float valueR; }
* struct ProcEntry
*   int size
*   int numParams
*   Param* params
* struct SymTblEntry
*   TokenT token
*   std::string lexeme
*   int depth
*   SymTblEntry* next
*   EntryType entryType
*   union { VarEntry variable; ConstEntry constant; ProcEntry procedure; }
* class SymTbl
*   void Insert(std::string lex, TokenT token, int depth)
*   SymTblEntry* Lookup(std::string lex)
*   void DeleteDepth(int depth)
*   void WriteTable(int depth)
********************************************************************/

#include <SymTbl.hpp>
#include <iostream>
#include <iomanip>

//hashpjw from P. J. Weinberger's C compiler
//Some Test Values: 
//  "hi" -> 4
//  "hello" -> 202
int SymTbl::hash(std::string lex)
{
    int h = 0, g;
    for (size_t i = 0; i < lex.length(); i++)
    {
        h = (h << 24) + static_cast<unsigned char>(lex[i]);
        g = h & 0xf0000000;
        if (g != 0)
        {
            h ^= (g >> 24);
            h ^= g;
        }
    }
    return h % TBL_SIZE;
}

void SymTbl::Insert(std::string lex, TokenT token, int depth)
{
    int key = hash(lex);
    SymTblEntry* newEntry = new SymTblEntry;
    newEntry->lexeme = lex;
    newEntry->token = token;
    newEntry->depth = depth;
    newEntry->next = entries[key];
    entries[key] = newEntry;
}

SymTblEntry* SymTbl::Lookup(std::string lex)
{
    int key = hash(lex);
    SymTblEntry* top = entries[key];
    while (top != nullptr && top->lexeme != lex)
    {
        top = top->next;
    }
    return top;
}

//Assuming depth is always the greatest value of depth in the SymTbl
void SymTbl::DeleteDepth(int depth)
{
    SymTblEntry *tmp;
    for (int i = 0; i < TBL_SIZE; i++)
    {
        while (entries[i] != nullptr && entries[i]->depth == depth)
        {
            tmp = entries[i];
            entries[i] = entries[i]->next;
            if (tmp->entryType == Procedure)
            {
                tmp->procedure.~ProcEntry();
            }
            delete tmp;
        }
    }
}

void SymTbl::WriteTable(int depth)
{
    std::cout << std::right <<std::setfill('=') << std::setw(27) << "Depth " << depth 
              << std::setw(20) << "" << std::setfill(' ') << std::endl
              << std::left << std::setw(15) << "Lexeme" << "|"
              << std::setw(15) << "Token" << "|"
              << std::setw(5) << "Depth" << "|"
              << std::setw(10) << "Entry Type" << std::endl
              << std::setfill('-') << std::setw(48) << "" << std::setfill(' ') << std::endl;

    SymTblEntry* top;
    for (int i = 0; i < TBL_SIZE; i++)
    {
        top = entries[i];
        while (top != nullptr)
        {
            if (top->depth == depth)
            {
                std::cout << std::left << std::setw(15) << top->lexeme << "|"
                          << std::setw(15) << TOKEN_NAMES.at(top->token) << "|"
                          << std::setw(5) << top->depth << "|"
                          << std::setw(10) << ENTRYTYPE_NAMES.at(top->entryType);
                
                if (top->entryType == Variable)
                {
                    std::cout << "Offset: " << top->variable.offset 
                              << " Size: " << top->variable.size 
                              << " Type: " << VARTYPE_NAMES.at(top->variable.type) 
                              << std::endl;
                }
                else if (top->entryType == Constant)
                {
                    std::cout << "Type: " << CONSTTYPE_NAMES.at(top->constant.type)
                              << " Value: " << top->constant.value
                              << " ValueR: " << top->constant.valueR
                              << std::endl;
                }
                else
                {
                    std::cout << "Size: " << top->procedure.size
                              << " NumParams: " << top->procedure.numParams
                              << std::endl;
                    Param* tmp = top->procedure.params;
                    if (tmp == nullptr)
                    {
                        std::cout << "    No Parameters" << std::endl;
                    }
                    else
                    {
                        int paramNum = 0;
                        while (tmp != nullptr)
                        {
                            std::cout << "    " << paramNum << ": " << PARAMMODE_NAMES.at(tmp->mode) 
                                      << " - " << VARTYPE_NAMES.at(tmp->type) << std::endl;
                            tmp = tmp->next;
                            paramNum++;
                        }
                    }
                }
            }
            top = top->next;
        }
    }
    std::cout << std::endl;
}

SymTbl::SymTbl()
{
    for (int i = 0; i < TBL_SIZE; i++)
    {
        entries[i] = nullptr;
    }
}

SymTbl::~SymTbl()
{
    SymTblEntry* tmp;
    for (int i = 0; i < TBL_SIZE; i++)
    {
        while (entries[i] != nullptr)
        {
            tmp = entries[i];
            entries[i] = entries[i]->next;
            if (tmp->entryType == Procedure)
            {
                Param* tmpParam;
                while (tmp->procedure.params != nullptr)
                {
                    tmpParam = tmp->procedure.params;
                    tmp->procedure.params = tmp->procedure.params->next;
                    delete tmpParam;
                }
            }
            delete tmp;
        }
    }
}

void ProcEntry::AddParam(VarType type, ParamMode mode)
{
    Param* newParam = new Param;
    newParam->type = type;
    newParam->mode = mode;
    newParam->next = params;
    params = newParam;
}
