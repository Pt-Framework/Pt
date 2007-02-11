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

#include "Pt/Db/Result.h"
#include "Pt/Db/Value.h"

namespace Pt {

namespace Db {

    Row Result::getRow(size_type tup_num) const
    {
        return _result->getRow(tup_num);
    }

    Value Result::getValue(size_type tup_num, size_type field_num) const
    {
        return getRow(tup_num).getValue(field_num);
    }

    Row Result::operator[] (size_type row_num) const
    {
        return getRow(row_num);
    }

    Result::ConstIterator Result::begin() const
    {
        return ConstIterator(*this, 0);
    }

    Result::ConstIterator Result::end() const
    {
        size_type s = size();
        return ConstIterator(*this, s);
    }

} // namespace Db

} // namespace Pt
