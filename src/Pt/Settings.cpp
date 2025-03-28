/*
 * Copyright (C) 2005-2014 by Dr. Marc Boris Duerner
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

#include <Pt/Settings.h>
#include <Pt/Composer.h>
#include "SettingsReader.h"
#include "SettingsWriter.h"

namespace Pt {

SettingsError::SettingsError(const char* what, std::size_t line)
: SerializationError(what)
, _line(line)
{}


Settings::Settings()
{}


void Settings::clear()
{
    SerializationInfo::clear();
}


bool Settings::isEmpty() const
{
    return SerializationInfo::isVoid();
}


void Settings::setName(const char* name)
{
    SerializationInfo::setName(name);
}


void Settings::load(std::basic_istream<Pt::Char>& is)
{
    SettingsReader reader(is);
    reader.parse(*this);
}


void Settings::load(Pt::Formatter& formatter)
{
    SerializationInfo& si = *this;

    BasicComposer<Pt::SerializationInfo> composer;
    composer.begin(si);
    
    formatter.beginParse(composer);
    formatter.parse();
}


void Settings::save(std::basic_ostream<Pt::Char>& os) const
{
    SettingsFormatter formatter(os);
    
    const SerializationInfo& si = *this;
    SerializationInfo::ConstIterator it;
    for(it = si.begin(); it != si.end(); ++it)
    {
        it->format(formatter);
    }
}


void Settings::save(Pt::Formatter& formatter) const
{
    const SerializationInfo& si = *this;
    si.format(formatter);
}

} // namespace Pt
