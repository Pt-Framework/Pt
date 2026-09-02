---
description: "Database connections, queries, result sets, transactions, and backends"
---

- Open, close, ping, execute SQL, select results, prepare statements, and use async connection operations:
  `include/Pt/Db/Connection.h`
- Implement a database connection backend, synchronous and asynchronous operation hooks, statement caches, or cursor fetching:
  `include/Pt/Db/IConnection.h`
- Provide a driver connector that allocates unconnected database backends:
  `include/Pt/Db/IConnector.h`
- Bind named SQL parameters, execute or select prepared statements, retrieve generated IDs, and open batch cursors:
  `include/Pt/Db/Statement.h`
- Implement a backend prepared statement and parameter binding:
  `include/Pt/Db/IStatement.h`
- Iterate large result sets in synchronous or asynchronous batches:
  `include/Pt/Db/Cursor.h`
- Implement a backend cursor and its batch-fetch lifecycle:
  `include/Pt/Db/ICursor.h`
- Store, index, and randomly iterate buffered query result rows:
  `include/Pt/Db/Result.h`
- Implement a backend buffered query result:
  `include/Pt/Db/IResult.h`
- Access values in a database row by index or iterate its columns:
  `include/Pt/Db/Row.h`
- Implement a backend database row:
  `include/Pt/Db/IRow.h`
- Read typed scalar, date/time, string, or binary column values:
  `include/Pt/Db/Value.h`
- Implement a backend database value and its typed conversions:
  `include/Pt/Db/IValue.h`
- Store, share, compare, and copy binary large-object data:
  `include/Pt/Db/Blob.h`
- Begin, commit, roll back, or customize database transactions:
  `include/Pt/Db/Transaction.h`
- Handle database connection, SQL query, constraint, and type-conversion failures:
  `include/Pt/Db/DbError.h`