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

#ifndef PT_DB_SQLITE_STATEMENT_H
#define PT_DB_SQLITE_STATEMENT_H

#include <Pt/Db/IStatement.h>
#include <Pt/Db/Blob.h>
#include <Pt/SmartPtr.h>
#include "sqlite3.h"

namespace Pt {

namespace Db {

namespace sqlite {

    class Connection;

    class Statement : public IStatement
    {
        sqlite3_stmt* _stmt;
        sqlite3_stmt* _stmtInUse;
        SmartPtr<Connection, InternalRefCounted<Connection> > _conn;
        const std::string _query;

        sqlite3_stmt* getBindStmt();
        int getBindIndex(const std::string& col);

        bool _needReset;
        void reset();

        public:
            Statement(Connection* conn, const std::string& query);
            ~Statement();

            // methods of IStatement

            virtual void clear();
            virtual void setNull(const std::string& col);
            virtual void setBool(const std::string& col, bool data);
            virtual void setInt(const std::string& col, int data);
            virtual void setUnsigned(const std::string& col, unsigned data);
            virtual void setFloat(const std::string& col, float data);
            virtual void setDouble(const std::string& col, double data);
            virtual void setChar(const std::string& col, char data);
            virtual void setString(const std::string& col, const std::string& data);
            virtual void setDate(const std::string& col, const Date& data);
            virtual void setTime(const std::string& col, const Time& data);
            virtual void setDatetime(const std::string& col, const DateTime& data);
            virtual void setBlob(const std::string& col, const Blob& data);

            virtual size_type execute();
            virtual Result select();
            virtual Row selectRow();
            virtual Value selectValue();
            virtual ICursor* createCursor();

            // specific methods of sqlite-driver
            sqlite3_stmt* getStmt() const   { return _stmt; }

            void putback(sqlite3_stmt* stmt);

    };

} //namespace sqlite

} //namespace Db

} //namespace Pt


#endif // PT_DB_SQLITE_STATEMENT_H

