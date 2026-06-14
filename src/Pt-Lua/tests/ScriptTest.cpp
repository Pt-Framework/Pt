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
    explicit WaitCall(int ms) : _ms(ms) {}

    static AsyncCall* create(int ms)
    {
      return new WaitCall(ms);
    }

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


const char script0[] =
  "local p = Point(3, 4)\n"
  "result = p:sum()\n";

const char script1[] =
  "wait(10)\n"
  "local c = Counter()\n"
  "c:increment()\n"
  "c:increment()\n"
  "result = c:increment()\n";

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

  _tm.registerAsyncFunction("wait", &WaitCall::create);

  _ctx = new Pt::Lua::Context(_tm);

  Pt::Unit::TestSuite::registerMethod("Advance",
                                      *this, &ScriptTest::Advance);
  Pt::Unit::TestSuite::registerMethod("AsyncAdvance",
                                      *this, &ScriptTest::AsyncAdvance);
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


Pt::Unit::RegisterTest<ScriptTest> register_LuaScriptTest;

} // namespace

} // namespace
