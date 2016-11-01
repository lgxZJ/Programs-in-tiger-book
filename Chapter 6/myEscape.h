#ifndef MY_ESCAPE_H
#define MY_ESCAPE_H

#include "myAbstractSyntax.h"

/// \brief      find and record escapes in the given expression.
/// \param[in] exp     the expression inside which to look for.
/// \return     this function has no return value.
/// \remark     after called, variable escape information is holded,
///             call Escape_isVarEscape() to get one.
void Escape_findEscape(myExp exp);

/// \brief      check if the given variable is escaped.
/// \param[in] varName   the name of a variable.
/// \return     a bool.
bool Escape_isVarEscaped(mySymbol varName);

/// \brief      get escape environment which is actually a table.
/// \param      this function has no parameters.
/// \return     the environment.
/// \remark     it's a checked runtime error when calling this before
///             any information is entered.
myTable Escape_getEscapeEnvironment(void);

/// \brief      set escape environment which is actually a table.
/// \param      escapeEnv   the environmet to set.
/// \return     this function has no return value.
/// \remark     it's a checked runtime error to pass a NULL environment.
void Escape_setEscapeEnvironment(myTable escapeEnv);

/*
/// \brief      check if the given variable is used ever.
/// \param[in]  varName     the name of the variable.
/// \return     a bool. 
bool Escape_isVarUsed(mySymbol varName);*/

//////////////////////////////////////////////////////////////////////////////

//  todo:
void Escape_findEscape_LValueExp(
    int depth, myLValueExp lValueExp);
void Escape_findEscape_FunctionCallExp(
    int depth, myFunctionCallExp functionCallExp);
void Escape_findEscape_NilExp(
    int depth, myNilExp nilExp);
void Escape_findEscape_IntegerLiteralExp(
    int depth, myIntegerLiteralExp integerLiteralExp);
void Escape_findEscape_StringLiteralExp(
    int depth, myStringLiteralExp stringLiteralExp);
void Escape_findEscape_ParenthesesExp(
    int depth, myParenthesesExp parenthesesExp);
void Escape_findEscape_NoValueExp(
    int depth, myNoValueExp noValueExp);
void Escape_findEscape_VarDec(
    int depth, myVarDec varDec);
void Escape_findEscape_FuncDec(
    int depth, myFuncDec funcDec);
void Escape_findEscape_ArrayCreationExp(
    int depth, myArrayCreationExp arrayCreationExp);
void Escape_findEscape_RecordCreationExp(
    int depth, myRecordCreationExp recordCreationExp);
void Escape_findEscape_ArithmeticExp(
    int depth, myArithmeticExp arithmeticExp);
void Escape_findEscape_SequencingExp(
    int depth, mySequencingExp sequencingExp);
void Escape_findEscape_ForExp(
    int depth, myForExp forExp);
void Escape_findEscape_IfThenElseExp(
    int depth, myIfThenElseExp ifThenElseExp);
void Escape_findEscape_IfThenExp(
    int depth, myIfThenExp ifThenExp);
void Escape_findEscape_ComparisonExp(
    int depth, myComparisonExp comparisonExp);
void Escape_findEscape_BooleanOperateExp(
    int depth, myBooleanOperateExp booleanOperateExp);
void Escape_findEscape_AssignmentExp(
    int depth, myAssignmentExp assignmentExp);
void Escape_findEscape_LetExp(
    int depth, myLetExp letExp);
void Escape_findEscape_WhileExp(
    int depth, myWhileExp whileExp);
void Escape_findEscape_NegationExp(
    int depth, myNegationExp negationExp);

#endif