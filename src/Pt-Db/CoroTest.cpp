/*
 * Copyright (C) 2026 by Marc Boris Duerner
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 */

#undef PT_DB_API_EXPORT

#if __cplusplus >= 202002L

#include <Pt/Unit/Assertion.h>
#include <Pt/Unit/TestSuite.h>
#include <Pt/Unit/RegisterTest.h>
#include <Pt/Task.h>
#include <Pt/Db/Connection.h>
#include <Pt/Db/Result.h>
#include <Pt/System/MainLoop.h>


class CoroTest : public Pt::Unit::TestSuite
{
public:
    CoroTest()
    : Pt::Unit::TestSuite("Pt::Db::CoroTest")
    {
        registerMethod("OpenCloseAsync", *this, &CoroTest::openCloseAsync);
        registerMethod("ExecuteAsync",   *this, &CoroTest::executeAsync);
        registerMethod("SelectAsync",    *this, &CoroTest::selectAsync);
        registerMethod("CancelSelect",   *this, &CoroTest::cancelSelect);
    }

protected:
    void openCloseAsync();
    void executeAsync();
    void selectAsync();
    void cancelSelect();
};

Pt::Unit::RegisterTest<CoroTest> register_CoroTest;


static Pt::Task coroOpenClose(Pt::System::MainLoop& loop)
{
    Pt::Db::Connection conn("sqlite");
    conn.setActive(loop);

    co_await conn.openAsync(":memory:");
    PT_UNIT_ASSERT( conn.isOpen() );

    co_await conn.closeAsync();
    PT_UNIT_ASSERT( ! conn.isOpen() );

    loop.exit();
}

void CoroTest::openCloseAsync()
{
    Pt::System::MainLoop loop;
    Pt::Task task = coroOpenClose(loop);
    task.run();
    loop.run();
    task.rethrowIfFailed();
}


static Pt::Task coroExecute(Pt::System::MainLoop& loop)
{
    Pt::Db::Connection conn("sqlite");
    conn.setActive(loop);

    co_await conn.openAsync(":memory:");
    co_await conn.executeAsync("CREATE TABLE t (id INTEGER)");

    std::size_t n = co_await conn.executeAsync("INSERT INTO t VALUES (1)");
    PT_UNIT_ASSERT( n == 1 );

    n = co_await conn.executeAsync("INSERT INTO t VALUES (2)");
    PT_UNIT_ASSERT( n == 1 );

    loop.exit();
}

void CoroTest::executeAsync()
{
    Pt::System::MainLoop loop;
    Pt::Task task = coroExecute(loop);
    task.run();
    loop.run();
    task.rethrowIfFailed();
}


static Pt::Task coroSelect(Pt::System::MainLoop& loop)
{
    Pt::Db::Connection conn("sqlite");
    conn.setActive(loop);

    co_await conn.openAsync(":memory:");
    co_await conn.executeAsync("CREATE TABLE t (id INTEGER, val TEXT)");
    co_await conn.executeAsync("INSERT INTO t VALUES (1, 'alpha')");
    co_await conn.executeAsync("INSERT INTO t VALUES (2, 'beta')");

    Pt::Db::Result r = co_await conn.selectAsync("SELECT * FROM t ORDER BY id");
    PT_UNIT_ASSERT( r.size() == 2 );
    PT_UNIT_ASSERT( r[0][1].getString() == "alpha" );
    PT_UNIT_ASSERT( r[1][1].getString() == "beta" );

    loop.exit();
}

void CoroTest::selectAsync()
{
    Pt::System::MainLoop loop;
    Pt::Task task = coroSelect(loop);
    task.run();
    loop.run();
    task.rethrowIfFailed();
}


static Pt::Task coroCancelSelect(Pt::System::MainLoop& loop)
{
    Pt::Db::Connection conn("sqlite");
    conn.setActive(loop);

    co_await conn.openAsync(":memory:");
    co_await conn.executeAsync("CREATE TABLE t (id INTEGER)");
    co_await conn.executeAsync("INSERT INTO t VALUES (1)");

    co_await conn.selectAsync("SELECT * FROM t");

    loop.exit();
}

void CoroTest::cancelSelect()
{
    Pt::System::MainLoop loop;
    Pt::Task task = coroCancelSelect(loop);

    task.run();    // start — coroutine suspends at openAsync
    task.cancel(); // request_stop → next co_await skips, coroutine completes

    PT_UNIT_ASSERT( task.done() );
    PT_UNIT_ASSERT( task.isCancelled() );
}

#endif // __cplusplus >= 202002L
