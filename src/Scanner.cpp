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

#include <Scanner.hpp>
#include <iostream>
#include <stdexcept>
#include <cctype>

Scanner::Scanner(std::string fileName)
{
    //Open the file
    inFile.open(fileName);
    if (inFile.is_open()) 
    {
        throw std::runtime_error("Unable to open " + fileName + ".");
    }

    //Move to the first non-blank character
    inFile.get(currentChar);
    while (currentChar != std::char_traits<char>::eof() && isblank(currentChar))
    {
        inFile.get(currentChar);
    }

    //If we have the eof char, then we should throw
    if (currentChar == std::char_traits<char>::eof())
    {
        throw std::runtime_error(fileName + " is empty.");
    }

    //If we got here, the file is open and ready to go.
}
