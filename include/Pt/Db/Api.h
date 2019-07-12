/*
 * Copyright (C) 2005-2007 by Dr. Marc Boris Duerner
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */
#ifndef PT_DB_API_H
#define PT_DB_API_H

#include <Pt/Api.h>

#define PT_DB_VERSION_MAJOR 1
#define PT_DB_VERSION_MINOR 2
#define PT_DB_VERSION_REVISION 0

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
