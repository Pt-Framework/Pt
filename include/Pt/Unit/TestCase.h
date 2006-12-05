/***************************************************************************
 *   Copyright (C) 2005-2006 by Dr. Marc Boris Dürner                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef PT_UNIT_TESTCASE_H
#define PT_UNIT_TESTCASE_H

#include <Pt/Unit/Assertion.h>
#include <Pt/Unit/Test.h>
#include <Pt/Unit/TestFixture.h>

#include <string>


namespace Pt {

namespace Unit {

    //! DEPRECATED. This class is obsolete.
    class TestCase : public Test, public TestFixture
    {
        public:
            TestCase(const std::string& name)
            : Test(name)
            {
                this->registerMethod(name, *this, &TestCase::test);
            }

            virtual void run()
            {
                bool isUp = false;

                try
                {
                    this->setUp();
                    isUp = true;
                    this->test();
                    this->tearDown();
                    Test::success( this->name() );
                    return;
                }
                catch(const Assertion& assertion)
                {
                    Test::assertion(this->name(), assertion);
                }
                catch(const std::exception& ex)
                {
                    Test::exception(this->name(), ex);
                }
                catch(...)
                {
                    Test::error(this->name());
                }

                try
                {
                    if(isUp)
                    {
                        this->tearDown();
                    }
                }
                catch(const Assertion& assertion)
                {
                    Test::assertion(this->name(), assertion);
                }
                catch(const std::exception& ex)
                {
                    Test::exception(this->name(), ex);
                }
                catch(...)
                {
                    Test::error(this->name());
                }
            }

        protected:
            virtual void test() = 0;
    };

} // namespace Unit

} // namespace Pt

#endif // for header

