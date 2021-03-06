#include "myControlable.h"
#include "../CXX_myFrame.h"
#include "../CXX_myTranslate.h"
#include "../CXX_myEnvironment.h"
#include "../CXX_mySemantic.h"

#include <cassert>

using namespace std;

namespace lgxZJ
{
    namespace IS
    {
        /////////////////////////////////////////////////////////////////////////
        //
        //                              Macros

#define DEFINE_JUMP_LIKE(CLSNAME, CLSSTR)       \
        CLSNAME::CLSNAME(myLabel oneDstLabel)   \
        {                                       \
            dstLabel = oneDstLabel;             \
        }                                       \
                                                \
        string CLSNAME::ToString() const        \
        {                                       \
            return ToCommonString("\t"#CLSSTR); \
        }                                       \
                                                \
        string CLSNAME::ToCode(RegisterMap& map) const\
        {                                       \
            return ToString();                  \
        }

        /////////////////////////////////////////////////////////////////////////
        //                              Label
        /////////////////////////////////////////////////////////////////////////

        Label::Label(myLabel oneLabel) : label(oneLabel)    {}

        string Label::ToString() const
        {
            return string(MySymbol_GetName(label)) + ":";
        }

        string Label::ToCode(RegisterMap& map) const
        {
            return ToString();
        }

        /////////////////////////////////////////////////////////////////////////
        //                              Cmp
        /////////////////////////////////////////////////////////////////////////

        DEFINE_TWOCOMPUTE_CTOR(Cmp);

        string Cmp::ToString() const
        {
            return ToCommonString("\tcmpl");
        }

        string Cmp::ToCode(RegisterMap& map) const
        {
            return ToCommonCode("\tcmpl", map);
        }

        Registers Cmp::GetDstRegs() const
        {
            return {};
        }

        Registers Cmp::GetSrcRegs() const
        {
            Registers result(1, dstReg);

            if (srcRep.kind == BinaryUnion::Kind::Reg)
                result.push_back(srcRep.u.reg);
            
            return result;
        }

        /////////////////////////////////////////////////////////////////////////
        //                              Jmp
        /////////////////////////////////////////////////////////////////////////

        DEFINE_JUMP_LIKE(Jmp, jmp);

        ///////////////////////////////////////////////////////////////////////////
        //                              Je
        ///////////////////////////////////////////////////////////////////////////

        DEFINE_JUMP_LIKE(Je, je);

        ///////////////////////////////////////////////////////////////////////////
        //                              Jne
        ///////////////////////////////////////////////////////////////////////////

        DEFINE_JUMP_LIKE(Jne, jne);

        ///////////////////////////////////////////////////////////////////////////
        //                              jg
        ///////////////////////////////////////////////////////////////////////////

        DEFINE_JUMP_LIKE(Jg, jg);

        //////////////////////////////////////////////////////////////////////////
        //                              jge
        //////////////////////////////////////////////////////////////////////////

        DEFINE_JUMP_LIKE(Jge, jge);

        //////////////////////////////////////////////////////////////////////////
        //                              jl
        //////////////////////////////////////////////////////////////////////////

        DEFINE_JUMP_LIKE(Jl, jl);

        //////////////////////////////////////////////////////////////////////////
        //                              jle
        //////////////////////////////////////////////////////////////////////////

        DEFINE_JUMP_LIKE(Jle, jle);

        /////////////////////////////////////////////////////////////////////////
        //                              Call
        /////////////////////////////////////////////////////////////////////////

        Call::Call(myString oneFuncName, myTempList oneRegList)
            : funcName(oneFuncName), regList(oneRegList)
        {}

        string Call::ToString() const
        {
            myTempList regs = regList;

            string result = "call " + string(funcName) + "(";
            while (regs)
                result += " register" + to_string(Temp_getTempNum(regs->head)) + ",",
                regs = regs->tail;

            result += ")";
            return result;
        }

        string Call::ToCode(RegisterMap& map) const
        {
            string resultStr;

            //  skip the static link(we pass a static link as a parameter, 
            //  but indeed it is not needed)
            assert (regList &&  (regList->head == Frame_EBP()));
            myTempList temps = regList->tail;

            //  push parameter registers in reverse order
            int varCount = 0;
            while (temps)
            {
                ++varCount;
                resultStr = ("\tpushl " + TwoOperandOperate::OneRegToCode(temps->head, map) + "\n")
                            + resultStr;
                temps = temps->tail;
            }

            resultStr += (string("\tcall ") + funcName + "\n");
            resultStr += string("\taddl $") + to_string(varCount * Frame_wordSize) + ", %esp\n";
            return resultStr;
        }

        Registers Call::GetDstRegs() const
        {
            Registers result;
        
            //  todo: ad caller-reg save insuctions
            //  caller-save contains eax register which is also the result value register.
            myTempList temps = Frame_callerSaveRegs();
            while (temps)
                result.push_back(temps->head),
                temps = temps->tail;

            return result;
        }

        Registers Call::GetSrcRegs() const
        {
            Registers result;
        
            myTempList temps = regList;
            while (temps)
                result.push_back(temps->head),
                temps = temps->tail;

            return result;
        }

        void Call::ReplaceReg(Register oldReg, Register newReg)
        {
            while (regList != nullptr)
            {
                if (regList->head == oldReg)
                    regList->head = newReg;
                regList = regList->tail;
            }
        }

        ////////////////////////////////////////////////////////////////////////////////

        #undef DEFINE_JUMP_LIKE
    }
}