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
#ifndef PT_UNIT_TESTMAIN_H
#define PT_UNIT_TESTMAIN_H
#include <Pt/Main.h>
#include <Pt/Unit/Api.h>
#include <Pt/Unit/Reporter.h>
#include <Pt/Unit/Application.h>

#include <fstream>
#include <cstring>


namespace TestMain
{
    static int argc = 0;
    static char** argv = 0;
}


int main(int argc, char** argv)
{
    TestMain::argc = argc;
    TestMain::argv = argv;

    // CppUnit(mini) test launcher
    // command line option syntax:
    // test [OPTIONS]
    // where OPTIONS are
    //     -t=CLASS[::TEST]  run the test class CLASS or member test CLASS::TEST
    //     -f=FILE           save output in file FILE instead of stdout

    Pt::Unit::BriefReporter fileReporter;
    Pt::Unit::BriefReporter consoleReporter;
    Pt::Unit::Application app;
    bool fileLoggingEnabled = false;

    const char* fileName = "";
    const char* testName = "";

    for(int i = 1; i < argc; ++i)
    {
        if(argv[i][0] != '-')
            break;

        if( !std::strncmp(argv[i], "--help", 6) )
        {
            std::cerr << "Usage: " << argv[0] << " [-t<testname>] [-f<logfile>]\n";
            std::cerr << "Available Tests:\n";
            std::list<Pt::Unit::Test*>::const_iterator it;
            for( it = app.tests().begin(); it != app.tests().end(); ++it)
            {
                std::cerr << "  - "<< (*it)->name() << std::endl;
            }
            return 0;
        }
        else if( !std::strncmp(argv[i], "-t", 2) )
        {
            testName = argv[i] + 2;
        }
        else if( !std::strncmp(argv[i], "-f", 2) )
        {
          fileName = argv[i] + 2;
          fileLoggingEnabled = true;
        }
    }

    std::ofstream logFile(fileName);
    if (fileLoggingEnabled)
    {
      fileReporter = Pt::Unit::BriefReporter(&logFile);
      app.addReporter(fileReporter);
    }
    app.addReporter(consoleReporter);

    try {
        return app.run(testName);
    }
    catch(const std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
    }

    return 1;
}



#endif// PT_UNIT_TESTMAIN_H

