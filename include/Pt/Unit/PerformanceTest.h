/***************************************************************************
 *   Copyright (C) 2005-2007 by Dr. Marc Boris Duerner                     *
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
#ifndef PT_UNIT_PERFORMANCETEST_H
#define PT_UNIT_PERFORMANCETEST_H

#include <Pt/Unit/Api.h>
#include <Pt/Unit/Test.h>
#include <Pt/System/Clock.h>
#include <Pt/System/TimeValue.h>

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