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
#include <Pt/Unit/Test.h>

namespace Pt {

namespace Unit {

const std::string& Test::name() const
{
    return _name;
}

void Test::started(const TestContext& ctx)
{
    std::list<Reporter*>::iterator it;
    for(it = _reporter.begin(); it != _reporter.end(); ++it)
    {
        (*it)->started(ctx);
    }

    if(_parent)
        _parent->started(ctx);
}


void Test::finished(const TestContext& ctx)
{
    std::list<Reporter*>::iterator it;
    for(it = _reporter.begin(); it != _reporter.end(); ++it)
    {
        (*it)->finished(ctx);
    }

    if(_parent)
        _parent->finished(ctx);
}


void Test::success(const TestContext& ctx)
{
    std::list<Reporter*>::iterator it;
    for(it = _reporter.begin(); it != _reporter.end(); ++it)
    {
        (*it)->success(ctx);
    }

    if(_parent)
        _parent->success(ctx);
}


void Test::assertion(const TestContext& ctx, const Assertion& ass)
{
    std::list<Reporter*>::iterator it;
    for(it = _reporter.begin(); it != _reporter.end(); ++it)
    {
        (*it)->assertion(ctx, ass);
    }

    if(_parent)
        _parent->assertion(ctx, ass);
}


void Test::exception(const TestContext& ctx, const std::exception& ex)
{
    std::list<Reporter*>::iterator it;
    for(it = _reporter.begin(); it != _reporter.end(); ++it)
    {
        (*it)->exception(ctx, ex);
    }

    if(_parent)
        _parent->exception(ctx, ex);
}


void Test::error(const TestContext& ctx)
{
    std::list<Reporter*>::iterator it;
    for(it = _reporter.begin(); it != _reporter.end(); ++it)
    {
        (*it)->error(ctx);
    }

    if(_parent)
        _parent->error(ctx);
}


void Test::message(const std::string& msg)
{
    std::list<Reporter*>::iterator it;
    for(it = _reporter.begin(); it != _reporter.end(); ++it)
    {
        (*it)->message(msg);
    }

    if(_parent)
        _parent->message(msg);
}


void Test::setParent(Test* test)
{
    _parent = test;
}


Test* Test::parent()
{
    return _parent;
}


const Test* Test::parent() const
{
    return _parent;
}


void Test::addReporter(Reporter& r)
{
    connect(r.destroyed, *this, &Test::removeReporter);
    _reporter.push_back(&r);
}


void Test::removeReporter(Reporter& r)
{
    _reporter.remove(&r);
}

}

}
