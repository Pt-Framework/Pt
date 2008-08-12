#include "Pt/Unit/assertion.h"
#include "Pt/Unit/testsuite.h"
#include "Pt/Unit/registertest.h"
#include "Pt/System/Application.h"

#ifndef SIGUSR1
#define SIGUSR1 -1
#endif

#ifndef SIGALRM
#define SIGALRM -1
#endif

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
            Pt::System::Application app;
            bool hasSigUsr1 = app.catchSystemSignal(SIGUSR1);
            bool hasSigAlrm = app.catchSystemSignal(SIGALRM);

            connect(app.timeout(), app, &Pt::System::Application::exit);
            connect(app.systemSignal, *this, &ApplicationTest::onSignal);

            app.setIdleTimeout(2000);

			if( ! hasSigUsr1 && ! hasSigAlrm )
			{
				reportMessage("signals not supported");
				return;
			}

            app.raiseSystemSignal(SIGUSR1);
            app.raiseSystemSignal(SIGALRM);

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
