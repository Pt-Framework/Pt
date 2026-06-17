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

namespace Pt {

namespace Db {

class Statement;

/** @brief Shared async batch cursor for large result sets.

    Obtained via Statement::getCursor(). Connect to fetched() and call
    beginFetch(n) to start iterating. Each signal invocation delivers one
    batch via endFetch(). An empty %Result returned by endFetch() signals
    end-of-data and closes the cursor automatically.

    %Cursor is a shared value type (like %Statement). Copying is cheap.
    The underlying cursor is closed when the last copy is destroyed or
    close() is called explicitly.

    @code
    Pt::Db::Cursor cursor = stmt.getCursor();
    cursor.fetched() += Pt::slot(rx, &Receiver::onBatch);
    cursor.beginFetch(100);
    loop.run();

    void Receiver::onBatch()
    {
        Pt::Db::Result batch = cursor.endFetch();
        if(!cursor.isOpen())
            return;  // closed automatically on empty result
        // process batch ...
        cursor.beginFetch(100);
    }
    @endcode

    @ingroup Pt-Db
*/
class PT_DB_API Cursor
{
    public:
        typedef std::size_t size_type;

        /** @brief Default-construct a null (closed) cursor.
        */
        Cursor()
        {}

        /** @brief Construct from a backend cursor (takes ownership).
        */
        explicit Cursor(ICursor* cursor);

        /** @brief Signal emitted when a batch is ready to retrieve.

            Connect before calling beginFetch(). In the slot call endFetch()
            to retrieve the batch, then call beginFetch() again for the next
            batch.
        */
        Signal<>& fetched();

        /** @brief Returns true if the cursor is open.
        */
        bool isOpen() const
        { return _cursor && _cursor->isOpen(); }

        /** @brief Start async retrieval of the next batch of rows.

            @param batchSize Number of rows to retrieve in this batch.
        */
        void beginFetch(size_type batchSize);

        /** @brief Retrieve the current batch of rows.

            Returns an empty %Result when no more rows are available; the
            cursor is closed automatically in that case.
        */
        Result endFetch();

        /** @brief Close the cursor and release all associated resources.

            Safe to call even if a fetch is currently in progress (cancels it).
            Calling on an already-closed cursor is a no-op.
        */
        void close();

    private:
        typedef SmartPtr<ICursor, InternalRefCounted<ICursor>> CursorImplPtr;
        CursorImplPtr _cursor;
};

} // namespace Db

} // namespace Pt

#endif // PT_DB_CURSOR_H
