/***************************************************************************
 *   Copyright (C) 2005-2007 by Dr. Marc Boris Drner                       *
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
#include <Pt/Main.h>
#include "Pt/Log/Logger.h"
#include "Pt/Log/Target.h"
#include "Pt/System/Thread.h"

#include <iostream>
#include <fstream>


class LogThread : public Pt::System::Thread
{
    public:
        LogThread( const std::string& message, const std::string& loggerName = "root")
        : _loggerName(loggerName)
        , _message(message)
        {
        }

    protected:
        void run()
        {
            while(true)
            {
                try
                {
                    Pt::Log::Logger logger(_loggerName, Pt::Log::Trace);
                    logger << Pt::Log::info << _message << ": " << std::boolalpha << true << " " << 123.123 << Pt::Log::endlog;

                    this->yield();
                }
                catch(const std::exception& e)
                {
                    std::cerr << e.what() << std::endl;
                }
            }
        }

    private:
        std::string _loggerName;
        std::string _message;
};


int main( int argc, char* argv[] )
{
    try {
        Pt::Log::Target::get("");

        /*Pt::Log::Logger logger("logger1", Pt::Log::Trace);
        logger << Pt::Log::info << "test " << std::boolalpha << true << " " << 123.123 << Pt::Log::endlog;

        Pt::Log::Target::get("").setProperty( "logLevel", Pt::Any( Pt::Log::Error ) );
        Pt::Log::Target::get("").setProperty( "channel", Pt::Any( std::string("console://") ) );

        Pt::Log::Target::get("Pt-Log").setProperty( "logLevel", Pt::Log::Info );
        Pt::Log::Target::get("Pt-Log").setProperty( "async", true);

        Pt::Log::Target::get("logger1").setProperty( "channel", std::string("comm:///dev/ttyS0") );
        Pt::Log::Target::get("logger1").setProperty( "logLevel", Pt::Log::Fatal );
        Pt::Log::Target::get("logger1").setProperty( "async", true);

        Pt::Log::Target::get("logger2").setProperty( "channel", std::string("console://") );
        Pt::Log::Target::get("logger2").setProperty( "logLevel", Pt::Log::Fatal );*/

        //return 0;
        LogThread lt0("thread 0", "logger1");
        lt0.start();

        LogThread lt1("thread 1", "logger1");
        lt1.start();

        LogThread lt2("thread 2", "logger1");
        lt2.start();

        LogThread lt3("thread 3", "logger2");
        lt3.start();

        LogThread lt4("thread 4", "logger2");
        lt4.start();

        lt1.wait();
        lt2.wait();
        lt3.wait();
        lt4.wait();

        lt0.wait();
        return 0;
    }
    catch(const std::exception& e)
    {
        std::cerr << "ERROR: " << e.what() << std::endl;
    }

    return 1;
}
