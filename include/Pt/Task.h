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

#ifndef PT_TASK_H
#define PT_TASK_H

#include <Pt/Api.h>
#include <Pt/Connectable.h>

#if __cplusplus >= 202002L
#include <coroutine>
#include <exception>

namespace Pt {

class Awaiter;
class Task;

/** @brief Coroutine return type for detached fire-and-forget coroutines.

    Use as the return type of a coroutine function that uses co_await
    but does not return a value to the caller. The coroutine runs
    detached; the caller has no handle and cannot await the result.

    @ingroup BasicTypes
*/
class DetachedTask
{
    public:
        struct promise_type
        {
            DetachedTask get_return_object()
            { return DetachedTask{}; }

            std::suspend_never initial_suspend() noexcept
            { return {}; }

            std::suspend_never final_suspend() noexcept
            { return {}; }

            void return_void()
            {}

            void unhandled_exception()
            { std::terminate(); }
        };
};

/** @brief Cancellable coroutine task for single-threaded async operations.

    Manages the lifetime of a C++20 coroutine frame. The task starts
    suspended; call run() to begin execution. Call cancel() to abort
    a suspended coroutine and immediately destroy its frame. Exceptions
    in the coroutine body must be handled with try/catch; unhandled
    exceptions call %std::terminate().

    @ingroup BasicTypes
*/
class Task
{
    public:
        struct Promise
        {
            Task get_return_object()
            {
                auto handle = Task::handle_type::from_promise(*this);
                return Task(handle);
            }

            std::suspend_always initial_suspend() noexcept
            { return {}; }

            std::suspend_always final_suspend() noexcept
            { return {}; }

            void return_void()
            {}

            void unhandled_exception()
            { std::terminate(); }

            template<typename A>
            A&& await_transform(A&& a)
            {
                _awaiter = &a;
                return std::forward<A>(a);
            }

            void setFinished()
            {
                _awaiter = nullptr;
            }

            void cancel();

            Awaiter* _awaiter = nullptr;
        };

    public:
        using promise_type = Promise;
        using handle_type = std::coroutine_handle<Promise>;

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
                _handle.resume();
        }

        /** @brief Request cancellation of the running coroutine.
        */
        void cancel()
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

    private:
        Task(const Task&) = delete;
        Task& operator=(const Task&) = delete;

        handle_type _handle;
};

/** @brief Base class for C++20 awaitables driven by an event-loop signal.

    Subclasses implement two customization points:
    - onBegin(): subscribe to the completion signal and start the operation.
    - onCancel(): abort the in-flight operation.

    The result is retrieved via await_resume() in the subclass.

    @ingroup BasicTypes
*/
class Awaiter : public Connectable
{
    using handle_type = std::coroutine_handle<Task::promise_type>;

    public:
        bool await_ready() const
        { return false; }

        bool await_suspend(handle_type h)
        {
            _handle = h;
            onBegin();
            return true;
        }

        void cancel()
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
                _handle.promise().setFinished();
                _handle.resume();
            }
        }

    protected:
        virtual void onBegin() = 0;

        virtual void onCancel() = 0;

    protected:
        handle_type _handle;
};


inline void Task::Promise::cancel()
{
    if(_awaiter)
    {
        _awaiter->cancel();
        _awaiter = nullptr;
    }
}

} // namespace Pt

#endif // __cplusplus >= 202002L

#endif // PT_TASK_H