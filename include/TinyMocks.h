#ifndef TINYMOCK_H
#define TINYMOCK_H

/*
The MIT License
Copyright (c) 2009 Marcin Czenko

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#include <assert.h>
#include <iostream>
#include <vector>
#include <deque>
#include <map>
#include <list>
#include <sstream>
#include <string>

namespace TinyMocks {


//#define CREATE_MOCK(MockRepositoryPtr,MockType,Notifier,MockName) dynamic_cast<MockType*>(MockRepositoryPtr->CreateMock<MockType,Notifier>(MockName));
#define CREATE_MOCK(MockRepositoryPtr,MockType,Notifier,MockName) MockRepositoryPtr->CreateMock<MockType,Notifier>(MockName);

//#define CREATE_MOCK_NO_OWNERSHIP(MockRepositoryPtr,MockType,Notifier,MockName) dynamic_cast<MockType*>(MockRepositoryPtr->CreateMockWithoutOwnership<MockType,Notifier>(MockName));
#define CREATE_MOCK_NO_OWNERSHIP(MockRepositoryPtr,MockType,Notifier,MockName) MockRepositoryPtr->CreateMockWithoutOwnership<MockType,Notifier>(MockName);

//#define CREATE_RT_MOCK(MockRepositoryPtr,MockType,MockName) dynamic_cast<MockType*>(MockRepositoryPtr->CreateRoseRTMock<MockType>(MockName));
#define CREATE_RT_MOCK(MockRepositoryPtr,MockType,MockName) MockRepositoryPtr->CreateRoseRTMock<MockType>(MockName);

#define CHECK_EXPECTATIONS(mockObject,failureMessage) if(true==mockObject.UnhandledExpectations()) { CPPUNIT_FAIL(failureMessage);}
#define CHECK_EXPECTATIONS_PTR(mockObjectPtr,failureMessage) if(true==mockObjectPtr->UnhandledExpectations()) { CPPUNIT_FAIL(failureMessage);}
#define ASSERT_EXPECTATIONS_FAILED(mockObject,failureMessage) if(false==mockObject.UnhandledExpectations()) { CPPUNIT_FAIL(failureMessage);}

class IgnoredMethodsContainer
{
public:
	IgnoredMethodsContainer() : IGNORE_ALL(-1) {}
	void ignoreAll(const std::string& methodName)
	{
		m_ignoredMethods[methodName] = IGNORE_ALL ;
	}

	bool isIgnored(const std::string& methodName)
	{
		return m_ignoredMethods.find(methodName)!=m_ignoredMethods.end() ;
	}

private:
	const int IGNORE_ALL ;
	std::map<std::string,int> m_ignoredMethods;
};

class TinyNotifier
{
public:
	virtual void Send(bool status=true) {}
};

class BaseMethod
{
public:	
	BaseMethod(const std::string& methodName="") : m_mockNotifier(NULL), m_externalMockNotifier(NULL), m_name(methodName), m_ignoreArguments(false) {}

	virtual ~BaseMethod()
	{
		delete m_mockNotifier;
	}

	virtual bool operator==(const BaseMethod& op)
	{
		return true ;
	}

	virtual std::string Signature()
	{
		return std::string("I am just a base type...");
	}

	virtual std::string ToString()
	{
		return std::string("I am just a base type...");
	}

	template<typename N>
	BaseMethod& AddNotifier()
	{
		m_mockNotifier = new N();
		return *this ;
	}	

	BaseMethod& AddNotifier(TinyNotifier * mockNotifier)
	{
		m_externalMockNotifier = mockNotifier;
		return *this ;
	}

	BaseMethod& ignoreArguments()
	{
		m_ignoreArguments = true ;
		return *this ;
	}	

	virtual void ExecuteMockNotifier()
	{
		if(m_mockNotifier)
		{
			m_mockNotifier->Send();
			return;
		}
		if(m_externalMockNotifier)
		{
			m_externalMockNotifier->Send();			
		}
	}

	virtual std::string GetName()
	{
		return m_name;
	}

protected:
	TinyNotifier* m_mockNotifier ;
	TinyNotifier* m_externalMockNotifier;
	std::string m_name ;
	bool m_ignoreArguments;
};

class ExpectationViolationException {};

class MockPrinter
{
public:
    static bool& Silent()
    {
        static bool silent = false ;
        return silent ;
    }
	static void Silent(bool silent)
    {
        bool& lsilent = Silent();
        lsilent = silent ;
    }
};

class Expectations
{
public:	
    Expectations() {}
    Expectations(const std::string& className) : m_className(className) {}
	~Expectations()
	{
		//UnhandledExpectations();		
	}
	TinyMocks::BaseMethod& AddExpectationFor(const std::string& signature, TinyMocks::BaseMethod* expectation)
	{
		m_methods[signature].push_back(expectation);
		return *expectation ;
	}
	TinyMocks::BaseMethod* GetFirstExpectationFor(const std::string& signature)
	{
		TinyMocks::BaseMethod* ret = 0 ;
		if(!IsEmpty(signature))
		{
			ret = m_methods[signature].front();
			m_methods[signature].pop_front();
		}
		return ret ;
	}
	bool IsEmpty(const std::string& signature)
	{
		return (m_methods[signature].size()==0);
	}
	bool UnhandledExpectations()
	{
		bool failed = false ;
		if(m_methods.size() != 0)
		{			
			typedef std::map<std::string,std::deque<TinyMocks::BaseMethod*> >::iterator I ;
			for(I m = m_methods.begin() ; m != m_methods.end(); ++m)
			{
				std::string signature = m->first;
				std::deque<TinyMocks::BaseMethod* >& deque = m->second;
				if(deque.size())
				{
					failed = true ;
					PrintExpectations(signature,deque);
				}
			}			
		}
		return failed ;
	}
	void PrintExpectations(const std::string & signature, std::deque<TinyMocks::BaseMethod*> & deque)
	{	
        if(!MockPrinter::Silent())
        {
            std::cout << std::endl << deque.front()->Signature() << " : Expectations violated. Expected calls:" << std::endl ;
        }
		
		while(deque.size())
		{
            if(!MockPrinter::Silent())
            {
                std::cout << m_className << "::" << deque.front()->ToString() << std::endl ;
            }
			delete deque.front();
			deque.pop_front();
		}
	}
	
private:
	std::map<std::string,std::deque<TinyMocks::BaseMethod*> > m_methods;
    std::string m_className ;
};

class Mock
{
public:    
    Mock()
        : m_className(""),m_mockNotifier(NULL), m_notifierAlreadyExecuted(false)
    {        
    }

    Mock(const std::string& className)
        : m_expectations(className), m_className(className),m_mockNotifier(NULL), m_notifierAlreadyExecuted(false)
    {		
    }

	virtual ~Mock() {}
	
	TinyMocks::BaseMethod& RegisterExpectation(TinyMocks::BaseMethod* exp)
	{						
		return m_expectations.AddExpectationFor(exp->Signature(),exp);
	}

	void RegisterFailureNotifier(TinyNotifier* mockNotifier)
	{
		m_mockNotifier = mockNotifier;
	}

	void IgnoreAll(const std::string& methodName)
	{
		m_ignoredMethods.ignoreAll(methodName);
	}
    
	void PrintNotExpected(TinyMocks::BaseMethod& method)
	{
        if(MockPrinter::Silent()) return ;
		std::cout << std::endl << "Expectation violated ! Call not expected:" << std::endl ;
        std::cout << m_className << "::" << method.ToString() << std::endl ;
	}
	
	void PrintExpectedEqual(TinyMocks::BaseMethod& expected, TinyMocks::BaseMethod& actual)
	{
        if(MockPrinter::Silent()) return ;
		std::cout << std::endl << "Expectation violated !" << std::endl ; 
        std::cout	<< "Expected: " << m_className << "::" << expected.ToString() << std::endl ;
		std::cout << "Actual: " << m_className << "::" << actual.ToString() << std::endl;
	}    	

	void Handle(TinyMocks::BaseMethod* expected, TinyMocks::BaseMethod* actual)
	{
		if(ActualMethodShouldBeIgnored(*actual))
		{
			return;
		}		

		if(CallIsNotExpected(expected))
		{
			HandleNotExpectedCall(*actual);			
			return ;		
		}

		if(ExpectedAndActualAreDifferent(*expected,*actual))
		{
			HandleExpectedAndActualDifferent(expected,actual);
			return;
		}
		
		expected->ExecuteMockNotifier();
        delete expected;
	}

	bool UnhandledExpectations()
	{
		return m_expectations.UnhandledExpectations();
	}	
	
	void ExecuteMockFailureNotifier()
	{
		if(! m_notifierAlreadyExecuted)
		{
			m_notifierAlreadyExecuted = true;
			m_mockNotifier->Send(false);
		}
	}

protected:
	Expectations m_expectations ;
	TinyNotifier* m_mockNotifier ;
private:
    std::string m_className ;
	IgnoredMethodsContainer m_ignoredMethods ;
	bool m_notifierAlreadyExecuted;

	bool ActualMethodShouldBeIgnored(TinyMocks::BaseMethod& actual)
	{
		return m_ignoredMethods.isIgnored(actual.GetName()) ;		
	}
	bool CallIsNotExpected(TinyMocks::BaseMethod* expected)
	{
		return static_cast<bool>(!expected);		
	}
	bool ExpectedAndActualAreDifferent(TinyMocks::BaseMethod& expected,TinyMocks::BaseMethod& actual)
	{
		return !(expected==actual);
	}	
	void HandleExpectedAndActualDifferent(TinyMocks::BaseMethod* expected,TinyMocks::BaseMethod* actual)
	{
		PrintExpectedEqual(*expected, *actual);
		delete expected ;						
		ExecuteMockFailureNotifier();
	}
	void HandleNotExpectedCall(TinyMocks::BaseMethod& actual)
	{
		PrintNotExpected(actual);
		ExecuteMockFailureNotifier();
	}
};

template<typename P=TinyNotifier>
class MockRepository
{
public:	
	MockRepository()
	{
		m_mockNotifier = new P();
	}	

	~MockRepository()
	{
		delete m_mockNotifier;
		for(MockContainer::iterator p=m_mocks.begin(); p!=m_mocks.end(); ++p)
		{			
			delete p->second;
		}

		for(FailureNotifierContainer::iterator p=m_failureNotifiers.begin(); p!=m_failureNotifiers.end(); ++p)
		{
			delete (*p);
		}
	}
	
	bool verifyAll()
	{
		bool allExpectationsOK = true ;

		for(MockContainer::iterator p=m_mocks.begin(); p!=m_mocks.end(); ++p)
		{			
			if(p->second->UnhandledExpectations())
			{
				allExpectationsOK = false ;
			}
		}

		if(allExpectationsOK)
		{
			return true ;
		}
		else
		{
			Fail();
			return false;
		}
	}

	bool verify(const std::string& mockName)
	{
		if(m_mocks.end() == m_mocks.find(mockName))
		{
			return false;
		}

		if(m_mocks[mockName]->UnhandledExpectations())
		{
			Fail();
			return false;
		}
		return true ;		
	}		

	bool verifyAll(TinyNotifier& notifier)
	{
		bool allExpectationsOK = true ;

		for(MockContainer::iterator p=m_mocks.begin(); p!=m_mocks.end(); ++p)
		{			
			if(p->second->UnhandledExpectations())
			{
				allExpectationsOK = false ;
			}
		}

		if(allExpectationsOK)
		{
			return true ;
		}
		else
		{
			Fail(notifier);
			return false;
		}
	}

	template<typename T, typename N> 
	T* CreateMock(const std::string& mockName)
	{
		T* mock = new T(mockName);
		N* failureNotifier = new N();
		mock->RegisterFailureNotifier(failureNotifier);
		m_failureNotifiers.push_back(failureNotifier);		
		m_mocks[mockName] = mock;
		return mock;
	}

	template<typename T, typename N> 
	T* CreateMockWithoutOwnership(const std::string& mockName)
	{
		T* mock = new T(mockName);
		N* failureNotifier = new N();
		mock->RegisterFailureNotifier(failureNotifier);
		m_failureNotifiers.push_back(failureNotifier);		
		return mock;
	}

	template<typename T>
	T* CreateMockWithoutFailureNotifier(const std::string& mockName)
	{
		T* mock = new T(mockName);				
		m_mocks[mockName] = mock;
		return mock;
	}

	void Fail()
	{
		m_mockNotifier->Send(false);
	}

	void Fail(TinyNotifier& notifier)
	{
		notifier.Send(false);
	}
private:
	typedef std::map<std::string,Mock*> MockContainer;
	typedef std::list<TinyNotifier*> FailureNotifierContainer;
	MockContainer m_mocks ;	
	FailureNotifierContainer m_failureNotifiers;
	P* m_mockNotifier ;		
};

template < typename P1, typename P2, typename P3, typename P4, typename R>
class Method : public BaseMethod
{
public:
	Method(const std::string& name, P1 p1, P2 p2, P3 p3, P4 p4, R r) : 
		BaseMethod(name), m_p1(p1), m_p2(p2), m_p3(p3), m_p4(p4), m_r(r) {}
	Method(const Method<P1,P2,P3,P4,R>& method) :
		BaseMethod(method.m_name), m_p1(method.m_p1), m_p2(method.m_p2), m_p3(method.m_p3), m_p4(method.m_p4), m_r(method.m_r)
	{		
	}
	virtual ~Method()
	{
	}
	virtual BaseMethod* CopyInstance()
	{
		return new Method<P1,P2,P3,P4,R>(*this);
	}	
	virtual bool operator==(const BaseMethod& op)
	{
		if(m_ignoreArguments) return true ;
		// Do not compare the return value !!!
		const Method<P1,P2,P3,P4,R>& opCast = (const Method<P1,P2,P3,P4,R>&)op;
		return ((m_p1 == opCast.m_p1) && (m_p2 == opCast.m_p2) && (m_p3 == opCast.m_p3) && (m_p4 == opCast.m_p4));
	}	
	virtual std::string Signature()
	{
		std::stringstream out;
		out << typeid(m_r).name() << " " << m_name << "(" << typeid(m_p1).name() << "," << typeid(m_p2).name() << "," << typeid(m_p3).name() << "," << typeid(m_p4).name() << ")";
		return out.str();		
	}
	virtual std::string ToString()
	{
		std::stringstream out;
		out << m_name << "(" << m_p1 << "," << m_p2 << "," << m_p3 << "," << m_p4 << ")" ;
		return out.str();
	}	
	P1 m_p1;
	P2 m_p2;
	P3 m_p3;
    P4 m_p4;
	R m_r ;
};

template < typename P1, typename P2, typename P3, typename P4 >
class Method<P1,P2,P3,P4,void> : public BaseMethod
{
public:
	Method(const std::string& name, P1 p1, P2 p2, P3 p3, P4 p4) : 
		BaseMethod(name), m_p1(p1), m_p2(p2), m_p3(p3), m_p4(p4) {}
	Method(const Method<P1,P2,P3,P4,void>& method) :
		BaseMethod(method.m_name), m_p1(method.m_p1), m_p2(method.m_p2), m_p3(method.m_p3), m_p4(method.m_p4)
	{		
	}
	virtual ~Method()
	{
	}
	virtual BaseMethod* CopyInstance()
	{
		return new Method<P1,P2,P3,P4,void>(*this);
	}
	virtual bool operator==(const BaseMethod& op)
	{
		if(m_ignoreArguments) return true ;
		// Do not compare the return value !!!
		const Method<P1,P2,P3,P4,void>& opCast = (const Method<P1,P2,P3,P4,void>&)op;
		return ((m_p1 == opCast.m_p1) && (m_p2 == opCast.m_p2) && (m_p3 == opCast.m_p3) && (m_p4 == opCast.m_p4));
	}	
	virtual std::string Signature()
	{
		std::stringstream out;
        out << "void " << m_name << "(" << typeid(m_p1).name() << "," << typeid(m_p2).name() << "," << typeid(m_p3).name() << "," << typeid(m_p4).name() << ")";
		return out.str();		
	}
	virtual std::string ToString()
	{
		std::stringstream out;        
        out << m_name << "(" << m_p1 << "," << m_p2 << "," << m_p3 << "," << m_p4 << ")" ;
		return out.str();
	}	
	P1 m_p1;
	P2 m_p2;
	P3 m_p3;
    P4 m_p4;
};

template < typename P1, typename P2, typename P3, typename R>
class Method<P1,P2,P3,void,R> : public BaseMethod
{
public:
	Method(const std::string& name, P1 p1, P2 p2, P3 p3, R r) : 
		BaseMethod(name), m_p1(p1), m_p2(p2), m_p3(p3), m_r(r) {}
	Method(const Method<P1,P2,P3,void,R>& method) :
		BaseMethod(method.m_name), m_p1(method.m_p1), m_p2(method.m_p2), m_p3(method.m_p3), m_r(method.m_r)
	{		
	}
	virtual ~Method()
	{
	}
	virtual BaseMethod* CopyInstance()
	{
		return new Method<P1,P2,P3,void,R>(*this);
	}	
	virtual bool operator==(const BaseMethod& op)
	{
		if(m_ignoreArguments) return true ;
		// Do not compare the return value !!!
		const Method<P1,P2,P3,void,R>& opCast = (const Method<P1,P2,P3,void,R>&)op;
		return ((m_p1 == opCast.m_p1) && (m_p2 == opCast.m_p2) && (m_p3 == opCast.m_p3));
	}	
	virtual std::string Signature()
	{
		std::stringstream out;
		out << typeid(m_r).name() << " " << m_name << "(" << typeid(m_p1).name() << "," << typeid(m_p2).name() << "," << typeid(m_p3).name() << ")";
		return out.str();		
	}
	virtual std::string ToString()
	{
		std::stringstream out;
		out << m_name << "(" << m_p1 << "," << m_p2 << "," << m_p3 << ")" ;
		return out.str();
	}	
	P1 m_p1;
	P2 m_p2;
	P3 m_p3;
	R m_r ;
};

template < typename P1, typename P2, typename P3>
class Method<P1,P2,P3,void,void> : public BaseMethod
{
public:
	Method(const std::string& name, P1 p1, P2 p2, P3 p3) : 
		BaseMethod(name), m_p1(p1), m_p2(p2), m_p3(p3) {}
	Method(const Method<P1,P2,P3,void,void>& method) :
		BaseMethod(method.m_name), m_p1(method.m_p1), m_p2(method.m_p2),m_p3(method.m_p3)
	{		
	}
	virtual ~Method()
	{
	}	
	virtual bool operator==(const BaseMethod& op)
	{
		if(m_ignoreArguments) return true ;
		// Do not compare the return value !!!
		const Method<P1,P2,P3,void,void>& opCast = (const Method<P1,P2,P3,void,void>&)op;
		return ((m_p1 == opCast.m_p1) && (m_p2 == opCast.m_p2) && (m_p3 == opCast.m_p3));
	}	
	virtual std::string Signature()
	{
		std::stringstream out;
		out << "void " << m_name << "(" << typeid(m_p1).name() << "," << typeid(m_p2).name() << "," << typeid(m_p3).name() << ")";
		return out.str();		
	}
	virtual std::string ToString()
	{
		std::stringstream out;
		out << m_name << "(" << m_p1 << "," << m_p2 << "," << m_p3 << ")" ;
		return out.str();
	}	
	P1 m_p1;
	P2 m_p2;
	P3 m_p3;	
};

template < typename P1, typename P2, typename R>
class Method<P1,P2,void,void,R> : public BaseMethod
{
public:
	Method(const std::string& name, P1 p1, P2 p2, R r) : 
		BaseMethod(name), m_p1(p1), m_p2(p2), m_r(r) {}
	Method(const Method<P1,P2,void,void,R>& method) :
		BaseMethod(method.m_name), m_p1(method.m_p1), m_p2(method.m_p2), m_r(method.m_r)
	{		
	}
	virtual ~Method()
	{
	}
	virtual BaseMethod* CopyInstance()
	{
		return new Method<P1,P2,void,void,R>(*this);
	}	
	virtual bool operator==(const BaseMethod& op)
	{
		if(m_ignoreArguments) return true ;
		// Do not compare the return value !!!
		const Method<P1,P2,void,void,R>& opCast = (const Method<P1,P2,void,void,R>&)op;
		return ((m_p1 == opCast.m_p1) && (m_p2 == opCast.m_p2));
	}	
	virtual std::string Signature()
	{
		std::stringstream out;
		out << typeid(m_r).name() << " " << m_name << "(" << typeid(m_p1).name() << "," << typeid(m_p2).name() << ")";
		return out.str();		
	}
	virtual std::string ToString()
	{
		std::stringstream out;
		out << m_name << "(" << m_p1 << "," << m_p2 << ")" ;
		return out.str();
	}	
	P1 m_p1;
	P2 m_p2;
	R m_r ;
};

template < typename P1, typename P2>
class Method<P1,P2,void,void,void> : public BaseMethod
{
public:
	Method(const std::string& name, P1 p1, P2 p2) : 
		BaseMethod(name), m_p1(p1), m_p2(p2) {}
	Method(const Method<P1,P2,void,void,void>& method) :
		BaseMethod(method.m_name), m_p1(method.m_p1), m_p2(method.m_p2)
	{		
	}
	virtual ~Method()
	{
	}
	virtual BaseMethod* CopyInstance()
	{
		return new Method<P1,P2,void,void,void>(*this);
	}	
	virtual bool operator==(const BaseMethod& op)
	{
		if(m_ignoreArguments) return true ;
		// Do not compare the return value !!!
		const Method<P1,P2,void,void,void>& opCast = (const Method<P1,P2,void,void,void>&)op;
		return ((m_p1 == opCast.m_p1) && (m_p2 == opCast.m_p2));
	}	
	virtual std::string Signature()
	{
		std::stringstream out;
		out << "void " << m_name << "(" << typeid(m_p1).name() << "," << typeid(m_p2).name() << ")";
		return out.str();		
	}
	virtual std::string ToString()
	{
		std::stringstream out;
		out << m_name << "(" << m_p1 << "," << m_p2 << ")" ;
		return out.str();
	}	
	P1 m_p1;
	P2 m_p2;
};

template < typename P1, typename R>
class Method<P1,void,void,void,R> : public BaseMethod
{
public:
	Method(const std::string& name, P1 p1, R r) : 
		BaseMethod(name), m_p1(p1), m_r(r) {}
	Method(const Method<P1,void,void,void,R>& method) :
		BaseMethod(method.m_name), m_p1(method.m_p1), m_r(method.m_r)
	{		
	}
	virtual ~Method()
	{
	}
	virtual bool operator==(const BaseMethod& op)
	{
		if(m_ignoreArguments) return true ;
		// Do not compare the return value !!!
		const Method<P1,void,void,void,R>& opCast = (const Method<P1,void,void,void,R>&)op;
		return (m_p1 == opCast.m_p1);
	}	
	virtual std::string Signature()
	{
		std::stringstream out;
		out << typeid(m_r).name() << " " << m_name << "(" << typeid(m_p1).name() << ")";
		return out.str();		
	}
	virtual std::string ToString()
	{
		std::stringstream out;
		out << m_name << "(" << m_p1 << ")" ;
		return out.str();
	}	
	P1 m_p1;	
	R m_r ;
};

template < typename P1>
class Method<P1,void,void,void,void> : public BaseMethod
{
public:
	Method(const std::string& name, P1 p1) : 
		BaseMethod(name), m_p1(p1) {}
	Method(const Method<P1,void,void,void,void>& method) :
		BaseMethod(method.m_name), m_p1(method.m_p1)
	{		
	}
	virtual ~Method()
	{
	}
	virtual bool operator==(const BaseMethod& op)
	{
		if(m_ignoreArguments) return true ;
		// Do not compare the return value !!!
		const Method<P1,void,void,void,void>& opCast = (const Method<P1,void,void,void,void>&)op;
		return (m_p1 == opCast.m_p1);		
	}	
	virtual std::string Signature()
	{
		std::stringstream out;
		out << "void " << m_name << "(" << typeid(m_p1).name() << ")";
		return out.str();		
	}
	virtual std::string ToString()
	{
		std::stringstream out;
		out << m_name << "(" << m_p1 << ")" ;
		return out.str();
	}	
	P1 m_p1;		
};

template < typename R>
class Method<void,void,void,void,R> : public BaseMethod
{
public:
	Method(const std::string& name,R r) : 
		BaseMethod(name), m_r(r) {}
	Method(const Method<void,void,void,void,R>& method) :
		BaseMethod(method.m_name), m_r(method.m_r)
	{		
	}
	virtual ~Method()
	{
	}
	virtual bool operator==(const BaseMethod& op)
	{
		if(m_ignoreArguments) return true ;
		// Do not compare the return value !!!		
		return true;
	}	
	virtual std::string Signature()
	{
		std::stringstream out;
		out << typeid(m_r).name() << " " << m_name << "(void)";
		return out.str();		
	}
	virtual std::string ToString()
	{
		std::stringstream out;
		out << m_name << "()" ;
		return out.str();
	}	
	R m_r ;
};

template <>
class Method<void,void,void,void,void> : public BaseMethod
{
public:
	Method(const std::string& name) : 
		BaseMethod(name) {}
	Method(const Method<void,void,void,void,void>& method) :
		BaseMethod(method.m_name)
	{		
	}
	virtual ~Method()
	{
	}
	virtual bool operator==(const BaseMethod& op)
	{		
		return true;
	}	
	virtual std::string Signature()
	{
		std::stringstream out;
		out << "void " << m_name << "()";
		return out.str();		
	}
	virtual std::string ToString()
	{
		std::stringstream out;
		out << m_name << "()" ;
		return out.str();
	}	
};

template < typename R>
class MethodIgnoringArguments : public BaseMethod
{
public:
	MethodIgnoringArguments(const std::string& name, R r) : 
		BaseMethod(name), m_r(r) {}
	MethodIgnoringArguments(const MethodIgnoringArguments<R>& method) :
		BaseMethod(method.m_name), m_r(method.m_r)
	{		
	}
	virtual ~MethodIgnoringArguments()
	{
	}
	virtual bool operator==(const BaseMethod& op)
	{
		// Do not compare the return value !!!
		// Do not compare arguments !!!		
		return true;
	}	
	virtual std::string Signature()
	{
		std::stringstream out;
		out << typeid(m_r).name() << " " << m_name << "(...)";
		return out.str();		
	}
	virtual std::string ToString()
	{
		std::stringstream out;
		out << m_name << "(...)" ;
		return out.str();
	}	
	R m_r;		
};

template <>
class MethodIgnoringArguments<void> : public BaseMethod
{
public:
	MethodIgnoringArguments(const std::string& name) : 
		BaseMethod(name) {}
	MethodIgnoringArguments(const MethodIgnoringArguments<void>& method) :
		BaseMethod(method.m_name)
	{		
	}
	virtual ~MethodIgnoringArguments()
	{
	}
	virtual bool operator==(const BaseMethod& op)
	{
		// Do not compare the return value !!!
		// Do not compare arguments !!!	
		return true;
	}	
	virtual std::string Signature()
	{
		std::stringstream out;
		out << "void " << m_name << "(...)";
		return out.str();		
	}
	virtual std::string ToString()
	{
		std::stringstream out;
		out << m_name << "(...)" ;
		return out.str();
	}	
};


template < typename P1, typename P2, typename P3, typename P4, typename R>
class MethodWithDereferencedArguments : public BaseMethod
{
public:
	MethodWithDereferencedArguments(const std::string& name, P1 p1, P2 p2, P3 p3, P4 p4, R r) : 
		BaseMethod(name), m_p1(p1), m_p2(p2), m_p3(p3), m_p4(p4), m_r(r) {}
	MethodWithDereferencedArguments(const MethodWithDereferencedArguments<P1,P2,P3,P4,R>& method) :
		BaseMethod(method.m_name), m_p1(method.m_p1), m_p2(method.m_p2), m_p3(method.m_p3), m_p4(method.m_p4), m_r(method.m_r)
	{		
	}
	virtual ~MethodWithDereferencedArguments()
	{
	}
	virtual BaseMethod* CopyInstance()
	{
		return new MethodWithDereferencedArguments<P1,P2,P3,P4,R>(*this);
	}
	//bool operator==(const Method<P1,P2,P3,R>& op)
	virtual bool operator==(const BaseMethod& op)
	{
		// Do not compare the return value !!!
		const MethodWithDereferencedArguments<P1,P2,P3,P4,R>& opCast = (const MethodWithDereferencedArguments<P1,P2,P3,P4,R>&)op;
		return ((*m_p1 == *(opCast.m_p1)) && (*m_p2 == *(opCast.m_p2)) && (*m_p3 == *(opCast.m_p3)) && (*m_p4 == *(opCast.m_p4)));
	}	
	virtual std::string Signature()
	{
		std::stringstream out;
		out << typeid(m_r).name() << " " << m_name << "(" << typeid(m_p1).name() << "," << typeid(m_p2).name() << "," << typeid(m_p3).name() << "," << typeid(m_p4).name() << ")";
		return out.str();		
	}
	virtual std::string ToString()
	{
		std::stringstream out;
		out << m_name << "(" << m_p1 << "," << m_p2 << "," << m_p3 << "," << m_p4 << ")" ;
		return out.str();
	}	
	P1 m_p1;
	P2 m_p2;
	P3 m_p3;
    P4 m_p4;
	R m_r ;
};

template < typename P1>
class MethodWithDereferencedArguments<P1,void,void,void,void> : public BaseMethod
{
public:
	MethodWithDereferencedArguments(const std::string& name, P1 p1) : 
		BaseMethod(name), m_p1(p1) {}
	MethodWithDereferencedArguments(const MethodWithDereferencedArguments<P1,void,void,void,void>& method) :
		BaseMethod(method.m_name), m_p1(method.m_p1)
	{		
	}
	virtual ~MethodWithDereferencedArguments()
	{
	}
	virtual bool operator==(const BaseMethod& op)
	{
		// Do not compare the return value !!!
		const MethodWithDereferencedArguments<P1,void,void,void,void>& opCast = (const MethodWithDereferencedArguments<P1,void,void,void,void>&)op;
		return (*m_p1 == *(opCast.m_p1));		
	}	
	virtual std::string Signature()
	{
		std::stringstream out;
		out << "void " << m_name << "(" << typeid(m_p1).name() << ")";
		return out.str();		
	}
	virtual std::string ToString()
	{
		std::stringstream out;
		out << m_name << "(" << *m_p1 << ")" ;
		return out.str();
	}	
	P1 m_p1;		
};


}

#endif

