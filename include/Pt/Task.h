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

#if __cplusplus >= 202002L

#include <coroutine>
#include <exception>
#include <stop_token>

namespace Pt {

/** @brief Coroutine return type for detached fire-and-forget coroutines.

    Use as the return type of a coroutine function that uses co_await
    but does not return a value to the caller. The coroutine runs
    detached — the caller has no handle and cannot await the result.

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


/** @brief Cancellable coroutine return type with lazy start.

    A Task starts suspended. Call run() to begin execution.
    The caller can cancel a running Task via cancel(), which
    propagates a stop request to any awaitable that supports
    setStopToken(). On cancel, awaitables skip their work and
    the coroutine runs through to completion. Check
    isCancelled() to detect cancellation.

    @ingroup BasicTypes
*/
class Task
{
    public:
        struct promise_type
        {
            Task get_return_object()
            {
                auto h = handle_type::from_promise(*this);
                return Task(h);
            }

            std::suspend_always initial_suspend() noexcept
            { return {}; }

            std::suspend_always final_suspend() noexcept
            { return {}; }

            void return_void()
            { _cancelled = _stopSource.stop_requested(); }

            void unhandled_exception()
            { _exception = std::current_exception(); }

            template<typename A>
            A&& await_transform(A&& a)
            {
                a.setStopToken(_stopSource.get_token());
                return static_cast<A&&>(a);
            }

            std::stop_source _stopSource;
            std::exception_ptr _exception;
            bool _cancelled = false;
        };

        using handle_type = std::coroutine_handle<promise_type>;

        Task(Task&& other) noexcept
        : _handle(other._handle)
        { other._handle = nullptr; }

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
                _handle.promise()._stopSource.request_stop();
        }

        /** @brief Returns true if the coroutine has finished.
        */
        bool done() const
        { return _handle && _handle.done(); }

        /** @brief Returns true if the coroutine was cancelled.
        */
        bool isCancelled() const
        { return _handle && _handle.promise()._cancelled; }

        /** @brief Re-throw any stored exception from the coroutine.
        */
        void rethrowIfFailed()
        {
            if( _handle && _handle.promise()._exception )
                std::rethrow_exception(_handle.promise()._exception);
        }

    private:
        explicit Task(handle_type h)
        : _handle(h)
        {}

        Task(const Task&) = delete;
        Task& operator=(const Task&) = delete;

        handle_type _handle;
};

} // namespace Pt

#endif // __cplusplus >= 202002L

#endif // PT_TASK_H
