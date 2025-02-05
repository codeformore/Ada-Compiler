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
#include <algorithm>
#include <cctype>
#include <cmath>

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

void Scanner::GetNextToken()
{
    //Read until eof reached or blank 
    while (currentChar != std::char_traits<char>::eof() && isblank(currentChar))
    {
        inFile.get(currentChar);
    }

    //If we have the eof char, return the eoft token. Otherwise, start processToken
    if (currentChar == std::char_traits<char>::eof())
    {
        Token = eoft;
    }
    else
    {
        processToken();
    }
}

void Scanner::processToken()
{
    //If the currentChar is a letter, process a word token
    if (isalpha(currentChar))
    {
        processWordToken();
    }
    //If currentChar is a digit, process a numerical token
    else if (isdigit(currentChar))
    {
        processNumToken();
    }
    //Otherwise, we will check if currentChar could be a double token
    else
    {
        //Check double token cases
        switch (currentChar)
        {
        //Could be a Comment or addop
        case '-':
            inFile.get(lookAheadChar);
            if (lookAheadChar == '-') 
            {
                processComment();
            }
            else
            {
                processSingleToken();
            }
            break;
        
        //This will be a literal
        case '"':
            processLiteral();
            break;

        //Other double token cases
        case ':':
        case '/':
        case '<':
        case '>':
            processDoubleToken();
            break;

        //Ok, it definitely is a single token or unknown
        default:
            processSingleToken();
            break;
        }
    }
}

void Scanner::processWordToken()
{
    readWord();
    std::string lowerLexeme = Lexeme; 
    std::transform(lowerLexeme.begin(), lowerLexeme.end(), lowerLexeme.begin(),
        [](unsigned char c){ return std::tolower(c); });
    std::map<std::string, TokenT>::const_iterator it;
    for (it = KEYWORDS.begin(); it != KEYWORDS.end(); it++)
    {
        if (it->first == lowerLexeme && isblank(currentChar))
        {
            Token = it->second;
            return;
        }
    }

    if (lowerLexeme == "or") 
    {
        Token = addopt;
    }
    else if (lowerLexeme == "rem" || lowerLexeme == "mod" || lowerLexeme == "and")
    {
        Token = mulopt;
    }
    else
    {
        if (lowerLexeme.length() <= 17)
        {
            Token = idt;
        }
        else
        {
            Token = unknownt;
            throw std::runtime_error("Identifier too long");
        }
    }
}

void Scanner::processNumToken()
{
    readNum();
    ValueR = atof(Lexeme.c_str());
    Value = (int)floor(ValueR);
    Token = numt;
}

void Scanner::readWord()
{
    Lexeme = "";
    Lexeme.push_back(currentChar);
    inFile.get(currentChar);
    while (currentChar != std::char_traits<char>::eof() && 
           (isalnum(currentChar) || currentChar == '_'))
    {
        Lexeme.push_back(currentChar);
        inFile.get(currentChar);
    }
}

void Scanner::readNum()
{
    Lexeme = "";
    Lexeme.push_back(currentChar);
    inFile.get(currentChar);
    while (currentChar != std::char_traits<char>::eof() && isdigit(currentChar))
    {
        Lexeme.push_back(currentChar);
        inFile.get(currentChar);
    }
    if (currentChar == '.')
    {
        Lexeme.push_back(currentChar);
        inFile.get(currentChar);
        while (currentChar != std::char_traits<char>::eof() && isdigit(currentChar))
        {
            Lexeme.push_back(currentChar);
            inFile.get(currentChar);
        }
    }
}

void Scanner::processSingleToken()
{
    Lexeme = ""; 
    Lexeme.push_back(currentChar);
    switch (currentChar)
    {
    case '=':
    case '<':
    case '>':
        Token = relopt;
        break;
    
    case '+':
    case '-':
        Token = addopt;
        break;
    
    case '*':
    case '/':
        Token = mulopt;
        break;
    
    case '(': Token = lpart; break;
    case ')': Token = rpart; break;
    case ',': Token = commat; break;
    case ':': Token = colont; break;
    case ';': Token = semit; break;
    case '.': Token = periodt; break;
    
    default:
        Token = unknownt;
        throw std::runtime_error("Unknown Symbol " + currentChar);
        break;
    }
}

void Scanner::processDoubleToken()
{
    inFile.get(lookAheadChar);
    if (lookAheadChar != '=') 
    {
        processSingleToken();
        currentChar = lookAheadChar;
    }
    else
    {
        Lexeme = "";
        Lexeme.push_back(currentChar);
        Lexeme.push_back(lookAheadChar);
        if (currentChar == ':')
        {
            Token = assignt;
        }
        else
        {
            Token = relopt;
        }
        inFile.get(currentChar);
    }
}

void Scanner::processComment()
{
    inFile.get(currentChar);
    while (currentChar != std::char_traits<char>::eof() && currentChar != '\n')
    {
        inFile.get(currentChar);
    }
    if (currentChar == '\n')
    {
        inFile.get(currentChar);
    }
    GetNextToken();
}

void Scanner::processLiteral()
{
    Literal = "";
    inFile.get(currentChar);
    while (currentChar != std::char_traits<char>::eof() && 
           currentChar != '\n' && 
           currentChar != '"')
    {
        Literal.push_back(currentChar);
        inFile.get(currentChar);
    }
    if (currentChar == '\n' || currentChar == std::char_traits<char>::eof())
    {
        throw std::runtime_error("Unclosed String Literal");
    }
    Lexeme = Literal.substr(0, 17);
    Token = strt;
}
