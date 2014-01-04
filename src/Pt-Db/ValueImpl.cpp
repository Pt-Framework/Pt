/*
 * Copyright (C) 2006 by Tommi Maekitalo
 * Copyright (C) 2006 by Marc Boris Duerner
 * Copyright (C) 2006 by Stefan Bueder
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "ValueImpl.h"

#include <Pt/Db/Blob.h>
#include <Pt/Date.h>
#include <Pt/Time.h>
#include <Pt/DateTime.h>
#include <Pt/SourceInfo.h>
#include <stdexcept>
#include <sstream>
#include <iostream>
#include <iomanip>

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

void ValueImpl::getBlob(Blob& blob) const
{
    if (_null)
        throw std::logic_error("Value is null." + PT_SOURCEINFO);

    blob.assign( _data.data(), _data.size() );
}


} // namespace Db

} // namespace Pt
