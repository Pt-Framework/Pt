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

#include "Pt/Db/Statement.h"
#include "Pt/Db/Result.h"
#include "Pt/Db/Row.h"
#include "Pt/Db/Value.h"

#include <iostream>
using namespace std;


namespace Pt {

namespace Db {

    Statement::size_type Statement::execute()
    {
        return _stmt->execute();
    }

    Result Statement::select()
    {
        return _stmt->select();
    }

    Row Statement::selectRow()
    {
        return _stmt->selectRow();
    }

    Value Statement::selectValue()
    {
        return _stmt->selectValue();
    }

    Statement::ConstIterator Statement::begin() const
    {
        return ConstIterator( _stmt->createCursor() );
    }

    Statement::ConstIterator Statement::end() const
    {
        return ConstIterator();
    }


    Statement::ConstIterator::ConstIterator(ICursor* cursor)
        : _cursor(cursor)
    {
        if (cursor)
        {
            _current = cursor->fetch();
            if (!_current) {
                //clog << "No row fetched" << endl;
                _cursor = 0;
            }
        }
    }


    Statement::ConstIterator& Statement::ConstIterator::operator++()
    {
        _current = _cursor->fetch();

        if (!_current) {
            //clog << "No row fetched" << endl;
            _cursor = 0;
        }

        return *this;
    }

} // namespace Db

} // namespace Pt
