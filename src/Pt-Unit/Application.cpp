/***************************************************************************
 *   Copyright (C) 2005-2008 by Dr. Marc Boris Duerner                     *
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
#include <Pt/Unit/Application.h>

namespace Pt {

namespace Unit {

Application* Application::_app = 0;


Application::Application()
: Test("")
, _errors(0)
{
    _app = this;

    std::list<Test*>::iterator it;
    for(it = Application::tests().begin(); it != Application::tests().end(); ++it)
    {
        (*it)->setParent( this );
    }
}


Application::~Application()
{
}


Application& Application::instance()
{
    if( ! _app )
        throw std::logic_error("application not initialized");

    return *_app;
}


Test* Application::findTest(const std::string& testname)
{
    std::list<Test*>::iterator it;
    for(it = Application::tests().begin(); it != Application::tests().end(); ++it)
    {
        if( (*it)->name() == testname)
            return *it;
    }

    return 0;
}


void Application::attachReporter(Reporter& r)
{
    Test::attachReporter(r);
}


void Application::attachReporter(Reporter& r, const std::string& testname)
{
    Test* test = this->findTest(testname);
    if( ! test )
        return;

    test->attachReporter(r);
}


void Application::run(const std::string& testName)
{
    _errors = 0;

    std::list<Test*>::iterator it;
    for(it = Application::tests().begin(); it != Application::tests().end(); ++it)
    {
        if(testName == "" || (*it)->name() == testName)
            (*it)->run();
    }
}


void Application::run()
{
    _errors = 0;

    std::list<Test*>::iterator it;
    for(it = Application::tests().begin(); it != Application::tests().end(); ++it)
    {
            (*it)->run();
    }
}


void Application::registerTest(Test& test)
{
    Application::tests().push_back(&test);
}


std::list<Test*>& Application::tests()
{
    static std::list<Test*> _allTests;
    return _allTests;
}

} //namespace Unit

} // namespace Pt
