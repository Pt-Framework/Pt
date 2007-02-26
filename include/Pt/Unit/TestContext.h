/***************************************************************************
 *   Copyright (C) 2005-2006 by Marc Boris Dürner                          *
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
#ifndef PT_UNIT_TESTCONTEXT_H
#define PT_UNIT_TESTCONTEXT_H

#include <Pt/Unit/Api.h>
#include <Pt/Unit/Test.h>

#include <string>


namespace Pt {

namespace Unit {
    /** @brief TestContext
        @ingroup UnitTests
    */
    class TestContext
    {
        public:
            virtual ~TestContext()
            {
                _test.finished.send<const TestContext&>(*this);
            }

            virtual const std::string& testName() const
            { return _test.name(); }

            void run()
            {
                try
                {
                    _test.started.send<const TestContext&>(*this);
                    this->_run();
                    _test.success.send<const TestContext&>(*this);
                }
                catch(const Assertion& assertion)
                {
                    _test.assertion.send<const TestContext&>(*this, assertion);
                }
                catch(const std::exception& ex)
                {
                    _test.exception.send<const TestContext&>(*this, ex);
                }
                catch(...)
                {
                    _test.error.send<const TestContext&>(*this);
                }
            }

        protected:
            virtual void _run()
            {}

            TestContext(Test& test)
            : _test(test)
            {}

        private:
            Test& _test;
    };

} // namespace Unit

} // namespace Pt

#endif
