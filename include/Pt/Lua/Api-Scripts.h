/* Copyright (C) 2020-2026 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#ifndef PT_LUA_API_SCRIPTS_H
#define PT_LUA_API_SCRIPTS_H

/** @addtogroup Pt-Lua-Scripts

    @brief Load, advance and read a Lua script.

    A %Script loads a C string of Lua source into a %Context and
    runs it as a coroutine of that context. Only one script may use
    a context at a time; a second construction throws
    %std::logic_error. A syntax error does not throw: the script
    is %ScriptError, %errorMessage() holds the compiler text, and
    the context can be used again.

    The script is a %Selectable. Blocking %advance() runs on the
    calling thread and returns at the next step. %Yield means the
    coroutine yielded so other work can run; call %advance()
    again. %NativeCall means Lua invoked a reflected method,
    property, or constructor; the next %advance() performs that
    %Call and resumes. %ScriptOk means the chunk finished.
    %ScriptError means the chunk or a native call failed. A loop
    that continues on %Yield and %NativeCall, and stops on
    %ScriptOk or %ScriptError, is the usual blocking path.

    Blocking %advance() cannot start an %AsyncCall. A function or
    method that returns %AsyncCall* needs the event-loop path:
    %setActive() on an %EventLoop, %beginAdvance() to start a
    step, %endAdvance() in the %advanced() slot, and another
    %beginAdvance() while the status is %Yield or %NativeCall.
    The loop does not own the script. Keep the script alive until
    no step is waiting. %cancel() aborts a pending step and any
    active async call.

    After %ScriptOk, %Result reads a named global from the
    context's Lua state as an int. Construct it with
    %Context::state(). C++20 %advanceAsync() is the same
    asynchronous step as an awaitable: %co_await script.advanceAsync()
    returns the %Status of that step. Only one awaiter may be
    pending on a script.
*/

#endif
