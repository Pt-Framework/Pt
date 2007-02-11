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
 
#include "Cursor.h"
#include "StmtRow.h"
#include "Error.h"

#include <Pt/Db/Row.h>


namespace Pt {
 
namespace Db {
 
namespace sqlite {

    Cursor::Cursor(Statement* statement, sqlite3_stmt* stmt)
        : _statement(statement)
        , _stmt(stmt)
    { }

    Cursor::~Cursor()
    {
        _statement->putback(_stmt);
    }

    Row Cursor::fetch()
    {
        //log_debug("sqlite3_step(" << stmt << ')');
        int ret = ::sqlite3_step(_stmt);
        if (ret == SQLITE_DONE)
        {
            return Row();
        }
        else if (ret != SQLITE_ROW)
        {
            Pt::Db::sqlite::Error(ret, PT_SOURCEINFO);
        }

        return Row(new StmtRow(getStmt()));
    }

} //namespace sqlite

} //namespace Db

} //namespace Pt

