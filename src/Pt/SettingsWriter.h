/***************************************************************************
 *   Copyright (C) 2005-2007 by Dr. Marc Boris Duerner                     *
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
#ifndef Pt_SettingsWriter_h
#define Pt_SettingsWriter_h

#include "Pt/Api.h"
#include "Pt/Char.h"
#include "Pt/SerializationInfo.h"
#include <iostream>

namespace Pt {

class SettingsWriter
{
    public:
        SettingsWriter( std::basic_ostream<Pt::Char>& os)
        : _os(&os)
        , _indent(0)
        { }

        ~SettingsWriter()
        {}

        void write(const SerializationInfo& si);

    protected:
        void writeParent(const SerializationInfo& sd, const std::string& prefix);

        void writeChild(const SerializationInfo& node);

        void writeEntry(const std::string& name, const Pt::String& value, const std::string& type);

        void writeSection(const Pt::String& prefix);

    private:
        std::basic_ostream<Pt::Char>* _os;
        size_t _indent;
};

}

#endif
