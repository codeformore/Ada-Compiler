/********************************************************************
*** NAME : Caden Fischer ***
*** CLASS : CSC 446 ***
*** INSTRUCTOR : Dr. George Hamer ***
*********************************************************************
*** FILE: testScanner.cpp ***
*** DESCRIPTION : A test program for the scanner. Program will print
  each token and related attributes to the console. ***
*** ARGUMENTS: inFile - The path to the file for 
  the scanner to read ***
********************************************************************/

#include <Scanner.hpp>
#include <string>
#include <iostream>
#include <iomanip>

/********************************************************************
*** FUNCTION printToken ***
*********************************************************************
*** DESCRIPTION : Prints a token, given the information ***
*** INPUT ARGS : Scanner & scanner - The scanner object to output token info***
*** OUTPUT ARGS : NA ***
*** IN/OUT ARGS : NA ***
*** RETURN : void ***
********************************************************************/
void printToken(Scanner & scanner);

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cout << "usage: " << argv[0] << " fileName" << std::endl;
        return 1;
    }

    Scanner scanner(argv[1]);
    scanner.GetNextToken();
    while (scanner.Token != eoft)
    {
        printToken(scanner);
        try
        {
            scanner.GetNextToken();
        }
        catch(const std::runtime_error& e)
        {
            std::cerr << e.what() << std::endl;
            return 1;
        }
        
    }
    
}


void printToken(Scanner & scanner)
{
    std::cout << "Token: " << std::setw(8) << TOKEN_NAMES.at(scanner.Token) << " | Lexeme: " << std::setw(17) << scanner.Lexeme;
    if (scanner.Token == numt)
    {
        std::cout << " | ValueR: " << std::setw(17) << scanner.ValueR << " | Value: " << std::setw(17) << scanner.Value;
    }
    else if (scanner.Token == strt)
    {
        std::cout << " | Literal: "  << scanner.Literal;
    }
    std::cout << std::endl;
}
