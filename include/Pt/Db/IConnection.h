/*
  Copyright (C) 2006 by Tommi Maekitalo
  Copyright (C) 2006-2026 by Marc Boris Duerner

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  As a special exception, you may use this file as part of a free
  software library without restriction. Specifically, if other files
  instantiate templates or use macros or inline functions from this
  file, or you compile this file and link it with other files to
  produce an executable, this file does not by itself cause the
  resulting executable to be covered by the GNU General Public
  License. This exception does not however invalidate any other
  reasons why the executable file might be covered by the GNU Library
  General Public License.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
  MA 02110-1301 USA
*/

#ifndef PT_DB_ICONNECTION_H
#define PT_DB_ICONNECTION_H

#include <Pt/Db/Api.h>
#include <Pt/RefCounted.h>
#include <Pt/SmartPtr.h>
#include <Pt/Signal.h>
#include <Pt/Db/ICursor.h>
#include <Pt/Db/IStatement.h>

#include <string>
#include <map>

namespace Pt {

namespace System {
class EventLoop;
}

namespace Db {

class Result;
class Row;
class Value;
class Statement;

/** \brief Base class for database connection backends.

    Implements the NVI pattern: the public non-virtual methods manage
    shared state (_isOpen, _state) and delegate to protected virtual
    onXxx() hooks that backends override.

    \see Db::Connection
*/
class PT_DB_API IConnection : public RefCounted
{
    public:
        typedef std::size_t size_type;

        /** \brief Returns true if the database is open.
        */
        bool isOpen() const
        { return _isOpen; }

        /** \brief Signal emitted when an async operation completes.

            Fires on the EventLoop thread. Call the corresponding endXxx()
            method to retrieve the result or receive any exception.
            Delegates to onFinished() — the signal object lives in the backend.
        */
        Pt::Signal<>& finished();

        Pt::Signal<>& executeFinished();

        Pt::Signal<>& selectFinished();

        /** \brief Attach to an EventLoop for async operations.

            Pass nullptr to detach.
        */
        void setActive(Pt::System::EventLoop* loop);

        /** \brief Synchronously open the database.
        */
        void open(const std::string& connStr);

        /** \brief Close the database and cancel any pending operation.
        */
        void close();

        /** \brief Cancel any pending async operation.

            Blocks until the backend has acknowledged cancellation.
            Sets state to Idle.
        */
        void cancelOp();

        /** \brief Begin async open of the database.
        */
        void beginOpen(const std::string& connStr);

        /** \brief Complete async open. Throws on failure.
        */
        void endOpen();

        /** \brief Begin async execution of a DML/SELECT SQL string.
        */
        void beginExec(const std::string& sql);

        size_type endExec();

        void beginSelect(const std::string& sql);

        Result endSelect();


        virtual void beginTransaction() = 0;

        virtual void commitTransaction() = 0;

        virtual void rollbackTransaction() = 0;

        virtual size_type execute(const std::string& query) = 0;

        virtual Result select(const std::string& query) = 0;

        virtual Statement prepare(const std::string& query) = 0;

        virtual Statement prepareCached(const std::string& query) = 0;

        virtual void clearStatementCache() = 0;

        virtual long long insertId() = 0;

    protected:
        IConnection();

        enum State
        {
            Idle          = 0,
            PendingOpen   = 1,
            PendingExec   = 2,
            PendingSelect = 3
        };

        virtual Pt::Signal<>& onFinished() = 0;

        virtual Pt::Signal<>& onExecuteFinished() = 0;

        virtual Pt::Signal<>& onSelectFinished() = 0;

        virtual void onSetActive(Pt::System::EventLoop* loop) = 0;

        virtual void onOpen(const std::string& connStr) = 0;

        virtual void onClose() = 0;

        virtual void onCancelOp() = 0;

        virtual void onBeginOpen(const std::string& connStr) = 0;

        virtual void onEndOpen() = 0;

        virtual void onBeginExec(const std::string& sql) = 0;

        virtual size_type onEndExec() = 0;

        virtual void onBeginSelect(const std::string& sql) = 0;

        virtual Result onEndSelect() = 0;

        bool                    _isOpen;
        State                   _state;
        Pt::System::EventLoop*  _loop;
};


/** \brief Base class for backends that need a statement cache.

    Provides default implementations of prepareCached() and
    clearStatementCache(). Backends inherit this instead of IConnection
    directly.

    \see Db::Connection
*/
class PT_DB_API IStmtCacheConnection : public IConnection
{
    public:
        virtual Statement prepareCached(const std::string& query);

        virtual void clearStatementCache();

    private:
        typedef SmartPtr<IStatement, InternalRefCounted<IStatement> > StatementPtr;
        typedef std::map<std::string, StatementPtr> StatementCache;
        StatementCache _stmtCache;
};

} // namespace Db

} // namespace Pt

#endif // PT_DB_ICONNECTION_H

