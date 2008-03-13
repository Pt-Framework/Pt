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
#include <Pt/Arg.h>
#include <Pt/Unit/Api.h>
#include <Pt/Unit/Reporter.h>
#include <Pt/Unit/Application.h>
#include <fstream>

// TODO: move to application class later
namespace TestMain
{
    static int argc = 0;
    static char** argv = 0;
}


int main(int argc, char** argv)
{
    TestMain::argc = argc;
    TestMain::argv = argv;
    Pt::Unit::Application app;

    Pt::Arg<bool> help(argc, argv, 'h');
    if( help )
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

    Pt::Unit::BriefReporter consoleReporter;
    app.attachReporter(consoleReporter);

    Pt::Arg<std::string> file(argc, argv, 'f');
    std::ofstream logFile;
    Pt::Unit::BriefReporter fileReporter;
    std::string fileName = file.getValue();

    if( ! fileName.empty() )
    {
        logFile.open( fileName.c_str() );
        fileReporter.setOutput(logFile);
        app.attachReporter(fileReporter);
    }

    try {
        Pt::Arg<std::string> test(argc, argv, 't');
        std::string testName = test.getValue();
        if( testName.empty() )
        {
            app.run();
            return app.errors();
        }
        else
        {
            app.run(testName);
            return app.errors();
        }

        return app.errors();
    }
    catch(const std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
    }

    return 1;
}

#endif// PT_UNIT_TESTMAIN_H
