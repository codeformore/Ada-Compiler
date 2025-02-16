/********************************************************************
*** NAME : Caden Fischer ***
*** CLASS : CSC 446 ***
*** INSTRUCTOR : Dr. George Hamer ***
*********************************************************************
*** FILE: main.cpp ***
*** DESCRIPTION : The main program. ***
********************************************************************/

#include <iostream>
#include <RDP.hpp>

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cout << "usage: " << argv[0] << " fileName" << std::endl;
        return 1;
    }

    try
    {
        RDP rdp(argv[1]);
        rdp.Parse();
    }
    catch(const std::runtime_error& e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    
}