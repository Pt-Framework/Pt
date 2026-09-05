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

#ifndef PT_LUA_SCRIPT_H
#define PT_LUA_SCRIPT_H

#include <Pt/Lua/Api.h>
#include <Pt/System/Selectable.h>
#include <Pt/Signal.h>

struct lua_State;
struct lua_Debug;

#include <string>

#if __cplusplus >= 202002L
#include <Pt/Slot.h>
#include <Pt/Coroutine.h>
#endif

namespace Pt {

class Any;

namespace Reflex { class Type; }

namespace Lua {

class AsyncCall;
class Context;
class Call;

#if __cplusplus >= 202002L
class AsyncAdvance;
#endif

class PT_LUA_API Script : public System::Selectable
                        , public Connectable
{
  public:
    enum Status
    {
      Yield,
      NativeCall,
      ScriptOk,
      ScriptError
    };

    Script(Context& ctx, const char* script);

    ~Script();

    void beginAdvance();

    Status endAdvance() const;

    Pt::Signal<>& advanced();

    Status advance();

    const std::string& errorMessage() const
    { return _errorMsg; }

    static Script* fromState(lua_State* L);

#if __cplusplus >= 202002L
    /** @brief Asynchronously advance the Lua script as a C++20 awaitable.

        Returns an awaitable for use with co_await.
        Wraps beginAdvance() / endAdvance() / advanced().
    */
    AsyncAdvance advanceAsync();
#endif

  protected:
    bool onRun();

    void onAttach(Pt::System::EventLoop& loop);

    void onDetach(Pt::System::EventLoop& loop);

    void onCancel();

  private:
    Script(const Script&);
    Script& operator=(const Script&);

#if __cplusplus >= 202002L
    friend class AsyncAdvance;

    void attachAwaiter(AsyncAdvance& awaiter);

    void detachAwaiter(AsyncAdvance& awaiter);
#endif

    bool onCall();

    void onAsyncCall();

    void onAsyncCallFinished();

    bool onAsyncCallReady();

    int pushResult(Pt::Any& value, Pt::Reflex::Type& type);

    void resume();

    bool isCancelled() const;

    void setError(const std::string& msg);

    static void yieldHook(lua_State* L, lua_Debug* ar);

  private:
    Context&      _ctx;
    lua_State*    _co;
    bool          _isCancelled;
    Status        _lastStatus;
    std::string   _errorMsg;
    Pt::Signal<>  _advanced;

    Call*         _pendingCall;
    AsyncCall*    _pendingAsyncCall;
    AsyncCall*    _activeAsyncCall;

  #if __cplusplus >= 202002L
    AsyncAdvance* _awaiter = nullptr;
  #endif
};

#if __cplusplus >= 202002L

/** @brief Awaitable for async advance of a Lua %Script.

    Wraps the beginAdvance() / endAdvance() / advanced() async pattern
    into a C++20 awaitable for use with co_await.

    @ingroup Pt-Lua
*/
class PT_LUA_API AsyncAdvance : public Pt::Awaiter
                              , public Pt::Connectable
{
    public:
    AsyncAdvance(Script& script);

    ~AsyncAdvance();

    Script::Status await_resume();

    private:
    friend class Script;

    void onBegin() override;

    void onCancel() override;

    void onDetach();

    Script& script();

    Script* _script;
};

#endif // __cplusplus >= 202002L

} // namespace

} // namespace

#endif // include guard
