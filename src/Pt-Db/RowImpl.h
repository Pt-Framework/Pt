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


#ifndef PT_DB_ROW_IMPL_H
#define PT_DB_ROW_IMPL_H

#include <Pt/Db/IRow.h>
#include <Pt/Db/Value.h>

#include <vector>

namespace Pt {

namespace Db {

    class RowImpl : public IRow
    {
        public:
            typedef std::vector<Value> data_type;

        private:
            data_type _data;

        public:
            RowImpl()
            { }

            explicit RowImpl(const data_type& data)
                : _data(data)
            { }

            // methods from IResult
            virtual size_type size() const;
            virtual Value getValue(size_type field_num) const;

            // specific methods
            void add(const Value& value)   
            { _data.push_back(value); }
    };

} // namespace Db

} // namespace Pt

#endif // PTV_DB_ROW_IMPL_H

