#ifndef PT_DB_SQLITE_SQLITECONNECTION_H
#define PT_DB_SQLITE_SQLITECONNECTION_H

#include <Pt/Db/IConnection.h>
#include <Pt/Db/ICursor.h>
#include <Pt/Db/Result.h>
#include <Pt/Db/Statement.h>
#include <Pt/SmartPtr.h>
#include <Pt/Signal.h>
#include <Pt/System/Selectable.h>
#include <Pt/System/Thread.h>
#include <Pt/System/Mutex.h>
#include <Pt/System/Condition.h>
#include "sqlite3.h"
#include <atomic>
#include <exception>
#include <string>

namespace Pt {

namespace Db {

class Transaction;

namespace sqlite {

class SqliteCursor;
class SqliteStatement;

class SqliteConnection : public IStmtCacheConnection
                       , public Pt::System::Selectable
{
    public:
        SqliteConnection();

        ~SqliteConnection();

        sqlite3* getSqlite3() const
        {
            return _db;
        }

        // --- Internal helpers for sqlite::SqliteStatement async operations ---
        void      enqueueStmtExec(SqliteStatement& stmt);
        size_type completeStmtExec();
        void      enqueueStmtSelect(SqliteStatement& stmt);
        Result    completeStmtSelect();

        // --- Internal helpers for SqliteCursor batch-fetch ---
        void   enqueueBatchFetch(SqliteCursor& cursor, size_type batchSize);
        Result completeBatchFetch(bool& done);

        // Selectable callbacks (EventLoop thread)
        void onAttach(Pt::System::EventLoop& loop) override
        { }
        void onDetach(Pt::System::EventLoop& loop) override
        { }
        void onCancel() override;
        bool onRun() override;

    protected:
        void onSetActive(Pt::System::EventLoop* loop) override;

        void onOpen(const std::string& connStr) override;
        void onClose() override;
        void onCancelPending() noexcept override;

        void onBeginOpen(const std::string& connStr) override;
        void onEndOpen() override;

        void onBeginClose() override;
        void onEndClose() override;

        void onNotifyPreparedCached() override;

        void onBeginExec(const std::string& sql) override;
        size_type onEndExec() override;
        void onBeginSelect(const std::string& sql) override;
        Result onEndSelect() override;

        void onBeginPrepare(const std::string& query) override;
        Pt::Db::Statement onEndPrepare() override;

        void onBeginPrepareCachedMiss(const std::string& query) override;
        Pt::Db::Statement onEndPrepareCachedMiss() override;

        void onBeginStartTransaction(Pt::Db::Transaction& txn, const char* sql) override;
        void onEndStartTransaction() override;
        void onBeginCommitTransaction(Pt::Db::Transaction& txn, const char* sql) override;
        void onEndCommitTransaction() override;
        void onBeginRollbackTransaction(Pt::Db::Transaction& txn, const char* sql) override;
        void onEndRollbackTransaction() override;

        // Sync operation hooks
        size_type onExecute(const std::string& query) override;
        Result onSelect(const std::string& query) override;
        Pt::Db::Statement onPrepare(const std::string& query) override;
        bool onPing() override;
        void onBeginPing() override;
        bool onEndPing() override;
        long long onLastInsertId(const std::string& name) override;
        void onStartTransaction(const char* sql) override;
        void onCommitTransaction(const char* sql) override;
        void onRollbackTransaction(const char* sql) override;

    private:
        // Helpers: Connection is friend of SqliteStatement, nested task structs are not
        static size_type callStatementExecute(SqliteStatement& stmt);
        static Result    callStatementSelect(SqliteStatement& stmt);

        void processTasks();
        void ensureWorker();

        // --- Task base type ---

        struct Task
        {
                std::exception_ptr exception;

                virtual ~Task()
                { }
                virtual void execute(SqliteConnection& conn) = 0;
                virtual void complete(SqliteConnection& conn) = 0;
        };

        // --- Concrete tasks ---

        struct OpenTask : Task
        {
                std::string connStr;
                void execute(SqliteConnection& conn) override;
                void complete(SqliteConnection& conn) override;
        };

        struct ExecTask : Task
        {
                std::string sql;
                size_type rowCount = 0;
                Result result;
                void execute(SqliteConnection& conn) override;
                void complete(SqliteConnection& conn) override;
        };

        struct SelectTask : Task
        {
                std::string sql;
                Result result;
                void execute(SqliteConnection& conn) override;
                void complete(SqliteConnection& conn) override;
        };

        struct PrepareTask : Task
        {
                std::string sql;
                Pt::Db::Statement result;
                void execute(SqliteConnection& conn) override;
                void complete(SqliteConnection& conn) override;
        };

        struct PrepareCachedTask : Task
        {
                std::string sql;
                Pt::Db::Statement result;
                void execute(SqliteConnection& conn) override;
                void complete(SqliteConnection& conn) override;
        };

        struct StmtExecTask : Task
        {
                SqliteStatement* stmt = nullptr;
                size_type rowCount = 0;
                void execute(SqliteConnection& conn) override;
                void complete(SqliteConnection& conn) override;
        };

        struct StmtSelectTask : Task
        {
                SqliteStatement* stmt = nullptr;
                Result result;
                void execute(SqliteConnection& conn) override;
                void complete(SqliteConnection& conn) override;
        };

        struct BeginTxnTask : Task
        {
                Pt::Db::Transaction* txn = nullptr;
                std::string sql;
                void execute(SqliteConnection& conn) override;
                void complete(SqliteConnection& conn) override;
        };

        struct CommitTxnTask : Task
        {
                Pt::Db::Transaction* txn = nullptr;
                std::string sql;
                void execute(SqliteConnection& conn) override;
                void complete(SqliteConnection& conn) override;
        };

        struct RollbackTxnTask : Task
        {
                Pt::Db::Transaction* txn = nullptr;
                std::string sql;
                void execute(SqliteConnection& conn) override;
                void complete(SqliteConnection& conn) override;
        };

        struct BatchFetchTask : Task
        {
                SqliteCursor* cursor = nullptr;
                size_type batchSize = 100;
                bool done = false;
                Result result;
                void execute(SqliteConnection& conn) override;
                void complete(SqliteConnection& conn) override;
        };

        struct CloseTask : Task
        {
                void execute(SqliteConnection& conn) override;
                void complete(SqliteConnection& conn) override;
        };

        struct PingTask : Task
        {
                bool result = false;
                void execute(SqliteConnection& conn) override;
                void complete(SqliteConnection& conn) override;
        };

        void enqueue(Task* task);

        // Task instances (no heap allocation)
        OpenTask _openTask;
        ExecTask _execTask;
        SelectTask _selectTask;
        PrepareTask _prepareTask;
        PrepareCachedTask _prepareCachedTask;
        StmtExecTask _stmtExecTask;
        StmtSelectTask _stmtSelectTask;
        BeginTxnTask _beginTxnTask;
        CommitTxnTask _commitTxnTask;
        RollbackTxnTask _rollbackTxnTask;
        BatchFetchTask _batchFetchTask;
        CloseTask _closeTask;
        PingTask _pingTask;

        Task* _pendingTask;
        Task* _completedTask;

        sqlite3* _db;
        std::string _conninfo;

        Pt::System::Thread _thread;
        Pt::System::Mutex _mutex;
        Pt::System::Condition _workReady;
        Pt::System::Condition _workDone;
        bool _shutdown;
        std::atomic<bool> _cancelFlag;

};

} // namespace sqlite
} // namespace Db
} // namespace Pt
#endif // PT_DB_SQLITE_SQLITECONNECTION_H
