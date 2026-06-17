#ifndef PT_DB_SQLITE_CONNECTION_H
#define PT_DB_SQLITE_CONNECTION_H

#include <Pt/Db/IConnection.h>
#include <Pt/Db/ICursor.h>
#include <Pt/Db/Result.h>
#include <Pt/SmartPtr.h>
#include <Pt/Signal.h>
#include <Pt/System/Selectable.h>
#include <Pt/System/Thread.h>
#include <Pt/System/Mutex.h>
#include <Pt/System/Condition.h>
#include "sqlite3.h"
#include <atomic>
#include <string>

namespace Pt {

namespace Db {

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

        void beginTransaction();
        void commitTransaction();
        void rollbackTransaction();

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
        void beginBatchFetch(SqliteCursor& cursor, size_type batchSize);
        Result endBatchFetch();
        void closeBatchFetch();

        // Selectable callbacks (EventLoop thread)
        void onAttach(Pt::System::EventLoop& loop) override {}
        void onDetach(Pt::System::EventLoop& loop) override {}
        void onCancel() override;
        bool onRun() override;

    protected:
        // --- IConnection hooks ---

        Pt::Signal<>& onFinished() override { return _finished; }
        Pt::Signal<>& onExecuteFinished() override { return _executeFinished; }
        Pt::Signal<>& onSelectFinished() override  { return _selectFinished; }

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

    private:
        void workerRun();
        void ensureWorker();

        enum OpType
        {
            WkNone       = 0,
            WkExec       = 1,
            WkSelect     = 2,
            WkStmtExec   = 3,
            WkStmtSelect = 4,
            WkOpen       = 5,
            WkBatchFetch = 6
        };

        void enqueue(OpType op);

        // Extends IConnection::State; must not overlap with Idle/PendingOpen/PendingExec/PendingSelect
        enum { PendingBatchFetch = 4 };

        sqlite3*                  _db;
        std::string               _conninfo;

        Pt::System::Thread        _worker;
        Pt::System::Mutex         _mutex;
        Pt::System::Condition     _workReady;
        Pt::System::Condition     _workDone;
        bool                      _shutdown;
        std::atomic<bool>         _cancelFlag;

        // Completion signals
        Pt::Signal<>              _finished;
        Pt::Signal<>              _executeFinished;
        Pt::Signal<>              _selectFinished;
        OpType                    _completedOp;

        // Pending operation
        OpType                    _pendingOp;
        std::string               _pendingSql;
        Statement*                _pendingStmt;
        size_type                 _pendingBatchSize;

        // Cursor state
        SqliteCursor*             _batchCursor;
        size_type                 _batchSize;
        bool                      _batchDone;

        // Operation result
        Result                    _resultSet;
        size_type                 _rowCount;
        bool                      _opFailed;
        std::string               _opError;
    };

}}} // Pt::Db::sqlite
#endif // PT_DB_SQLITE_CONNECTION_H
