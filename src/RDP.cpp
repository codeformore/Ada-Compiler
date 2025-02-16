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

void RDP::Parse()
{
    scanner.GetNextToken();
    prog();
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
// Prog -> proct idt Args is DeclarativePart Procedures begint SeqOfStatements endt idt semit
void RDP::prog()
{
    match(proct);
    match(idt);
    args();
    match(ist);
    declarativePart();
    procedures();
    match(begint);
    seqOfStatements();
    match(endt);
    match(idt);
    match(semit);
}

//Implements:
// DeclarativePart -> IdentifierList colont TypeMark semit DeclarativePart | o
void RDP::declarativePart()
{
    if (scanner.Token == idt)
    {
        identifierList();
        match(colont);
        typeMark();
        match(semit);
        declarativePart();
    }
    //Otherwise, nullable
}

//Implements: 
// IdentifierList -> idt MoreIdentifiers
void RDP::identifierList()
{
    match(idt);
    moreIdentifiers();
}

//Implements:
// MoreIdentifiers -> commat idt MoreIdentifiers | o
void RDP::moreIdentifiers()
{
    if (scanner.Token == commat)
    {
        match(commat);
        match(idt);
        moreIdentifiers();
    }
    //Otherwise, nullable
}

//Implements:
// TypeMark -> integert | floatt | chart | constt assignt Value
void RDP::typeMark()
{
    //Check each case
    switch (scanner.Token)
    {
    case integert:
        match(integert);
        break;
    
    case floatt:
        match(floatt);
        break;
    
    case chart:
        match(chart);
        break;
    
    case constt:
        match(constt);
        match(assignt);
        value();
        break;
    
    //If we didn't find anything, then throw an error
    default:
        throw std::runtime_error(scanner.FileName + ":" + std::to_string(scanner.LineNum) + ": Got " + TOKEN_NAMES.at(scanner.Token) + " but expected one of integert, floatt, chart, or constt.");
        break;
    }
}

//Implements:
// Value -> numt
void RDP::value()
{
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
void RDP::args()
{
    if (scanner.Token == lpart)
    {
        match(lpart);
        argList();
        match(rpart);
    }
    //Otherwise, nullable
}

//Implements:
// ArgList -> Mode IdentifierList colont TypeMark MoreArgs
void RDP::argList()
{
    mode();
    identifierList();
    match(colont);
    typeMark();
    moreArgs();
}

//Implements:
// MoreArgs -> semit ArgList | o
void RDP::moreArgs()
{
    if (scanner.Token == semit)
    {
        match(semit);
        argList();
    }
    //Otherwise, nullable
}

//Implements:
// Mode -> intok | outt | inoutt | o
void RDP::mode()
{
    switch (scanner.Token)
    {
    case intok:
        match(intok);
        break;
    
    case outt:
        match(outt);
        break;
    
    case inoutt:
        match(inoutt);
        break;
    
    default:
        break;
    }
    //Otherwise, nullable
}

//Implements:
// SeqOfStatements -> o
void RDP::seqOfStatements()
{
    //Nullable
}