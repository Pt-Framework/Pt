/***************************************************************************
 *   Copyright (C) 2008 by Marc Duerner                                    *
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
#include "Pt/System/Thread.h"
#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestSuite.h"
#include "Pt/Unit/RegisterTest.h"

class ThreadTest : public Pt::Unit::TestSuite
{
    public:
        ThreadTest()
        : Pt::Unit::TestSuite("ThreadTest")
        {
            Pt::Unit::TestSuite::registerMethod( "MethodAttached", *this, &ThreadTest::MethodAttached );
        }

    protected:
        void MethodAttached();
};


inline void ThreadTest::MethodAttached()
{
}


Pt::Unit::RegisterTest<ThreadTest> register_ThreadTest;
