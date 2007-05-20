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
#undef PT_API_LOG_EXPORT

#include "DllLoggerTest.h"

#include "Pt/Log/Logger.h"
#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestSuite.h"
#include "Pt/Unit/TestMain.h"
#include "Pt/Unit/RegisterTest.h"
#include <string>


class LoggerTest : public Pt::Unit::TestSuite
{
    public:
        LoggerTest()
        : Pt::Unit::TestSuite("LoggerTest")
        {
            Pt::Unit::TestSuite::registerMethod( "CreateLogger", *this, &LoggerTest::CreateLogger );
            Pt::Unit::TestSuite::registerMethod( "DllLoggerTest", *this, &LoggerTest::DllLoggerTest );
        }

    protected:
        void CreateLogger()
        {
            Pt::Log::Logger logger("LoggerTest.a.b.c");

            try {
                Pt::Log::Logger logger(".");
                PT_UNIT_ASSERT(false);
            }
            catch(const std::invalid_argument& e)
            {}

            try {
                Pt::Log::Logger logger("a..b");
                PT_UNIT_ASSERT(false);
            }
            catch(const std::invalid_argument& e)
            {}

            try {
                Pt::Log::Logger logger("a.");
                PT_UNIT_ASSERT(false);
            }
            catch(const std::invalid_argument& e)
            { }

            try {
                Pt::Log::Logger logger(".a");
                PT_UNIT_ASSERT(false);
            }
            catch(const std::invalid_argument& e)
            {  }
        }

	void DllLoggerTest()
	{
            Pt::Log::DllLoggerTest();
	}

};

Pt::Unit::RegisterTest<LoggerTest> register_LoggerTest;

