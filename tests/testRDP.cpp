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
    EXPECT_NO_THROW({
        rdp.Parse();
    });
}

INSTANTIATE_TEST_SUITE_P(validProgs, RDPE2EValid,
    testing::Values("tests/inputs/procSyntax/validProg1.ada", "tests/inputs/procSyntax/validProg2.ada", 
                    "tests/inputs/procSyntax/validProg3.ada", "tests/inputs/procSyntax/validProg4.ada",
                    "tests/inputs/procSyntax/validProg5.ada"));

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
            EXPECT_STREQ(std::get<1>(GetParam()).c_str(), e.what());
            throw;
        }
    }, std::runtime_error);
}

INSTANTIATE_TEST_SUITE_P(invalidProgs, RDPE2EInvalid,
    testing::Values(std::make_tuple("tests/inputs/procSyntax/invalidProg1.ada", "tests/inputs/procSyntax/invalidProg1.ada:5: Unused Tokens"),
                    std::make_tuple("tests/inputs/procSyntax/invalidProg2.ada", "tests/inputs/procSyntax/invalidProg2.ada:7: Expected colont, but got endt instead."))
);

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}