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

#include "ResultImpl.h"

#include "Pt/Db/Row.h"


namespace Pt {

namespace Db {

	Row ResultImpl::getRow(size_type tup_num) const
	{
		return _data[tup_num];
	}

	ResultImpl::size_type ResultImpl::size() const
	{
		return _data.size();
	}

	ResultImpl::size_type ResultImpl::getFieldCount() const
	{
		return _data.empty() ? 0 : _data.begin()->size();
	}

} // namespace Db

} // namespace Pt
