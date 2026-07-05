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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301 USA
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

/** @brief Base class for all co_await-able types used inside a %Task coroutine.

    Both %Awaiter (IO-driven awaitables) and %Task<T> (coroutine chaining)
    derive from this class, allowing to cancel any pending operation through
    a single virtual dispatch.

    @ingroup BasicTypes
*/
class AwaiterBase
{
    public:
        /** @brief Cancel the pending operation.
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

/** @brief Base class for C++20 awaitables driven by an event-loop signal.

    Subclasses implement two customization points:
    - onBegin(): subscribe to the completion signal and start the operation.
    - onCancel(): abort the in-flight operation.

    The result is retrieved via await_resume() in the subclass.

    @ingroup BasicTypes
*/
class Awaiter : public AwaiterBase
{
    public:
        bool await_ready() const
        { return false; }

        template<typename P>
        bool await_suspend(std::coroutine_handle<P> h)
        {
            _handle = h;
            onBegin();
            return true;
        }

        void cancel() override
        {
            onCancel();
        }

    protected:
        Awaiter()
        {}

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
        virtual void onBegin() = 0;

        virtual void onCancel() = 0;

    protected:
        std::coroutine_handle<> _handle;
};


class PromiseBase
{
    public:
        void setFinished()
        {
            _pending = nullptr;
        }

        void cancel()
        {
            if(_pending)
            {
                _pending->cancel();
                _pending = nullptr;
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

/** @brief Cancellable coroutine task for single-threaded async operations.

    Manages the lifetime of a C++20 coroutine frame. The task starts
    suspended; call run() to begin execution. Call cancel() to abort
    a suspended coroutine and immediately destroy its frame. Exceptions
    in the coroutine body must be handled with try/catch; unhandled
    exceptions call %std::terminate().

    A %Task<T> is itself co_await-able, allowing coroutine chaining: an
    outer coroutine can co_await an inner %Task<T> and resume when it
    completes, with the result available as the co_await expression value.

    @ingroup BasicTypes
*/
template<typename T = void>
class Task : public AwaiterBase
{
    public:
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
        explicit Task(handle_type h)
        : _handle(h)
        {}

        Task(Task&& other) noexcept
        : _handle(other._handle)
        {
            other._handle = nullptr;
        }

        ~Task()
        {
            if( _handle )
                _handle.destroy();
        }

        /** @brief Start execution of the coroutine.
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

        /** @brief Request cancellation of the running coroutine.
        */
        void cancel() override
        {
            if( _handle )
            {
                _handle.promise().cancel();
                _handle.destroy();
                _handle = nullptr;
            }
        }

        /** @brief Returns true if the coroutine has finished normally.
        */
        bool done() const
        { return _handle && _handle.done(); }

        /** @brief Returns true if the task has an associated coroutine frame.
        */
        explicit operator bool() const
        { return _handle != nullptr; }

        /** @brief Retrieve the coroutine result. Only valid after done() == true.
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

        /** @brief Suspend the outer coroutine and start the inner coroutine.
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

        /** @brief Resume the outer coroutine with the result of the inner task.
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