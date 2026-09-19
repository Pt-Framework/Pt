/* Copyright (C) 2006-2026 by Tommi Maekitalo
   Copyright (C) 2006-2026 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#ifndef PT_DB_ICURSOR_H
#define PT_DB_ICURSOR_H

#include <Pt/Db/Api.h>
#include <Pt/Db/Result.h>
#include <Pt/RefCounted.h>
#include <Pt/Signal.h>
#include <cstddef>

namespace Pt {

namespace Db {

class Row;
class IConnection;


/** @brief Batch-cursor backend.
*/
class PT_DB_API ICursor : public RefCounted
{
    friend class IConnection;

    public:
        typedef std::size_t size_type;

        /** @brief Signal emitted when an asynchronous batch is ready.
        */
        Signal<>& fetchFinished()
        { return _fetched; }

        /** @brief Returns true if the cursor is open.
        */
        bool isOpen() const
        { return _open; }

        /** @brief Returns the connection that owns this cursor.
        */
        IConnection* connection()
        { return _conn; }

    protected:
        explicit ICursor(IConnection* conn)
        : _conn(conn)
        , _open(false)
        {}

        virtual Result onFetchBatch(size_type batchSize) = 0;

        virtual void   onBeginBatchFetch(size_type batchSize) = 0;
        virtual Result onEndBatchFetch() = 0;
        virtual void   onClose() = 0;

        Signal<>     _fetched;
        IConnection* _conn;
        bool         _open;
};

} // namespace Db

} // namespace Pt

#endif // PT_DB_ICURSOR_H
