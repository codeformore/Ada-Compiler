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
#include <map>

const int TBL_SIZE = 211;

enum EntryType { Variable, Constant, Procedure };
const std::map<EntryType, std::string> ENTRYTYPE_NAMES = {
    {Variable, "Variable"}, {Constant, "Constant"}, {Procedure, "Procedure"}
};

enum VarType { IntVar, RealVar, CharVar };
const std::map<VarType, std::string> VARTYPE_NAMES = {
    {IntVar, "IntVar"}, {RealVar, "RealVar"}, {CharVar, "CharVar"}
};

enum ConstType { IntConst, RealConst };
const std::map<ConstType, std::string> CONSTTYPE_NAMES = {
    {IntConst, "IntConst"}, {RealConst, "RealConst"}
};

enum ParamMode { InMode, OutMode, InoutMode };
const std::map<ParamMode, std::string> PARAMMODE_NAMES = {
    {InMode, "InMode"}, {OutMode, "OutMode"}, {InoutMode, "InoutMode"}
};

union VarConstType
{
    VarType varType;
    ConstType constType;
};

struct IdList
{
    std::string id;
    IdList* next = nullptr;
};

struct Param
{
    VarType type;
    ParamMode mode;
    Param* next = nullptr;
};

struct VarEntry
{
    VarType type;
    int offset;
    int size;
    bool ref;
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
    int curTemp;
public:
    std::string CreateTemp(int depth, int & size);
    void Insert(std::string lex, TokenT token, int depth);
    SymTblEntry* Lookup(std::string lex);
    void DeleteDepth(int depth);
    void WriteTable(int depth);
    SymTbl();
    ~SymTbl();
};

#endif