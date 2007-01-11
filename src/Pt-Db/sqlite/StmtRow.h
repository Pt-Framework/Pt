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
 
#ifndef PT_DB_SQLITE_STMTROW_H
#define PT_DB_SQLITE_STMTROW_H

#include <Pt/SmartPtr.h>
#include <Pt/Db/IRow.h>

#include "Statement.h"


namespace Pt {

namespace Db {

namespace sqlite {

    /// Row of a statement
    class StmtRow : public IRow
    {
		sqlite3_stmt* _stmt;

		public:
			StmtRow(sqlite3_stmt* stmt)
			: _stmt(stmt)
			{ }

		size_t size() const;
		Value getValue(size_type field_num) const;

		// specific methods of sqlite-driver
		sqlite3_stmt* getStmt() const   
		{ return _stmt; }
    };

} // namespace sqlite

} // namespace Db

} // namespace Pt

#endif // PT_DB_SQLITE_STMTROW_H
