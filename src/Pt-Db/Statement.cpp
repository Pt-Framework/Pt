/*
  Copyright (C) 2006 by Tommi Maekitalo
  Copyright (C) 2006 by Marc Boris Duerner

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  As a special exception, you may use this file as part of a free
  software library without restriction. Specifically, if other files
  instantiate templates or use macros or inline functions from this
  file, or you compile this file and link it with other files to
  produce an executable, this file does not by itself cause the
  resulting executable to be covered by the GNU General Public
  License. This exception does not however invalidate any other
  reasons why the executable file might be covered by the GNU Library
  General Public License.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
  MA 02110-1301 USA
 */

#include <Pt/Db/Statement.h>
#include <Pt/Db/IConnection.h>
#include <Pt/Db/Result.h>
#include <Pt/Db/Row.h>
#include <Pt/Db/Value.h>

namespace Pt {

namespace Db {

//
// execute SQL statement
//

Statement::size_type Statement::execute()
{
    return _stmt->connection()->execute(*_stmt);
}


void Statement::beginExecute()
{
    _stmt->connection()->beginExecute(*_stmt);
}


Statement::size_type Statement::endExecute()
{
    return _stmt->connection()->endExecute(*_stmt);
}


Signal<>& Statement::executeFinished()
{
    return _stmt->executeFinished();
}

//
// select SQL statement
//

Result Statement::select()
{
    return _stmt->connection()->select(*_stmt);
}


Row Statement::selectRow()
{
    return _stmt->connection()->selectRow(*_stmt);
}


Value Statement::selectValue()
{
    return _stmt->connection()->selectValue(*_stmt);
}


void Statement::beginSelect()
{
    _stmt->connection()->beginSelect(*_stmt);
}


Result Statement::endSelect()
{
    return _stmt->connection()->endSelect(*_stmt);
}


Signal<>& Statement::selectFinished()
{
    return _stmt->selectFinished();
}


void Statement::cancel()
{
    _stmt->connection()->cancelOp();
}


Cursor Statement::getCursor(size_type batchSize)
{
    return _stmt->connection()->getCursor(*_stmt, batchSize);
}

} // namespace Db

} // namespace Pt
