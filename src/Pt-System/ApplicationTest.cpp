#include "Pt/Unit/assertion.h"
#include "Pt/Unit/testsuite.h"
#include "Pt/Unit/registertest.h"
#include "Pt/System/Application.h"

#include <signal.h>
#include <unistd.h>

class ApplicationTest : public Pt::Unit::TestSuite
{
    public:
        ApplicationTest()
        : Pt::Unit::TestSuite("ApplicationTest")
        {
            Pt::Unit::TestSuite::registerMethod( "SignalTest", *this, &ApplicationTest::SignalTest);
        }

        void setUp()
        {
            _cnt= 0;
        }

    private:
        void SignalTest()
        {
            Pt::Application app;
            app.catchSystemSignal(SIGUSR1);
            app.catchSystemSignal(SIGALRM);

            connect(app.timeout(), app, &Pt::Application::exit);
            connect(app.systemSignal, *this, &ApplicationTest::onSignal);

            app.setIdleTimeout(2000);

            ::raise(SIGUSR1);
            alarm(1);

            app.run();
            PT_UNIT_ASSERT(_cnt == 2);
        }

        void onSignal(int sig)
        {

            reportMessage("catched signal");
            PT_UNIT_ASSERT(sig == SIGUSR1 || sig == SIGALRM);
            ++_cnt;
        }

        int _cnt;
};


Pt::Unit::RegisterTest<ApplicationTest> register_ApplicationTest;
