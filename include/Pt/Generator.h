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

/** @brief A coroutine based active generator that allows co_await functionality.

    This Generator integrates natively into Pt::Task and Pt::Awaiter semantics.
    It produces values lazily by resuming via a NextAwaiter. Unlike standard synchronous
    generators, its body may also co_await as it maps back to the EventLoop correctly.

    @ingroup BasicTypes
*/
template<typename T>
class Generator : public AwaiterBase
{
    public:
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

        class NextAwaiter : public AwaiterBase
        {
            public:
                explicit NextAwaiter(handle_type h)
                : _handle(h)
                {}

                void cancel() override
                {
                    if( _handle && ! _handle.done() )
                        _handle.promise().cancel();
                }

                bool await_ready() const noexcept
                { return ! _handle || _handle.done(); }

                template<typename FormP>
                std::coroutine_handle<> await_suspend(std::coroutine_handle<FormP> outer)
                {
                    if( _handle.promise()._outer )
                        throw std::logic_error("generator pending");

                    _handle.promise()._continuation = outer;
                    _handle.promise()._outer = &outer.promise();
                    return _handle;
                }

                bool await_resume()
                {
                    if (_handle.promise()._exception)
                        std::rethrow_exception(_handle.promise()._exception);

                    return ! _handle.done();
                }

            private:
                handle_type _handle;
        };

    public:
        explicit Generator(handle_type h)
        : _handle(h)
        {}

        Generator(Generator&& other) noexcept
        : _handle(other._handle)
        {
            other._handle = nullptr;
        }

        ~Generator()
        {
            if (_handle)
                _handle.destroy();
        }

        void cancel() override {
            if (_handle) {
                _handle.promise().cancel();
                _handle.destroy();
                _handle = nullptr;
            }
        }

        NextAwaiter next()
        {
            return NextAwaiter{_handle};
        }

        T value()
        {
            return _handle.promise().get();
        }

    private:
        Generator(const Generator&) = delete;
        Generator& operator=(const Generator&) = delete;

    private:
        handle_type _handle;
};

} // namespace Pt

#endif // __cplusplus >= 202002L

#endif // PT_GENERATOR_H