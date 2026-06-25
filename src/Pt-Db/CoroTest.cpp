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
            registerMethod("OpenClose", *this, &CoroTest::openClose);
            registerMethod("Execute",   *this, &CoroTest::execute);
            registerMethod("Select",    *this, &CoroTest::select);
            registerMethod("CancelSelect",   *this, &CoroTest::cancelSelect);
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

        Pt::Task<> openCloseAsync();
        Pt::Task<> executeAsync();
        Pt::Task<> selectAsync();
        Pt::Task<> cancelSelectAsync();

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

#endif // __cplusplus >= 202002L
