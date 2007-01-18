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

#ifndef PT_DB_IRESULT_H
#define PT_DB_IRESULT_H

#include <Pt/Api.h>
#include <Pt/Types.h>
#include <Pt/RefCounted.h>
#include <Pt/NonCopyable.h>
#include <Pt/Db/Api.h>


namespace Pt {

namespace Db {

	class Row;

	/** \brief Interface for DB Values
		\see Db::Result
	*/
	class PT_DB_API IResult : public RefCounted, private NonCopyable
	{
	public:
		typedef size_t size_type;
		typedef Row value_type;

	public:
		virtual ~IResult()  
		{ }

		virtual Row getRow(size_type tup_num) const = 0;

		virtual size_type size() const = 0;

		virtual size_type getFieldCount() const = 0;
	};
  
} // namespace Db

} // namespace Pt

#endif // PTV_DB_IRESULT_H

