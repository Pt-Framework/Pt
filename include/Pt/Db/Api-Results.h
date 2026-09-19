/* Copyright (C) 2026 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#ifndef PT_DB_API_RESULTS_H
#define PT_DB_API_RESULTS_H

/** @addtogroup Pt-Db-Results

    @brief Buffered rows, column values and binary data.

    A %Result is a query result held entirely in memory. Rows are
    addressed by index and visited with a random-access iterator, so
    the set can be walked more than once and in any order. %size() is
    the number of rows. %getFieldCount() is the number of columns.
    %empty() is true when there are no rows. %operator[] and
    %getRow() return a %Row at an index without range checking.

    A %Row is one of those rows. Columns are addressed by index.
    %size() is the column count. %getValue() and %operator[] return a
    %Value. Typed getters such as %getInt() and %getString() convert
    that column. %isNull() reports SQL NULL at an index. The row also
    has a random-access iterator over its values.

    A %Value is one column. %isNull() is true for SQL NULL and for an
    unbound value. Typed getters convert the stored value; a null or
    a conversion that cannot be performed throws %TypeMismatch.
    %getBlob() writes binary data into a %Blob.

    %Blob is a copy-on-write binary value. %data() and %size()
    describe the bytes. %assign() replaces them. Equality compares
    the bytes.

    %Result, %Row, %Value and %Blob are shared values. Copying is
    cheap. They do not keep the connection busy: once a %Result is
    returned, the connection can run another statement.

    Use a %Result when the whole set fits in memory and random access
    is useful. Use a %Cursor when the set is large and rows should
    arrive in batches.

    @code
    Pt::Db::Result result = conn.select("SELECT id, name FROM users");
    for(Pt::Db::Result::ConstIterator it = result.begin();
        it != result.end(); ++it)
    {
        int id = it->getInt(0);
        std::string name = it->getString(1);
    }
    @endcode
*/

#endif
