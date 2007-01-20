/***************************************************************************
 *   Copyright (C) 2005-2006 by Dr. Marc Boris Duerner                     *
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
#ifndef PT_UNIT_TESTPROTOCOL_H
#define PT_UNIT_TESTPROTOCOL_H

#include <Pt/Unit/Api.h>
#include <Pt/Unit/Test.h>
#include <Pt/Unit/Assertion.h>


namespace Pt {

namespace Unit {

    class TestSuite;

    /** @brief Protocol for test suites

        This is the base class for protocols that can be used to run a test
        suite. The default implementation will simply run each registered
        test of the test suite without passing it any data. Implementors
        need to override the method TestProtocol::run.
    */
    class TestProtocol
    {
        public:
            /** @brief Destructor
            */
            virtual ~TestProtocol()
            {}

            /** @brief Executes the protocol

                This method can be overriden to specify a custom protocol
                for a test suite. The default implementation will simply
                call each registered method of the test suite. Implementors
                will most likely call TestSuite::runTest to resolve a test
                method by name and pass it required arguments.

                @param test The test suite to apply the protocol
            */
            virtual void run(TestSuite& test);

        protected:
            void runTest(TestSuite& suite, const std::string& name, const Args& args);
    };



} // namespace Unit

} // namespace Pt

#endif // for header

