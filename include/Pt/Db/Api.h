/* Copyright (C) 2005-2007 by Dr. Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#ifndef PT_DB_API_H
#define PT_DB_API_H

#include <Pt/Api.h>

#define PT_DB_VERSION_MAJOR PT_VERSION_MAJOR
#define PT_DB_VERSION_MINOR PT_VERSION_MINOR
#define PT_DB_VERSION_REVISION PT_VERSION_REVISION
#define PT_DB_VERSION_PRERELEASE PT_VERSION_PRERELEASE

#if defined(PT_DB_API_EXPORT)
#    define PT_DB_API PT_EXPORT
#  else
#    define PT_DB_API PT_IMPORT
#  endif

namespace Pt {

    /** @namespace Pt::Db
        @brief Transparent Database Access

        This module provides abstract access to sql-based databases. Backends exist for
        sqlite, postgresql and mysql. All classes and functions are in the namespace Db,
        which is nested in the Pt namespace.
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

//
// NOTE: normal comments, so doxygen does not pick this up until the docs are usable
//

/*
\page "Opening Connections"
!!! Opening Connections
TODO.

\page "Transactions"
!!! Transactions
TODO.

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
