#include <Scanner.hpp>
#include <gtest/gtest.h>

TEST(ScannerE2E, tokenCoverage)
{
    Scanner scanner("tests/inputs/scannerTokenCoverage.ada");
    for (int tok = 0; tok <= 44; tok++)
    {
        scanner.GetNextToken();
        // std::cout << tok << ": " << scanner.Lexeme << std::endl;

        //Check token attributes if there are any
        if ((TokenT)tok == numt)
        {
            EXPECT_FLOAT_EQ(scanner.ValueR, 3.25f);
            EXPECT_EQ(scanner.Value, 3);
        }
        else if ((TokenT)tok == strt)
        {
            EXPECT_STREQ(scanner.Literal.c_str(), "Hello World!");
        }

        if (tok >= relopt)
        {
            if (tok <= 33)
            {
                EXPECT_EQ(scanner.Token, relopt);
            }
            else if (tok <= 36)
            {
                EXPECT_EQ(scanner.Token, addopt);
            }
            else if (tok <= 41)
            {
                EXPECT_EQ(scanner.Token, mulopt);
            }
            else
            {
                EXPECT_EQ(scanner.Token, (TokenT)(tok-11));
            }
        }
        else
        {
            EXPECT_EQ(scanner.Token, (TokenT)tok);
        }
    }
}

TEST(ScannerE2E, fakeInputFile)
{
    const std::string FAKE_FILE_PATH = "NotReal";
    const std::string ERROR_FAKE_FILE = "Unable to open " + FAKE_FILE_PATH + ".";
    EXPECT_THROW({
        try
        {
            Scanner scannerFail(FAKE_FILE_PATH);
        }
        catch( const std::runtime_error& e )
        {
            // and this tests that it has the correct message
            EXPECT_STREQ(ERROR_FAKE_FILE.c_str(), e.what());
            throw;
        }
    }, std::runtime_error);
}

TEST(ScannerE2E, emptyInputFile)
{
    const std::string FILE_PATH = "tests/inputs/empty.ada";
    const std::string ERROR_STRING = FILE_PATH + " is empty.";
    EXPECT_THROW({
        try
        {
            Scanner scannerFail(FILE_PATH);
        }
        catch( const std::runtime_error& e )
        {
            // and this tests that it has the correct message
            EXPECT_STREQ(ERROR_STRING.c_str(), e.what());
            throw;
        }
    }, std::runtime_error);
}

TEST(ScannerE2E, edgeCases)
{
    const std::string FILE_PATH = "tests/inputs/scannerEdgeCases.ada";
    Scanner scanner(FILE_PATH);

    scanner.GetNextToken(); //Get float
    EXPECT_EQ(scanner.Token, floatt);

    scanner.GetNextToken(); //Get period
    EXPECT_EQ(scanner.Token, periodt);

    scanner.GetNextToken(); //Get length 16 idt
    EXPECT_EQ(scanner.Token, idt);

    scanner.GetNextToken(); //Get length 17 idt
    EXPECT_EQ(scanner.Token, idt);

    EXPECT_THROW({ //Expect length 18 exception
        try
        {
            scanner.GetNextToken();
        }
        catch( const std::runtime_error& e )
        {
            // and this tests that it has the correct message
            EXPECT_STREQ("Identifier abcdefghijklmnopqr too long", e.what());
            throw;
        }
    }, std::runtime_error);

    EXPECT_THROW({ //Expect invalid float exception
        try
        {
            scanner.GetNextToken();
        }
        catch( const std::runtime_error& e )
        {
            // and this tests that it has the correct message
            EXPECT_STREQ("Number with period not followed by decimal part.", e.what());
            throw;
        }
    }, std::runtime_error);

    scanner.GetNextToken(); //Get period token
    EXPECT_EQ(scanner.Token, periodt);

    scanner.GetNextToken(); //Get number token
    EXPECT_EQ(scanner.Token, numt);
    EXPECT_EQ(scanner.Value, 5);

    scanner.GetNextToken(); //Get the empty literal
    EXPECT_EQ(scanner.Token, strt);
    EXPECT_STREQ(scanner.Literal.c_str(), "");

    scanner.GetNextToken(); //Get the eof token
    EXPECT_EQ(scanner.Token, eoft);
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}