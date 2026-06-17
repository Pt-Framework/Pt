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

} // namespace Pt

#endif // __cplusplus >= 202002L

#endif // PT_TASK_H
