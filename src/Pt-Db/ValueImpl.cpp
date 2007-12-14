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
#include <Pt/SourceInfo.h>
#include <stdexcept>
#include <sstream>
#include <iostream>


namespace
{
    bool isTrue(char ch)
    {
        return ch == 't' || ch == 'T' || ch == 'y' || ch == 'Y' || ch == '1';
    }

    template <typename T>
    T getValue(const std::string& s)
    {
        std::istringstream in(s);
        T ret;
        in >> ret;

        if ( in.fail() )
            throw std::logic_error( "Conversion error" + PT_SOURCEINFO );

        return ret;
    }

    double getValue(const std::string& s)
    {
        std::istringstream in(s);
        double ret;
        in >> std::scientific >> std::setprecision(15) >> ret;

        if ( in.fail() )
            throw std::logic_error( "Conversion error" + PT_SOURCEINFO );

        return ret;
    }
}

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

    return !_data.empty() && ::isTrue(_data.at(0));
}


int ValueImpl::getInt() const
{
    if (_null)
        throw std::logic_error("Value is null." + PT_SOURCEINFO);

    return ::getValue<int>(_data);
}


unsigned ValueImpl::getUnsigned() const
{
    if (_null)
        throw std::logic_error("Value is null." + PT_SOURCEINFO);

    return ::getValue<unsigned>(_data);
}


float ValueImpl::getFloat() const
{
    if (_null)
        throw std::logic_error("Value is null." + PT_SOURCEINFO);

    return ::getValue<float>(_data);
}


double ValueImpl::getDouble() const
{
    if (_null)
        throw std::logic_error("Value is null." + PT_SOURCEINFO);

    return ::getValue<double>(_data);
}


char ValueImpl::getChar() const
{
    if (_null)
        throw std::logic_error("Value is null." + PT_SOURCEINFO);

    return _data.at(0);
}


void ValueImpl::getString(std::string& s) const
{
    if (_null)
        throw std::logic_error("Value is null." + PT_SOURCEINFO);

    s = _data;
}


Date ValueImpl::getDate() const
{
    if (_null)
        throw std::logic_error("Value is null." + PT_SOURCEINFO);

    return Date::fromIsoString(_data);
}


Time ValueImpl::getTime() const
{
    if (_null)
        throw std::logic_error("Value is null." + PT_SOURCEINFO);

    return Time::fromIsoString(_data);
}


DateTime ValueImpl::getDateTime() const
{
    if (_null)
        throw std::logic_error("Value is null." + PT_SOURCEINFO);

    return DateTime::fromIsoString(_data);
}

void ValueImpl::getBlob(Pt::Blob& blob) const
{
    if (_null)
        throw std::logic_error("Value is null." + PT_SOURCEINFO);

    blob.assign( _data.data(), _data.size() );
}


} // namespace Db

} // namespace Pt
