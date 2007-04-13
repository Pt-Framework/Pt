/***************************************************************************
 *   Copyright (C) 2007 by Laurentiu-Gheorghe Crisan                       *
 *   Copyright (C) 2007 by Marc Boris Dürner                               *
 *   Copyright (C) 2007 by Bjoern Oliver Streule                           *
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
#include "Pt/System/Selector.h"

#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestSuite.h"
#include "Pt/Unit/TestMain.h"
#include "Pt/Unit/RegisterTest.h"

#include "Pt/Connectable.h"
#include "Pt/System/Pipe.h"
#include "Pt/System/Clock.h"
#include "Pt/System/Timer.h"
#include "Pt/System/Thread.h"

#include <string>
#include <iostream>

using namespace Pt::System;

class SelectorTest : public Pt::Unit::TestSuite, public Pt::Connectable
{
    public:
        SelectorTest()
        : Pt::Unit::TestSuite("SelectorTest")
        {            
            Pt::Unit::TestSuite::registerMethod( "WaitTimer", *this, &SelectorTest::WaitTimer );
            Pt::Unit::TestSuite::registerMethod( "WaitMultipleTimers", *this, &SelectorTest::WaitMultipleTimers );
            Pt::Unit::TestSuite::registerMethod( "WaitIODevice", *this, &SelectorTest::WaitIODevice );
            Pt::Unit::TestSuite::registerMethod( "WaitIODeviceTimeOut", *this, &SelectorTest::WaitIODeviceTimeOut );
            Pt::Unit::TestSuite::registerMethod( "WaitTimeOutWhithoutDevice", *this, &SelectorTest::WaitTimeOut );            
        }

    private:
        void timeout1()
        {            
            _timeValue = _clock.stop();
            _counter++;            
        }

        void timeout2()
        {
            _counter++;            
        }
        
        void timeout3()
        {
            _counter++;            
        }

        void WaitTimer()
        {
            Selector        selector;
            Timer           timer;                
            size_t          timeElapsed = 0;
            const size_t    timeout = 100; //ms
            const size_t    count = 5;


            Pt::connect(timer.timeout, *this, &SelectorTest::timeout1);
            
            timer.start(timeout);
            selector.addTimer(timer);
            
            _counter    = 0;
            for (size_t i = 0; i != count; i++)
            {
                _timeValue  = 0;
                _clock.start();                
                
                selector.wait();                
                timeElapsed += _timeValue.seconds() * 1000 + _timeValue.microSeconds() / 1000;
            }

            PT_UNIT_ASSERT(count == _counter);
            PT_UNIT_ASSERT(timeElapsed >(timeout - 10) * count);
            PT_UNIT_ASSERT(timeElapsed < (timeout + 10) * count);            
        }

        void WaitMultipleTimers()
        {
            Selector        selector;
            Timer           timer1;
            Timer           timer2;
            Timer           timer3;
            size_t          timeElapsed = 0;
            const size_t    timeout1 = 100; //ms
            const size_t    timeout2 = 200; //ms
            const size_t    timeout3 = 110; //ms
            const size_t    count = 6;
            const size_t    timeToRun = 600; //ms


            Pt::connect(timer1.timeout, *this, &SelectorTest::timeout1);
            Pt::connect(timer2.timeout, *this, &SelectorTest::timeout2);
            Pt::connect(timer3.timeout, *this, &SelectorTest::timeout3);
            
            timer1.start(timeout1);
            timer2.start(timeout2);
            timer3.start(timeout3);
            
            selector.addTimer(timer1);
            selector.addTimer(timer2);
            selector.addTimer(timer3);
            
            _counter    = 0;               
            const size_t startTime = Clock::getTime();
            size_t runTime = 0;            
            while (runTime < timeToRun)
            {
                selector.wait();           
                runTime = Clock::getTime() - startTime;
            }

            const size_t expectedCount = (size_t) (timeToRun/timeout3 + timeToRun/timeout2 + timeToRun/timeout1);
            PT_UNIT_ASSERT(expectedCount == _counter);
        }

        void WaitIODevice()
        {
            Selector    selector;
            Pipe        pipe;

            selector.addDevice( pipe.input(), Selector::WaitInput );

            const int           size = 10;
            char                buffer[size];
            size_t              sz;
            std::stringstream   input;
            std::string         out("Hello World, where do you want to GOTO day!");
            
            pipe.output().write(out.c_str(), out.size());            
            
            while (selector.wait(100))
            {
                memset(&buffer, 0, size);
                sz = pipe.input().read(buffer, size);
                input.write(buffer, sz);        
            }            

            PT_UNIT_ASSERT(input.str() == out);
        }        

        void WaitIODeviceTimeOut()
        {
            Selector    selector;
            Pipe        pipe;
            Clock       clock;
            TimeValue   TimeValue;
            size_t      timeout = 100; //ms

            selector.addDevice( pipe.input(), Selector::WaitInput );

            clock.start();
            bool sucess = selector.wait(timeout);
            TimeValue = clock.stop();

            PT_UNIT_ASSERT(!sucess);
            
            size_t timeElapsed = TimeValue.seconds() * 1000 + TimeValue.microSeconds() / 1000;                        
            // check time with tolerance because of rounding error 
            PT_UNIT_ASSERT(timeElapsed <= timeout + 10);
            PT_UNIT_ASSERT(timeElapsed >= timeout - 10);
            
        }

        void WaitTimeOut()
        {
            Selector    selector;            
            Clock       clock;
            TimeValue   TimeValue;
            size_t      timeout = 100; //ms
            
            clock.start();
            bool sucess = selector.wait(timeout);
            TimeValue = clock.stop();

            PT_UNIT_ASSERT(!sucess);
            
            size_t timeElapsed = TimeValue.seconds() * 1000 + TimeValue.microSeconds() / 1000;
            // check time with tolerance because of rounding error 
            PT_UNIT_ASSERT(timeElapsed <= timeout + 10);
            PT_UNIT_ASSERT(timeElapsed >= timeout - 10);
        }

    private:
        TimeValue   _timeValue;
        Clock       _clock;
        size_t      _counter;
};

Pt::Unit::RegisterTest<SelectorTest> register_SelectorTest;
