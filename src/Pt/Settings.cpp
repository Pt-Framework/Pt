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

namespace Pt {

void Settings::load(std::basic_istream<Pt::Char>& is)
{
    SettingsReader reader(is);
    reader.read(*this);
}


void Settings::save(std::basic_ostream<Pt::Char>& is) const
{
    SettingsWriter writer(is);
    writer.write(*this);
}


void SettingsWriter::write(const SerializationData& sd)
{
    SerializationData::ConstNodeIterator it;
    for(it = sd.begin(); it != sd.end(); ++it)
    {
        if(const SerializationEntry* entry = it->toEntry() )
        {
            (this->*_write)(*it);
        }
        else if(const SerializationData* subdata = it->toData() )
        {
            this->writeSection( subdata->name() );
            this->writeParent(*subdata);
        }
    }
}


void SettingsWriter::writeParent(const SerializationData& sd)
{
    SerializationData::ConstNodeIterator it;
    for(it = sd.begin(); it != sd.end(); ++it)
    {
        if(const SerializationEntry* entry = it->toEntry() )
        {
            this->writeEntry( entry->name(), Pt::String::widen( entry->value().str() ) );
        }
        else if(const SerializationData* subdata = it->toData() )
        {
            SerializationData::ConstNodeIterator it;
            for(it = subdata->begin(); it != subdata->end(); ++it)
            {
                *_os << subdata->name() << Pt::String(L" = ( ");
                this->writeChild(*it);
                *_os << Pt::String(L" ) ") << std::endl;
            }
        }
    }
}


void SettingsWriter::writeChild(const SerializationNode& node)
{
    if(const SerializationEntry* entry = node.toEntry() )
    {
        this->writeEntry2( entry->name(), Pt::String::widen( entry->value().str() ) );
    }
    else if(const SerializationData* subdata = node.toData() )
    {
        SerializationData::ConstNodeIterator it;
        for(it = subdata->begin(); it != subdata->end(); ++it)
        {
            *_os << subdata->name() << Pt::String(L" = ( ");
            this->writeChild(*it);
            *_os << Pt::String(L" ) ") << std::endl;
        }
    }
}


void SettingsWriter::writeEntry(const Pt::String& name, const Pt::String& value)
{
    *_os << name << Pt::String(L" = \"") << value << Pt::String(L"\"") << std::endl;
}


void SettingsWriter::writeEntry2(const Pt::String& name, const Pt::String& value)
{
    *_os << name << Pt::String(L" = \"") << value << Pt::String(L"\"");
}


void SettingsWriter::writeSection(const Pt::String& prefix)
{
    *_os << Pt::String(L"[") << prefix << Pt::String(L"]") << std::endl;
}

}
