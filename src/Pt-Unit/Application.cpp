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
#include <Pt/Unit/Application.h>

namespace Pt {

namespace Unit {

std::size_t Application::_errors = 0;

Reporter* Application::_reporter = 0;

std::list<Reporter*> Application::_reporterList;


void Application::setReporter(Reporter& reporter)
{
    Application::_reporter = &reporter;
}


void Application::addReporter(Reporter& reporter)
{
    Application::_reporterList.push_back(&reporter);
}


void Application::registerTest(Test& test)
{
    connect(test.started, &Application::started);
    connect(test.finished, &Application::finished);
    connect(test.success, &Application::success);
    connect(test.assertion, &Application::assertion);
    connect(test.exception, &Application::exception);
    connect(test.error, &Application::error);
    connect(test.message, &Application::message);
    Application::tests().push_back(&test);
}


int Application::run()
{
    _errors = 0;

    std::list<Test*>::iterator it;
    for(it = Application::tests().begin(); it != Application::tests().end(); ++it)
    {
            (*it)->run();
    }

    return _errors;
}


int Application::run(const std::string& testName)
{
    _errors = 0;

    std::list<Test*>::iterator it;
    for(it = Application::tests().begin(); it != Application::tests().end(); ++it)
    {
        if(testName == "" || (*it)->name() == testName)
            (*it)->run();
    }

    return _errors;
}


std::list<Test*>& Application::tests()
{
    static std::list<Test*> _allTests;
    return _allTests;
}


void Application::started(const TestContext& test)
{
    if(_reporter)
    {
        _reporter->started(test);
    }


    std::list<Reporter*>::iterator it;
    for(it = _reporterList.begin(); it != _reporterList.end(); ++it)
    {
        (*it)->started(test);
    }
}


void Application::finished(const TestContext& test)
{
    if(_reporter)
    {
        _reporter->finished(test);
    }


    std::list<Reporter*>::iterator it;
    for(it = _reporterList.begin(); it != _reporterList.end(); ++it)
    {
        (*it)->finished(test);
    }
}


void Application::success(const TestContext& test)
{
    if(_reporter)
    {
        _reporter->success(test);
    }


    std::list<Reporter*>::iterator it;
    for(it = _reporterList.begin(); it != _reporterList.end(); ++it)
    {
        (*it)->success(test);
    }
}


void Application::assertion(const TestContext& test, const Assertion& a)
{
    ++_errors;

    if(_reporter)
    {
        _reporter->assertion(test, a);
    }


    std::list<Reporter*>::iterator it;
    for(it = _reporterList.begin(); it != _reporterList.end(); ++it)
    {
        (*it)->assertion(test, a);
    }
}


void Application::exception(const TestContext& test, const std::exception& ex)
{
    ++_errors;

    if(_reporter)
    {
        _reporter->exception(test, ex);
    }


    std::list<Reporter*>::iterator it;
    for(it = _reporterList.begin(); it != _reporterList.end(); ++it)
    {
        (*it)->exception(test, ex);
    }
}


void Application::error(const TestContext& test)
{
    ++_errors;

    if(_reporter)
    {
        _reporter->error(test);
    }


    std::list<Reporter*>::iterator it;
    for(it = _reporterList.begin(); it != _reporterList.end(); ++it)
    {
        (*it)->error(test);
    }
}


void Application::message(const std::string& msg)
{
    if(_reporter)
    {
        _reporter->message(msg);
    }


    std::list<Reporter*>::iterator it;
    for(it = _reporterList.begin(); it != _reporterList.end(); ++it)
    {
        (*it)->message(msg);
    }
}

} //namespace Unit

} // namespace Pt
