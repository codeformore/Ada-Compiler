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

Scanner::~Scanner()
{
    if (inFile.is_open())
    {
        inFile.close();
    }
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
    //First read the rest of the word
    readWord();

    //Make a lowercase version of the Lexeme
    std::string lowerLexeme = Lexeme; 
    std::transform(lowerLexeme.begin(), lowerLexeme.end(), lowerLexeme.begin(),
        [](unsigned char c){ return std::tolower(c); });

    //Check if it is a keyword
    std::map<std::string, TokenT>::const_iterator it;
    for (it = KEYWORDS.begin(); it != KEYWORDS.end(); it++)
    {
        if (it->first == lowerLexeme && isblank(currentChar))
        {
            Token = it->second;
            return;
        }
    }

    //If it is not a keyword, check if it is or
    if (lowerLexeme == "or") 
    {
        Token = addopt;
    }
    //If not, check if it is one of rem mod and
    else if (lowerLexeme == "rem" || lowerLexeme == "mod" || lowerLexeme == "and")
    {
        Token = mulopt;
    }
    //Otherwise, it could be an identifier
    else
    {
        //An identifier must be <= 17 characters
        if (lowerLexeme.length() <= 17)
        {
            Token = idt;
        }
        else
        {
            Token = unknownt;
            throw std::runtime_error("Identifier " + Lexeme + " too long");
        }
    }
}

void Scanner::processNumToken()
{
    //First read the rest of the number
    readNum();

    //Get the numeric values
    ValueR = atof(Lexeme.c_str());
    Value = (int)floor(ValueR);
    Token = numt;
}

void Scanner::readWord()
{
    //Clear Lexeme and add the currentChar
    Lexeme = "";
    Lexeme.push_back(currentChar);

    //Grab chars until we get something besides letters, numbers, and underscores
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
    //Clear Lexeme and add the currentChar
    Lexeme = "";
    Lexeme.push_back(currentChar);

    //Grab chars until we don't have a digit
    inFile.get(currentChar);
    while (currentChar != std::char_traits<char>::eof() && isdigit(currentChar))
    {
        Lexeme.push_back(currentChar);
        inFile.get(currentChar);
    }
    
    //If there is a period, then it is a decimal.
    if (currentChar == '.')
    {
        //Add the period to Lexeme
        Lexeme.push_back(currentChar);

        //Get the decimal part using same loop as the integral part
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
    //Clear Lexeme and add the currentChar
    Lexeme = ""; 
    Lexeme.push_back(currentChar);

    //Check the various cases of currentChar
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
    
    //If nothing matched, then we don't know the token.
    default:
        Token = unknownt;
        throw std::runtime_error("Unknown Symbol " + currentChar);
        break;
    }
}

void Scanner::processDoubleToken()
{
    //Look 1 character ahead
    inFile.get(lookAheadChar);

    //All double tokens have the second token as an =
    //If it is not an equal, then the token could be a single token
    if (lookAheadChar != '=') 
    {
        processSingleToken();
        
        //Since we did not use the lookAheadChar, it becomes the next char
        currentChar = lookAheadChar;
    }
    else
    {
        //Clear Lexeme and add currentChar and lookAheadChar
        Lexeme = "";
        Lexeme.push_back(currentChar);
        Lexeme.push_back(lookAheadChar);

        //Is it an assign token or relational operator token?
        if (currentChar == ':')
        {
            Token = assignt;
        }
        else
        {
            Token = relopt;
        }

        //Since we used the lookAheadChar, we get the next char
        inFile.get(currentChar);
    }
}

void Scanner::processComment()
{
    //Grab characters until EOF or \n
    inFile.get(currentChar);
    while (currentChar != std::char_traits<char>::eof() && currentChar != '\n')
    {
        inFile.get(currentChar);
    }

    //If it is a \n, then get the next character.
    if (currentChar == '\n')
    {
        inFile.get(currentChar);
    }

    //Comment is not a token, see if there is a token next
    GetNextToken();
}

void Scanner::processLiteral()
{
    //Clear the literal
    Literal = "";

    //Grab characters until EOF, \n, or "
    inFile.get(currentChar);
    while (currentChar != std::char_traits<char>::eof() && 
           currentChar != '\n' && 
           currentChar != '"')
    {
        Literal.push_back(currentChar);
        inFile.get(currentChar);
    }

    //If it is either \n or EOF, then we did not close the literal
    if (currentChar == '\n' || currentChar == std::char_traits<char>::eof())
    {
        throw std::runtime_error("Unclosed String Literal");
    }

    //Otherwise, we have a literal.
    Lexeme = Literal.substr(0, 17);
    Token = strt;
}
