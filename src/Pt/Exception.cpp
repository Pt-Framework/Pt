/***************************************************************************
 *   Copyright (C) 2004-2006 Marc Boris Dürner                             *
 *   Copyright (C) 2005-2006 Aloysius Indrayanto                           *
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

#include "Pt/Exception.h"

#include <string>
using namespace std;


namespace Pt {

Exception::Exception(const std::string & what, const SourceInfo& si) throw()
: _what(what), _source(si)
{ }

Exception::Exception(const Exception& err) throw()
: _what(err._what), _source(err._source)
{ }


Exception::~Exception() throw()
{ }

const SourceInfo& Exception::sourceInfo() const throw()
{ return _source; }


const char* Exception::what() const throw()
{ return _what.c_str(); }

Exception& Exception::operator=(const Exception& err) throw()
{
	_what   = err._what;
	_source = err._source;
	return *this;
}




RuntimeError::RuntimeError(const std::string & what, const SourceInfo& si) throw()
: Exception(what, si)
{ }

RuntimeError::~RuntimeError() throw()
{ }




LogicError::LogicError(const std::string & what, const SourceInfo& si) throw()
: Exception(what, si)
{ }

LogicError::~LogicError() throw()
{ }




AccessError::AccessError(const std::string& what, const SourceInfo& si) throw()
: LogicError(what, si)
{ }


AccessError::~AccessError() throw()
{ }

} // namespace Ptv
