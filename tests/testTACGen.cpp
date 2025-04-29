#include <TACGen.hpp>
#include <gtest/gtest.h>
#include <iostream>

TEST(TACGenE2E, correctness)
{
    TACGen codeGen("test.ada");
    TACArg w(StackTAC, false, 5);
    TACArg x(StackTAC, true, -5);
    TACArg y(GlobalTAC, false, "y");
    TACArg z(ConstTAC, false, "25");

    //Emit Lines
    codeGen.EmitProcHead("main");
    codeGen.EmitBinaryAssign(x, y, z, addBOP);
    codeGen.EmitUnaryAssign(x, y, negUOP);
    codeGen.EmitCopy(x, w);
    codeGen.EmitPush(x, false);
    codeGen.EmitPush(y, true);
    codeGen.EmitProcCall("one");
    codeGen.EmitProcEnd("main");

    //Close the file
    codeGen.~TACGen();

    //Open the file
    std::ifstream inFile;
    inFile.open("test.tac");
    EXPECT_EQ(inFile.is_open(), true);

    //Read Entire File
    std::stringstream buffer;
    buffer << inFile.rdbuf();
    std::string theFile = buffer.str();

    //The File to Match
    std::string toMatch =
    "PROC    main            \n"
    "        *_BP-5            =  y                 +  25              \n"
    "        *_BP-5            =    -y               \n"
    "        *_BP-5            =  _BP+5           \n"
    "        push    *_BP-5          \n"
    "        push    &y              \n"
    "        call    one             \n"
    "ENDP    main            \n";

    //Check equality
    EXPECT_STREQ(theFile.c_str(), toMatch.c_str());

    //Delete File
    std::remove("test.tac");
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}