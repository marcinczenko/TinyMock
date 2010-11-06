#ifndef TEST_H
#define TEST_H

class ComplexArgument;

class Test
{
public:
	virtual void TestMethod() = 0;
	virtual void TestMethodWithAnArgument(int arg)=0;
	virtual int TestMethodWithReturnValue() = 0;
	virtual void TestMethodWithAPointerArgument(ComplexArgument* p_arg) = 0;
};

#endif
