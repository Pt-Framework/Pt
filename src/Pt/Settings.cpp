/***************************************************************************
 *   Copyright (C) 2005-2007 by Dr. Marc Boris Duerner                     *
 *   Copyright (C) 2005 Stephan Beal                                       *
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

#include "Pt/Settings.h"
#include "SettingsReader.h"
#include "SettingsWriter.h"

namespace Pt {

SettingsError::SettingsError(const std::string& what, unsigned line, const SourceInfo& si)
: SerializationError(what, si)
, _line(line)
{}


SettingsError::SettingsError(const std::string& what, unsigned line)
: SerializationError(what, PT_SOURCEINFO)
, _line(line)
{}


Settings::Settings()
{}


void Settings::load(std::basic_istream<Pt::Char>& is)
{
    SettingsReader reader(is);
    reader.parse(*this);
}


void Settings::save(std::basic_ostream<Pt::Char>& os ) const
{
    SettingsWriter writer(os);
    writer.write(*this);
}

}
