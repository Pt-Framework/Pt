/* Copyright (C) 2020-2026 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
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

/** @brief Lua script.

    %Script is the coroutine that runs Lua source in a %Context.
    The source is a C string loaded at construction. Only one
    script may use a context at a time; a second construction
    throws %std::logic_error. A syntax error does not throw: the
    status is %ScriptError, %errorMessage() holds the compiler
    text, and the context is free for another script.

    The script is a %Selectable. Blocking %advance() runs on the
    calling thread and returns at the next step. Continue while
    the status is %Yield or %NativeCall. %Yield is a cooperative
    pause so other work can run. %NativeCall means Lua invoked a
    reflected method, property, or constructor; the next
    %advance() performs that %Call and resumes. Stop on %ScriptOk
    or %ScriptError. Blocking %advance() throws if an %AsyncCall
    is pending.

    Asynchronous work attaches the script with %setActive() on an
    %EventLoop. %beginAdvance() starts a step, %advanced() is
    emitted when the step completes, and %endAdvance() returns the
    status. Call %beginAdvance() again while the status is %Yield
    or %NativeCall. The loop does not own the script. Keep it
    alive until no step is waiting. %cancel() aborts a pending
    step and any active async call.

    C++20 %advanceAsync() is that asynchronous step as an
    awaitable. %fromState() finds the script stored in a Lua
    registry, for Lua C API code that needs it. The script is not
    copyable.

    @ingroup Pt-Lua-Scripts
*/
class PT_LUA_API Script : public System::Selectable
                        , public Connectable
{
  public:
    /** @brief Step reached by the last advance.
    */
    enum Status
    {
      Yield,       //!< Cooperative pause of the Lua coroutine.
      NativeCall,  //!< A reflected or asynchronous native call is pending.
      ScriptOk,    //!< The chunk finished.
      ScriptError  //!< Compile or runtime failure.
    };

    /** @brief Loads @a script into @a ctx as a coroutine.

        @throw %std::logic_error if @a ctx already has a script.
    */
    Script(Context& ctx, const char* script);

    /** @brief Cancels pending work and releases the context.
    */
    ~Script();

    /** @brief Starts an asynchronous advance step on the event loop.
    */
    void beginAdvance();

    /** @brief Returns the status of the last advance step.
    */
    Status endAdvance() const;

    /** @brief Signal emitted when an asynchronous advance step completes.
    */
    Pt::Signal<>& advanced();

    /** @brief Advances the script on the calling thread.

        @throw %std::logic_error if an asynchronous native call is pending.
    */
    Status advance();

    /** @brief Returns the last compile or runtime error text.
    */
    const std::string& errorMessage() const
    { return _errorMsg; }

    /** @brief Returns the script stored in the registry of @a L, or a null pointer.
    */
    static Script* fromState(lua_State* L);

#if __cplusplus >= 202002L
    /** @brief Returns an awaitable for one asynchronous advance step.
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

/** @brief Awaitable advance of a Lua script.

    %AsyncAdvance is the C++20 form of %beginAdvance(),
    %endAdvance(), and %advanced(). %co_await script.advanceAsync()
    runs one step and %await_resume() returns that step's %Status.
    Continue awaiting while the status is %Yield or %NativeCall.

    Only one awaiter may be pending on a script. A second
    construction throws %std::logic_error. Destroying a pending
    awaiter cancels the script. Destroying the script detaches the
    awaiter.

    @ingroup Pt-Lua-Scripts
*/
class PT_LUA_API AsyncAdvance : public Pt::Awaiter
                              , public Pt::Connectable
{
    public:
    /** @brief Creates an awaitable for one advance of @a script.

        @throw %std::logic_error if another awaiter is already pending.
    */
    AsyncAdvance(Script& script);

    /** @brief Cancels a pending step and detaches from the script.
    */
    ~AsyncAdvance();

    /** @brief Returns the status of the completed advance step.
    */
    Script::Status await_resume();

    private:
    friend class Script;

    void onBegin() override;

    void onCancel() override;

    void onDetach();

    void onAdvanced();

    Script& script();

    Script* _script;
    bool _isPending;
};

#endif // __cplusplus >= 202002L

} // namespace

} // namespace

#endif // include guard
