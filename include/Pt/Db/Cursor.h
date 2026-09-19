/* Copyright (C) 2006 by Tommi Maekitalo
   Copyright (C) 2006-2026 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#ifndef PT_DB_CURSOR_H
#define PT_DB_CURSOR_H

#include <Pt/Db/Api.h>
#include <Pt/Db/ICursor.h>
#include <Pt/Db/Result.h>
#include <Pt/SmartPtr.h>
#include <Pt/Signal.h>
#include <cstddef>
#include <iterator>

namespace Pt {

namespace Db {

class Statement;
class CursorIterator;

/** @brief Batch cursor for large query results.

    %Cursor is the batch reader the group described. Obtain it from
    %Statement::getCursor() with the number of rows per batch. It is
    a shared value. The backend cursor is closed when the last copy
    is destroyed or when %close() is called. A default-constructed
    cursor is closed.

    Range-for walks every row and fetches the next batch when the
    current one is exhausted. That path is synchronous. %begin()
    fetches the first batch; %end() is the sentinel.

    %fetch() loads the next batch into the cursor's buffer and
    returns true while the batch is not empty. %result() is that
    buffer, a %Result of the current batch only.

    Asynchronous fetch needs a connection attached with %setActive().
    Connect to %fetchFinished(), then call %beginFetch(). In the
    slot, %endFetch() returns the batch. %isOpen() is true while the
    cursor can still produce rows. Call %beginFetch() again for the
    next batch while it stays open. %close() cancels a fetch in
    progress.

    Do not run another operation on the same connection until the
    cursor is closed.

    @code
    for(const Pt::Db::Row& row : stmt.getCursor(100))
        process(row);
    @endcode

    @ingroup Pt-Db-Cursors
*/
class PT_DB_API Cursor
{
    public:
        typedef std::size_t size_type;
        typedef CursorIterator Iterator;

        /** @brief Creates a closed cursor.
        */
        Cursor()
        : _batchSize(0)
        {}

        /** @brief Takes ownership of @a cursor with a fixed @a batchSize.
        */
        Cursor(ICursor* cursor, size_type batchSize);

        /** @brief Fetches the next batch synchronously.

            @return True if the batch is not empty.
        */
        bool fetch();

        /** @brief Returns the current batch buffer.
        */
        Result& result()
        { return _batch; }

        /** @brief Returns the current batch buffer.
        */
        const Result& result() const
        { return _batch; }

        /** @brief Signal emitted when an asynchronous batch is ready.
        */
        Signal<>& fetchFinished();

        /** @brief Returns true if the cursor is open.
        */
        bool isOpen() const
        { return _cursor && _cursor->isOpen(); }

        /** @brief Starts asynchronous retrieval of the next batch.
        */
        void beginFetch();

        /** @brief Completes an asynchronous fetch.

            Returns an empty result when no more rows are available; the
            cursor is closed in that case.
        */
        Result& endFetch();

        /** @brief Returns the backend cursor.
        */
        ICursor* impl()
        { return _cursor.get(); }

        /** @brief Returns the backend cursor.
        */
        const ICursor* impl() const
        { return _cursor.get(); }

        /** @brief Closes the cursor and releases its resources.
        */
        void close();

        /** @brief Returns an iterator to the first row, fetching the first batch.
        */
        Iterator begin();

        /** @brief Returns the end sentinel iterator.
        */
        Iterator end();

    private:
        typedef SmartPtr<ICursor, InternalRefCounted<ICursor>> CursorImplPtr;
        CursorImplPtr _cursor;
        Result        _batch;
        size_type     _batchSize;
};


/** @brief Forward iterator over the rows of a %Cursor.
*/
class PT_DB_API CursorIterator
{
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type        = Row;
        using difference_type   = std::ptrdiff_t;
        using pointer           = const Row*;
        using reference         = const Row&;

    private:
        Cursor      _cursor;
        std::size_t _index;
        Row         _current;

    public:
        /** @brief Creates the end sentinel.
        */
        CursorIterator()
        : _index(0)
        {}

        /** @brief Creates an iterator on a cursor that already has a batch.
        */
        explicit CursorIterator(const Cursor& cursor);

        bool operator==(const CursorIterator& other) const;

        bool operator!=(const CursorIterator& other) const
        { return ! operator==(other); }

        CursorIterator& operator++();

        const Row& operator*() const
        { return _current; }

        const Row* operator->() const
        { return &_current; }
};

} // namespace Db

} // namespace Pt

#endif // PT_DB_CURSOR_H
