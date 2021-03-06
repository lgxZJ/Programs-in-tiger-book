#include "myFrame.h"

#include <assert.h> //  for assert()
#include <stdlib.h> //  for NULL macro

///////////////////////////////////////////////////////////////////
//                          macros
#define RETURN_COUNT()  int count = 0;          \
                                                \
                        while (list)            \
                        {                       \
                            ++count;            \
                            list = list->tail;  \
                        }                       \
                        return count;

#define BASE_SIZE       Frame_wordSize       //  base size of each local var


///////////////////////////////////////////////////////////////////
//                          globals
const int Frame_wordSize = 4;

///////////////////////////////////////////////////////////////////
//                          structures

struct myAccess_
{
    enum AccessKind { InFrame, InReg } kind;
    union
    {
        int     offset; //  offset comapred to frame pointer
        myTemp  reg;    //  register representation
    }u;
};

struct myFrame_
{
    myAccessList    formals;
    myString        viewShift;
    int             localCount;         //  not include `formalLocalCount`
    int             formalLocalCount;   //  in frame formal local count
    myLabel         label;
};

/////////////////////////////////////////////////////////////////////
//                          functions

static myAccess makeInFrameAccess(int offset)
{
    //  4 is the value of BASE_SIZE
    //__asm__ __volatile__ ("subl $4, %esp\n\t");

    myAccess access = makeMemoryBlock(sizeof(*access), MEMORY_TYPE_NONE);
    assert (access);

    access->kind = InFrame;
    access->u.offset=  offset;
    return access;
}

static myAccess makeInRegAccess(myTemp reg)
{
    myAccess access = makeMemoryBlock(sizeof(*access), MEMORY_TYPE_NONE);
    assert (access);

    access->kind = InReg;
    access->u.reg = reg;
    return access;
}

/////////////////////////////////////////

bool Frame_isAccessInFrame(myAccess access)
{
    return access->kind == InFrame;
}

bool Frame_isAccessInReg(myAccess access)
{
    return access->kind == InReg;
}

/////////////////////////////////////////

myBoolList Frame_makeBoolList(myBoolList formers, bool newOne)
{
    myBoolList bools = makeMemoryBlock(sizeof(*bools), MEMORY_TYPE_NONE);
    assert(bools);

    bools->head = newOne;
    bools->tail = formers;
    return bools;
}

myAccessList Frame_makeAccessList(myAccess newOne, myAccessList formers)
{
    myAccessList accesses = makeMemoryBlock(sizeof(*accesses), MEMORY_TYPE_NONE);
    assert(accesses);

    accesses->head = newOne;
    accesses->tail = formers;
    return accesses;
}

int Frame_getBoolListCount(myBoolList list)
{
    RETURN_COUNT();
}

int Frame_getAccessListCount(myAccessList list)
{
    RETURN_COUNT();
}

Frame_myFrag Frame_makeStringFrag(myLabel label, myString str)
{
    Frame_myFrag one = makeMemoryBlock(sizeof(*one), MEMORY_TYPE_NONE);
    assert (one);

    one->kind = Frame_StringFrag;
    one->u.strFrag.label = label;
    one->u.strFrag.str = str;
    return one;
}

Frame_myFrag Frame_makeProcFrag(
    IR_myStatement bodyStatement, myFrame frame, mySymbol funcName)
{
    Frame_myFrag one = makeMemoryBlock(sizeof(*one), MEMORY_TYPE_NONE);
    assert (one);

    one->kind = Frame_ProcFrag;
    one->u.procFrag.body = bodyStatement;
    one->u.procFrag.frame = frame;
    one->u.procFrag.funcName = funcName;
    return one;
}

Frame_myFragList Frame_makeFragList(Frame_myFrag head, Frame_myFragList tail)
{
    Frame_myFragList one = makeMemoryBlock(sizeof(*one), MEMORY_TYPE_NONE);
    assert (one);

    one->head = head;
    one->tail = tail;
    return one;
}

/////////////////////////////////////////////

void fillFormalsFromFlags(myFrame frame, myBoolList formalFlags);
void fillViewShiftFromFlags(myFrame frame);

myFrame Frame_newFrame(myLabel frameLabel, myBoolList formalFlags)
{
    myFrame frame = makeMemoryBlock(sizeof(*frame), MEMORY_TYPE_NONE);
    assert (frame);

    frame->label = frameLabel;
    fillFormalsFromFlags(frame, formalFlags);
    fillViewShiftFromFlags(frame);
    frame->localCount = 0;  //  no include formal locals

    return frame;
}

void fillFormalsFromFlags(myFrame frame, myBoolList formalFlags)
{
    frame->formalLocalCount = 0;
    if (formalFlags == NULL)
        frame->formals = NULL;
    else
        frame->formals = Frame_makeAccessList(NULL, NULL);

    //  use iterations to replace recursive function calls
    myAccessList accessList = frame->formals;
    while (formalFlags)
    {
        myAccess access;

        if (formalFlags->head)
            accessList->head = makeInFrameAccess(-frame->formalLocalCount++ * BASE_SIZE);
        else
            accessList->head = makeInRegAccess(Temp_newTemp());

        formalFlags->tail == NULL ?
            (0) :
            (accessList->tail = Frame_makeAccessList(NULL, NULL));
        accessList = accessList->tail;
        formalFlags = formalFlags->tail;
    }
}

void fillViewShiftFromFlags(myFrame frame)
{
    //  todo: include save callee-save registers
    frame->viewShift = "movl %esp, %ebp\n\t";
}

void Frame_resetFrame(myFrame frame)
{
    assert (frame);
    frame->formals = NULL;
    frame->localCount = 0;
    frame->formalLocalCount = 0;
}

/////////////////////////////////////

bool Frame_isFrameEqual(myFrame lhs, myFrame rhs)
{
    return lhs == rhs;
}

bool Frame_isFrameSame(myFrame lhs, myFrame rhs)
{
    return Frame_isFrameEqual(lhs, rhs) &&
            lhs->formalLocalCount == rhs->formalLocalCount &&
            lhs->formals == rhs->formals &&
            lhs->label == rhs->label &&
            lhs->localCount == rhs->localCount &&
            lhs->viewShift == rhs->viewShift;
}

/////////////////////////////////////

myAccess Frame_allocateLocal(myFrame frame, bool escapeFlag)
{
    if (escapeFlag)
        return makeInFrameAccess(-(++frame->localCount) * BASE_SIZE);
    else    
        return makeInRegAccess(Temp_newTemp());
}

int Frame_getLocalCount(myFrame frame)
{
    assert (frame->localCount >= 0);
    return frame->localCount;
}

int Frame_getformalLocalCount(myFrame frame)
{
    assert (frame->localCount >= 0);
    return frame->formalLocalCount;
}

///////////////////////////////////

myAccessList Frame_getFormals(myFrame frame)
{
    return frame->formals;
}

////////////////////////////////////////////////////////////////////

int Frame_getAccessOffset(myAccess access)
{
    assert (Frame_isAccessInFrame(access));
    return access->u.offset;
}

myTemp Frame_getAccessReg(myAccess access)
{
    assert (Frame_isAccessInReg(access));
    return access->u.reg;
}

/////////////////////////////////////////////////////////////////////
//                              X86 Registers
/////////////////////////////////////////////////////////////////////

#define REG_GETTER(regName ,regGlobal)  \
    static myTemp regGlobal = NULL;     \
    myTemp Frame_##regName(void)          \
    {                                   \
        if (regGlobal == NULL)          \
            regGlobal = Temp_newTemp(); \
        return regGlobal;               \
    }


/////////

REG_GETTER(EAX, g_regEax);
REG_GETTER(EBX, g_regEbx);
REG_GETTER(ECX, g_regEcx);
REG_GETTER(EDX, g_regEdx);

REG_GETTER(ESI, g_regEsi);
REG_GETTER(EDI, g_regEdi);

REG_GETTER(EBP, g_regEbp);


REG_GETTER(ESP, g_regSP);

myTemp Frame_FP(void)
{
    return Frame_EBP();
}

myTemp Frame_RV(void)
{
    return Frame_EAX();
}

myTempList Frame_callerSaveRegs(void)
{
    return Temp_makeTempList(Frame_EAX(),
            Temp_makeTempList(Frame_ECX(),
                Temp_makeTempList(Frame_EDX(), NULL)));
}

myTempList Frame_calleeSaveRegs(void)
{
    return Temp_makeTempList(Frame_EBX(),
            Temp_makeTempList(Frame_EBP(),
                Temp_makeTempList(Frame_ESI(),
                    Temp_makeTempList(Frame_EDI(),
                        Temp_makeTempList(Frame_ESP(), NULL)))));
}

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

IR_myExp Frame_externalCall(myString str, IR_myExpList args)
{
    //  todo:   maybe adjust static link
    //  todo:   push arguments, save registers and so on
    IR_myExp callRet = IR_makeCall(str, args);

    IR_myStatement callstate;
    IR_myExp callValue;
    IR_divideExp(callRet, &callstate, &callValue);

    //  IR_divideExp() ensure callValue is a register representation
    return IR_makeESeq(callstate, callValue);
}

////////////////

IR_myStatement Frame_procEntryExit1(myFrame frame, IR_myStatement statement)
{
    //  todo:   implement this function in the later phase.
    return statement;
}
