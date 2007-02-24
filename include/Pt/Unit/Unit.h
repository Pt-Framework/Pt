#ifndef PT_UNIT_UNIT_H
#define PT_UNIT_UNIT_H

namespace Pt {

/** @namespace Pt::Unit
    @brief Protocol and data driven Unit-testing

    This module provides a complete framework for effective unit-testing. All Unit
    tests in the Platinum framework are written using this module. Data-driven,
    as well as and protocol-driven testing is possible. %Unit-tests can easily
    be integrated into the build process and test results can be reported and logged.
    Writing tests is similar to writing tests with the CppUnit framework, so that
    porting tests to use %Pt::Unit is simple.
*/
namespace Unit {

}

}

/** \page "A Simple Test Case"
!!!A Simple Test Case
\n
The TestCase class can be used to implement simple Unit test. This is simply
done by deriving from TestCase and implementing the %TestCase::test method.
The member functions %TestCase::setUp and %TestCase::tearDown can be used
to manage any resources the test might require. The PTV_UNIT_ASSERT macro
can be used to assert test conditions during the test.

@code
class MyTestCase : public Pt::Unit::TestCase
{
    public:
        MyTestCase()
        : Pt::Unit::TestCase("MyTestCase")
        , _a(0), _b(0)
        { }

        void setUp()
        {
            _a = 5;
            _b = 5;
        }

        void tearDown()
        {
        }

        void test()
        {
            _a += _b;
            PTV_UNIT_ASSERT(_a == 10);
        }

    private:
        int _a;
        int _b;
};
@endcode
*/


/** \page "Build System Integration"
!!!Build System Integration
\n
The Jamrules.Unit rule set containes rules to build a Unit test and have it
automatically run after it has been build. Instead of the rule 'Main' the rule
'PtvUnitTest' has to be used in a Jamfile:

@code
PtvUnitTest mytest : mytest.cpp ;
@endcode

This will cause the jam tool to report a failed build if the Unit
test does not run through, which is the desired behaviour. This way
Unit tests can easily be integrated in automated builds.
*/


/** \page "Data Driven Testing"
!!!Data Driven Testing
\n
Sometimes it is desirable to repeat a test with different data. This is achieved
by registering methods that take arguments (the test data) in a TestSuite.

@code
class MyTestSuite
{
    public:
        MyTestSuite()
        : Pt::Unit::TestSuite("MyTestSuite")
        {
            this->registerMethod("AdditionTest", *this, &MyTestSuite::AdditionTest);
        }

        void AdditionTest(int a, int b)
        {
            // testing code
        }
};
@endcode

A protocol can then by used to call the AdditionTest method multiple times
with different data. For convenience, such a protocol is included in the Unit
test module already and is called TestSchedule. Here we can assign data to
tests.

@code
class MyTestSchedule : public Pt::Unit::TestSchedule
{
    public:
        MyTestSchedule()
        , _first(1, 1)
        , _second(2, 2)
        , _three(3, 3)
        {
            this->includeTest("AdditionTest", _first);
            this->includeTest("AdditionTest", _second);
            this->includeTest("AdditionTest", _third);
        }

    private:
        Args _first;
        Args _second;
        Args _third;
};
@endcode

When this protocol is applied to MyTestSuite, the AdditionTest will be called
three times with different test data. It is very possible to read data from a
file or another data source.
*/


/** \page "Protocol Driven Testing"
!!!Protocol Driven Testing
\n
Protocol driven testing implements the idea to control the order in which
tests are executed, or to run test methods multiple times. To accomplish this
a protocol is defined by deriving from the TestProtocol class.The
following example will run a test called 'MyTest' three times and sleeps
between the tests for 1 second.

@code
#include <Pt/Unit/TestProtocol.h>
#include <Pt/System/Process.h>

class MyProtocol : public Pt::Unit::TestProtocol
{
    public:
        MyProtocol()
        {}

        void run(Pt::Unit::TestSuite& suite)
        {
            suite.runTest( "MyTest" );
            Pt::System::Process::sleep(1000);
            suite.runTest( "MyTest" );
            Pt::System::Process::sleep(1000);
            suite.runTest( "MyTest" );
        }
};
@endcode

The protocol can then be applied to a TestSuite. The methods are
resolved using object reflection. The TestSuite class requires
that all runnable tests are registered for reflection. TestSuite inherits
reflection capabilities from Pt::Reflectable. If a test can not be executed
through the TestSuite::runTest method an exception of the type %Pt::LogicError
is thrown and the test fails if it is allowed to propagate. The new protocol
can be assigned to a TestSuite in the constructor:

@code
MyProtocol protocol;

class MyTestSuite
{
    public:
        MyTestSuite()
        : Pt::Unit::TestSuite("MyTestSuite", protocol)
        {
            this->registerMethod("MyTest", *this, &MyTestSuite::MyTest);
        }

        void MyTest()
        {
            // testing code
        }
};
@endcode

Alternatively, a protocol can be set using the Unit::TestSuite::setProtocol
method. It is entirely possible to load and assign protocols at run-time.
*/


/** \page "Running Tests"
!!!Running Tests
\n
All tests can be run by the Application object of the %Unit module. To register
a test with the application object the RegisterTest class template can be used
or Application::registerTest can be called. A typical test program will
instanciate an Application object and set reporters for result reporting and
logging.

@code
int main()
{
    Pt::Unit::Application app;
    std::ofstream fs("log.txt");
    Pt::Unit::Reporter reporter(fs);
    app.setReporter(reporter);
    return app.run();
}
@endcode

For convenience, the header file TestMain.h already contains such a %main loop
where reporters can be selected by command line arguments. So the implementor
of a test only has to include TestMain.h in the file where be derives and
registers the tests.

*/

#endif
