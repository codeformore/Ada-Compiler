#include <SymTbl.hpp>
#include <gtest/gtest.h>

TEST(SymTblTest, Destructors)
{
    SymTbl symTbl;
    symTbl.Insert("test", idt, 1);
    symTbl.Insert("anotherTest", idt, 1);
    symTbl.Insert("anotherTest", idt, 1);
    symTbl.WriteTable(1);
    SymTblEntry* test = symTbl.Lookup("anotherTest");
    test->entryType = Procedure;
    test->procedure.numParams = 1;
    test->procedure.size = 4;
    test->procedure.params = new Param;
    test->procedure.params->mode = InMode;
    test->procedure.params->next = nullptr;
    test->procedure.params->type = RealVar;
    Param* another = new Param;
    another->mode = OutMode;
    another->next = test->procedure.params;
    another->type = CharVar;
    test->procedure.params = another;
    symTbl.DeleteDepth(1);
    symTbl.WriteTable(1);
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}