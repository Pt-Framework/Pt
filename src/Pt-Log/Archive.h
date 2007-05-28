/***************************************************************************
 *   Copyright (C) 2005-2007 by Dr. Marc Boris Drner                       *
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
#ifndef Pt_Archive_h
#define Pt_Archive_h

#include <Pt/Api.h>
#include <Pt/String.h>
#include <Pt/Reflectable.h>


namespace Pt {

class Archive
{
    public:
        Archive()
        {}

        virtual ~Archive()
        {}

        virtual const Pt::String* value(const Pt::String& name) const = 0;

        virtual void addValue(const Pt::String& name, const Pt::String& value) = 0;

        virtual const Archive* subArchive(const Pt::String& name) const = 0;

        virtual Archive& addArchive(const Pt::String& name) = 0;
};


class ArchiveReader
{
    public:
        ArchiveReader()
        {}

        virtual ~ArchiveReader()
        {}

        template <typename T>
        bool extract(T& type, const Pt::String& typeName)
        {
            const Archive* archive = this->_extract(typeName);
            if(archive == 0)
                return false;

            return *archive >> type;
        }

    protected:
        virtual const Archive* _extract(const Pt::String& typeName) = 0;
};


inline bool operator>>(const Archive& archive, Reflectable& r)
{
    PropertyMap& pmap = r.properties();
    for(PropertyMap::iterator it = pmap.begin(); it != pmap.end(); ++it)
    {
        Pt::String name;
        const std::string& pname = it->first;
        for(size_t n = 0; n < pname.size(); ++n)
            name += Pt::Char( pname[n] );

        std::string typeName = it->second->value().typeName();

        const Pt::String* value = archive.value(name);
        if(value)
        {
            Pt::StringStream ss(*value);
            Pt::Any a = Pt::Any::create(typeName, ss);
            it->second->setValue(a);
        }

        const Archive* subarchive = archive.subArchive(name);
        if(subarchive)
        {
            //Pt::Any::create(typeName, *subnode);
        }
    }

    return true;
}

}

#endif

