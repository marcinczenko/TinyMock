#include <iostream>
using namespace std;

#include "yaffut.h"
#include "TinyMock.h"
using namespace TinyMock;

#include "ConcreteNotifier.h"
#include "TestMock.h"

struct TestMockRepository
{
    TestMockRepository()
    {        
    }
	
    ~TestMockRepository()
    {        
    }
};

TEST(TestMockRepository,TestCheckingExpectationForAllCreatedMocks)
{
	ConcreteNotifier failureNotifier ;

	MockRepository<> mockRepository ;	

	Mock* testMock_1 = mockRepository.CreateMock<TestMock,ConcreteNotifier>("TestMock_1");
        testMock_1->RegisterExpectation(new TinyMock::Method<void,void,void,void,void>("TestMethod"));

	const int argValue = 10;	
	Mock* testMock_2 = mockRepository.CreateMock<TestMock,ConcreteNotifier>("TestMock_2");
        testMock_2->RegisterExpectation(new TinyMock::Method<int,void,void,void,void>("TestMethodWithAnArgument",argValue));

	mockRepository.verifyAll(failureNotifier);

	CHECK(failureNotifier.sendWasCalled);

	failureNotifier.ResetNotificationFlag();

	mockRepository.verifyAll(failureNotifier);

	CHECK(!failureNotifier.sendWasCalled);	
}

TEST(TestMockRepository,TestReturningFALSEFromTheVerifyAllMethod)
{	
	MockRepository<> mockRepository ;	

	Mock* testMock = mockRepository.CreateMock<TestMock,ConcreteNotifier>("TestMock");
        testMock->RegisterExpectation(new TinyMock::Method<void,void,void,void,void>("TestMethod"));

	CHECK(!mockRepository.verifyAll());	
}

TEST(TestMockRepository,TestReturningTRUEFromTheVerifyAllMethod)
{	
	MockRepository<> mockRepository ;	

	TestMock* testMock = dynamic_cast<TestMock*>(mockRepository.CreateMock<TestMock,ConcreteNotifier>("TestMock"));
        testMock->RegisterExpectation(new TinyMock::Method<void,void,void,void,void>("TestMethod"));

	testMock->TestMethod();

	CHECK(mockRepository.verifyAll());
}

TEST(TestMockRepository,TestCheckingIfAGivenExpectationIsNOTSatisfied)
{
	MockRepository<> mockRepository ;

	TestMock* testMock = dynamic_cast<TestMock*>(mockRepository.CreateMock<TestMock,ConcreteNotifier>("TestMock"));
        testMock->RegisterExpectation(new TinyMock::Method<void,void,void,void,void>("TestMethod"));

	CHECK(!mockRepository.verify("TestMock"));

	CHECK(mockRepository.verifyAll());
}
