#ifndef PT_DB_SQLITE_CONNECTION_H
#define PT_DB_SQLITE_CONNECTION_H

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
    class Statement;

    class Connection : public IStmtCacheConnection
                     , public Pt::System::Selectable
    {
    public:
        Connection();

        ~Connection();

        // --- Synchronous operations ---

        void startTransaction(const char* sql = nullptr);
        void commitTransaction(const char* sql = nullptr);
        void rollbackTransaction(const char* sql = nullptr);

        size_type execute(const std::string& query);
        Result select(const std::string& query);
        Pt::Db::Statement prepare(const std::string& query);
        long long insertId();

        sqlite3* getSqlite3() const
        { return _db; }

        // --- Async Statement operations (called by sqlite::Statement) ---
        void      beginExec(Statement& stmt);
        size_type endExec(Statement& stmt);
        void      beginSelect(Statement& stmt);
        Result    endSelect(Statement& stmt);

        // --- Async batch cursor ---
        void beginBatchFetch(ICursor& cursor, size_type batchSize);
        Result endBatchFetch();
        void closeBatchFetch();

        // Selectable callbacks (EventLoop thread)
        void onAttach(Pt::System::EventLoop& loop) override {}
        void onDetach(Pt::System::EventLoop& loop) override {}
        void onCancel() override;
        bool onRun() override;

    protected:
        // --- IConnection signal hooks ---

        Pt::Signal<>& onOpenFinished() override    { return _finished; }
        Pt::Signal<>& onExecuteFinished() override { return _executeFinished; }
        Pt::Signal<>& onSelectFinished() override  { return _selectFinished; }
        Pt::Signal<>& onPrepareFinished() override { return _prepareFinished; }

        void onSetActive(Pt::System::EventLoop* loop) override;

        void onOpen(const std::string& connStr) override;
        void onClose() override;
        void onCancelOp() override;

        void onBeginOpen(const std::string& connStr) override;
        void onEndOpen() override;

        void onBeginExec(const std::string& sql) override;
        size_type onEndExec() override;
        void onBeginSelect(const std::string& sql) override;
        Result onEndSelect() override;

        void onBeginPrepare(const std::string& query) override;
        Pt::Db::Statement onEndPrepare() override;

        void onBeginStartTransaction(Pt::Db::Transaction& txn, const char* sql) override;
        void onEndStartTransaction() override;
        void onBeginCommitTransaction(Pt::Db::Transaction& txn, const char* sql) override;
        void onEndCommitTransaction() override;
        void onBeginRollbackTransaction(Pt::Db::Transaction& txn, const char* sql) override;
        void onEndRollbackTransaction() override;

    private:
        void processTasks();
        void ensureWorker();

        // --- Task base type ---

        struct Task
        {
            std::exception_ptr exception;

            virtual ~Task() {}
            virtual void execute(Connection& conn) = 0;
            virtual void complete(Connection& conn) = 0;
        };

        // --- Concrete tasks ---

        struct OpenTask : Task {
            std::string connStr;
            void execute(Connection& conn) override;
            void complete(Connection& conn) override;
        };

        struct ExecTask : Task {
            std::string sql;
            size_type   rowCount = 0;
            Result      result;
            void execute(Connection& conn) override;
            void complete(Connection& conn) override;
        };

        struct SelectTask : Task {
            std::string sql;
            Result      result;
            void execute(Connection& conn) override;
            void complete(Connection& conn) override;
        };

        struct PrepareTask : Task {
            std::string       sql;
            Pt::Db::Statement result;
            void execute(Connection& conn) override;
            void complete(Connection& conn) override;
        };

        struct StmtExecTask : Task {
            Statement* stmt     = nullptr;
            size_type  rowCount = 0;
            void execute(Connection& conn) override;
            void complete(Connection& conn) override;
        };

        struct StmtSelectTask : Task {
            Statement* stmt = nullptr;
            Result     result;
            void execute(Connection& conn) override;
            void complete(Connection& conn) override;
        };

        struct BeginTxnTask : Task {
            Pt::Db::Transaction* txn = nullptr;
            std::string          sql;
            void execute(Connection& conn) override;
            void complete(Connection& conn) override;
        };

        struct CommitTxnTask : Task {
            Pt::Db::Transaction* txn = nullptr;
            std::string          sql;
            void execute(Connection& conn) override;
            void complete(Connection& conn) override;
        };

        struct RollbackTxnTask : Task {
            Pt::Db::Transaction* txn = nullptr;
            std::string          sql;
            void execute(Connection& conn) override;
            void complete(Connection& conn) override;
        };

        struct BatchFetchTask : Task {
            SqliteCursor* cursor    = nullptr;
            size_type     batchSize = 100;
            bool          done      = false;
            Result        result;
            void execute(Connection& conn) override;
            void complete(Connection& conn) override;
        };

        void enqueue(Task* task);

        // Task instances (no heap allocation)
        OpenTask         _openTask;
        ExecTask         _execTask;
        SelectTask       _selectTask;
        PrepareTask      _prepareTask;
        StmtExecTask     _stmtExecTask;
        StmtSelectTask   _stmtSelectTask;
        BeginTxnTask     _beginTxnTask;
        CommitTxnTask    _commitTxnTask;
        RollbackTxnTask  _rollbackTxnTask;
        BatchFetchTask   _batchFetchTask;

        Task*            _pendingTask;
        Task*            _completedTask;

        sqlite3*                  _db;
        std::string               _conninfo;

        Pt::System::Thread        _thread;
        Pt::System::Mutex         _mutex;
        Pt::System::Condition     _workReady;
        Pt::System::Condition     _workDone;
        bool                      _shutdown;
        std::atomic<bool>         _cancelFlag;

        // Completion signals for Connection-level operations
        Pt::Signal<>              _finished;
        Pt::Signal<>              _executeFinished;
        Pt::Signal<>              _selectFinished;
        Pt::Signal<>              _prepareFinished;

        // Batch cursor state
        SqliteCursor*             _batchCursor;
        bool                      _batchDone;
    };

}}} // Pt::Db::sqlite
#endif // PT_DB_SQLITE_CONNECTION_H
