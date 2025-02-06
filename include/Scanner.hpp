/********************************************************************
*** NAME : Caden Fischer ***
*** CLASS : CSC 446 ***
*** INSTRUCTOR : Dr. George Hamer ***
*********************************************************************
*** FILE: Scanner.hpp ***
*** DESCRIPTION : A scanner for the Ada language subset. ***
*** EXPORTED FUNCTIONALITY : 
* enum TokenT
* std::map<std::string, TokenT> KEYWORDS
* class Scanner
*   TokenT Token;
*   std::string Lexeme;
*   int Value;
*   float ValueR;
*   std::string Literal;
*   void GetNextToken();
*   Scanner(std::string fileName);
********************************************************************/

#ifndef SCANNER_HPP
#define SCANNER_HPP

#include <string>
#include <map>
#include <fstream>

enum TokenT
{
    begint, modulet, constt, proct, ist, ift, thent,
    elset, elseift, whilet, loopt, floatt, integert, chart,
    gett, putt, endt, lpart, rpart, commat, colont,
    idt, numt, strt, assignt, relopt, addopt, mulopt,
    semit, periodt, eoft, unknownt
};

const std::map<std::string, TokenT> KEYWORDS = {
    {"begin", begint}, {"module", modulet}, {"constant", constt}, {"procedure", proct}, {"is", ist},
    {"if", ift}, {"then", thent}, {"else", elset}, {"elseif", elseift}, {"while", whilet},
    {"loop", loopt}, {"float", floatt}, {"integer", integert}, {"char", chart}, {"get", gett},
    {"put", putt}, {"end", endt}
};

const std::map<TokenT, std::string> TOKEN_NAMES = {
    {begint, "begint"}, {modulet, "modulet"}, {constt, "constantt"}, {proct, "proct"}, {ist, "ist"},
    {ift, "ift"}, {thent, "thent"}, {elset, "elset"}, {elseift, "elseift"}, {whilet, "whilet"},
    {loopt, "loopt"}, {floatt, "floatt"}, {integert, "integert"}, {chart, "chart"}, {gett, "gett"},
    {putt, "putt"}, {endt, "endt"}, {lpart, "lpart"}, {rpart, "rpart"}, {commat, "commat"},
    {colont, "colont"}, {idt, "idt"}, {numt, "numt"}, {strt, "strt"}, {assignt, "assignt"},
    {relopt, "relopt"}, {addopt, "addopt"}, {mulopt, "mulopt"}, {semit, "semit"}, {periodt, "periodt"}, 
    {eoft, "eoft"}, {unknownt, "unknownt"}
};

class Scanner
{
private:
    char currentChar;
    char lookAheadChar;
    std::ifstream inFile;

    void processToken();
    void processWordToken();
    void processNumToken();
    void readWord();
    void readNum();
    void processSingleToken();
    void processDoubleToken();
    void processComment();
    void processLiteral();
public:
    TokenT Token;
    std::string Lexeme;
    int Value;
    float ValueR;
    std::string Literal;

    Scanner(std::string fileName);
    ~Scanner();
    void GetNextToken();
};

#endif