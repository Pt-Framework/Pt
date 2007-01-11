#ifndef PT_DB_H
#define PT_DB_H


/** \dir
This module provides abstract access to sql-based databases. Backends exist for
sqlite, postgresql and mysql.
All classes and functions are in the namespace Db, which is nested
in the Pt namespace.
*/

namespace Pt {

	/** The DB Module.
	This module provides abstract access to sql-based databases. Backends exist for
	sqlite, postgresql and mysql.
	All classes and functions are in the namespace Db, which is nested
	in the Pt namespace.
	 */
	namespace Db {
		class Connection;
        class Statement;
        class Transacion;
        class Value;
        class Resilt;
        class Row;
	}

}

/** 
\page "Opening Connections"
!!! Opening Connections
TODO.
*/

/**
\page "Transactions"
!!! Transactions
TODO.
*/

/**
\page "Retreiving Data"
!!! Retreiving Data
The DB Module offers two ways of retreiving data from a database: the Result set and
a cursor/iterator based API, where the first allows buffered random-access and
the latter unbuffered sequential-access.

A resultset is similar to a two-dimensional array and is represented by the Db::Result
class. The result of a select statement is read completely into memory and random-access
to the rows and values is possible. Accordingly, the iterator on a Db::Result is
a random-access iterator.

The cursor-based API does not read a complete reslutset into memory, but only the
current value. Thus it does not alow random-access and the cursor is implemented
as a forward-iterator. It is the preferred way to acces resultsets which are 
too large to be held in memory. 

A cursor is created as a const iterator when Db::Statement::begin() is called
and the iteration is started.
*/


#endif

