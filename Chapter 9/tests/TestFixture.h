#include "../AAI/myAaiBase.h"

template <typename T>
class TwoComputeMocker : public T
{
    public:
        explicit TwoComputeMocker(T t) : T(t)    {}

        myTemp GetDstReg()  { return T::dstReg; }
        myTemp GetSrcReg()  { return T::srcRep.u.reg; }
        int    GetSrcValue(){ return T::srcRep.u.value; }
        lgxZJ::IS::BinaryUnion::Kind GetSrcType() { return T::srcRep.kind; }
};

template <typename T>
class OneComputeMocker : public T
{
    public:
        explicit OneComputeMocker(T t) : T(t)    {}

        myTemp GetSrcReg()  { return T::srcRep.u.reg; }
        int    GetSrcValue(){ return T::srcRep.u.value; }
        lgxZJ::IS::BinaryUnion::Kind GetSrcType() { return T::srcRep.kind; }
};

//////////////////////////////////////////////////////////////////////
//                          test helpers
template <typename T>
void testTwoOperandCtor_BothOperandsReg_SetWhatPassed()
{
    myTemp dst = Temp_newTemp();
    myTemp src = Temp_newTemp();
    T add(dst, src);

    TwoComputeMocker<T> addMocker(add);
    CPPUNIT_ASSERT_EQUAL(dst, addMocker.GetDstReg());
    CPPUNIT_ASSERT_EQUAL(lgxZJ::IS::BinaryUnion::Kind::Reg, addMocker.GetSrcType());
    CPPUNIT_ASSERT_EQUAL(src, addMocker.GetSrcReg());
}
template <typename T>
void testTwoOperandCtor_LeftRegRightConstValue_SetWhatPassed()
{
    myTemp dst = Temp_newTemp();
    int srcValue = 1;
    T one(dst, srcValue);

    TwoComputeMocker<T> oneMocker(one);
    CPPUNIT_ASSERT_EQUAL(dst, oneMocker.GetDstReg());
    CPPUNIT_ASSERT_EQUAL(lgxZJ::IS::BinaryUnion::Kind::Value, oneMocker.GetSrcType());
    CPPUNIT_ASSERT_EQUAL(srcValue, oneMocker.GetSrcValue());
}

template <typename T>
void testOneOperandCtor_RegSource_SetWhatPassed()
{
    myTemp src = Temp_newTemp();
    T one(src);

    OneComputeMocker<T> oneMocker(one);
    CPPUNIT_ASSERT_EQUAL(lgxZJ::IS::BinaryUnion::Kind::Reg, oneMocker.GetSrcType());
    CPPUNIT_ASSERT_EQUAL(src, oneMocker.GetSrcReg());
}

template <typename T>
void testOneOperandCtor_ValueSource_SetWhatPassed()
{
    int srcValue;
    T one(srcValue);

    OneComputeMocker<T> oneMocker(one);
    CPPUNIT_ASSERT_EQUAL(lgxZJ::IS::BinaryUnion::Kind::Value, oneMocker.GetSrcType());
    CPPUNIT_ASSERT_EQUAL(srcValue, oneMocker.GetSrcValue());
}
