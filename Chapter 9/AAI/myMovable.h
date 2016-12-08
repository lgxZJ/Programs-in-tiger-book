#ifndef MY_MOVABLE_H
#define MY_MOVABLE_H

#include "myAAIs.h"

namespace lgxZJ
{
    namespace IS
    {
        //  Here we use on class to represent abstract assembly language instruction.
        //  So, we need to know when to treat as register values and when as memory
        //  locations pointed to by registers. However, there are too many conditions.
        //  To make things simple, i just use enumerators rather than more subclass to
        //  differ them.
        class Move : public Movable
        {
            public:
                //  Content -> Register
                //  Memory  -> [Register]
                enum OperandType { Content, Memory };

                explicit Move(myTemp dst,myTemp src,
                            OperandType dstType = Content,
                            OperandType srcType = Content);

            private:
                myTemp      dst;
                myTemp      src;
                OperandType dstType;
                OperandType srcType;
        };
    }
}

#endif