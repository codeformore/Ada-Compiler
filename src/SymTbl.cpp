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
#include <cstring>

//hashpjw from P. J. Weinberger's C compiler
//Some Test Values: 
//  "hi" -> 4
//  "hello" -> 202
int SymTbl::hash(std::string lex)
{
    char *s;
    s = new char[lex.length()];
    strncpy(s, lex.c_str(), lex.length());
    int h = 0, g;
    for (int i = 0; i < (int)lex.length(); i++)
    {
        h = (h << 24) + s[i];
        g = h & 0xf0000000;
        if (g != 0)
        {
            h = h ^ (g >> 24);
            h = h ^ g;
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

void SymTbl::WriteTable(int depth)
{
    SymTblEntry* top;
    for (int i = 0; i < TBL_SIZE; i++)
    {
        top = entries[i];
        while (top != nullptr)
        {
            if (top->depth == depth)
            {
                printf("%s\n", top->lexeme.c_str());
            }
            top = top->next;
        }
    }
}

SymTbl::SymTbl()
{
    for (int i = 0; i < TBL_SIZE; i++)
    {
        entries[i] = nullptr;
    }
}
