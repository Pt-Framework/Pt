/*
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */
#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestSuite.h"
#include "Pt/Unit/RegisterTest.h"
#include "Pt/System/Application.h"
#include <csignal>

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

            app.loop().timeout() += Pt::slot(app, &Pt::System::Application::exit);
            app.systemSignal() += Pt::slot(*this, &ApplicationTest::onSignal);

            app.loop().setIdleTimeout(2000);

            if( ! hasSigUsr1 && ! hasSigAlrm )
            {
                reportMessage("signals not supported");
                return;
            }

            app.raiseSystemSignal(SIGUSR1);
            app.raiseSystemSignal(SIGALRM);

            app.run();
            PT_UNIT_ASSERT(_cnt == hasSigUsr1 + hasSigAlrm);
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
