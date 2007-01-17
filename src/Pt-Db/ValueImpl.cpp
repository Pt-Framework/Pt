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

#include "ValueImpl.h"

#include <Pt/Blob.h>
#include <Pt/Date.h>
#include <Pt/Time.h>
#include <Pt/DateTime.h>
#include <Pt/Exception.h>

#include <sstream>
#include <iostream>


namespace Pt {

namespace Db {

bool ValueImpl::isNull() const
{
    return _null;
}


bool ValueImpl::getBool() const
{
    if (_null)
        throw std::logic_error("Value is null." + PT_SOURCEINFO);

    if( _data.empty() )
        return false;

    return this->isTrue( _data.str()[0] );

}


int ValueImpl::getInt() const
{
    if (_null)
        throw std::logic_error("Value is null." + PT_SOURCEINFO);

    int ret = 0;
    _data.get(ret);
    return ret;
}


unsigned ValueImpl::getUnsigned() const
{
    if (_null)
        throw std::logic_error("Value is null." + PT_SOURCEINFO);

    unsigned ret = 0;
    _data.get(ret);
    return ret;
}


float ValueImpl::getFloat() const
{
    if (_null)
        throw std::logic_error("Value is null." + PT_SOURCEINFO);

    float ret = 0.0f;
    _data.get(ret);
    return ret;
}


double ValueImpl::getDouble() const
{
    if (_null)
        throw std::logic_error("Value is null." + PT_SOURCEINFO);

    double ret = 0.0f;
    _data.get(ret);
    return ret;
}


char ValueImpl::getChar() const
{
    if (_null)
        throw std::logic_error("Value is null." + PT_SOURCEINFO);

    return _data.str()[0];
}


void ValueImpl::getString(std::string& stringdata) const
{
    if (_null)
        throw std::logic_error("Value is null." + PT_SOURCEINFO);

    stringdata = _data.str();
}


Date ValueImpl::getDate() const
{
    if (_null)
        throw std::logic_error("Value is null." + PT_SOURCEINFO);

    return Date::fromIsoString( _data.str() );
}


Time ValueImpl::getTime() const
{
    if (_null)
        throw std::logic_error("Value is null." + PT_SOURCEINFO);

    return Time::fromIsoString( _data.str() );
}


DateTime ValueImpl::getDateTime() const
{
    if (_null)
        throw std::logic_error("Value is null." + PT_SOURCEINFO);

    return DateTime::fromIsoString( _data.str() );
}

void ValueImpl::getBlob(Pt::Blob& blobdata) const
{
    if (_null)
        throw std::logic_error("Value is null." + PT_SOURCEINFO);

    blobdata.assign(_data.str().data(), _data.str().length());
}


} // namespace Db

} // namespace Pt
