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
#ifndef PT_UNIT_TESTFIXTURE_H
#define PT_UNIT_TESTFIXTURE_H


namespace Pt {

namespace Unit {

    /** @brief Maintain test context

        The TestFixture class serves as an interface to maintain any resources
        that need to be setup before a test or cleaned up after a test. The 
        method SetUp is called before a test is run and the method tearDown
        is run after a test.
    */
    class TestFixture2
    {
        public:
            /** \brief Destructor.
            */
            virtual ~TestFixture2()
            {}

            /** \brief Set up context before running a test.

                This function is called before each registered tester function
                is invoked. It is meant to initialize any required resources.
            */
            virtual void setUp()
            {}

            /** \brief Clean up after the test run.

                This function is called after each registered tester function
                is invoked. It is meant to remove any resources previously
                initialized in TestFixture::setUp.
            */
            virtual void tearDown()
            {}
    };

} // namespace Unit

} // namespace Pt


#endif

