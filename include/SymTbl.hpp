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

#ifndef SYMTBL_HPP
#define SYMTBL_HPP

#include <Scanner.hpp>
#include <string>

const int TBL_SIZE = 211;

enum EntryType { Variable, Constant, Procedure };

enum VarType { IntVar, RealVar, CharVar };

enum ConstType { IntConst, RealConst };

enum ParamMode { InMode, OutMode, InoutMode };

struct Param
{
    VarType type;
    ParamMode mode;
    Param* next;
};

struct VarEntry
{
    VarType type;
    int offset;
    int size;
};

struct ConstEntry
{
    ConstType type;
    union
    {
        int value;
        float valueR;
    };
};

struct ProcEntry
{
    int size;
    int numParams;
    Param* params = nullptr;
    void AddParam(VarType type, ParamMode mode);
};

struct SymTblEntry
{
    TokenT token;
    std::string lexeme;
    int depth;
    SymTblEntry* next = nullptr;
    EntryType entryType;
    union
    {
        VarEntry variable;
        ConstEntry constant;
        ProcEntry procedure;
    };
    SymTblEntry() : entryType(Variable), variable{} {}
};

class SymTbl
{
protected:
    SymTblEntry* entries[TBL_SIZE];
    int hash(std::string lex);
public:
    void Insert(std::string lex, TokenT token, int depth);
    SymTblEntry* Lookup(std::string lex);
    void DeleteDepth(int depth);
    void WriteTable(int depth);
    SymTbl();
    ~SymTbl();
};

#endif