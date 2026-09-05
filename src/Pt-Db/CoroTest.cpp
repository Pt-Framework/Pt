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
#include <Pt/Db/Connection.h>
#include <Pt/Db/Result.h>
#include <Pt/System/MainLoop.h>


class CoroTest : public Pt::Unit::TestSuite
{
    public:
        CoroTest()
        : Pt::Unit::TestSuite("Pt::Db::CoroTest")
        , _loop(nullptr)
        {
            registerMethod("OpenClose",    *this, &CoroTest::openClose);
            registerMethod("Execute",       *this, &CoroTest::execute);
            registerMethod("Select",        *this, &CoroTest::select);
            registerMethod("CancelSelect",  *this, &CoroTest::cancelSelect);
            registerMethod("DestroyConnection", *this, &CoroTest::destroyConnection);
            registerMethod("DestroyStoredAwaiterConnection", *this,
                           &CoroTest::destroyStoredAwaiterConnection);
            registerMethod("DestroyTask",    *this, &CoroTest::destroyTask);
            registerMethod("PendingAwaiter",  *this, &CoroTest::pendingAwaiter);
            registerMethod("NestedTask",    *this, &CoroTest::nestedTask);
            registerMethod("DeferredArguments", *this, &CoroTest::deferredArguments);
        }

    protected:
        void setUp() override
        {
            _loop = new Pt::System::MainLoop();
        }

        void tearDown() override
        {
            delete _loop;
            _loop = nullptr;
        }

        void openClose();
        void execute();
        void select();
        void cancelSelect();
        void destroyConnection();
        void destroyStoredAwaiterConnection();
        void destroyTask();
        void pendingAwaiter();
        void nestedTask();
        void deferredArguments();

        Pt::Task<>             openCloseAsync();
        Pt::Task<>             executeAsync();
        Pt::Task<>             selectAsync();
        Pt::Task<>             cancelSelectAsync();
        Pt::Task<>             awaitOpenAsync(Pt::Db::Connection& conn);
        Pt::Task<>             awaitSelectAsync(Pt::Db::AsyncSelect& awaiter);
        Pt::Task<>             awaitOpenAndExit(Pt::Db::AsyncOpen& awaiter,
                            Pt::System::EventLoop& loop);
        Pt::Task<>             awaitExecuteAndExit(Pt::Db::AsyncExecute& awaiter,
                               Pt::System::EventLoop& loop);
        Pt::Task<>             awaitSelectAndExit(Pt::Db::AsyncSelect& awaiter,
                              Pt::System::EventLoop& loop);
        Pt::Task<std::size_t>  insertRowAsync(Pt::Db::Connection& conn, int id);
        Pt::Task<>             nestedTaskAsync();

    private:
        Pt::System::MainLoop* _loop;
};

Pt::Unit::RegisterTest<CoroTest> register_CoroTest;


Pt::Task<> CoroTest::openCloseAsync()
{
    Pt::Db::Connection conn("sqlite");
    conn.setActive(*_loop);

    co_await conn.openAsync(":memory:");
    PT_UNIT_ASSERT( conn.isOpen() );

    co_await conn.closeAsync();
    PT_UNIT_ASSERT( ! conn.isOpen() );

    _loop->exit();
}

void CoroTest::openClose()
{
    Pt::Task<> task = openCloseAsync();
    task.run();
    _loop->run();
}


Pt::Task<> CoroTest::executeAsync()
{
    Pt::Db::Connection conn("sqlite");
    conn.setActive(*_loop);

    co_await conn.openAsync(":memory:");
    co_await conn.executeAsync("CREATE TABLE t (id INTEGER)");

    std::size_t n = co_await conn.executeAsync("INSERT INTO t VALUES (1)");
    PT_UNIT_ASSERT( n == 1 );

    n = co_await conn.executeAsync("INSERT INTO t VALUES (2)");
    PT_UNIT_ASSERT( n == 1 );

    _loop->exit();
}

void CoroTest::execute()
{
    Pt::Task<> task = executeAsync();
    task.run();
    _loop->run();
}


Pt::Task<> CoroTest::selectAsync()
{
    Pt::Db::Connection conn("sqlite");
    conn.setActive(*_loop);

    co_await conn.openAsync(":memory:");
    co_await conn.executeAsync("CREATE TABLE t (id INTEGER, val TEXT)");
    co_await conn.executeAsync("INSERT INTO t VALUES (1, 'alpha')");
    co_await conn.executeAsync("INSERT INTO t VALUES (2, 'beta')");

    Pt::Db::Result r = co_await conn.selectAsync("SELECT * FROM t ORDER BY id");
    PT_UNIT_ASSERT( r.size() == 2 );
    PT_UNIT_ASSERT( r[0][1].getString() == "alpha" );
    PT_UNIT_ASSERT( r[1][1].getString() == "beta" );

    _loop->exit();
}

void CoroTest::select()
{
    Pt::Task<> task = selectAsync();
    task.run();
    _loop->run();
}


Pt::Task<> CoroTest::cancelSelectAsync()
{
    Pt::Db::Connection conn("sqlite");
    conn.setActive(*_loop);

    co_await conn.openAsync(":memory:");
    _loop->exit();
}

void CoroTest::cancelSelect()
{
    Pt::Task<> task = cancelSelectAsync();
    task.run();    // suspends at co_await openAsync
    task.cancel(); // doCancel() + frame destroyed

    PT_UNIT_ASSERT( ! task );
}


Pt::Task<> CoroTest::awaitOpenAsync(Pt::Db::Connection& conn)
{
    co_await conn.openAsync(":memory:");
}


Pt::Task<> CoroTest::awaitSelectAsync(Pt::Db::AsyncSelect& awaiter)
{
    co_await awaiter;
}


void CoroTest::destroyConnection()
{
    Pt::Db::Connection* conn = new Pt::Db::Connection("sqlite");
    conn->setActive(*_loop);

    Pt::Task<> task = awaitOpenAsync(*conn);
    task.run();
    delete conn;

    PT_UNIT_ASSERT( task );
    task.cancel();
    PT_UNIT_ASSERT( ! task );
}


void CoroTest::destroyStoredAwaiterConnection()
{
    Pt::Db::Connection* conn = new Pt::Db::Connection("sqlite");

    {
        Pt::Db::AsyncSelect awaiter = conn->selectAsync("SELECT 1");
        delete conn;

        Pt::Task<> task = awaitSelectAsync(awaiter);
        task.run();

        PT_UNIT_ASSERT( task.done() );
        PT_UNIT_ASSERT_THROW( task.result(), std::logic_error );
    }
}


void CoroTest::destroyTask()
{
    Pt::Db::Connection conn("sqlite");
    conn.setActive(*_loop);

    {
        Pt::Task<> task = awaitOpenAsync(conn);
        task.run();
        PT_UNIT_ASSERT( ! task.done() );
    }

    PT_UNIT_ASSERT( conn.isIdle() );
}


void CoroTest::pendingAwaiter()
{
    Pt::Db::Connection conn("sqlite");
    Pt::Db::AsyncOpen open = conn.openAsync(":memory:");

    PT_UNIT_ASSERT_THROW( conn.pingAsync(), std::logic_error );
}


Pt::Task<> CoroTest::awaitOpenAndExit(Pt::Db::AsyncOpen& awaiter,
                                       Pt::System::EventLoop& loop)
{
    co_await awaiter;
    loop.exit();
}


Pt::Task<> CoroTest::awaitExecuteAndExit(Pt::Db::AsyncExecute& awaiter,
                                          Pt::System::EventLoop& loop)
{
    co_await awaiter;
    loop.exit();
}


Pt::Task<> CoroTest::awaitSelectAndExit(Pt::Db::AsyncSelect& awaiter,
                                         Pt::System::EventLoop& loop)
{
    co_await awaiter;
    loop.exit();
}


void CoroTest::deferredArguments()
{
    {
        Pt::System::MainLoop loop;
        Pt::Db::Connection conn("sqlite");
        conn.setActive(loop);

        Pt::Db::AsyncOpen awaiter = conn.openAsync(std::string(":memory:"));
        Pt::Task<> task = awaitOpenAndExit(awaiter, loop);
        task.run();
        loop.run();

        PT_UNIT_ASSERT( conn.isOpen() );
    }

    {
        Pt::System::MainLoop loop;
        Pt::Db::Connection conn("sqlite");
        conn.open(":memory:");
        conn.setActive(loop);

        Pt::Db::AsyncExecute awaiter =
            conn.executeAsync(std::string("CREATE TABLE t (id INTEGER)"));
        Pt::Task<> task = awaitExecuteAndExit(awaiter, loop);
        task.run();
        loop.run();

        Pt::Db::Result result = conn.select("SELECT COUNT(*) FROM sqlite_master WHERE name='t'");
        PT_UNIT_ASSERT( result[0][0].getInt() == 1 );
    }

    {
        Pt::System::MainLoop loop;
        Pt::Db::Connection conn("sqlite");
        conn.open(":memory:");
        conn.setActive(loop);
        conn.execute("CREATE TABLE t (id INTEGER)");
        conn.execute("INSERT INTO t VALUES (1)");

        Pt::Db::AsyncSelect awaiter =
            conn.selectAsync(std::string("SELECT * FROM t"));
        Pt::Task<> task = awaitSelectAndExit(awaiter, loop);
        task.run();
        loop.run();

        PT_UNIT_ASSERT( task.done() );
    }
}


Pt::Task<std::size_t> CoroTest::insertRowAsync(Pt::Db::Connection& conn, int id)
{
    std::string sql = "INSERT INTO t VALUES (" + std::to_string(id) + ")";
    std::size_t n = co_await conn.executeAsync(sql);
    co_return n;
}


Pt::Task<> CoroTest::nestedTaskAsync()
{
    Pt::Db::Connection conn("sqlite");
    conn.setActive(*_loop);

    co_await conn.openAsync(":memory:");
    co_await conn.executeAsync("CREATE TABLE t (id INTEGER)");

    std::size_t n1 = co_await insertRowAsync(conn, 1);
    PT_UNIT_ASSERT( n1 == 1 );

    std::size_t n2 = co_await insertRowAsync(conn, 2);
    PT_UNIT_ASSERT( n2 == 1 );

    _loop->exit();
}


void CoroTest::nestedTask()
{
    Pt::Task<> task = nestedTaskAsync();
    task.run();
    _loop->run();
}

#endif // __cplusplus >= 202002L
