/*
 * Copyright (C) 2020-2026 by Marc Boris Duerner
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

#ifndef WORKBENCH_SCRIPTTEST_H
#define WORKBENCH_SCRIPTTEST_H

#include <Pt/Lua/TypeManager.h>
#include <Pt/Lua/Type.h>
#include <Pt/Lua/AsyncCall.h>
#include <Pt/System/MainLoop.h>
#include <Pt/Unit/TestSuite.h>
#include <Pt/Connectable.h>

namespace Pt {

namespace Lua {

class Context;
class Script;


struct Point
{
  Point()
  : x(0), y(0)
  {}

  Point(int x_, int y_)
  : x(x_), y(y_)
  {}

  int x;
  int y;
};


class PointType : public Type
{
  public:
    PointType()
    : Type(typeid(Point), "Point")
    {}

    std::size_t size() const { return sizeof(Point); }

    void define(TypeManager& tm)
    {
      this->registerConstructor(tm, *this, &PointType::construct0);
      this->registerConstructor(tm, *this, &PointType::construct2);
      this->registerMethod(tm, "sum", &sum);
      this->registerProperty(tm, "x", &getX, &setX);
      this->registerProperty(tm, "y", &getY, &setY);
    }

  private:
    void construct0(void* mem)
    { new (mem) Point(); }

    void construct2(void* mem, int x, int y)
    { new (mem) Point(x, y); }

    static int  getX(Point& p)
    { return p.x; }

    static void setX(Point& p, int v)
    { p.x = v; }

    static int  getY(Point& p)
    { return p.y; }

    static void setY(Point& p, int v)
    { p.y = v; }

    static int  sum(Point& p)
    { return p.x + p.y; }
};


struct Counter
{
  Counter()
  : value(0)
  {}

  AsyncCall* increment();

  int value;
};


class IncrementCall : public BasicAsyncCall<int>
{
  public:
    explicit IncrementCall(Counter& c)
    : _counter(c)
    {}

  private:
    void onBeginCall(Pt::System::EventLoop& /*loop*/) override
    {
      _counter.value += 1;
      setReady();
    }

    int onResult() override
    { return _counter.value; }

    void onCancel() override
    {}

  private:
    Counter& _counter;
};


inline AsyncCall* Counter::increment()
{
  return new IncrementCall(*this);
}


class CounterType : public Type
{
  public:
    CounterType()
    : Type(typeid(Counter), "Counter")
    {}

    std::size_t size() const { return sizeof(Counter); }

    void define(TypeManager& tm)
    {
      this->registerConstructor(tm, *this, &CounterType::construct);
      this->registerProperty(tm, "value", &getValue, &setValue);
      this->registerAsyncMethod(tm, "increment", &Counter::increment);
    }

  private:
    void construct(void* mem) { new (mem) Counter(); }

    static int  getValue(Counter& c)        { return c.value; }
    static void setValue(Counter& c, int v) { c.value = v; }
};

// Integration tests for the Script advance() API.
//
// AdvanceLoop      — Calls advance() in a tight loop on the calling thread.
//                    Sync NativeCalls execute automatically inside advance().
// EventLoopAdvance — Attaches Script to a MainLoop and drives it via
//                    beginAdvance()/endAdvance()/advanced() signal.
class ScriptTest : public Pt::Unit::TestSuite
                     , public Pt::Connectable
{
  public:
    ScriptTest();

    ~ScriptTest();

  protected:
    void setUp();

    void tearDown();

  private:
    void Advance();

    void AsyncAdvance();

    void onAsyncAdvanced();

  private:
    TypeManager        _tm;
    PointType             _pointType;
    CounterType           _counterType;

    Pt::System::MainLoop* _loop;
    Pt::Lua::Context*    _ctx;
    Script*            _script;
};

} // namespace

} // namespace

#endif // include guard
