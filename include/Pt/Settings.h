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
#ifndef Pt_Settings_h
#define Pt_Settings_h

#include <Pt/Api.h>
#include <Pt/String.h>
#include <Pt/SerializationInfo.h>
#include <iostream>
#include <sstream>
#include <cassert>

namespace Pt {

class PT_API SettingsError : public SerializationError
{
    public:
        SettingsError(const std::string& what, unsigned line, const SourceInfo& si);

        SettingsError(const std::string& what, unsigned line);

        //! @brief Destructor
        ~SettingsError() throw()
        {}

        unsigned line() const
        { return _line; }

    private:
        unsigned _line;
};


class PT_API Settings : public SerializationInfo
{
    public:
        Settings();

        void load( std::basic_istream<Pt::Char>& is );

        void save( std::basic_ostream<Pt::Char>& os ) const;

        // TODO getSerializable
        template <typename T>
        const bool getObject(T& type, const std::string& name) const
        {
            const SerializationInfo* si = this->findMember(name);
            if(si == 0)
                return false;

            *si >>= type;
            return true;
        }

        template <typename T>
        const void setObject(const T& type, const std::string& name)
        {
            SerializationInfo& si = this->addMember(name);
            si <<= type;
        }
};

}

#endif
