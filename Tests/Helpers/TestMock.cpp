#include "TestMock.h"
#include "ComplexArgument.h"

TestMock::TestMock()
    : Test(), TinyMocks::Mock()
{
}

TestMock::TestMock(const std::string& className)
    : Test(), TinyMocks::Mock(className)
{
}

void TestMock::TestMethod()
{
    TinyMocks::Method<void,void,void,void,void> actual("TestMethod");
	TinyMocks::BaseMethod* expected = m_expectations.GetFirstExpectationFor(actual.Signature());
	
	Handle(expected,(TinyMocks::BaseMethod*)&actual);
}

void TestMock::TestMethodWithAnArgument(int arg)
{
	//TinyMocks::MethodIgnoringArguments<void> actual("TestMethodWithAnArgument");
	TinyMocks::Method<int,void,void,void,void> actual("TestMethodWithAnArgument",arg);
	TinyMocks::BaseMethod* expected = m_expectations.GetFirstExpectationFor(actual.Signature());
	
	Handle(expected,(TinyMocks::BaseMethod*)&actual);
}

//void TestMock::TestMethodWithAnArgumentIgnoringArguments(const ComplexArgument& arg)
//{
//	TinyMocks::MethodIgnoringArguments<void> actual("TestMethodWithAnArgument");
//	TinyMocks::BaseMethod* expected = m_expectations.GetFirstExpectationFor(actual.Signature());
//	
//	Handle(expected,(TinyMocks::BaseMethod*)&actual);
//}

int TestMock::TestMethodWithReturnValue()
{
	TinyMocks::Method<void,void,void,void,int> actual("TestMethodWithReturnValue",0);
	TinyMocks::BaseMethod* expected = m_expectations.GetFirstExpectationFor(actual.Signature());
	
	int ret = ((TinyMocks::Method<void,void,void,void,int>*)expected)->m_r ;

	Handle(expected,(TinyMocks::BaseMethod*)&actual);

	return ret ;
}

void TestMock::TestMethodWithAPointerArgument(ComplexArgument* p_arg)
{
	TinyMocks::MethodWithDereferencedArguments<ComplexArgument*,void,void,void,void> actual("TestMethodWithAPointerArgument",p_arg);
	TinyMocks::BaseMethod* expected = m_expectations.GetFirstExpectationFor(actual.Signature());	

	Handle(expected,(TinyMocks::BaseMethod*)&actual);
}
