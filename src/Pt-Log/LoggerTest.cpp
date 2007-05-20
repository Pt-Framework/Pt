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
#include "Pt/Log/Target.h"
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
            /*Pt::Unit::TestSuite::registerMethod( "CreateLogger", *this, &LoggerTest::CreateLogger );
            Pt::Unit::TestSuite::registerMethod( "LogFatal", *this, &LoggerTest::LogFatal );
            Pt::Unit::TestSuite::registerMethod( "LogError", *this, &LoggerTest::LogError );
            Pt::Unit::TestSuite::registerMethod( "LogWarn", *this, &LoggerTest::LogWarn );
            Pt::Unit::TestSuite::registerMethod( "LogInfo", *this, &LoggerTest::LogInfo );
            Pt::Unit::TestSuite::registerMethod( "LogDebug", *this, &LoggerTest::LogDebug );
            Pt::Unit::TestSuite::registerMethod( "LogTrace", *this, &LoggerTest::LogTrace );
            Pt::Unit::TestSuite::registerMethod( "DllLoggerTest", *this, &LoggerTest::DllLoggerTest );*/
            Pt::Unit::TestSuite::registerMethod( "SerialChannel", *this, &LoggerTest::SerialChannel );

            Pt::Log::Target::get("Pt-Log").setLogLevel(Pt::Log::Trace);
            Pt::Log::Target::get("LoggerTest").setLogLevel(Pt::Log::Trace);
        }

    protected:
        void CreateLogger()
        {
            Pt::Log::Logger logger("LoggerTest.a.b.c");

            try {
                Pt::Log::Logger logger(".");
                PT_UNIT_ASSERT(false);
            }
            catch(const std::invalid_argument&)
            {}

            try {
                Pt::Log::Logger logger("a..b");
                PT_UNIT_ASSERT(false);
            }
            catch(const std::invalid_argument&)
            {}

            try {
                Pt::Log::Logger logger("a.");
                PT_UNIT_ASSERT(false);
            }
            catch(const std::invalid_argument&)
            { }

            try {
                Pt::Log::Logger logger(".a");
                PT_UNIT_ASSERT(false);
            }
            catch(const std::invalid_argument&)
            {  }
        }

        void LogFatal()
        {
            Pt::Log::Logger logger("LoggerTest");
            logger << Pt::Log::fatal << "Fatal message" << Pt::Log::endlog;
        }

        void LogError()
        {
            Pt::Log::Logger logger("LoggerTest");
            logger << Pt::Log::error << "Error message" << Pt::Log::endlog;
        }

        void LogWarn()
        {
            Pt::Log::Logger logger("LoggerTest");
            logger << Pt::Log::warn << "Warn message" << Pt::Log::endlog;
        }

        void LogInfo()
        {
            Pt::Log::Logger logger("LoggerTest");
            logger << Pt::Log::info << "Info message" << Pt::Log::endlog;
        }

        void LogDebug()
        {
            Pt::Log::Logger logger("LoggerTest");
            logger << Pt::Log::debug << "Debug message" << Pt::Log::endlog;
        }

        void LogTrace()
        {
            Pt::Log::Logger logger("LoggerTest");
            logger << Pt::Log::trace << "Trace message" << Pt::Log::endlog;
        }

        void DllLoggerTest()
        {
                Pt::Log::DllLoggerTest();
        }

        void SerialChannel()
        {
            Pt::Log::Logger logger("LoggerTest.SerialChannelTest");
            Pt::Log::Target::get("LoggerTest.SerialChannelTest").setChannel("comm:///dev/ttyS0");
            Pt::Log::Target::get("LoggerTest.SerialChannelTest").setLogLevel(Pt::Log::Trace);

            for(int n = 0; n < 10; ++n)
            {
                logger << Pt::Log::info << "Info Message on serial device" << Pt::Log::endlog;
            }
        }
};

Pt::Unit::RegisterTest<LoggerTest> register_LoggerTest;

