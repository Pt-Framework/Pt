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
#ifndef Pt_PropertiesWriter_h
#define Pt_PropertiesWriter_h

#include <Pt/Api.h>
#include <Pt/ArchiveWriter.h>
#include <Pt/String.h>
#include <iostream>


namespace Pt {

/*
class PropertiesWriter : public ArchiveWriter
{
    public:
        PropertiesWriter(std::basic_ostream<Pt::Char>& os)
        : _os(os)
        {}


        ~PropertiesWriter()
        {}

    protected:
        void _write(const Archive& archive)
        {
            Pt::String prefix = archive.name();
            this->writeArchive( archive, prefix );
        }

        void _commit()
        {
            _os.flush();
        }

        void writeValue(const ArchiveValue& value, const Pt::String& prefix)
        {
            static const Pt::String equal = L" = \"";
            static const Pt::Char qoute = Pt::Char(L'\"');
            static const Pt::Char dot = Pt::Char(L'.');

            if( ! prefix.empty() )
            {
                _os << prefix << dot;
            }

            _os << value.name() << equal << value.value() << qoute << std::endl;
        }

        void writeArchive(const Archive& archive, const Pt::String& prefix)
        {
            Pt::String archPrefix = prefix;
            if( ! prefix.empty() )
                archPrefix += Pt::Char(L'.');

            Archive::ConstIterator it;
            for(it = archive.begin(); it != archive.end(); ++it)
            {
                const ArchiveNode& node = *it;
                const Archive* subarchive = node.toArchive();
                if(subarchive)
                {
                    this->writeArchive( *subarchive, archPrefix + archive.name() );
                    continue;
                }

                const ArchiveValue* av = node.toValue();
                assert( av );

                this->writeValue( *av, archPrefix + archive.name() );
            }
        }

    private:
        std::basic_ostream<Pt::Char>& _os;
};
*/
}

#endif

