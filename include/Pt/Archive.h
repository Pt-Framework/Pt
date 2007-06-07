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


namespace Pt {

class Archive
{
    public:
        Archive()
        {}

        virtual ~Archive()
        {}

        const Pt::String* value(const Pt::String& name) const
        { return this->_value(name); }

        void addValue(const Pt::String& name, const Pt::String& value)
        { this->_addValue(name, value); }

        const Archive* findArchive(const Pt::String& name) const
        { return this->_findArchive(name); }

        Archive& addArchive(const Pt::String& name)
        { return this->_addArchive(name); }

        template <typename T>
        bool extract(T& type, const Pt::String& typeName)
        {
            const Archive* archive = this->findArchive(typeName);
            if(archive == 0)
                return false;

            return *archive >> type;
        }

    protected:
        virtual const Pt::String* _value(const Pt::String& name) const = 0;

        virtual void _addValue(const Pt::String& name, const Pt::String& value) = 0;

        virtual const Archive* _findArchive(const Pt::String& name) const = 0;

        virtual Archive& _addArchive(const Pt::String& name) = 0;
};

}

#endif
