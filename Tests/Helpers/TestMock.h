#include "Test.h"
#include "TinyMocks.h"

class TestMock : public Test, public TinyMocks::Mock
{
public:    
    TestMock();
	TestMock(const std::string& className);
	void TestMethod();
	void TestMethodWithAnArgument(int arg);
	int TestMethodWithReturnValue();
	void TestMethodWithAPointerArgument(ComplexArgument* p_arg);	
};
