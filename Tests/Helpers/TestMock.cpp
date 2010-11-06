#include "TestMock.h"
#include "ComplexArgument.h"

TestMock::TestMock()
    : Test(), TinyMock::Mock()
{
}

TestMock::TestMock(const std::string& className)
    : Test(), TinyMock::Mock(className)
{
}

void TestMock::TestMethod()
{
    TinyMock::Method<void,void,void,void,void> actual("TestMethod");
	TinyMock::BaseMethod* expected = m_expectations.GetFirstExpectationFor(actual.Signature());
	
	Handle(expected,(TinyMock::BaseMethod*)&actual);
}

void TestMock::TestMethodWithAnArgument(int arg)
{
        //TinyMock::MethodIgnoringArguments<void> actual("TestMethodWithAnArgument");
	TinyMock::Method<int,void,void,void,void> actual("TestMethodWithAnArgument",arg);
	TinyMock::BaseMethod* expected = m_expectations.GetFirstExpectationFor(actual.Signature());
	
	Handle(expected,(TinyMock::BaseMethod*)&actual);
}

//void TestMock::TestMethodWithAnArgumentIgnoringArguments(const ComplexArgument& arg)
//{
//	TinyMock::MethodIgnoringArguments<void> actual("TestMethodWithAnArgument");
//	TinyMock::BaseMethod* expected = m_expectations.GetFirstExpectationFor(actual.Signature());
//	
//	Handle(expected,(TinyMock::BaseMethod*)&actual);
//}

int TestMock::TestMethodWithReturnValue()
{
	TinyMock::Method<void,void,void,void,int> actual("TestMethodWithReturnValue",0);
	TinyMock::BaseMethod* expected = m_expectations.GetFirstExpectationFor(actual.Signature());
	
	int ret = ((TinyMock::Method<void,void,void,void,int>*)expected)->m_r ;

	Handle(expected,(TinyMock::BaseMethod*)&actual);

	return ret ;
}

void TestMock::TestMethodWithAPointerArgument(ComplexArgument* p_arg)
{
	TinyMock::MethodWithDereferencedArguments<ComplexArgument*,void,void,void,void> actual("TestMethodWithAPointerArgument",p_arg);
	TinyMock::BaseMethod* expected = m_expectations.GetFirstExpectationFor(actual.Signature());	

	Handle(expected,(TinyMock::BaseMethod*)&actual);
}
