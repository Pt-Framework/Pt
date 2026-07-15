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

#include "ScriptTest.h"

#include <Pt/Lua/Context.h>
#include <Pt/Lua/Script.h>
#include <Pt/Lua/Result.h>

#include <Pt/System/MainLoop.h>
#include <Pt/System/Timer.h>
#include <Pt/Slot.h>
#include <Pt/Unit/Assertion.h>
#include <Pt/Unit/RegisterTest.h>

#include <string>

namespace Pt {

namespace Lua {

namespace {

class WaitCall : public BasicAsyncCall<void>
               , public Pt::Connectable
{
  public:
    explicit WaitCall(int ms)
    : _ms(ms)
    {}

  private:
    void onBeginCall(Pt::System::EventLoop& loop) override
    {
      _timer.setActive(loop);
      _timer.timeout() += Pt::slot(*this, &WaitCall::onTimeout);
      _timer.start(static_cast<std::size_t>(_ms));
    }

    void onCancel() override
    { _timer.stop(); }

    void onTimeout()
    {
      _timer.stop();
      setReady();
    }

  private:
    int               _ms;
    Pt::System::Timer _timer;
};


inline AsyncCall* waitAsync(int ms)
{
  return new WaitCall(ms);
}


// WaitFunction demonstrates a user-managed BasicAsyncFunction subclass that
// holds host-program state (a scale factor) and forwards it to the AsyncCall
// constructor. It auto-unregisters from the TypeManager when destroyed.
class WaitFunction : public BasicAsyncFunction<int>
{
  public:
    WaitFunction(Pt::Lua::TypeManager& tm, int scale)
    : BasicAsyncFunction<int>("scaledWait", tm, tm.asyncCallType())
    , _scale(scale)
    {}

  protected:
    AsyncCall* onCall(int ms) override
    { return new WaitCall(ms * _scale); }

  private:
    int _scale;
};


const char script0[] =
  "local p = Point(3, 4)\n"
  "result = p:sum()\n";

const char script1[] =
  "wait(10)\n"
  "local c = Counter()\n"
  "c:increment()\n"
  "c:increment()\n"
  "result = c:increment()\n";

const char script2[] =
  "scaledWait(5)\n"
  "result = 42\n";

} // namespace

ScriptTest::ScriptTest()
: Pt::Unit::TestSuite("Pt::Lua::ScriptTest")
, _loop(0)
, _ctx(0)
, _script(0)
{
  _tm.registerType<Point>(_pointType);
  _pointType.define(_tm);

  _tm.registerType<Counter>(_counterType);
  _counterType.define(_tm);

  _tm.registerAsyncFunction("wait", &waitAsync);

  _ctx = new Pt::Lua::Context(_tm);

  Pt::Unit::TestSuite::registerMethod("Advance",
                                      *this, &ScriptTest::Advance);
  Pt::Unit::TestSuite::registerMethod("AsyncAdvance",
                                      *this, &ScriptTest::AsyncAdvance);
  Pt::Unit::TestSuite::registerMethod("AsyncAdvanceWithState",
                                      *this, &ScriptTest::AsyncAdvanceWithState);
  Pt::Unit::TestSuite::registerMethod("ReturnObjectByValue",
                                      *this, &ScriptTest::ReturnObjectByValue);
}


ScriptTest::~ScriptTest()
{
  delete _ctx; _ctx = 0;
}


void ScriptTest::setUp()
{
  _loop = new Pt::System::MainLoop;
  _script = 0;
}


void ScriptTest::tearDown()
{
  delete _loop;
  _loop = 0;
}


void ScriptTest::Advance()
{
  Script script(*_ctx, script0);
  _script = &script;

  while(true)
  {
    Script::Status s = _script->advance();

    if(s == Script::Yield)
      continue;

    if(s == Script::NativeCall)
      continue;

    if(s == Script::ScriptError)
      PT_UNIT_FAIL(_script->errorMessage());

    break; // ScriptOk
  }

  Result result(_ctx->state());
  PT_UNIT_ASSERT_EQUAL(result.get("result"), 7);
}


void ScriptTest::AsyncAdvance()
{
  Script script(*_ctx, script1);
  script.setActive(*_loop);
  script.advanced() += Pt::slot(*this, &ScriptTest::onAsyncAdvanced);
  script.beginAdvance();

  _script = &script;

  _loop->run();

  Result result(_ctx->state());
  PT_UNIT_ASSERT_EQUAL(result.get("result"), 3);
}


void ScriptTest::onAsyncAdvanced()
{
  Script::Status s = _script->endAdvance();

  if(s == Script::Yield || s == Script::NativeCall)
  {
    _script->beginAdvance();
    return;
  }

  if(s == Script::ScriptError)
    PT_UNIT_FAIL(_script->errorMessage());

  _loop->exit();
}


void ScriptTest::AsyncAdvanceWithState()
{
  WaitFunction wf(_tm, 2);
  _tm.registerAsyncFunction(wf);

  Pt::Lua::Context ctx(_tm);

  Script script(ctx, script2);
  script.setActive(*_loop);
  script.advanced() += Pt::slot(*this, &ScriptTest::onAsyncAdvanced);
  script.beginAdvance();

  _script = &script;

  _loop->run();

  Result result(ctx.state());
  PT_UNIT_ASSERT_EQUAL(result.get("result"), 42);
}


// Regression test for return-by-value of a registered non-trivially-copyable type.
//
// Prior to the fix, Script::pushResult() used std::memcpy to place the
// returned object into the Lua userdata. For types like std::vector<int> this
// is a shallow byte copy: the vector's internal heap pointer is duplicated.
// The Pt::Any holding the return value is destroyed immediately after
// pushResult() returns, freeing the vector's heap storage. The Lua userdata
// then holds a dangling pointer. When the Context is destroyed (lua_close),
// the Lua GC calls the __gc destructor on the userdata — which calls
// ~vector<int>() on the dangling copy → double-free → crash.
//
// The fix replaces memcpy with a placement-new copy-construction thunk
// registered alongside the destructor thunk in TypeManager::TypeBinding.
void ScriptTest::ReturnObjectByValue()
{
  // Isolated TypeManager: VectorInt must be registered before Point calls
  // defineToNumbers() so that std::vector<int> can be resolved as a return type.
  TypeManager tm;

  VectorIntType vectorType;
  tm.registerType<std::vector<int>>(vectorType);
  vectorType.define(tm);

  PointType localPointType;
  tm.registerType<Point>(localPointType);
  localPointType.define(tm);
  localPointType.defineToNumbers(tm);

  Pt::Lua::Context ctx(tm);

  const char* script =
    "local p = Point(3, 4)\n"
    "local v = p:toNumbers()\n"
    "result = v:length()\n";

  Script s(ctx, script);
  while(true)
  {
    Script::Status st = s.advance();
    if(st == Script::NativeCall) continue;
    if(st == Script::ScriptError) PT_UNIT_FAIL(s.errorMessage());
    break;
  }

  Result r(ctx.state());
  PT_UNIT_ASSERT_EQUAL(r.get("result"), 2);
}


Pt::Unit::RegisterTest<ScriptTest> register_LuaScriptTest;

} // namespace

} // namespace
