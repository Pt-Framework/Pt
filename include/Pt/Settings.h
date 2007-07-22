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
#ifndef Pt_Settings_h
#define Pt_Settings_h

#include <Pt/Api.h>
#include <Pt/SerializationData.h>
#include <Pt/String.h>
#include <Pt/Exception.h>
#include <Pt/Unicode.h>

#include <iostream>
#include <sstream>
#include <cassert>


namespace Pt {

class PT_API Settings : public SerializationData
{
    public:
        Settings()
        {}

        void load(std::basic_istream<Pt::Char>& is);

        void save(std::basic_ostream<Pt::Char>& is) const;

        template <typename T>
        const void get(T& type, const Pt::String& name) const
        {
            const SerializationData* data = this->getData(name);
            if(data == 0)
                return;

            *data >> type;
        }
};


class PT_API SettingsWriter
{
    public:
        SettingsWriter(std::basic_ostream<Pt::Char>& os)
        : _os(&os)
        { }

        ~SettingsWriter()
        {}

        void write(const SerializationData& sd);

    protected:
        void writeParent(const SerializationData& sd);

        void writeChild(const SerializationNode& node);

        void writeEntry(const Pt::String& name, const Pt::String& value);

        void writeEntry2(const Pt::String& name, const Pt::String& value);

        void writeSection(const Pt::String& prefix);

    private:
        std::basic_ostream<Pt::Char>* _os;
        typedef void (SettingsWriter::*Write)(const SerializationNode&);
        Write _write;
};


class PT_API SettingsReader
{
    class ParseContext;

    public:
        Pt::Char eof;

        class ParseError : public std::logic_error
        {
            public:
                ParseError(const std::string& what, unsigned line)
                : std::logic_error("Line " + ParseError::str(line) + ":" + what)
                {}

                static std::string str(unsigned n)
                {
                    std::stringstream ss;
                    ss << n;
                    return ss.str();
                }
        };

    public:
        SettingsReader(std::basic_istream<Pt::Char>& is)
        : _is(&is)
        {
            typedef std::char_traits<Pt::Char> Traits;
            eof = Traits::to_char_type( Traits::eof() );
        }

        ~SettingsReader()
        {}

        void attach(std::basic_istream<Pt::Char>& is)
        {
            _is = &is;
        }

        //! @brief Reads content into an archive
        void read(SerializationData& data)
        {
            this->_read(data);
        }

        //! @brief Reads content into an archive
        void read(Settings& settings)
        {
            this->_read( settings );
        }

    protected:
        void _read(SerializationData& data);

        void beginStatement(const Pt::Char& ch, ParseContext& context);

        void parseName(const Pt::Char& ch, ParseContext& context);

        void beginEqual(const Pt::Char& ch, ParseContext& context);

        void finishEqual(const Pt::Char& ch, ParseContext& context);

        void parseValue(const Pt::Char& ch, ParseContext& context);

        void finishValue(const Pt::Char& ch, ParseContext& context);

        void afterValue(const Pt::Char& ch, ParseContext& context);

        void parseQuotedValue(const Pt::Char& ch, ParseContext& context);

        void finishQuotedValue(const Pt::Char& ch, ParseContext& context);

        void endStatement(const Pt::Char& ch, ParseContext& context);

        void parseArray(const Pt::Char& ch, ParseContext& context);

        void parseArrayValue(const Pt::Char& ch, ParseContext& context);

        void finishArrayValue(const Pt::Char& ch, ParseContext& context);

        void parseQuotedArrayValue(const Pt::Char& ch, ParseContext& context);

        void finishQuotedArrayValue(const Pt::Char& ch, ParseContext& context);

        void beginSection(const Pt::Char& ch, ParseContext& context);

        void parseSection(const Pt::Char& ch, ParseContext& context);

        void finishSection(const Pt::Char& ch, ParseContext& context);

        void getEscaped(ParseContext& context);

    private:
        std::basic_istream<Pt::Char>* _is;
        typedef void (SettingsReader::*Parse)(const Pt::Char&, ParseContext&);
        Parse _parse;
};


}

#endif

