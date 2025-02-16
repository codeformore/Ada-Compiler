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

class RDP
{
private:
    Scanner scanner;

    void match(TokenT expected);
    void prog();
    void declarativePart();
    void identifierList();
    void moreIdentifiers();
    void typeMark();
    void value();
    void procedures();
    void args();
    void argList();
    void moreArgs();
    void mode();
    void seqOfStatements();
public:
    void Parse();
    RDP(std::string fileName);
};

#endif