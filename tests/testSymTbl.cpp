#include <SymTbl.hpp>
#include <gtest/gtest.h>

//A version of the SymTbl class to test the hash function
class TestableSymTbl : public SymTbl {
public:
    using SymTbl::hash;  // Expose for testing
};

//Create test fixture
class SymTblTest : public ::testing::Test {
protected:
    TestableSymTbl symTbl;  // Test fixture's member

    // Optional setup function (runs before each test)
    void SetUp() override {
        SymTblEntry* cur;
        symTbl.Insert("first", idt, 1);
        cur = symTbl.Lookup("first");
        cur->entryType = Variable;
        symTbl.Insert("second", idt, 1);
        cur = symTbl.Lookup("second");
        cur->entryType = Constant;
        symTbl.Insert("first", idt, 2);
        cur = symTbl.Lookup("first");
        cur->entryType = Procedure;
        cur->procedure.AddParam(CharVar, InoutMode);
        cur->procedure.AddParam(IntVar, InMode);
        cur->procedure.numParams = 2;
        cur->procedure.size = 4;
    }

    void TearDown() override {
        symTbl = TestableSymTbl();
    }
};

TEST_F(SymTblTest, testHash)
{
    EXPECT_EQ(symTbl.hash("hi"), 4);
    EXPECT_EQ(symTbl.hash("hello"), 209); //is this correct?
}

TEST_F(SymTblTest, testLookup)
{
    SymTblEntry* cur = symTbl.Lookup("first");
    EXPECT_STREQ(cur->lexeme.c_str(), "first");
    EXPECT_EQ(cur->depth, 2);
    EXPECT_EQ(cur->entryType, Procedure);
    EXPECT_EQ(cur->token, idt);
    EXPECT_EQ(cur->procedure.params->mode, InMode);
    EXPECT_EQ(cur->procedure.params->type, IntVar);
    EXPECT_EQ(cur->procedure.params->next->mode, InoutMode);
    EXPECT_EQ(cur->procedure.params->next->type, CharVar); 
}

TEST_F(SymTblTest, testInsert)
{
    symTbl.Insert("second", idt, 2);
    SymTblEntry* cur = symTbl.Lookup("second");
    EXPECT_STREQ(cur->lexeme.c_str(), "second");
    EXPECT_EQ(cur->token, idt);
    EXPECT_EQ(cur->depth, 2);
}

TEST_F(SymTblTest, testDelete)
{
    symTbl.DeleteDepth(2);
    SymTblEntry* cur = symTbl.Lookup("first");
    EXPECT_EQ(cur->depth, 1);
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}