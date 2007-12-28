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

#ifndef PT_DB_SQLITE_STMTVALUE_H
#define PT_DB_SQLITE_STMTVALUE_H

#include "Statement.h"

#include <Pt/Db/Blob.h>
#include <Pt/SmartPtr.h>
#include <Pt/Db/IValue.h>


namespace Pt {

namespace Db {

namespace sqlite {

    class StmtValue : public IValue
    {
        sqlite3_stmt* _stmt;
        int _iCol;

        public:
            StmtValue(sqlite3_stmt* stmt, int iCol)
                : _stmt(stmt)
                , _iCol(iCol)
            { }

            virtual bool isNull() const;
            virtual bool getBool() const;
            virtual int getInt() const;
            virtual unsigned getUnsigned() const;
            virtual float getFloat() const;
            virtual double getDouble() const;
            virtual char getChar() const;
            virtual void getString(std::string& stringdata) const;
            virtual Date getDate() const;
            virtual Time getTime() const;
            virtual DateTime getDateTime() const;

            //TODO: extra blob function needed??? ->    alternative: read all text values with blob sql function
            //virtual void getBlob(std::string& blobdata) const;
            virtual void getBlob(Pt::Blob& blobdata) const;

            // specific methods of sqlite-driver
            sqlite3_stmt* getStmt() const   { return _stmt; }
    };

} // namespace sqlite

} // namespace Db

} // namespace Pt

#endif // PTV_DB_SQLITE_STMTVALUE_H

