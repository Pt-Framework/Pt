#ifndef PTV_UNIT_PERFORMANCETEST_H
#define PTV_UNIT_PERFORMANCETEST_H


#include <Pt/System/Clock.h>
#include <Pt/System/TimeValue.h>
#include <Pt/Unit/Test.h>
#include <sstream>


namespace Pt
{

namespace Unit
{

/**
 * Performance tests can be done by creating and destroying an object of this class (for example in a scope).
 */
class PerformanceTest
{
public:
    /**
     * The constructor initializes the test member for logging and starts the clock.
     */
    PerformanceTest(Test& test)
    : _test(test)
    {
        _clock.start();
    }

    /**
     * The destructor stops the clock and does the logging to the test.
     */
    ~PerformanceTest()
    {
        Pt::System::TimeValue delta = _clock.stop();
        std::stringstream msg;
        msg << "Duration: "
            << (delta.seconds() * 1000.0 + delta.microSeconds() / 1000.0)
            << " ms";
        _test.message.send(msg.str());
    }

private:
    Test& _test;
    Pt::System::Clock _clock;
};

}

}


#endif  // PTV_UNIT_PERFORMANCETEST_H