/* Copyright (C) 2026 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#ifndef PT_DB_API_STATEMENTS_H
#define PT_DB_API_STATEMENTS_H

/** @addtogroup Pt-Db-Statements

    @brief Bind parameters and run prepared SQL.

    %Statement is a compiled query with named host variables. Obtain
    one from %Connection::prepare() or %Connection::prepareCached().
    It is a shared value: copying is cheap, and the backend lives
    until the last wrapper is destroyed. An unbound statement is
    empty; %operator!() is true for that state.

    A host variable is a colon, then a name that starts with a letter
    and continues with letters, digits or underscore, for example
    `:id`. Names are not scanned inside strings delimited by
    apostrophes, quotation marks or backticks. A backslash prevents
    the next character from being special. %set() binds a name to a
    typed value. %setNull() binds a name to SQL NULL. %clear() sets
    every host variable to NULL.

    %execute() runs a statement that does not return rows, typically
    INSERT, UPDATE or DELETE, and returns how many rows changed.
    %lastInsertId() is the generated row id after that execute.
    %select() runs a query and returns a fully buffered %Result.
    %selectRow() returns the first row and discards the rest; it
    throws %InvalidQuery when the query returns no row.
    %selectValue() returns the first column of the first row, with
    the same empty-result error.

    For a result that should not be fully buffered, %getCursor()
    opens a batch %Cursor on this statement. Cursor use is documented
    with that type.

    Asynchronous execute and select follow the connection model:
    %beginExecute() / %endExecute() / %executeFinished(), and
    %beginSelect() / %endSelect() / %selectFinished(). %cancel()
    aborts a pending operation on this statement. The connection
    must have been attached with %setActive() first.

    @code
    Pt::Db::Statement stmt = conn.prepare(
        "INSERT INTO users(name) VALUES(:name)");
    stmt.set("name", std::string("Ada"));
    stmt.execute();
    long long id = stmt.lastInsertId();
    @endcode
*/

#endif
