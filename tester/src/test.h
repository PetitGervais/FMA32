#ifndef TEST_H
#define TEST_H

#include <string>
#include <iostream>
#include <sstream>
#include <list>

class TestBase
{
  public:
    TestBase(){}
    TestBase(const std::string & name) : m_name(name){}
    virtual ~TestBase(){}

    virtual const bool Execute(void) = 0;

    void SetName(const std::string & name) {m_name = name;}
    const std::string & GetName(void) const {return m_name;}

    std::stringstream & GetError(void) {return m_error;}

  private:
    std::string m_name;
    std::stringstream m_error;
};

typedef std::list<TestBase *> TestList;

class Tester
{
	public:
		Tester(){}
		virtual ~Tester()
		{
      for(TestList::iterator iter = m_tests.begin(); iter != m_tests.end(); ++iter)
        delete *iter;
		}

    void Register(TestBase * test) {m_tests.push_back(test);}

		void Run(void)
		{
		  for(TestList::iterator iter = m_tests.begin(); iter != m_tests.end(); ++iter)
      {
        if(!(*iter)->Execute())
        {
          std::cout << "Test " << (*iter)->GetName() << " FAILED !!!" << std::endl;
          std::cout << "Reason : " << (*iter)->GetError().str() << std::endl;
          break;
        }
        std::cout << "Test " << (*iter)->GetName() << " PASSED" << std::endl;
      }
		}

	private:
	  TestList m_tests;

};

#endif // TEST_H
