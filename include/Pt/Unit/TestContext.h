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
#ifndef PTV_UNIT_TESTCONTEXT_H
#define PTV_UNIT_TESTCONTEXT_H

#include <string>
#include <Pt/Unit/Test.h>


namespace Pt {

namespace Unit {

    class PT_API TestConText
    {
        public:
            virtual ~TestConText()
            {
                _test.finished.send<const TestConText&>(*this);
            }

            virtual const std::string& testName() const
            { return _test.name(); }

            void run()
            {
                try
                {
                    _test.started.send<const TestConText&>(*this);
                    this->_run();
                    _test.success.send<const TestConText&>(*this);
                }
                catch(const Assertion& assertion)
                {
                    _test.assertion.send<const TestConText&>(*this, assertion);
                }
                catch(const std::exception& ex)
                {
                    _test.exception.send<const TestConText&>(*this, ex);
                }
                catch(...)
                {
                    _test.error.send<const TestConText&>(*this);
                }
            }

        protected:
            virtual void _run()
            {}

            TestConText(Test& test)
            : _test(test)
            {}

        private:
            Test& _test;
    };

} // namespace Unit

} // namespace Pt

#endif
