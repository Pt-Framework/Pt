/* Copyright (C) 2020-2026 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#ifndef PT_LUA_API_CALLS_H
#define PT_LUA_API_CALLS_H

/** @addtogroup Pt-Lua-Calls

    @brief Native work Lua invokes in C++.

    Lua reaches C++ through the Reflex catalog the context bound.
    A method, property, or constructor becomes a %Call. The script
    yields, reports %NativeCall, runs that call, and pushes the
    result back to Lua. The script owns the %Call. Application
    code does not construct %MethodCall, %PropertyGetCall,
    %PropertySetCall, or %ConstructorCall.

    A function or method whose return type is %AsyncCall* is an
    asynchronous native call. Lua invokes it, the function returns
    a heap %AsyncCall, and the script takes ownership. The script
    must be attached to an %EventLoop. It binds the call, starts
    it with the loop, and resumes when %finished() is emitted.
    Blocking %advance() throws if such a call is pending.

    Implement async work by subclassing %BasicAsyncCall. For a
    result type R, override %onBeginCall() to start work on the
    loop, %onResult() to produce R, and %onCancel() to abort.
    For void, there is no %onResult(). Call %setReady() when the
    work is done, or %setError() if it failed. %AsyncFunction is
    a Reflex function helper whose return type is already
    %AsyncCall*.

    The example is a timer wait registered as a Lua global. The
    script must use %beginAdvance() on an event loop.

    @code
    class WaitCall : public Pt::Lua::BasicAsyncCall<void>
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

        int _ms;
        Pt::System::Timer _timer;
    };

    Pt::Lua::AsyncCall* waitAsync(int ms)
    {
        return new WaitCall(ms);
    }

    tm.registerFunction("wait", &waitAsync);
    @endcode
*/

#endif
