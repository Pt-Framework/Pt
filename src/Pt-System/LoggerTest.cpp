/*
 * Copyright (C) 2005-2006 by Marc Boris Duerner
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */
#undef PT_API_SYSTEM_EXPORT

//#include "DllLoggerTest.h"

#include "Pt/System/Logger.h"
#include "Pt/System/LogTarget.h"
#include "Pt/System/IOError.h"
#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestSuite.h"
#include "Pt/Unit/RegisterTest.h"
#include <string>


class LoggerTest : public Pt::Unit::TestSuite
{
    public:
        LoggerTest()
        : Pt::Unit::TestSuite("LoggerTest")
        {
            Pt::Unit::TestSuite::registerMethod( "CreateLogger", *this, &LoggerTest::CreateLogger );
            Pt::Unit::TestSuite::registerMethod( "TestInheritance", *this, &LoggerTest::TestInheritance );
            Pt::Unit::TestSuite::registerMethod( "LogFatal", *this, &LoggerTest::LogFatal );
            Pt::Unit::TestSuite::registerMethod( "LogError", *this, &LoggerTest::LogError );
            Pt::Unit::TestSuite::registerMethod( "LogWarn", *this, &LoggerTest::LogWarn );
            Pt::Unit::TestSuite::registerMethod( "LogInfo", *this, &LoggerTest::LogInfo );
            Pt::Unit::TestSuite::registerMethod( "LogDebug", *this, &LoggerTest::LogDebug );
            Pt::Unit::TestSuite::registerMethod( "LogTrace", *this, &LoggerTest::LogTrace );
            //Pt::Unit::TestSuite::registerMethod( "DllLoggerTest", *this, &LoggerTest::DllLoggerTest );
            Pt::Unit::TestSuite::registerMethod( "SerialChannel", *this, &LoggerTest::SerialChannel );
        }

    protected:
        void CreateLogger()
        {
            Pt::System::Logger logger("LoggerTest.a.b.c");

            try {
                Pt::System::Logger logger(".");
                PT_UNIT_ASSERT(false);
            }
            catch(const std::invalid_argument&)
            {}

            try {
                Pt::System::Logger logger("a..b");
                PT_UNIT_ASSERT(false);
            }
            catch(const std::invalid_argument&)
            {}

            try {
                Pt::System::Logger logger("a.");
                PT_UNIT_ASSERT(false);
            }
            catch(const std::invalid_argument&)
            { }

            try {
                Pt::System::Logger logger(".a");
                PT_UNIT_ASSERT(false);
            }
            catch(const std::invalid_argument&)
            {  }
        }

        void TestInheritance()
        { 
            Pt::System::Logger logger_m("m");
            Pt::System::Logger logger_m_a("m.a");
            Pt::System::Logger logger_m_a_a("m.a.a");
            Pt::System::Logger logger_m_aa_aa("m.aa.aa");
            Pt::System::Logger logger_m_a_A("m.a.A");
            Pt::System::Logger logger_m_A_a("m.A.a");
            Pt::System::Logger logger_m_a_a_a("m.a.a.a");
            Pt::System::Logger logger_m_a_a_b("m.a.a.b");
            Pt::System::Logger logger_m_a_b("m.a.b");
            Pt::System::Logger logger_m_a_b_a("m.a.b.a");
            Pt::System::Logger logger_m_a_b_b("m.a.b.b");
            Pt::System::Logger logger_m_b("m.b");
            Pt::System::Logger logger_m_b_a("m.b.a");
            
            logger_m.target().setLogLevel(Pt::System::Trace);
            PT_UNIT_ASSERT( logger_m.target().logLevel()        == Pt::System::Trace );
            PT_UNIT_ASSERT( logger_m_a.target().logLevel()      == Pt::System::Trace );
            PT_UNIT_ASSERT( logger_m_a_a.target().logLevel()    == Pt::System::Trace );
            PT_UNIT_ASSERT( logger_m_aa_aa.target().logLevel()  == Pt::System::Trace );
            PT_UNIT_ASSERT( logger_m_a_A.target().logLevel()    == Pt::System::Trace );
            PT_UNIT_ASSERT( logger_m_A_a.target().logLevel()    == Pt::System::Trace );
            PT_UNIT_ASSERT( logger_m_a_a_a.target().logLevel()  == Pt::System::Trace );
            PT_UNIT_ASSERT( logger_m_a_a_b.target().logLevel()  == Pt::System::Trace );
            PT_UNIT_ASSERT( logger_m_a_b.target().logLevel()    == Pt::System::Trace );
            PT_UNIT_ASSERT( logger_m_a_b_a.target().logLevel()  == Pt::System::Trace );
            PT_UNIT_ASSERT( logger_m_a_b_b.target().logLevel()  == Pt::System::Trace );
            PT_UNIT_ASSERT( logger_m_b.target().logLevel()      == Pt::System::Trace );
            PT_UNIT_ASSERT( logger_m_b_a.target().logLevel()    == Pt::System::Trace );
            
            logger_m_a_b.target().setLogLevel(Pt::System::Error);
            PT_UNIT_ASSERT( logger_m.target().logLevel()        == Pt::System::Trace );
            PT_UNIT_ASSERT( logger_m_a.target().logLevel()      == Pt::System::Trace );
            PT_UNIT_ASSERT( logger_m_a_a.target().logLevel()    == Pt::System::Trace );
            PT_UNIT_ASSERT( logger_m_aa_aa.target().logLevel()  == Pt::System::Trace );
            PT_UNIT_ASSERT( logger_m_a_A.target().logLevel()    == Pt::System::Trace );
            PT_UNIT_ASSERT( logger_m_A_a.target().logLevel()    == Pt::System::Trace );
            PT_UNIT_ASSERT( logger_m_a_a_a.target().logLevel()  == Pt::System::Trace );
            PT_UNIT_ASSERT( logger_m_a_a_b.target().logLevel()  == Pt::System::Trace );
            PT_UNIT_ASSERT( logger_m_a_b.target().logLevel()    == Pt::System::Error );
            PT_UNIT_ASSERT( logger_m_a_b_a.target().logLevel()  == Pt::System::Error );
            PT_UNIT_ASSERT( logger_m_a_b_b.target().logLevel()  == Pt::System::Error );
            PT_UNIT_ASSERT( logger_m_b.target().logLevel()      == Pt::System::Trace );
            PT_UNIT_ASSERT( logger_m_b_a.target().logLevel()    == Pt::System::Trace );

            logger_m_a.target().setLogLevel(Pt::System::Info);
            PT_UNIT_ASSERT( logger_m.target().logLevel()        == Pt::System::Trace );
            PT_UNIT_ASSERT( logger_m_a.target().logLevel()      == Pt::System::Info );
            PT_UNIT_ASSERT( logger_m_a_a.target().logLevel()    == Pt::System::Info );
            PT_UNIT_ASSERT( logger_m_aa_aa.target().logLevel()  == Pt::System::Trace );
            PT_UNIT_ASSERT( logger_m_a_A.target().logLevel()    == Pt::System::Info );
            PT_UNIT_ASSERT( logger_m_A_a.target().logLevel()    == Pt::System::Trace );
            PT_UNIT_ASSERT( logger_m_a_a_a.target().logLevel()  == Pt::System::Info );
            PT_UNIT_ASSERT( logger_m_a_a_b.target().logLevel()  == Pt::System::Info );
            PT_UNIT_ASSERT( logger_m_a_b.target().logLevel()    == Pt::System::Error );
            PT_UNIT_ASSERT( logger_m_a_b_a.target().logLevel()  == Pt::System::Error );
            PT_UNIT_ASSERT( logger_m_a_b_b.target().logLevel()  == Pt::System::Error );
            PT_UNIT_ASSERT( logger_m_b.target().logLevel()      == Pt::System::Trace );
            PT_UNIT_ASSERT( logger_m_b_a.target().logLevel()    == Pt::System::Trace );

            logger_m.target().setLogLevel(Pt::System::Fatal);
            PT_UNIT_ASSERT( logger_m.target().logLevel()        == Pt::System::Fatal );
            PT_UNIT_ASSERT( logger_m_a.target().logLevel()      == Pt::System::Info );
            PT_UNIT_ASSERT( logger_m_a_a.target().logLevel()    == Pt::System::Info );
            PT_UNIT_ASSERT( logger_m_aa_aa.target().logLevel()  == Pt::System::Fatal );
            PT_UNIT_ASSERT( logger_m_a_A.target().logLevel()    == Pt::System::Info );
            PT_UNIT_ASSERT( logger_m_A_a.target().logLevel()    == Pt::System::Fatal );
            PT_UNIT_ASSERT( logger_m_a_a_a.target().logLevel()  == Pt::System::Info );
            PT_UNIT_ASSERT( logger_m_a_a_b.target().logLevel()  == Pt::System::Info );
            PT_UNIT_ASSERT( logger_m_a_b.target().logLevel()    == Pt::System::Error );
            PT_UNIT_ASSERT( logger_m_a_b_a.target().logLevel()  == Pt::System::Error );
            PT_UNIT_ASSERT( logger_m_a_b_b.target().logLevel()  == Pt::System::Error );
            PT_UNIT_ASSERT( logger_m_b.target().logLevel()      == Pt::System::Fatal );
            PT_UNIT_ASSERT( logger_m_b_a.target().logLevel()    == Pt::System::Fatal );
        }

        void LogFatal()
        {
            Pt::System::Logger logger("LoggerTest");
            logger.fatal() << "Fatal message" << Pt::System::endlog;
        }

        void LogError()
        {
            Pt::System::Logger logger("LoggerTest");
            logger.error() << "Error message" << Pt::System::endlog;
        }

        void LogWarn()
        {
            Pt::System::Logger logger("LoggerTest");
            logger.warn() << "Warn message" << Pt::System::endlog;
        }

        void LogInfo()
        {
            Pt::System::Logger logger("LoggerTest");
            logger.info() << "Info message" << Pt::System::endlog;
        }

        void LogDebug()
        {
            Pt::System::Logger logger("LoggerTest");
            logger.debug() << "Debug message" << Pt::System::endlog;
        }

        void LogTrace()
        {
            Pt::System::Logger logger("LoggerTest");
            logger.trace() << "Trace message" << Pt::System::endlog;
        }

        //void DllLoggerTest()
        //{
        //        Pt::System::DllLoggerTest();
        //}

        void SerialChannel()
        {
            std::string url;
            try
            {
                Pt::System::Logger logger("LoggerTest.SerialChannelTest");
#ifdef _WIN32
                url = "comm://COM1:";
#else
                url = "comm:///dev/ttyS0";
#endif
                Pt::System::LogTarget::get("LoggerTest.SerialChannelTest").setChannel(url);
                Pt::System::LogTarget::get("LoggerTest.SerialChannelTest").setLogLevel(Pt::System::Trace);
    
                for(int n = 0; n < 10; ++n)
                {
                    logger.info() << "Info Message on serial device" << Pt::System::endlog;
                }
            }
            catch( const Pt::System::AccessFailed& )
            {
                reportMessage( "No such serial device: " + url ); 
            }
        }
};

Pt::Unit::RegisterTest<LoggerTest> register_LoggerTest;

