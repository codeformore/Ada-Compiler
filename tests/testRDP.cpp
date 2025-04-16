#include <RDP.hpp>
#include <gtest/gtest.h>
#include <tuple>

class RDPE2EValid : public testing::TestWithParam<std::string> 
{
protected:
    RDPE2EValid() : rdp(GetParam()) {}

    RDP rdp;
};

TEST_P(RDPE2EValid, validProg)
{
    ASSERT_NO_THROW({
        rdp.Parse();
    });

    std::string fileName = GetParam();
    fileName.replace(fileName.length() - 4, 4, ".tac");
    std::remove(fileName.c_str());
}

INSTANTIATE_TEST_SUITE_P(validProgs, RDPE2EValid,
    testing::Values("tests/inputs/procSyntax/validProg1.ada", "tests/inputs/procSyntax/validProg2.ada", 
                    "tests/inputs/procSyntax/validProg3.ada", "tests/inputs/procSyntax/validProg4.ada",
                    "tests/inputs/procSyntax/validProg5.ada", "tests/inputs/procSemantic/validProg1.ada",
                    "tests/inputs/procSemantic/validProg2.ada", "tests/inputs/procSemantic/validProg3.ada",
                    "tests/inputs/procSemantic/validProg4.ada", "tests/inputs/stmtSyntax/validProg1.ada",
                    "tests/inputs/stmtSyntax/validProg2.ada", "tests/inputs/stmtSyntax/validProg3.ada",
                    "tests/inputs/stmtSyntax/validProg4.ada", "tests/inputs/stmtSyntax/validProg5.ada",
                    "tests/inputs/stmtSyntax/validProg6.ada", "tests/inputs/stmtSyntax/validProg7.ada",
                    "tests/inputs/stmtSyntax/validProg8.ada"
                   ));

class RDPE2EInvalid : public testing::TestWithParam<std::tuple<std::string, std::string>> 
{
protected:
    RDPE2EInvalid() : rdp( std::get<0>(GetParam()) ) {}

    RDP rdp;
};

TEST_P(RDPE2EInvalid, invalidProg)
{
    EXPECT_THROW({
        try
        {
            rdp.Parse();
        }
        catch( const std::runtime_error& e )
        {
            // and this tests that it has the correct message
            ASSERT_STREQ(std::get<1>(GetParam()).c_str(), e.what());
            std::string fileName = std::get<0>(GetParam());
            fileName.replace(fileName.length() - 4, 4, ".tac");
            std::remove(fileName.c_str());
            throw;
        }
    }, std::runtime_error);
}

INSTANTIATE_TEST_SUITE_P(invalidProgs, RDPE2EInvalid,
    testing::Values(std::make_tuple("tests/inputs/procSyntax/invalidProg1.ada", "tests/inputs/procSyntax/invalidProg1.ada:5: Unused Tokens"),
                    std::make_tuple("tests/inputs/procSyntax/invalidProg2.ada", "tests/inputs/procSyntax/invalidProg2.ada:7: Expected colont, but got endt instead."),
                    std::make_tuple("tests/inputs/procSemantic/invalidProg1.ada", "tests/inputs/procSemantic/invalidProg1.ada:6: sum is already defined."),
                    std::make_tuple("tests/inputs/procSemantic/invalidProg2.ada", "tests/inputs/procSemantic/invalidProg2.ada:2: x is already defined."),
                    std::make_tuple("tests/inputs/procSemantic/invalidProg3.ada", "tests/inputs/procSemantic/invalidProg3.ada:3: a is already defined."),
                    std::make_tuple("tests/inputs/procSemantic/invalidProg4.ada", "tests/inputs/procSemantic/invalidProg4.ada:3: test9 did not match the procedure name test8."),
                    std::make_tuple("tests/inputs/stmtSyntax/invalidProg1.ada", "tests/inputs/stmtSyntax/invalidProg1.ada:8: d is not defined."),
                    std::make_tuple("tests/inputs/stmtSyntax/invalidProg2.ada", "tests/inputs/stmtSyntax/invalidProg2.ada:8: d is not defined.")
                   )
);

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}