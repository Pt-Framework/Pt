/***************************************************************************
 *   Copyright (C) 2006 - 2007 by Marc Boris Duerner                       *
 *   Copyright (C) 2006 - 2007 by Tommi Maekitalo                          *
 *   Copyright (C) 2006 - 2007 by Sebastian Pieck                          *
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
#include "Pt/System/Mutex.h"
#include "Pt/System/Condition.h"
#include "Pt/Net/TcpSocket.h"
#include "Pt/Net/TcpServerSocket.h"
#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestMain.h"
#include "Pt/Unit/TestCase.h"
#include "Pt/Unit/RegisterTest.h"
#include <string>


class TcpSocketTest : public Pt::Unit::TestCase
{
    public:
        TcpSocketTest()
        : TestCase("TcpSocketTest")
        { }

        void setUp()
        {

        }

        void test()
        {

        }

        void tearDown()
        {

        }

    private:

};

Pt::Unit::RegisterTest<TcpSocketTest> register_TcpSocketTest;
