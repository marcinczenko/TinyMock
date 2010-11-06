#include <iostream>
using namespace std;
#include <memory>

#include "yaffut.h"
#include "TinyMocks.h"
using namespace TinyMocks;

#include "TestMock.h"
#include "TestNotifier.h"
#include "ComplexArgument.h"
#include "ConcreteNotifier.h"

class LoggerInterface
{
public:
	virtual void log()=0;
};

class MockLogger : public LoggerInterface, public TinyMocks::Mock
{
public:
	MockLogger() {}
	MockLogger(const std::string& className) : TinyMocks::Mock(className) {}
	void log()
	{
		TinyMocks::Method<void,void,void,void,void> actual("log");
		TinyMocks::BaseMethod* expected = m_expectations.GetFirstExpectationFor(actual.Signature());
	
		Handle(expected,(TinyMocks::BaseMethod*)&actual);
	}
};


class OurTestClass
{
public:
	OurTestClass(LoggerInterface* logger) : _logger(logger) {}
	void methodA()
	{
		_logger->log();
	}
private:
	LoggerInterface* _logger ;
};


class StandardNotifier : public TinyMocks::TinyNotifier
{
public:
	class MockFailureException {};
	StandardNotifier() {}
	void Send(bool status=true)
	{
		throw MockFailureException();
	}
	void ResetNotificationFlag()
	{
		sendWasCalled = false ;
	}
	bool sendWasCalled ;
};

class YaffutFailureNotifier : public TinyMocks::TinyNotifier
{
public:	
	YaffutFailureNotifier() {}
	void Send(bool status=true)
	{
		FAIL("");
	}	
};

class ExceptionFailureNotifier : public TinyMocks::TinyNotifier
{
public:	
	ExceptionFailureNotifier() {}
	void Send(bool status=true)
	{
		throw exception();
	}	
};


struct TestTinyMocks
{
    TestTinyMocks()
    {        
    }
	
    ~TestTinyMocks()
    {        
    }
};

TEST(TestTinyMocks,TestIfANotifierRegisteredWithAnExpectationIsCalled)
{
	ConcreteNotifier notifier ;

	MockRepository<YaffutFailureNotifier> mockRepository ;

	TestMock* testMock = dynamic_cast<TestMock*>(mockRepository.CreateMock<TestMock,ConcreteNotifier>("TestMock"));	

	testMock->RegisterExpectation(new TinyMocks::Method<void,void,void,void,void>("TestMethod")).AddNotifier<ExceptionFailureNotifier>();

	try
	{
		testMock->TestMethod();
	}
	catch (exception)
	{
		mockRepository.verifyAll();
		return;
	}
	FAIL("");
}

TEST(TestTinyMocks,TestReturningArguments)
{	
	MockRepository<YaffutFailureNotifier> mockRepository ;

	TestMock* testMock = dynamic_cast<TestMock*>(mockRepository.CreateMock<TestMock,YaffutFailureNotifier>("TestMock"));	

	testMock->RegisterExpectation(new TinyMocks::Method<void,void,void,void,int>("TestMethodWithReturnValue",125));

	EQUAL(125,testMock->TestMethodWithReturnValue());

	mockRepository.verifyAll();
}

//TEST(TestTinyMocks,TestIgnoringArguments)
//{
//	TestMock testMock("TestMock");	
//
//	ArgumentWithoutEqualityOperator arg ;	
//
//	testMock.RegisterExpectation(new TinyMocks::MethodIgnoringArguments<void>("TestMethodWithAnArgument"));
//
//	testMock.TestMethodWithAnArgument(arg);
//
//	if(testMock.UnhandledExpectations())
//	{
//		FAIL("");
//	}
//}

TEST(TestTinyMocks,TestFailureNotifier)
{
	const int argValue = 25 ;

	MockRepository<YaffutFailureNotifier> mockRepository;

	TestMock* testMock = dynamic_cast<TestMock*>(mockRepository.CreateMock<TestMock, ExceptionFailureNotifier>("TestMock") );

	testMock->RegisterExpectation(new TinyMocks::Method<int,void,void,void,void>("TestMethodWithAnArgument",argValue));

	try
	{
	   testMock->TestMethodWithAnArgument(argValue-1);
	}
	catch(exception)
	{
		mockRepository.verifyAll();
		return;
	}
	FAIL("");
}

TEST(TestTinyMocks,TestIfFailureNotifierIsCalledOnlyWithTheFirstFailingExpectation)
{
	const int argValue = 25 ;

	MockRepository<YaffutFailureNotifier> mockRepository;

	TestMock* testMock = dynamic_cast<TestMock*>(mockRepository.CreateMock<TestMock, ExceptionFailureNotifier>("TestMock") );

	testMock->RegisterExpectation(new TinyMocks::Method<int,void,void,void,void>("TestMethodWithAnArgument",argValue));
	testMock->RegisterExpectation(new TinyMocks::Method<int,void,void,void,void>("TestMethodWithAnArgument",argValue));

	try
	{
	   testMock->TestMethodWithAnArgument(argValue-1);
	}
	catch(exception)
	{
		try
		{
			testMock->TestMethodWithAnArgument(argValue-1);
		}
		catch(exception)
		{
			FAIL("PIES");
		}
        mockRepository.verifyAll();
		return;
	}
	FAIL("");
}

TEST(TestTinyMocks,TestArgumentDereference)
{
    ComplexArgument complexArg(151) ;	
	ComplexArgument complexArg2(151) ;		

	MockRepository<YaffutFailureNotifier> mockRepository;

	TestMock* testMock = dynamic_cast<TestMock*>(mockRepository.CreateMock<TestMock, YaffutFailureNotifier>("TestMock") );

	testMock->RegisterExpectation(new TinyMocks::MethodWithDereferencedArguments<ComplexArgument*,void,void,void,void>("TestMethodWithAPointerArgument",&complexArg2));
	
	testMock->TestMethodWithAPointerArgument(&complexArg);

	mockRepository.verifyAll();
}

TEST(TestTinyMocks,SequentialCheckingIfAMethodForWhichAllCallsShouldBeIgnoredIsIgnoredShouldReturnTrue)
{
	TinyMocks::IgnoredMethodsContainer ignoredMethodContainer ;

	ignoredMethodContainer.ignoreAll("AMethod") ;

	// we use 4 calls - it should pass even if you add more calls - the number of calls should not matter
	CHECK(ignoredMethodContainer.isIgnored("AMethod")) ;
	CHECK(ignoredMethodContainer.isIgnored("AMethod")) ;
	CHECK(ignoredMethodContainer.isIgnored("AMethod")) ;
	CHECK(ignoredMethodContainer.isIgnored("AMethod")) ;
}

TEST(TestTinyMocks,TestIfAMethodWhichWasNotMarkedAsIgnoredWillBeTreatedAsSuch)
{
	TinyMocks::IgnoredMethodsContainer ignoredMethodContainer ;	
	
	CHECK(!ignoredMethodContainer.isIgnored("AMethod")) ;
}

TEST(TestTinyMocks,AsADevelopperIWantToBeAbleToIgnoreAllCallsToACertainMethod)
{
	MockRepository<YaffutFailureNotifier> mockRepository;

	TestMock* testMock = dynamic_cast<TestMock*>(mockRepository.CreateMock<TestMock, YaffutFailureNotifier>("TestMock") );

	testMock->IgnoreAll("TestMethod");

	testMock->RegisterExpectation(new TinyMocks::Method<void,void,void,void,void>("TestMethod")).AddNotifier<YaffutFailureNotifier>();

	testMock->TestMethod();
	testMock->TestMethod();
	
	mockRepository.verifyAll();
}

TEST(TestTinyMocks,WhenIgnoringAllCallsToAMethodYouDoNotNeedToRegisterExpectationForThisMethodCall)
{
	MockRepository<YaffutFailureNotifier> mockRepository;

	TestMock* testMock = dynamic_cast<TestMock*>(mockRepository.CreateMock<TestMock, YaffutFailureNotifier>("TestMock") );

	testMock->IgnoreAll("TestMethod");	

	testMock->TestMethod();
	testMock->TestMethod();
	
	mockRepository.verifyAll();
}

TEST(TestTinyMocks,IgnoringArgumentsInACallToASpecificMethod)
{
	MockRepository<YaffutFailureNotifier> mockRepository;

	TestMock* testMock = dynamic_cast<TestMock*>(mockRepository.CreateMock<TestMock, YaffutFailureNotifier>("TestMock") );

	const int doesNotMatter = 0 ;	
	const int actualArgument = 255 ;

	testMock->RegisterExpectation(new TinyMocks::Method<int,void,void,void,void>("TestMethodWithAnArgument",doesNotMatter)).ignoreArguments();	
	
	testMock->TestMethodWithAnArgument(actualArgument);

	mockRepository.verifyAll();
}

TEST(TestTinyMocks,IgnoringArgumentsInACallToASpecificMethodButNotIgnoringThemForTheSubsequentCalls)
{
	MockRepository<YaffutFailureNotifier> mockRepository;

	TestMock* testMock = dynamic_cast<TestMock*>(mockRepository.CreateMock<TestMock, ExceptionFailureNotifier>("TestMock") );

	const int doesNotMatter = 0 ;
	const int expectedArgument = 1 ;
	const int actualArgument = 255 ;

	testMock->RegisterExpectation(new TinyMocks::Method<int,void,void,void,void>("TestMethodWithAnArgument",doesNotMatter)).ignoreArguments();
	testMock->RegisterExpectation(new TinyMocks::Method<int,void,void,void,void>("TestMethodWithAnArgument",expectedArgument));

	try
	{
		testMock->TestMethodWithAnArgument(actualArgument);
	}
	catch(std::exception)
	{
		FAIL("");
	}

	try
	{
		testMock->TestMethodWithAnArgument(actualArgument);
	}
	catch(std::exception)
	{
		mockRepository.verifyAll();
		return ;
	}
	FAIL("");	
}

TEST(TestTinyMocks,TestIfAMethodIsCalled)
{
	MockRepository<YaffutFailureNotifier> mockRepository;
	MockLogger* logger = dynamic_cast<MockLogger*>(mockRepository.CreateMock<MockLogger, ExceptionFailureNotifier>("Logger") );
	logger->RegisterExpectation(new TinyMocks::Method<void,void,void,void,void>("log"));

	OurTestClass testClass(logger) ;

	testClass.methodA() ;

	mockRepository.verifyAll();
}



/*
TEST(TestTinyMocks,AsADevelopperIWantToBeAbleToIgnoreASpecificCallSoThatICanSaveSomeTimeWritingPreciseExpectationForThisCall)
{
}


TEST(TestTinyMocks,AsADevelopperIWantToBeAbleToIgnoreTheFirstCallToACertainMethod)
{
}

TEST(TestTinyMocks,AsADevelopperIWantToBeAbleToIgnoreTheFirstNCallsToACertainMethod)
{
}

TEST(TestTinyMocks,AsADevelopperIWantToBeAbleToIgnoreAllTheMethodArguments)
{
}

TEST(TestTinyMocks,AsADevelopperIWantToBeAbleToIgnoreTheSelectedMethodArgument)
{
}

TEST(TestTinyMocks,AsADevelopperIWantToBeAbleToUseTheDefaultNotifierSoThatIDoNotHaveToRegisterItEveryTime)
{
}
*/

//TEST(TestTinyMocks,TestBoostSharedPointers)
//{
//	std::tr1::shared_ptr<ConcreteNotifier> concreteNotifierPtr(new ConcreteNotifier());
//	//boost::shared_ptr<ConcreteNotifier> concreteNotifierPtr(new ConcreteNotifier());
//}


//TEST(TestTinyMocks,TestWithoutIgnoringArguments)
//{
//	TestMock testMock("TestMock");
//
//	ComplexArgument arg ;	
//
//	testMock.RegisterExpectation(new TinyMocks::Method<const ComplexArgument&,void,void,void,void>("TestMethod",arg));	
//
//	testMock.TestMethodWithAnArgument(arg);
//
//	if(testMock.UnhandledExpectations())
//	{
//		FAIL("");
//	}
//}
