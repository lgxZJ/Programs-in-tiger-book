#ifndef MY_MOVABLE_H
#define MY_MOVABLE_H

#include "myAaiBase.h"

namespace lgxZJ
{
    namespace IS
    {
        //  Here we use on class to represent abstract assembly language instruction.
        //  So, we need to know when to treat as register values and when as memory
        //  locations pointed to by registers. However, there are too many conditions.
        //  To make things simple, i just use enumerators rather than more subclass to
        //  differ them.
        class Move : public AAI
        {
            public:
                //  Content -> Register
                //  Memory  -> [Register]
                enum OperandType { Content, Memory, None };

                explicit Move(myTemp oneDstReg, myTemp oneSrcReg,
                            OperandType oneDstType = Content,
                            OperandType oneSrcType = Content);
                explicit Move(myTemp oneDstReg, OperandType oneDstType, int constValue);
                explicit Move(myTemp oneDstReg, myLabel stringLabel);

                //  The Move AAI have the following special syntax:
                //      1.mov reg, reg
                //      2.mov [reg], reg
                //      3.mov reg, [reg]
                //      4.mov [reg], reg
                //  So, the arithmetic semantics are also special:
                //      when registers represent memory location, they are source registers
                //      but not destination registers.
                virtual Registers GetDstRegs() const;
                virtual Registers GetSrcRegs() const;

                void ReplaceReg(Register oldReg, Register newReg);

                virtual std::string ToString() const;
                virtual std::string ToCode(RegisterMap& map) const;

            private:
                std::string OneOperandTo(OperandType type, std::string operandRegStr) const;
                std::string OneOperandToString(myTemp operand, OperandType type) const;
                std::string OneOperandToCode(myTemp operand, OperandType type, RegisterMap& map) const;

            protected:
                myTemp      dstReg;
                OperandType dstType;

                //  source operand can only be one of the two things: label or srcRep
                //  since BinaryUnion is common to all TwoOperandOperate bug label is
                //  unique to Move, we simply add a label member here.
                bool        isLabel;
                myLabel     stringLabel;

                BinaryUnion srcRep;
                OperandType srcType;
        };
    }
}

#endif