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
#ifndef PT_UNIT_LISTEDPROTOCOL_H
#define PT_UNIT_LISTEDPROTOCOL_H

#include <Pt/Args.h>
#include <Pt/Unit/Api.h>
#include <Pt/Unit/TestProtocol.h>
#include <Pt/Unit/TestSuite.h>

#include <map>


namespace Pt {

namespace Unit {
    /** @brief Protocol and data driven tests

        This is a simple implementation of a TestProtocol where methods and
        data can be added to a schedule. When the TestSchedule is applied to
        a test suite it will try to run all scheduled methods and pass it
        the previously assigned data. The methods will be run in the order in
        which they have been scheduled. It is allowed to schedule a method
        multiple times. The methods must be registered as usual in the test
        suite.
    */
    class PT_UNIT_API TestSchedule : public TestProtocol
    {
        public:
            typedef std::multimap<std::string, const Args*> TestMap;

        public:
            /** @brief Include a method in the schedule

                The method name passed to this method will be added to the
                schedule. When the protocol is run the method will be invoked
                without arguments.

                @param testName Name of the test to be run.
            */
            void includeTest(const std::string& testName);

            /** @brief Include a method in the schedule

                The method name passed to this method will be added to the
                schedule. When the protocol is run the method will be invoked
                the argguments assigned to it. The arguments are not copied,
                rather a referenece is stored in the test schedule, thus the
                Arguments must exist for as long as the schedule exists. This
                aloows to reuse large data structures for many test without the
                need to copy it.

                @param testName Name of the test to be run
                @param args Arguments to be passed to the test
            */
            void includeTest(const std::string& testName, const Args& args);

            /** @brief Executes the protocol

                This method will simply call each method that has been added to
                the schedule and pass the assigned arguments. Methods can be
                called multiple times.

                @param test The test suite to apply the protocol
            */
            void run(TestSuite& suite);

        private:
            Args _nullArgs;
            TestMap _items;
    };

} // namespace Unit

} // namespace Pt

#endif // for header

