/* Copyright (C) 2026 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#ifndef PT_DB_API_CURSORS_H
#define PT_DB_API_CURSORS_H

/** @addtogroup Pt-Db-Cursors

    @brief Fetch large results in batches.

    A %Cursor reads a query a batch at a time instead of loading every
    row into one %Result. Obtain it from %Statement::getCursor() with
    the number of rows per batch. It is a shared value: copying is
    cheap, and the backend cursor is closed when the last copy is
    destroyed or when %close() is called.

    Range-for walks every row and fetches the next batch when the
    current one is exhausted. That path is synchronous.

    %fetch() loads the next batch into the cursor's buffer and returns
    true while the batch is not empty. %result() is that buffer, a
    %Result with random access to the rows of the current batch only.
    When %fetch() returns false, no more rows are available.

    Asynchronous fetch needs a connection that was attached with
    %setActive(). Connect to %fetchFinished(), then call
    %beginFetch(). In the slot, %endFetch() returns the batch.
    %isOpen() is true while the cursor can still produce rows; when
    it becomes false, the cursor has closed itself. Call
    %beginFetch() again for the next batch while it stays open.

    A cursor holds the statement's result stream. Do not run another
    operation on the same connection until the cursor is closed.

    @code
    Pt::Db::Cursor cursor = stmt.getCursor(100);
    for(const Pt::Db::Row& row : cursor)
        process(row);
    @endcode
*/

#endif
