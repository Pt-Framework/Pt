/* Copyright (C) 2005-2026 by Dr. Marc Boris Duerner
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
        @brief Portable SQL database access.

        Connections, prepared statements, buffered results and batch
        cursors for SQL databases, using the same types on every
        supported platform.
    */
    namespace Db {
        class AccessDenied;
        class Blob;
        class Connection;
        class ConnectionError;
        class ConstraintMismatch;
        class Cursor;
        class DbError;
        class InvalidConnection;
        class InvalidQuery;
        class QueryFailed;
        class Result;
        class Row;
        class Statement;
        class Transaction;
        class TypeMismatch;
        class Value;
    }

}

#endif
