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

#include <Pt/Unit/TestSuite.h>
#include <Pt/Unit/Assertion.h>
#include <Pt/Unit/RegisterTest.h>

#include <Pt/Coroutine.h>
#include <Pt/Generator.h>

namespace Pt {

#if __cplusplus >= 202002L

class PendingAwaiter : public Pt::Awaiter
{
    public:
        explicit PendingAwaiter(bool& cancelled)
        : _cancelled(cancelled)
        {}

        void await_resume()
        {}

    private:
        void onBegin() override
        {}

        void onCancel() override
        { _cancelled = true; }

        bool& _cancelled;
};


class CoroutineTest : public Pt::Unit::TestSuite
{
    public:
        CoroutineTest()
        : Pt::Unit::TestSuite("Pt::CoroutineTest")
        {
            registerMethod("GeneratorTask",                        *this, &CoroutineTest::GeneratorTask);
            registerMethod("GeneratorException",                  *this, &CoroutineTest::GeneratorException);
            registerMethod("TaskException",                       *this, &CoroutineTest::TaskException);
            registerMethod("GeneratorByReference",                *this, &CoroutineTest::GeneratorByReference);
            registerMethod("TaskByReference",                     *this, &CoroutineTest::TaskByReference);
            registerMethod("TaskAssign",                          *this, &CoroutineTest::TaskAssign);
            registerMethod("TaskDestructorCancels",               *this, &CoroutineTest::TaskDestructorCancels);
            registerMethod("NestedTaskDestructorCancels",         *this, &CoroutineTest::NestedTaskDestructorCancels);
            registerMethod("GeneratorCancel",                     *this, &CoroutineTest::GeneratorCancel);
        }

    protected:
        void GeneratorTask()
        {
            // the sum of 1...5 squared is 1 + 4 + 9 + 16 + 25 = 55
            Pt::Task<int> task = squaresTask(5);
            task.run();

            PT_UNIT_ASSERT( task.done() );
            PT_UNIT_ASSERT_EQUAL(task.result(), 55);
        }

        void GeneratorException()
        {
            Pt::Task<int> task = throwingGeneratorTask();
            task.run();

            PT_UNIT_ASSERT( task.done() );
            PT_UNIT_ASSERT_THROW( task.result(), std::runtime_error );
        }

        void TaskException()
        {
            Pt::Task<int> task = throwingTask();
            task.run();

            PT_UNIT_ASSERT( task.done() );
            PT_UNIT_ASSERT_THROW( task.result(), std::runtime_error );
        }

        void GeneratorByReference()
        {
            // Generator yields int& into its own frame locals;
            // consumer doubles each value in place and sums: (10+20+30)*2 = 120
            Pt::Task<int> task = refsTask();
            task.run();

            PT_UNIT_ASSERT( task.done() );
            PT_UNIT_ASSERT_EQUAL( task.result(), 120 );
        }

        void TaskByReference()
        {
            // Inner Task<int&> returns a reference to a local in the outer frame;
            // outer modifies through the reference: 5 * 10 = 50
            Pt::Task<int> task = modifyRefTask();
            task.run();

            PT_UNIT_ASSERT( task.done() );
            PT_UNIT_ASSERT_EQUAL( task.result(), 50 );
        }

        void TaskAssign()
        {
            // Test default constructor
            Pt::Task<int> task;
            PT_UNIT_ASSERT( ! task );  // Should have no handle

            // Test move assignment
            task = squaresTask(3);  // 1² + 2² + 3² = 14
            PT_UNIT_ASSERT( task );  // Should have a handle now

            // Verify task runs correctly
            task.run();
            PT_UNIT_ASSERT( task.done() );
            PT_UNIT_ASSERT_EQUAL( task.result(), 14 );
        }

        void TaskDestructorCancels()
        {
            bool cancelled = false;

            {
                Pt::Task<> task = pendingTask(cancelled);
                task.run();
                PT_UNIT_ASSERT( task );
            }

            PT_UNIT_ASSERT( cancelled );
        }

        void NestedTaskDestructorCancels()
        {
            bool cancelled = false;

            {
                Pt::Task<> task = nestedPendingTask(cancelled);
                task.run();
                PT_UNIT_ASSERT( task );
            }

            PT_UNIT_ASSERT( cancelled );
        }

        void GeneratorCancel()
        {
            bool cancelled = false;
            Pt::Generator<int> generator = pendingGenerator(cancelled);
            Pt::Task<> task = awaitNext(generator);
            task.run();

            generator.cancel();
            PT_UNIT_ASSERT( cancelled );

            task.cancel();
            PT_UNIT_ASSERT( ! task );
        }

    private:
        Pt::Generator<int> throwingGenerator()
        {
            co_yield 1;
            co_yield 2;
            throw std::runtime_error("generator error");
        }

        Pt::Task<int> throwingGeneratorTask()
        {
            auto gen = throwingGenerator();
            int sum = 0;

            while( co_await gen.next() )
                sum += gen.value();

            co_return sum;
        }

        Pt::Task<int> throwingInnerTask()
        {
            throw std::runtime_error("task error");
            co_return 0;
        }

        Pt::Task<int> throwingTask()
        {
            int val = co_await throwingInnerTask();
            co_return val;
        }

        Pt::Generator<int&> refs()
        {
            int values[3] = {10, 20, 30};
            co_yield values[0];
            co_yield values[1];
            co_yield values[2];
        }

        Pt::Task<int> refsTask()
        {
            auto gen = refs();
            int sum = 0;
            while( co_await gen.next() )
            {
                int& v = gen.value();
                v *= 2;
                sum += v;
            }
            co_return sum;
        }

        Pt::Task<int&> refTask(int& val)
        {
            co_return val;
        }

        Pt::Task<int> modifyRefTask()
        {
            int x = 5;
            decltype(auto) ref = co_await refTask(x);
            ref *= 10;
            co_return x;
        }

        Pt::Generator<int> squares(int upTo)
        {
            for (int i = 1; i <= upTo; ++i)
                co_yield i * i;
        }

        Pt::Task<int> squaresTask(int upTo)
        {
            auto gen = squares(upTo);
            int sum = 0;

            while( co_await gen.next() )
                sum += gen.value();

            co_return sum;
        }

        Pt::Task<> pendingTask(bool& cancelled)
        {
            co_await PendingAwaiter(cancelled);
        }

        Pt::Task<> nestedPendingTask(bool& cancelled)
        {
            co_await pendingTask(cancelled);
        }

        Pt::Generator<int> pendingGenerator(bool& cancelled)
        {
            co_await PendingAwaiter(cancelled);
            co_yield 1;
        }

        Pt::Task<> awaitNext(Pt::Generator<int>& generator)
        {
            co_await generator.next();
        }
};

Pt::Unit::RegisterTest<CoroutineTest> _registerCoroutineTest;

#endif // __cplusplus >= 202002L

} // namespace Pt