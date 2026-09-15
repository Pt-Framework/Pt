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

#ifndef PT_GENERATOR_H
#define PT_GENERATOR_H

#include <Pt/Api.h>
#include <Pt/Coroutine.h>

#if __cplusplus >= 202002L

namespace Pt {

/** @internal
*/
template<typename T>
class GeneratorResult
{
    public:
        void set(const T& v)
        { _result = v; }

        void set(T&& v)
        { _result = std::move(v); }

        T get()
        { return std::move(_result); }

    protected:
        GeneratorResult() = default;
        ~GeneratorResult() = default;

        GeneratorResult(const GeneratorResult&) = delete;
        GeneratorResult& operator=(const GeneratorResult&) = delete;

    private:
        T _result{};
};


/** @internal
*/
template<typename T>
class GeneratorResult<T&>
{
    public:
        void set(T& v)
        { _result = &v; }

        T&   get()
        { return *_result; }

    protected:
        GeneratorResult() = default;
        ~GeneratorResult() = default;

        GeneratorResult(const GeneratorResult&) = delete;
        GeneratorResult& operator=(const GeneratorResult&) = delete;

    private:
        T* _result = nullptr;
};


/** @internal
*/
class YieldAwaiter
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

/** @brief Represents a coroutine that yields a sequence of values and may itself co_await.

    A %Generator produces values lazily with co_yield. Unlike a
    synchronous generator, its body may also co_await.

    Consume a generator from a %Task. Await %Generator::next() until it
    returns false and read each value with %Generator::value().

    A %Generator is move-only and owns the coroutine frame.
    %Generator<T&> yields a reference. That object must remain valid
    until the next %Generator::next() or until the generator is
    destroyed.

    Only one %Generator::next() may be pending. Awaiting next() while
    another next() is already pending throws %std::logic_error.

    Exceptions that leave the coroutine body are rethrown from the
    %Generator::next() await.

    The following task sums the values of a generator:

    @code
    Pt::Generator<int> squares(int n)
    {
        for(int i = 1; i <= n; ++i)
            co_yield i * i;
    }

    Pt::Task<int> sumSquares(int n)
    {
        auto gen = squares(n);
        int sum = 0;

        while( co_await gen.next() )
            sum += gen.value();

        co_return sum;
    }
    @endcode

    @ingroup Pt-Coroutines
*/
template<typename T>
class Generator : public AwaiterBase
{
    public:
        /** @internal
        */
        class Promise : public GeneratorResult<T>
                      , public Pt::PromiseBase
        {
            public:
                std::exception_ptr _exception;

                Generator get_return_object()
                {
                    return Generator(std::coroutine_handle<Promise>::from_promise(*this));
                }

                std::suspend_always initial_suspend() noexcept
                { return {}; }

                FinalAwaiter final_suspend() noexcept
                { return {}; }

                void unhandled_exception()
                { _exception = std::current_exception(); }

                void return_void() noexcept
                {}

                template<typename A>
                AwaiterProxy<A> await_transform(A&& a)
                {
                    _pending = &a;
                    return AwaiterProxy<A>{ std::forward<A>(a), this };
                }

                YieldAwaiter yield_value(T value) noexcept
                {
                    this->set(std::forward<T>(value));
                    return {};
                }
        };

        using promise_type = Promise;
        using handle_type = std::coroutine_handle<promise_type>;

        /** @brief Provides the awaitable returned by %Generator::next().

            co_await resumes when the generator yields a value or
            finishes. The resume value is true if %Generator::value()
            is valid. Cancelling the generator cancels a pending
            next() await.

            @ingroup Pt-Coroutines
        */
        class NextAwaiter : public AwaiterBase
        {
            public:
                /** @brief Constructs a next-awaiter for @a generator.
                */
                explicit NextAwaiter(Generator& generator)
                : _generator(&generator)
                , _handle(generator._handle)
                , _isPending(false)
                {}

                /** @brief Detaches from the generator if still pending.
                */
                ~NextAwaiter()
                {
                    if(_generator && _isPending)
                        _generator->detachAwaiter(*this);
                }

                /** @brief Cancels the generator's pending operation.
                */
                void cancel() override
                {
                    if( _handle && ! _handle.done() )
                        _handle.promise().cancel();
                }

                /** @brief Returns true if the generator has already finished.
                */
                bool await_ready() const noexcept
                { return ! _handle || _handle.done(); }

                /** @brief Suspends the consumer and resumes the generator.

                    @throw %std::logic_error if the generator is already pending.
                */
                template<typename FormP>
                std::coroutine_handle<> await_suspend(std::coroutine_handle<FormP> outer)
                {
                    _generator->attachAwaiter(*this);
                    _isPending = true;

                    if( _handle.promise()._outer )
                        throw std::logic_error("generator pending");

                    _handle.promise()._continuation = outer;
                    _handle.promise()._outer = &outer.promise();
                    return _handle;
                }

                /** @brief Returns true if a yielded value is available.

                    Rethrows if the generator body exited with an exception.
                */
                bool await_resume()
                {
                    if(_generator && _isPending)
                    {
                        _generator->detachAwaiter(*this);
                        _isPending = false;
                    }

                    if( ! _handle )
                        return false;

                    if (_handle.promise()._exception)
                        std::rethrow_exception(_handle.promise()._exception);

                    return ! _handle.done();
                }

            private:
                friend class Generator;

                void onDetach()
                {
                    _generator = nullptr;
                    _handle = nullptr;
                    _isPending = false;
                }

                Generator* _generator;
                handle_type _handle;
                bool _isPending;
        };

    public:
        /** @brief Constructs a generator that takes ownership of @a h.
        */
        explicit Generator(handle_type h)
        : _handle(h)
        {}

        /** @brief Moves the coroutine frame from @a other.
        */
        Generator(Generator&& other) noexcept
        : _handle(other._handle)
        {
            other._handle = nullptr;
        }

        /** @brief Cancels the generator if it still owns a coroutine frame.
        */
        ~Generator()
        {
            cancel();
        }

        /** @brief Cancels the generator.

            Aborts the pending awaitable, if any, detaches an in-flight
            next() await, and destroys the coroutine frame.
        */
        void cancel() override
        {
            if(_awaiter)
            {
                _awaiter->onDetach();
                _awaiter = nullptr;
            }

            if (_handle)
            {
                _handle.promise().cancel();
                _handle.destroy();
                _handle = nullptr;
            }
        }

        /** @brief Returns an awaitable that produces the next value.

            co_await of the result is true while a value is available.
            Use %Generator::value() to read that value.
        */
        NextAwaiter next()
        {
            return NextAwaiter(*this);
        }

        /** @brief Returns the last yielded value.

            Valid after a co_await of %Generator::next() returned true.
        */
        T value()
        {
            return _handle.promise().get();
        }

    private:
        void attachAwaiter(NextAwaiter& awaiter)
        {
            if(_awaiter && _awaiter != &awaiter)
                throw std::logic_error("generator pending");

            _awaiter = &awaiter;
        }

        void detachAwaiter(NextAwaiter& awaiter)
        {
            if(_awaiter == &awaiter)
                _awaiter = nullptr;
        }

        Generator(const Generator&) = delete;
        Generator& operator=(const Generator&) = delete;

    private:
        handle_type _handle;
        NextAwaiter* _awaiter = nullptr;
};

} // namespace Pt

#endif // __cplusplus >= 202002L

#endif // PT_GENERATOR_H
