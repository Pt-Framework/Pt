/*
 * Copyright (C) 2026 by Marc Boris Duerner
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the:
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA
 */

#ifndef PT_COROUTINE_H
#define PT_COROUTINE_H

#include <Pt/Api.h>
#include <Pt/Connectable.h>

#if __cplusplus >= 202002L

#include <coroutine>
#include <exception>
#include <stdexcept>

namespace Pt {

/** @brief Defines the cancellation interface for a pending awaitable.

    %Task, %Awaiter and %Generator derive from this class so a single
    %AwaiterBase::cancel() can abort whichever operation is in flight.
    Application code uses those derived types rather than %AwaiterBase
    directly.

    @ingroup Pt-Coroutines
*/
class AwaiterBase
{
    public:
        /** @brief Cancels the pending operation.
        */
        virtual void cancel() = 0;

    protected:
        /** @brief Constructor.
        */
        AwaiterBase() = default;

        /** @brief No copy constructor.
        */
        AwaiterBase(const AwaiterBase&) = delete;

        /** @brief No copy assignment.
        */
        AwaiterBase& operator=(const AwaiterBase&) = delete;

        /** @brief Destructor.
        */
        virtual ~AwaiterBase() = default;
};

/** @brief Provides the base class for I/O-driven co_await-able operations.

    Derive from %Awaiter to wrap an asynchronous operation so a %Task
    or %Generator can suspend until it completes. Implement %Awaiter::onBegin()
    to subscribe to completion and start the work. Implement
    %Awaiter::onCancel() to abort that work. Call %Awaiter::setReady()
    when the operation finishes; that resumes the waiting coroutine.

    Implement await_resume() in the subclass to deliver the result, or
    derive from %BasicAwaiter when the awaitable only needs to produce
    a value through %BasicAwaiter::onReady().

    The following awaiter starts a device operation and resumes when
    the device signals completion:

    @code
    class AsyncOp : public Pt::Awaiter
    {
        public:
            explicit AsyncOp(Device& device)
            : _device(device)
            {}

            int await_resume()
            {
                return _device.endOp();
            }

        protected:
            void onBegin() override
            {
                _device.finished() += Pt::slot(*this, &AsyncOp::setReady);
                _device.beginOp();
            }

            void onCancel() override
            {
                _device.cancel();
            }

        private:
            Device& _device;
    };
    @endcode

    @ingroup Pt-Coroutines
*/
class Awaiter : public AwaiterBase
{
    public:
        /** @brief Returns false so co_await always suspends.
        */
        bool await_ready() const
        { return false; }

        /** @brief Starts the operation and suspends the coroutine.
        */
        template<typename P>
        bool await_suspend(std::coroutine_handle<P> h)
        {
            _handle = h;
            onBegin();
            return true;
        }

        /** @brief Cancels the in-flight operation.
        */
        void cancel() override
        {
            _handle = nullptr;
            onCancel();
        }

    protected:
        /** @brief Constructor.
        */
        Awaiter()
        {}

        /** @brief Resumes the waiting coroutine.

            Call this when the asynchronous operation has completed.
            Typically connect a completion signal to this method.
        */
        void setReady()
        {
            if( _handle )
            {
                std::coroutine_handle<> h = _handle;
                _handle = nullptr;
                h.resume();
            }
        }

    protected:
        /** @brief Starts the asynchronous operation.

            Subscribe to completion and begin the work. The coroutine
            remains suspended until %Awaiter::setReady() is called.
        */
        virtual void onBegin() = 0;

        /** @brief Aborts the in-flight operation.

            Called from %Awaiter::cancel() when the waiting task or
            generator is cancelled.
        */
        virtual void onCancel() = 0;

    protected:
        std::coroutine_handle<> _handle;
};


/** @brief Provides an awaitable that delivers a result through onReady().

    Use %BasicAwaiter instead of %Awaiter when the awaitable only needs
    to produce a value, or to complete with no value. Subclasses still
    implement %Awaiter::onBegin() and %Awaiter::onCancel(). Implement
    %BasicAwaiter::onReady() to return the result. The co_await
    expression is that return value.

    %BasicAwaiter<void> is the specialization for operations that do
    not produce a result. Its %BasicAwaiter::onReady() returns nothing
    and can finalize or clean up the operation.

    @ingroup Pt-Coroutines
*/
template<typename R>
class BasicAwaiter : public Awaiter
{
    public:
        /** @brief Returns the result produced when the awaitable resumes.
        */
        R await_resume()
        {
            return onReady();
        }

    protected:
        /** @brief Returns the result for the co_await expression.

            Called exactly once when the coroutine is resumed.
        */
        virtual R onReady() = 0;
};


/** @brief Specializes %BasicAwaiter for awaitables without a result.

    Use this when the operation completes without producing a value.
    Implement %BasicAwaiter::onReady() to finalize or clean up.

    @ingroup Pt-Coroutines
*/
template<>
class BasicAwaiter<void> : public Awaiter
{
    public:
        /** @brief Resumes the coroutine after the operation completed.
        */
        void await_resume()
        { onReady(); }

    protected:
        /** @brief Finalizes the operation when the awaitable resumes.

            Called exactly once when the coroutine is resumed.
        */
        virtual void onReady() = 0;
};


/** @internal
*/
class PromiseBase
{
    public:
        void setFinished()
        {
            _pending = nullptr;
        }

        void cancel()
        {
            AwaiterBase* pending = _pending;
            _pending = nullptr;
            if(pending)
            {
                pending->cancel();
            }
        }

        AwaiterBase*            _pending      = nullptr;
        std::coroutine_handle<> _continuation = nullptr;
        PromiseBase*            _outer        = nullptr;

    protected:
        PromiseBase() = default;
        ~PromiseBase() = default;

        PromiseBase(const PromiseBase&) = delete;
        PromiseBase& operator=(const PromiseBase&) = delete;
};


/** @internal
*/
template<typename T>
class PromiseResult
{
    public:
        void return_value(T v)
        { _result = std::move(v); }

        T getResult()
        { return std::move(_result); }

        T _result{};

    protected:
        PromiseResult() = default;
        ~PromiseResult() = default;

        PromiseResult(const PromiseResult&) = delete;
        PromiseResult& operator=(const PromiseResult&) = delete;
};


/** @internal
*/
template<typename T>
class PromiseResult<T&>
{
    public:
        void return_value(T& v)
        { _result = &v; }

        T& getResult()
        { return *_result; }

        T* _result = nullptr;

    protected:
        PromiseResult() = default;
        ~PromiseResult() = default;

        PromiseResult(const PromiseResult&) = delete;
        PromiseResult& operator=(const PromiseResult&) = delete;
};


/** @internal
*/
template<>
class PromiseResult<void>
{
    public:
        void return_void()
        {}

        void getResult()
        {}

    protected:
        PromiseResult() = default;
        ~PromiseResult() = default;

        PromiseResult(const PromiseResult&) = delete;
        PromiseResult& operator=(const PromiseResult&) = delete;
};


/** @internal
*/
template<typename A>
class AwaiterProxy
{
    public:
        A&& _awaitable;
        PromiseBase* _promise;

        bool await_ready()
        { return _awaitable.await_ready(); }

        template<typename P>
        auto await_suspend(std::coroutine_handle<P> h) -> decltype(_awaitable.await_suspend(h))
        { return _awaitable.await_suspend(h); }

        auto await_resume() -> decltype(_awaitable.await_resume())
        {
            _promise->setFinished();
            return _awaitable.await_resume();
        }
};


/** @internal
*/
class FinalAwaiter
{
    public:
        bool await_ready() const noexcept
        { return false; }

        template<typename P>
        std::coroutine_handle<> await_suspend(std::coroutine_handle<P> h) noexcept
        {
            PromiseBase* outer = h.promise()._outer;

            if( outer && outer != &h.promise() )
                outer->setFinished();

            h.promise()._outer = nullptr;

            if( h.promise()._continuation )
                return h.promise()._continuation;

            return std::noop_coroutine();
        }

        void await_resume() noexcept
        {}
};

/** @brief Represents a cancellable C++20 coroutine that produces a single result.

    A %Task is move-only and owns the coroutine frame. A
    default-constructed task is empty; %Task::operator bool() is false
    until a coroutine is assigned. Move assignment cancels the current
    task first.

    %Task<void> produces no value. %Task<T&> returns a reference; that
    object must outlive the consumer.

    An outer coroutine can co_await an inner %Task. The co_await
    expression is the inner result:

    @code
    Pt::Task<int> inner()
    {
        co_return 41;
    }

    Pt::Task<int> outer()
    {
        int n = co_await inner();
        co_return n + 1;
    }
    @endcode

    %Task::run() and awaiting a task that is already pending throw
    %std::logic_error.

    @ingroup Pt-Coroutines
*/
template<typename T = void>
class Task : public AwaiterBase
{
    public:
        /** @internal
        */
        class Promise : public PromiseResult<T>
                      , public PromiseBase
        {
            public:
                std::exception_ptr _exception;

                template<typename A>
                AwaiterProxy<A> await_transform(A&& a)
                {
                    _pending = &a;
                    return AwaiterProxy<A>{ std::forward<A>(a), this };
                }

                Task get_return_object()
                {
                    return Task(std::coroutine_handle<promise_type>::from_promise(*this));
                }

                std::suspend_always initial_suspend() noexcept
                { return {}; }

                FinalAwaiter final_suspend() noexcept
                { return {}; }

                void unhandled_exception()
                { _exception = std::current_exception(); }
        };

        using promise_type = Promise;
        using handle_type = std::coroutine_handle<promise_type>;

    public:
        /** @brief Constructs an empty task with no coroutine frame.
        */
        Task() noexcept
        : _handle(nullptr)
        {}

        /** @brief Constructs a task that takes ownership of @a h.
        */
        explicit Task(handle_type h)
        : _handle(h)
        {}

        /** @brief Moves the coroutine frame from @a other.
        */
        Task(Task&& other) noexcept
        : _handle(other._handle)
        {
            other._handle = nullptr;
        }

        /** @brief Moves the coroutine frame from @a other.

            Cancels this task before taking ownership of @a other.
        */
        Task& operator=(Task&& other) noexcept
        {
            if(this != &other)
            {
                cancel();
                _handle = other._handle;
                other._handle = nullptr;
            }
            return *this;
        }

        /** @brief Cancels the task if it still owns a coroutine frame.
        */
        ~Task()
        { cancel(); }

        /** @brief Starts execution of the coroutine.

            Has no effect if the task is empty or already finished.

            @throw %std::logic_error if the task is already pending.
        */
        void run()
        {
            if( _handle && ! _handle.done() )
            {
                if (_handle.promise()._outer)
                    throw std::logic_error("task pending");

                _handle.promise()._outer = &_handle.promise();
                _handle.resume();
            }
        }

        /** @brief Cancels the running coroutine.

            Aborts the pending awaitable, if any, and destroys the
            coroutine frame. The task is empty afterwards.
        */
        void cancel() override
        {
            if( _handle )
            {
                _handle.promise().cancel();
                handle_type handle = _handle;
                _handle = nullptr;
                handle.destroy();
            }
        }

        /** @brief Returns true if the coroutine has finished.
        */
        bool done() const
        { return _handle && _handle.done(); }

        /** @brief Returns true if the task has an associated coroutine frame.
        */
        explicit operator bool() const
        { return _handle != nullptr; }

        /** @brief Returns the coroutine result.

            Use this after %Task::done() is true. If the coroutine body
            exited with an exception, that exception is rethrown.
        */
        T result()
        {
            if( _handle.promise()._exception )
                std::rethrow_exception(_handle.promise()._exception);

            return _handle.promise().getResult();
        }

        /** @brief Returns true if the inner coroutine has already finished.
        */
        bool await_ready() const noexcept
        { return done(); }

        /** @brief Suspends the outer coroutine and starts the inner coroutine.

            @throw %std::logic_error if the task is already pending.
        */
        template<typename P>
        std::coroutine_handle<> await_suspend(std::coroutine_handle<P> outer)
        {
            if(_handle.promise()._outer)
            {
                throw std::logic_error("task pending");
            }

            _handle.promise()._continuation = outer;
            _handle.promise()._outer = &outer.promise();
            return _handle;
        }

        /** @brief Returns the result of the inner task.

            Rethrows if the inner coroutine body exited with an exception.
        */
        T await_resume()
        {
            if( _handle.promise()._exception )
                std::rethrow_exception(_handle.promise()._exception);

            return _handle.promise().getResult();
        }

    private:
        Task(const Task&) = delete;
        Task& operator=(const Task&) = delete;

        handle_type _handle;

};

} // namespace Pt

#endif // __cplusplus >= 202002L

#endif // PT_COROUTINE_H
