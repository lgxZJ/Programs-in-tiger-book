#include "../testHelper.h"
#include "debugPrint.h"

#include "../../myTranslate.h"
#include "../../myEnvironment.h"
#include "../../myParser.h"
#include "../../mySemantic.h"

#include <stdlib.h>
#include <assert.h>

//////////////////////     private forwards  //////////////////////
myFrame Trans_getFrame(Trans_myLevel level);

///////////////////////     test helpers    ///////////////////////

int getFormalCountFromLevel(Trans_myLevel level)
{
    CU_ASSERT((bool)level);
    return Trans_getAccessListCount(Trans_getFormals(level));
}

static IR_myExp parseFile(char* filename)
{
    MySemantic_setVarAndFuncEnvironment(myEnvironment_BaseVarAndFunc());
    MySemantic_setTypeEnvironment(myEnvironment_BaseType());

    myExp exp = parseOneFile(filename);
    assert (exp != NULL);
    assert (exp != ABSTRACT_SYNTAX_ERROR);

    myTranslationAndType result = MySemantic_Exp(exp);
    assert (result != SEMANTIC_ERROR);
    return result->translation;
}

///////////////////////        tests       ////////////////////////

void test_TransMakeAccessList_ByDefault_MakeWhatPassed(void)
{
    Trans_myAccess headPassed = (Trans_myAccess)12;
    Trans_myAccessList tailPassed = (Trans_myAccessList)34;

    Trans_myAccessList listGot = Trans_makeAccessList(headPassed, tailPassed);

    CU_ASSERT_EQUAL(headPassed, listGot->head);
    CU_ASSERT_EQUAL(tailPassed, listGot->tail); 
}

///////////////////////

void test_TransGetAccessListCount_ByDefault_WorkRight(void)
{
    Trans_myAccessList twoElementList = Trans_makeAccessList((Trans_myAccess)NULL,
        Trans_makeAccessList((Trans_myAccess)NULL, NULL));

    int countGot = Trans_getAccessListCount(twoElementList);

    CU_ASSERT_EQUAL(2, countGot);
}

///////////////////////

void test_TransIsLevelEquals_ByDefault_WorkRight(void)
{
    Trans_myLevel levelOne = Trans_newLevel(Trans_outermostLevel(),
        (myLabel)34, (myBoolList)NULL);
    Trans_myLevel levelTwo = Trans_newLevel(Trans_outermostLevel(),
        (myLabel)45, (myBoolList)NULL);

    CU_ASSERT(Trans_isLevelEqual(levelOne, levelOne));
    CU_ASSERT(!Trans_isLevelEqual(levelOne, levelTwo));
}

///////////////////////

void test_TransOutermostLevel_ByDefault_ContainsNoFormals(void)
{
    Trans_myLevel outermostLevel = Trans_outermostLevel();

    int formalCount = getFormalCountFromLevel(outermostLevel);
    CU_ASSERT_EQUAL(formalCount, 0);
}

void test_TransOutermostLevel_EveryCall_ReturnSame(void)
{
    Trans_myLevel firstLevel = Trans_outermostLevel();
    Trans_myLevel secondLevel = Trans_outermostLevel();

    CU_ASSERT(Trans_isLevelEqual(firstLevel, secondLevel));
}

///////////////////////

void test_TransNewLevel_EveryCall_ReturnDifferent(void)
{
    Trans_myLevel fakeLevel = Trans_outermostLevel();
    myLabel fakeLabel = (myLabel)34;
    myBoolList fakeFormals = (myBoolList)NULL;
    Trans_myLevel levelOne = Trans_newLevel(fakeLevel, fakeLabel, fakeFormals);
    Trans_myLevel levelTwo = Trans_newLevel(fakeLevel, fakeLabel, fakeFormals);

    CU_ASSERT(!Trans_isLevelEqual(levelOne, levelTwo));
}

void test_TransNewLevel_AfterNewLevel_ParentLevelHoldActualFormals(void)
{
    Trans_myLevel fakeLevel = Trans_outermostLevel();
    myLabel fakeLabel = (myLabel)34;
    myBoolList fakeFormals = Frame_makeBoolList(NULL, true);

    Trans_myLevel levelOne = Trans_newLevel(fakeLevel, fakeLabel, fakeFormals);

    int expectedAccessCount = Frame_getBoolListCount(fakeFormals) + 2;
    CU_ASSERT_EQUAL(
        Frame_getAccessListCount(Frame_getFormals(Trans_getFrame(fakeLevel))),
        expectedAccessCount);
}

///////////////////////

void test_TransAllocateLocal_ByDefault_AccessWithinGivenLevel(void)
{
    Trans_myLevel level = Trans_outermostLevel();

    Trans_myAccess accessOne = Trans_allocateLocal(level, true);
    Trans_myAccess accessTwo = Trans_allocateLocal(level, false);

    CU_ASSERT(Trans_isLevelEqual(level, Trans_getAccessLevel(accessOne)));
    CU_ASSERT(Trans_isLevelEqual(level, Trans_getAccessLevel(accessTwo)));
}

///////////////////////

void test_TransGetFormals_PassOutermostLevel_FormalCountIsZero(void)
{
    Trans_myLevel outermostLevel = Trans_outermostLevel();

    Trans_myAccessList formals = Trans_getFormals(outermostLevel);

    int formalCount = Trans_getAccessListCount(formals);
    CU_ASSERT_EQUAL(formalCount, 0);
}

void test_TransGetFormals_PassOtherLevels_FormalCountEqualsToBoolFlags(void)
{
    myBoolList oneFlags = Frame_makeBoolList(NULL, true);
    myBoolList twoFlags = Frame_makeBoolList(Frame_makeBoolList(NULL, true), true);
    Trans_myLevel levelOne = Trans_newLevel(Trans_outermostLevel(),
        (myLabel)NULL, oneFlags);
    Trans_myLevel levelTwo = Trans_newLevel(Trans_outermostLevel(),
        (myLabel)NULL, twoFlags);

    int formalCountOne = getFormalCountFromLevel(levelOne);
    int formalCountTwo = getFormalCountFromLevel(levelTwo);

    CU_ASSERT_EQUAL(formalCountOne, 1);
    CU_ASSERT_EQUAL(formalCountTwo, 2);
}

/////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

void testOneFileAndShow(char* filename, int flag)
{
    char* testFilePath = "./test-files/";
    char* outputFilePath = "./testOutputs/"; 
    char buffer[100];

    memcpy(buffer, testFilePath, strlen(testFilePath) + 1);
    strcat(buffer, filename);
    IR_myExp result = parseFile(buffer);

    memcpy(buffer, outputFilePath, strlen(outputFilePath) + 1);
    strcat(buffer, filename);
    FILE* file = fopen(buffer, "w");
    assert (file != NULL);

    if (flag == 0)
        printExpression(file, result, 0);
    else if(flag == 1)
        printFragments(file, Trans_getProcFrags());
    else if (flag == -1)
        printFragments(file, Trans_getStringFrags());

    fclose(file);
}

void testOneFileAndOutputCode(char* filename)
{
    testOneFileAndShow(filename, 0);
}

void testOneFileAndOutputStrings(char* filename)
{
    testOneFileAndShow(filename, -1);
}

void testOneFileAndOutputProcs(char* filename)
{
    testOneFileAndShow(filename, 1);
}

void resetTestEnv()
{
    Trans_resetProcFrags();
    Trans_resetStringFrags();
    MySemantic_setTypeEnvironment(myEnvironment_BaseType());
    MySemantic_setVarAndFuncEnvironment(myEnvironment_BaseVarAndFunc());
}

//////////////////////////////////////////////////////////////////////

void test_TransIntegerLiteralExp_ByDefault_SeeOutput(void)
{
    resetTestEnv();
    testOneFileAndOutputCode("integerLiteral.tig");
}

//////////////

void test_TransVarDecSimpleVarInReg_IntConst_SeeOutput(void)
{
    resetTestEnv();
    testOneFileAndOutputCode("varDec_simpleVarInReg_constInt.tig");
}

void test_TransVarDecSimpleVarInFrame_IntConst_SeeOutput(void)
{
    resetTestEnv();
    testOneFileAndOutputCode("varDec_simpleVarInFrame_constInt.tig");
}

void test_TransVarDecSimpleVarInReg_IntVarInReg_SeeOutput(void)
{
    resetTestEnv();
    testOneFileAndOutputCode("varDec_simpleVar_intVarInReg.tig");
}

void test_TransVarDecSimpleVarInReg_IntVarInFrame_SeeOutput(void)
{
    resetTestEnv();
    testOneFileAndOutputCode("varDec_simpleVar_intVarInFrame.tig");
}

//////////////

void test_TransFuncDec_EmptyFunctionFormals_SeeOutput(void)
{
    resetTestEnv();
    testOneFileAndOutputProcs("funcDec_function_emptyFormals.tig");
}

void test_TransFuncDec_EmptyProcedureFormals_SeeOutput(void)
{
    resetTestEnv();
    testOneFileAndOutputProcs("funcDec_procedure_emptyFormals.tig");
}

void test_TransFuncDec_SomeFunctionFormals_SeeOutput(void)
{
    resetTestEnv();
    testOneFileAndOutputProcs("funcDec_function_someFormals.tig");
}

//////////////

void test_TransLetExp_EmptyDecsAndBody_SeeOutput(void)
{
    resetTestEnv();
    testOneFileAndOutputCode("let_emptyBoth.tig");
}

void test_TransLetExp_EmptyDecs_SeeOutput(void)
{
    resetTestEnv();
    testOneFileAndOutputCode("let_emptyDecs.tig");
}

void test_TransLetExp_EmptyExps_SeeOutput(void)
{
    resetTestEnv();
    testOneFileAndOutputCode("let_emptyExps.tig");
}

///////////////////////////         main        ///////////////////////////////

int main()
{
    CU_pSuite suite;
    if (!initialTests(&suite))  return EXIT_FAILURE;

    CU_TestInfo tests[] = {
        { "", test_TransMakeAccessList_ByDefault_MakeWhatPassed },

        { "", test_TransGetAccessListCount_ByDefault_WorkRight },

        { "", test_TransIsLevelEquals_ByDefault_WorkRight },

        { "", test_TransOutermostLevel_ByDefault_ContainsNoFormals },
        { "", test_TransOutermostLevel_EveryCall_ReturnSame },

        { "", test_TransNewLevel_EveryCall_ReturnDifferent },
        { "", test_TransNewLevel_AfterNewLevel_ParentLevelHoldActualFormals },

        { "", test_TransAllocateLocal_ByDefault_AccessWithinGivenLevel },

        { "", test_TransGetFormals_PassOtherLevels_FormalCountEqualsToBoolFlags },
        { "", test_TransGetFormals_PassOutermostLevel_FormalCountIsZero },

        ///////////////////////////////////
        { "", test_TransIntegerLiteralExp_ByDefault_SeeOutput },

        { "", test_TransVarDecSimpleVarInReg_IntConst_SeeOutput },
        { "", test_TransVarDecSimpleVarInFrame_IntConst_SeeOutput },
        { "", test_TransVarDecSimpleVarInReg_IntVarInReg_SeeOutput },
        #error "here"
        /*{ "", test_TransVarDecSimpleVarInReg_IntVarInFrame_SeeOutput },

        { "", test_TransFuncDec_EmptyFunctionFormals_SeeOutput },
        { "", test_TransFuncDec_EmptyProcedureFormals_SeeOutput },
        { "", test_TransFuncDec_SomeFunctionFormals_SeeOutput },

        { "", test_TransLetExp_EmptyDecsAndBody_SeeOutput },
        { "", test_TransLetExp_EmptyDecs_SeeOutput },
        { "", test_TransLetExp_EmptyExps_SeeOutput }*/
    };
    if (!addTests(&suite, tests, sizeof(tests) / sizeof(tests[0])))
        return EXIT_FAILURE;

    if (!runTests(&suite))  return EXIT_FAILURE;
    freeMemoryChain();
    return EXIT_SUCCESS;
}