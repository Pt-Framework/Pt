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
#include <Pt/Unit/Assertion.h>
#include <Pt/Unit/TestSuite.h>
#include <Pt/Unit/RegisterTest.h>
#include <Pt/Db/Connection.h>
#include <Pt/Db/Result.h>
#include <Pt/Db/Row.h>
#include <Pt/Db/Value.h>
#include <Pt/Db/Transaction.h>
#include <Pt/System/MainLoop.h>
#include <Pt/Db/Cursor.h>
#include <Pt/Connectable.h>
#include <Pt/Slot.h>

#include <string>
#include <stdexcept>

#if __cplusplus >= 202002L
#include <Pt/Task.h>
#include <Pt/Db/Connection.h>
#endif


// Helper receiver for openFinished() signal
struct FinishedReceiver : public Pt::Connectable
{
    Pt::System::MainLoop* loop;
    Pt::Db::Connection*   conn;
    bool  done;
    bool  failed;
    std::string error;

    FinishedReceiver(Pt::System::MainLoop& l, Pt::Db::Connection& c)
    : loop(&l), conn(&c), done(false), failed(false)
    {}

    void onFinished()
    {
        try { conn->endExec(); }
        catch(const std::exception& e) { failed = true; error = e.what(); }
        done = true;
        loop->exit();
    }
};


class AsyncTest : public Pt::Unit::TestSuite
{
public:
    AsyncTest()
    : Pt::Unit::TestSuite("Pt::Db::AsyncTest")
    {
        registerMethod("SyncExec",              *this, &AsyncTest::syncExec);
        registerMethod("AsyncExec",             *this, &AsyncTest::asyncExec);
        registerMethod("AsyncSelect",           *this, &AsyncTest::asyncSelect);
        registerMethod("AsyncCancel",           *this, &AsyncTest::asyncCancel);
        registerMethod("AsyncSequence",         *this, &AsyncTest::asyncSequence);
        registerMethod("AsyncCursor",           *this, &AsyncTest::asyncCursor);
        registerMethod("AsyncOpen",             *this, &AsyncTest::asyncOpen);
        registerMethod("AsyncStmtExecSignal",   *this, &AsyncTest::asyncStmtExecSignal);
        registerMethod("AsyncStmtSelectSignal", *this, &AsyncTest::asyncStmtSelectSignal);
        registerMethod("AsyncPrepare",          *this, &AsyncTest::asyncPrepare);
        registerMethod("AsyncBeginTxn",         *this, &AsyncTest::asyncBeginTxn);
        registerMethod("AsyncTransactionCycle", *this, &AsyncTest::asyncTransactionCycle);
        registerMethod("AsyncRollback",         *this, &AsyncTest::asyncRollback);
        registerMethod("AsyncTransactionClass", *this, &AsyncTest::asyncTransactionClass);
#if __cplusplus >= 202002L
        registerMethod("AsyncOpenCoro", *this, &AsyncTest::asyncOpenCoro);
#endif
    }

protected:
    void syncExec();
    void asyncExec();
    void asyncSelect();
    void asyncCancel();
    void asyncSequence();
    void asyncCursor();
    void asyncOpen();
    void asyncStmtExecSignal();
    void asyncStmtSelectSignal();
    void asyncPrepare();
    void asyncBeginTxn();
    void asyncTransactionCycle();
    void asyncRollback();
    void asyncTransactionClass();
#if __cplusplus >= 202002L
    void asyncOpenCoro();
#endif
};

Pt::Unit::RegisterTest<AsyncTest> register_AsyncTest;


// Helper: extract string from Value
static std::string valueString(const Pt::Db::Value& v)
{
    return v.getString();
}


void AsyncTest::syncExec()
{
    Pt::Db::Connection conn("sqlite");
    conn.open(":memory:");

    conn.execute("CREATE TABLE t (id INTEGER, val TEXT)");
    conn.execute("INSERT INTO t VALUES (1, 'hello')");

    Pt::Db::Result r = conn.select("SELECT * FROM t");
    PT_UNIT_ASSERT( r.size() == 1 );
    PT_UNIT_ASSERT( valueString(r[0][1]) == "hello" );
}


void AsyncTest::asyncExec()
{
    Pt::System::MainLoop loop;
    Pt::Db::Connection conn("sqlite");
    conn.open(":memory:");
    conn.setActive(loop);

    conn.execute("CREATE TABLE t (id INTEGER, val TEXT)");

    FinishedReceiver rx(loop, conn);
    conn.executeFinished() += Pt::slot(rx, &FinishedReceiver::onFinished);

    conn.beginExec("INSERT INTO t VALUES (1, 'async')");;
    loop.run();

    PT_UNIT_ASSERT( ! rx.failed );

    Pt::Db::Result r = conn.select("SELECT * FROM t WHERE id=1");
    PT_UNIT_ASSERT( r.size() == 1 );
    PT_UNIT_ASSERT( valueString(r[0][1]) == "async" );
}


// Receiver that captures the select result
struct SelectReceiver : public Pt::Connectable
{
    Pt::System::MainLoop* loop;
    Pt::Db::Connection*   conn;
    bool  done;
    bool  failed;
    Pt::Db::Result result;

    SelectReceiver(Pt::System::MainLoop& l, Pt::Db::Connection& c)
    : loop(&l), conn(&c), done(false), failed(false)
    {}

    void onFinished()
    {
        try { result = conn->endSelect(); }
        catch(const std::exception&) { failed = true; }
        done = true;
        loop->exit();
    }
};


void AsyncTest::asyncSelect()
{
    Pt::System::MainLoop loop;
    Pt::Db::Connection conn("sqlite");
    conn.open(":memory:");
    conn.setActive(loop);

    conn.execute("CREATE TABLE t (id INTEGER, val TEXT)");
    conn.execute("INSERT INTO t VALUES (1, 'row1')");
    conn.execute("INSERT INTO t VALUES (2, 'row2')");

    SelectReceiver rx(loop, conn);
    conn.selectFinished() += Pt::slot(rx, &SelectReceiver::onFinished);

    conn.beginSelect("SELECT * FROM t ORDER BY id");
    loop.run();

    PT_UNIT_ASSERT( ! rx.failed );
    PT_UNIT_ASSERT( rx.result.size() == 2 );
    PT_UNIT_ASSERT( valueString(rx.result[0][1]) == "row1" );
    PT_UNIT_ASSERT( valueString(rx.result[1][1]) == "row2" );
}


void AsyncTest::asyncCancel()
{
    Pt::System::MainLoop loop;
    Pt::Db::Connection conn("sqlite");
    conn.open(":memory:");
    conn.setActive(loop);

    conn.execute("CREATE TABLE t (id INTEGER)");
    for(int i = 1; i <= 10; ++i)
        conn.execute("INSERT INTO t VALUES (" + std::to_string(i) + ")");

    conn.beginSelect("SELECT * FROM t");
    conn.cancel();
    conn.execute("INSERT INTO t VALUES (99)");
    Pt::Db::Result r = conn.select("SELECT COUNT(*) FROM t");
    PT_UNIT_ASSERT( r[0][0].getInt() == 11 );
}


// Receiver for a two-step sequence test
struct SequenceReceiver : public Pt::Connectable
{
    Pt::System::MainLoop* loop;
    Pt::Db::Connection*   conn;
    int  step;
    bool failed;

    SequenceReceiver(Pt::System::MainLoop& l, Pt::Db::Connection& c)
    : loop(&l), conn(&c), step(0), failed(false)
    {}

    void onFinished()
    {
        try { conn->endExec(); }
        catch(const std::exception&)
        {
            failed = true;
            loop->exit();
            return;
        }
        ++step;
        if(step == 1)
        {
            conn->beginExec("INSERT INTO t VALUES (2)");
        }
        else
        {
            loop->exit();
        }
    }
};


void AsyncTest::asyncSequence()
{
    Pt::System::MainLoop loop;
    Pt::Db::Connection conn("sqlite");
    conn.open(":memory:");
    conn.setActive(loop);

    conn.execute("CREATE TABLE t (id INTEGER)");

    SequenceReceiver rx(loop, conn);
    conn.executeFinished() += Pt::slot(rx, &SequenceReceiver::onFinished);

    conn.beginExec("INSERT INTO t VALUES (1)");
    loop.run();

    PT_UNIT_ASSERT( ! rx.failed );
    PT_UNIT_ASSERT( rx.step == 2 );

    Pt::Db::Result r = conn.select("SELECT COUNT(*) FROM t");
    PT_UNIT_ASSERT( r[0][0].getInt() == 2 );
}


// Self-contained async cursor test — setUp in constructor, tearDown via RAII
struct AsyncCursorTest : public Pt::Connectable
{
    Pt::System::MainLoop  loop;
    Pt::Db::Connection    conn;
    Pt::Db::Statement     stmt;
    Pt::Db::Cursor        cursor;
    int  totalRows;
    bool done;

    AsyncCursorTest()
    : conn("sqlite")
    , totalRows(0)
    , done(false)
    {
        conn.open(":memory:");
        conn.setActive(loop);

        conn.execute("CREATE TABLE t (id INTEGER)");
        for(int i = 1; i <= 25; ++i)
            conn.execute("INSERT INTO t VALUES (" + std::to_string(i) + ")");

        stmt   = conn.prepare("SELECT * FROM t ORDER BY id");
        cursor = stmt.getCursor();
        cursor.fetched() += Pt::slot(*this, &AsyncCursorTest::onFetched);
    }

    void run()
    {
        cursor.beginFetch(10);
        loop.run();

        PT_UNIT_ASSERT( done );
        PT_UNIT_ASSERT( totalRows == 25 );
    }

    void onFetched()
    {
        Pt::Db::Result batch = cursor.endFetch();

        totalRows += static_cast<int>( batch.size() );

        if( ! cursor.isOpen() )
        {
            done = true;
            loop.exit();
            return;
        }

        cursor.beginFetch(10);
    }
};


void AsyncTest::asyncCursor()
{
    AsyncCursorTest test;
    test.run();
}


// Receiver for async open
struct OpenReceiver : public Pt::Connectable
{
    Pt::System::MainLoop* loop;
    Pt::Db::Connection*   conn;
    bool done;
    bool failed;
    std::string error;

    OpenReceiver(Pt::System::MainLoop& l, Pt::Db::Connection& c)
    : loop(&l), conn(&c), done(false), failed(false)
    {}

    void onFinished()
    {
        try { conn->endOpen(); }
        catch(const std::exception& e) { failed = true; error = e.what(); }
        done = true;
        loop->exit();
    }
};


void AsyncTest::asyncOpen()
{
    Pt::System::MainLoop loop;
    Pt::Db::Connection conn("sqlite");
    conn.setActive(loop);

    OpenReceiver rx(loop, conn);
    conn.openFinished() += Pt::slot(rx, &OpenReceiver::onFinished);

    conn.beginOpen(":memory:");
    loop.run();

    PT_UNIT_ASSERT( rx.done );
    PT_UNIT_ASSERT( ! rx.failed );
    PT_UNIT_ASSERT( conn.isOpen() );

    conn.execute("CREATE TABLE t (id INTEGER)");
    conn.execute("INSERT INTO t VALUES (42)");
    Pt::Db::Result r = conn.select("SELECT id FROM t");
    PT_UNIT_ASSERT( r.size() == 1 );
    PT_UNIT_ASSERT( r[0][0].getInt() == 42 );
}


// --- Async Statement exec/select with separate signals ---

struct StmtExecReceiver : public Pt::Connectable
{
    Pt::System::MainLoop* loop;
    Pt::Db::Statement*    stmt;
    bool  done;
    bool  failed;
    std::size_t rowCount;

    StmtExecReceiver(Pt::System::MainLoop& l, Pt::Db::Statement& s)
    : loop(&l), stmt(&s), done(false), failed(false), rowCount(0)
    {}

    void onExecuteFinished()
    {
        try { rowCount = stmt->endExec(); }
        catch(const std::exception&) { failed = true; }
        done = true;
        loop->exit();
    }
};

void AsyncTest::asyncStmtExecSignal()
{
    Pt::System::MainLoop loop;
    Pt::Db::Connection conn("sqlite");
    conn.open(":memory:");
    conn.setActive(loop);

    conn.execute("CREATE TABLE t (id INTEGER)");
    Pt::Db::Statement stmt = conn.prepare("INSERT INTO t VALUES (:id)");
    stmt.set("id", 7);

    StmtExecReceiver rx(loop, stmt);
    stmt.executeFinished() += Pt::slot(rx, &StmtExecReceiver::onExecuteFinished);

    stmt.beginExec();
    loop.run();

    PT_UNIT_ASSERT( ! rx.failed );
    PT_UNIT_ASSERT( rx.done );

    Pt::Db::Result r = conn.select("SELECT COUNT(*) FROM t WHERE id=7");
    PT_UNIT_ASSERT( r[0][0].getInt() == 1 );
}


struct StmtSelectReceiver : public Pt::Connectable
{
    Pt::System::MainLoop* loop;
    Pt::Db::Statement*    stmt;
    bool  done;
    bool  failed;
    Pt::Db::Result result;

    StmtSelectReceiver(Pt::System::MainLoop& l, Pt::Db::Statement& s)
    : loop(&l), stmt(&s), done(false), failed(false)
    {}

    void onSelectFinished()
    {
        try { result = stmt->endSelect(); }
        catch(const std::exception&) { failed = true; }
        done = true;
        loop->exit();
    }
};

void AsyncTest::asyncStmtSelectSignal()
{
    Pt::System::MainLoop loop;
    Pt::Db::Connection conn("sqlite");
    conn.open(":memory:");
    conn.setActive(loop);

    conn.execute("CREATE TABLE t (id INTEGER, val TEXT)");
    conn.execute("INSERT INTO t VALUES (1, 'alpha')");
    conn.execute("INSERT INTO t VALUES (2, 'beta')");

    Pt::Db::Statement stmt = conn.prepare("SELECT * FROM t ORDER BY id");

    StmtSelectReceiver rx(loop, stmt);
    stmt.selectFinished() += Pt::slot(rx, &StmtSelectReceiver::onSelectFinished);

    stmt.beginSelect();
    loop.run();

    PT_UNIT_ASSERT( ! rx.failed );
    PT_UNIT_ASSERT( rx.result.size() == 2 );
    PT_UNIT_ASSERT( rx.result[0][1].getString() == "alpha" );
}


// --- Async prepare ---

struct PrepareReceiver : public Pt::Connectable
{
    Pt::System::MainLoop* loop;
    Pt::Db::Connection*   conn;
    bool  done;
    bool  failed;
    Pt::Db::Statement stmt;

    PrepareReceiver(Pt::System::MainLoop& l, Pt::Db::Connection& c)
    : loop(&l), conn(&c), done(false), failed(false)
    {}

    void onPrepareFinished()
    {
        try { stmt = conn->endPrepare(); }
        catch(const std::exception&) { failed = true; }
        done = true;
        loop->exit();
    }
};

void AsyncTest::asyncPrepare()
{
    Pt::System::MainLoop loop;
    Pt::Db::Connection conn("sqlite");
    conn.open(":memory:");
    conn.setActive(loop);

    conn.execute("CREATE TABLE t (id INTEGER)");

    PrepareReceiver rx(loop, conn);
    conn.prepareFinished() += Pt::slot(rx, &PrepareReceiver::onPrepareFinished);

    conn.beginPrepare("INSERT INTO t VALUES (:id)");
    loop.run();

    PT_UNIT_ASSERT( ! rx.failed );
    PT_UNIT_ASSERT( ! ! rx.stmt );

    rx.stmt.set("id", 42);
    rx.stmt.execute();
    Pt::Db::Result r = conn.select("SELECT id FROM t");
    PT_UNIT_ASSERT( r.size() == 1 );
    PT_UNIT_ASSERT( r[0][0].getInt() == 42 );
}


// --- Async transaction ---

void AsyncTest::asyncBeginTxn()
{
    Pt::System::MainLoop loop;
    Pt::Db::Connection conn("sqlite");
    conn.open(":memory:");
    conn.setActive(loop);

    struct TxnReceiver : public Pt::Connectable
    {
        Pt::System::MainLoop* loop;
        Pt::Db::Transaction*  txn;
        bool done   = false;
        bool failed = false;

        TxnReceiver(Pt::System::MainLoop& l, Pt::Db::Transaction& t)
        : loop(&l), txn(&t)
        {}

        void onStartFinished()
        {
            try { txn->endStart(); }
            catch(const std::exception&) { failed = true; }
            done = true;
            loop->exit();
        }
    };

    Pt::Db::Transaction txn(conn, false);
    TxnReceiver rx(loop, txn);
    txn.startFinished() += Pt::slot(rx, &TxnReceiver::onStartFinished);

    txn.beginStart();
    loop.run();

    PT_UNIT_ASSERT( ! rx.failed );
    PT_UNIT_ASSERT( rx.done );

    txn.rollback();
}


void AsyncTest::asyncTransactionCycle()
{
    Pt::System::MainLoop loop;
    Pt::Db::Connection conn("sqlite");
    conn.open(":memory:");
    conn.setActive(loop);

    conn.execute("CREATE TABLE t (id INTEGER)");

    struct CycleReceiver : public Pt::Connectable
    {
        Pt::System::MainLoop* loop;
        Pt::Db::Connection*   conn;
        Pt::Db::Transaction*  txn;
        int  step   = 0;
        bool failed = false;

        CycleReceiver(Pt::System::MainLoop& l, Pt::Db::Connection& c, Pt::Db::Transaction& t)
        : loop(&l), conn(&c), txn(&t)
        {}

        void onStartFinished()
        {
            try
            {
                txn->endStart();
                conn->execute("INSERT INTO t VALUES (1)");
                ++step;
                txn->beginCommit();
            }
            catch(const std::exception&)
            {
                failed = true;
                loop->exit();
            }
        }

        void onCommitFinished()
        {
            try { txn->endCommit(); }
            catch(const std::exception&) { failed = true; }
            ++step;
            loop->exit();
        }
    };

    Pt::Db::Transaction txn(conn, false);
    CycleReceiver rx(loop, conn, txn);
    txn.startFinished()  += Pt::slot(rx, &CycleReceiver::onStartFinished);
    txn.commitFinished() += Pt::slot(rx, &CycleReceiver::onCommitFinished);

    txn.beginStart();
    loop.run();

    PT_UNIT_ASSERT( ! rx.failed );
    PT_UNIT_ASSERT( rx.step == 2 );

    Pt::Db::Result r = conn.select("SELECT COUNT(*) FROM t");
    PT_UNIT_ASSERT( r[0][0].getInt() == 1 );
}


void AsyncTest::asyncRollback()
{
    Pt::System::MainLoop loop;
    Pt::Db::Connection conn("sqlite");
    conn.open(":memory:");
    conn.setActive(loop);

    conn.execute("CREATE TABLE t (id INTEGER)");

    struct RollReceiver : public Pt::Connectable
    {
        Pt::System::MainLoop* loop;
        Pt::Db::Connection*   conn;
        Pt::Db::Transaction*  txn;
        int  step   = 0;
        bool failed = false;

        RollReceiver(Pt::System::MainLoop& l, Pt::Db::Connection& c, Pt::Db::Transaction& t)
        : loop(&l), conn(&c), txn(&t)
        {}

        void onStartFinished()
        {
            try
            {
                txn->endStart();
                conn->execute("INSERT INTO t VALUES (99)");
                ++step;
                txn->beginRollback();
            }
            catch(const std::exception&)
            {
                failed = true;
                loop->exit();
            }
        }

        void onRollbackFinished()
        {
            try { txn->endRollback(); }
            catch(const std::exception&) { failed = true; }
            ++step;
            loop->exit();
        }
    };

    Pt::Db::Transaction txn(conn, false);
    RollReceiver rx(loop, conn, txn);
    txn.startFinished()    += Pt::slot(rx, &RollReceiver::onStartFinished);
    txn.rollbackFinished() += Pt::slot(rx, &RollReceiver::onRollbackFinished);

    txn.beginStart();
    loop.run();

    PT_UNIT_ASSERT( ! rx.failed );
    PT_UNIT_ASSERT( rx.step == 2 );

    Pt::Db::Result r = conn.select("SELECT COUNT(*) FROM t");
    PT_UNIT_ASSERT( r[0][0].getInt() == 0 );
}


void AsyncTest::asyncTransactionClass()
{
    Pt::System::MainLoop loop;
    Pt::Db::Connection conn("sqlite");
    conn.open(":memory:");
    conn.setActive(loop);

    conn.execute("CREATE TABLE t (id INTEGER)");

    struct TxnClassReceiver : public Pt::Connectable
    {
        Pt::System::MainLoop*  loop;
        Pt::Db::Connection*    conn;
        Pt::Db::Transaction*   txn;
        int  step   = 0;
        bool failed = false;

        TxnClassReceiver(Pt::System::MainLoop& l, Pt::Db::Connection& c, Pt::Db::Transaction& t)
        : loop(&l), conn(&c), txn(&t)
        {}

        void onStartFinished()
        {
            try
            {
                txn->endStart();
                conn->execute("INSERT INTO t VALUES (5)");
                ++step;
                txn->beginCommit();
            }
            catch(const std::exception&)
            {
                failed = true;
                loop->exit();
            }
        }

        void onCommitFinished()
        {
            try { txn->endCommit(); }
            catch(const std::exception&) { failed = true; }
            ++step;
            loop->exit();
        }
    };

    Pt::Db::Transaction txn(conn, false);
    TxnClassReceiver rx(loop, conn, txn);
    txn.startFinished()  += Pt::slot(rx, &TxnClassReceiver::onStartFinished);
    txn.commitFinished() += Pt::slot(rx, &TxnClassReceiver::onCommitFinished);

    txn.beginStart();
    loop.run();

    PT_UNIT_ASSERT( ! rx.failed );
    PT_UNIT_ASSERT( rx.step == 2 );

    Pt::Db::Result r = conn.select("SELECT COUNT(*) FROM t WHERE id=5");
    PT_UNIT_ASSERT( r[0][0].getInt() == 1 );
}


#if __cplusplus >= 202002L

static Pt::DetachedTask coroOpen(Pt::Db::Connection& conn, Pt::System::MainLoop& loop)
{
    co_await conn.openAsync(":memory:");

    conn.execute("CREATE TABLE t (id INTEGER)");
    conn.execute("INSERT INTO t VALUES (42)");

    Pt::Db::Result r = conn.select("SELECT * FROM t");
    PT_UNIT_ASSERT( r.size() == 1 );
    PT_UNIT_ASSERT( r[0][0].getInt() == 42 );

    loop.exit();
}


void AsyncTest::asyncOpenCoro()
{
    Pt::System::MainLoop loop;
    Pt::Db::Connection conn("sqlite");
    conn.setActive(loop);

    coroOpen(conn, loop);
    loop.run();
}

#endif // __cplusplus >= 202002L

