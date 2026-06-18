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

/** @brief Shared batch cursor for large result sets.

    Obtained via Statement::getCursor(batchSize). Supports both synchronous
    and asynchronous batch iteration.

    Sync usage — range-for iterates all rows, fetching batches automatically:
    @code
    for(auto& row : stmt.getCursor(100))
        process(row);
    @endcode

    Sync usage — manual batch control with random access:
    @code
    Pt::Db::Cursor cursor = stmt.getCursor(100);
    while(cursor.fetch())
    {
        Pt::Db::Result& batch = cursor.result();
        for(std::size_t i = 0; i < batch.size(); ++i)
            process(batch[i]);
    }
    @endcode

    Async usage — signal-driven batch iteration:
    @code
    Pt::Db::Cursor cursor = stmt.getCursor(100);
    cursor.fetched() += Pt::slot(rx, &Receiver::onBatch);
    cursor.beginFetch();
    loop.run();

    void Receiver::onBatch()
    {
        Pt::Db::Result& batch = cursor.endFetch();
        if( ! cursor.isOpen())
            return;
        // process batch ...
        cursor.beginFetch();
    }
    @endcode

    %Cursor is a shared value type (like %Statement). Copying is cheap.
    The underlying cursor is closed when the last copy is destroyed or
    close() is called explicitly.

    @ingroup Pt-Db
*/
class PT_DB_API Cursor
{
    public:
        typedef std::size_t size_type;
        typedef CursorIterator Iterator;

        /** @brief Default-construct a null (closed) cursor.
        */
        Cursor()
        : _batchSize(0)
        {}

        /** @brief Construct from a backend cursor with a fixed batch size.
        */
        Cursor(ICursor* cursor, size_type batchSize);

        /** @brief Fetch the next batch of rows synchronously.

            Loads the next batch into the internal buffer accessible via result().
            Returns true if the batch is non-empty, false when no more rows
            are available.
        */
        bool fetch();

        /** @brief Returns a reference to the current batch buffer.

            Valid after a call to fetch() or endFetch(). Provides random access
            to all rows in the current batch.
        */
        Result& result()
        { return _batch; }

        /** @brief Returns a const reference to the current batch buffer.
        */
        const Result& result() const
        { return _batch; }

        /** @brief Signal emitted when an async batch is ready to retrieve.

            Connect before calling beginFetch(). In the slot call endFetch()
            to retrieve the batch, then call beginFetch() again for the next
            batch.
        */
        Signal<>& fetched();

        /** @brief Returns true if the cursor is open (async path).
        */
        bool isOpen() const
        { return _cursor && _cursor->isOpen(); }

        /** @brief Start async retrieval of the next batch of rows.
        */
        void beginFetch();

        /** @brief Retrieve the current batch after an async fetch completes.

            Stores the batch in the internal buffer and returns a reference to
            it. Returns an empty Result& when no more rows are available; the
            cursor is closed automatically in that case.
        */
        Result& endFetch();

        /** @brief Return the underlying backend cursor pointer.
        */
        ICursor* impl()
        { return _cursor.get(); }

        const ICursor* impl() const
        { return _cursor.get(); }

        /** @brief Close the cursor and release all associated resources.

            Safe to call even if a fetch is currently in progress (cancels it).
            Calling on an already-closed cursor is a no-op.
        */
        void close();

        /** @brief Return an iterator to the first row, fetching the first batch.

            Triggers the first synchronous batch fetch. Returns end() if no
            rows are available.
        */
        Iterator begin();

        /** @brief Return the end sentinel iterator.
        */
        Iterator end();

    private:
        typedef SmartPtr<ICursor, InternalRefCounted<ICursor>> CursorImplPtr;
        CursorImplPtr _cursor;
        Result        _batch;
        size_type     _batchSize;
};


/** @brief Forward iterator over rows of a %Cursor, fetching batches as needed.

    Obtained via Cursor::begin(). Iterates all rows across all batches; a new
    batch is fetched automatically when the current batch is exhausted. The
    end sentinel is a default-constructed %CursorIterator.

    @ingroup Pt-Db
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
        /** @brief Default-construct the end sentinel.
        */
        CursorIterator()
        : _index(0)
        {}

        /** @brief Construct from a cursor that already has a batch loaded.
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
