/***************************************************************************
 *   Copyright (C) 2006 by Tommi Maekitalo                                 *
 *   Copyright (C) 2006 by Marc Boris Duerner                              *
 *   Copyright (C) 2006 by Stefan Bueder                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef PT_DB_ICONNECTION_H
#define PT_DB_ICONNECTION_H

#include <Pt/Db/Api.h>
#include <Pt/RefCounted.h>
#include <Pt/SmartPtr.h>

#include <string>
#include <map>


namespace Pt {

namespace Db {

    class Result;
    class Row;
    class Value;
    class Statement;
    class IStatement;

    /** \brief Interface for database connection
		\see Db::Connection
	*/
    class PT_DB_API IConnection : public RefCounted
    {
        public:
            typedef size_t size_type;

            virtual void beginTransaction() = 0;
            virtual void commitTransaction() = 0;
            virtual void rollbackTransaction() = 0;

            virtual size_type execute(const std::string& query) = 0;
            virtual Result select(const std::string& query) = 0;
            virtual Row selectRow(const std::string& query) = 0;
            virtual Value selectValue(const std::string& query) = 0;
            virtual Statement prepare(const std::string& query) = 0;
            virtual Statement prepareCached(const std::string& query) = 0;
            virtual void clearStatementCache() = 0;
            virtual long long insertId() = 0;

    };

    //---------------------------------------------------------------------------------

    /** \brief Interface for a statement cached connection
		\see Db::Connection
	*/
    class IStmtCacheConnection : public IConnection
    {
        typedef SmartPtr<IStatement, InternalRefCounted<IStatement> > StatementPtr;
        typedef std::map<std::string,  StatementPtr > StatementCache;
        StatementCache _stmtCache;

    public:
        virtual Statement prepareCached(const std::string& query);
        virtual void clearStatementCache();
    };

} // namespace Db

} // namespace Pt


#endif // PTV_DB_ICONNECTION_H

