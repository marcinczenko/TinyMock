#include "Test.h"
#include "TinyMock.h"

class TestMock : public Test, public TinyMock::Mock
{
public:    
    TestMock();
	TestMock(const std::string& className);
	void TestMethod();
	void TestMethodWithAnArgument(int arg);
	int TestMethodWithReturnValue();
	void TestMethodWithAPointerArgument(ComplexArgument* p_arg);	
};
