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
        if( it->toEntry() )
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
            this->writeEntry( entry->name(), entry->value().str() );
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
        this->writeEntry2( entry->name(), entry->value().str() );
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




class SettingsReader::ParseContext
{
    public:
        ParseContext(Pt::SerializationData& data)
        : _data( &data )
        , _line(1)
        , _depth(0)
        , _hasName(false)
        {}

        void reset()
        {
            if( _section.empty() )
                _name.clear();
            else
                _name = _section + Pt::Char(L'.');

            _value.clear();
        }

        unsigned line() const
        { return _line; }

        void endl()
        { ++_line; }

        Pt::String& section()
        { return _section; }

        Pt::String& name()
        { return _name; }

        Pt::String& value()
        { return _value; }

        Pt::String& type()
        { return _type; }

        unsigned depth() const
        {
            return _depth;
        }

        void enterNode()
        {
            //std::cerr << "entered: " << "(" << _type.narrow() << ")" << _name.narrow() << std::endl;

            Pt::SerializationData* data = _data->getData( _name );
            if(data == 0)
                data = &( _data->addData( _name ) );

            _data = data;

            _type.clear();
            _name.clear();
            ++_depth;
        }

        void enter()
        {
            if( _nodeName.empty() == false)
                this->pushNode();

            _nodeName = _name;
            _nodeValue = _value;
            _hasName = true;

            _name.clear();
            _value.clear();
        }

        void pushNode()
        {
            //std::cerr << "pushed: " << _nodeName.narrow() << std::endl;
            Pt::SerializationData* data = _data->getData( _nodeName );
            if(data == 0)
                data = &( _data->addData( _nodeName ) );

            _data = data;

            ++_depth;
        }

        void pushValue()
        {
            _value = _name;
            _name = _nodeName;
            _type = _nodeValue;
            _nodeName.clear();
            _hasName = false;
            _nodeValue.clear();
        }

        void leaveNode()
        {
            //std::cerr << "left: " << _data->name().narrow() << std::endl;

            assert(_depth > 0);
            _data = _data->parent();
            --_depth;
        }

        void addValue()
        {
            if( _hasName)
                this->pushNode();

            _nodeName.clear();
            _hasName = false;

            //std::cerr << "value: " << "(" << _type.narrow() << ")" << _name.narrow() << ":" << _value << std::endl;
            size_t pos  = _name.rfind( Pt::Char(L'.') );

            if(pos != Pt::String::npos)
            {
                Pt::SerializationData* data = _data->getData( _name.substr( 0, pos ) );
                if(data == 0)
                    data = &( _data->addData( _name.substr( 0, pos ) ) );

                data->addEntry( _name.substr( ++pos ), _value );
            }
            else
            {
                _data->addEntry(_name, _value);
            }

            _type.clear();
            _value.clear();
            _name.clear();
        }

    private:
        Pt::SerializationData* _data;
        unsigned _line;
        unsigned _depth;
        Pt::String _name;
        Pt::String _type;
        Pt::String _value;
        Pt::String _section;
        Pt::String _nodeName;
        bool _hasName;
        Pt::String _nodeValue;
};


void SettingsReader::_read(SerializationData& data)
{
    ParseContext context(data);
    _parse = &SettingsReader::beginStatement;

    Pt::String comment;
    Pt::Char ch;
    while( _is->get(ch) )
    {
        char xx = ch.narrow('*');
        if( ch == Pt::Char(L'#') )
        {
            getline( *_is, comment, Pt::Char(L'\n') );
            ch = Pt::Char(L'\n');
        }

        if( ch == Pt::Char(L'\n') )
        {
            context.endl();
        }

        (this->*_parse)(ch, context);
    }

    (this->*_parse)( eof, context );

    assert( context.depth() == 0 );
}

void SettingsReader::beginStatement(const Pt::Char& ch, ParseContext& context)
{
    //if( ch == eof || Pt::Unicode::isSpace(ch) )
    //    return;

    switch( ch.value() )
    {
        case '\n':
        case '\r':
        case '\t':
        case ' ':
        case Pt::uint32_t(-1):
            break;

        case '[':
            _parse = &SettingsReader::beginSection;
            context.section().clear();
            break;

        case ')':
            context.leaveNode();

            if( context.depth() == 0 )
                _parse = &SettingsReader::beginStatement;
            else
                _parse = &SettingsReader::afterValue;

            break;

        case '(':
        case '=':
            throw ParseError( "Unexpected token " + ch.narrow(' '), context.line() );

        default:
            context.name() += ch;
            _parse = &SettingsReader::parseName;
    }
}


void SettingsReader::parseName(const Pt::Char& ch, ParseContext& context)
{
    if( ch == eof )
        throw ParseError("Expected \'=\' token", context.line());

    if( Pt::Unicode::isSpace(ch) )
    {
        _parse = &SettingsReader::beginEqual;
        return;
    }

    switch( ch.value() )
    {
        case '=':
            _parse = &SettingsReader::finishEqual;
            break;

        case ')':
            context.pushValue();
            context.addValue();
            _parse = &SettingsReader::afterValue;
            break;

        default:
            context.name() += ch;
    }
}


void SettingsReader::beginEqual(const Pt::Char& ch, ParseContext& context)
{
    if( ch == eof )
        throw ParseError("Expected \'=\' token", context.line());

    if( Pt::Unicode::isSpace(ch) )
        return;

    switch( ch.value() )
    {
        case '=':
            _parse = &SettingsReader::finishEqual;
            break;

        default:
            throw ParseError("Expected \'=\' token", context.line());
    }
}


void SettingsReader::finishEqual(const Pt::Char& ch, ParseContext& context)
{
    if( ch == eof )
        throw ParseError("Expected token after \'=\'", context.line());

    if( Pt::Unicode::isSpace(ch) || ch == Pt::Char(L'\n') )
        return;

    switch( ch.value() )
    {
        case '=':
            throw ParseError("Expected token before \'=\'", context.line());

        case '"':
            _parse = &SettingsReader::parseQuotedValue;
            break;

        case '{':
            context.enterNode();
            _parse = &SettingsReader::parseArray;
            break;

        case '(':
            context.enter();
            _parse = &SettingsReader::beginStatement;
            break;

        default:
            context.value() += ch;
            _parse = &SettingsReader::parseValue;
    }
}

void SettingsReader::parseValue(const Pt::Char& ch, ParseContext& context)
{
    if( ch == Pt::Char(L'=') )
        throw ParseError("Invalid token after \'=\'", context.line());

    if( ch == eof  )
    {
        context.addValue();
        if(context.depth() == 0)
            _parse = &SettingsReader::beginStatement;
        else
            throw ParseError( "Expected token before EOF", context.line() );

        return;
    }

    if( Pt::Unicode::isSpace(ch) || ch == Pt::Char(L'\n') )
    {
        _parse = &SettingsReader::finishValue;///
        return;
    }

    switch( ch.value() )
    {
        case ',':
            context.addValue();
            _parse = &SettingsReader::beginStatement;
            break;

        case ')':
            context.addValue();
            context.leaveNode();

            if( context.depth() == 0 )
                _parse = &SettingsReader::beginStatement;
            else
                _parse = &SettingsReader::afterValue;

            break;

        case '(':
            context.enter();
            _parse = &SettingsReader::beginStatement;
            break;

        default:
            context.value() += ch;
    }
}

void SettingsReader::finishValue(const Pt::Char& ch, ParseContext& context)
{
    if( ch == eof  )
    {
        context.addValue();
        if(context.depth() == 0)
            _parse = &SettingsReader::beginStatement;
        else
            throw ParseError( "Expected token before EOF", context.line() );

        return;
    }

    if( Pt::Unicode::isSpace(ch) || ch == Pt::Char(L'\n') )
        return;

    switch( ch.value() )
    {
        case '(':
            context.enter();
            _parse = &SettingsReader::beginStatement;
            break;

        case ',':
            context.addValue();
            _parse = &SettingsReader::beginStatement;
            break;

        case ')':
            context.addValue();
            context.leaveNode();

            if( context.depth() == 0 )
                _parse = &SettingsReader::beginStatement;
            else
                _parse = &SettingsReader::afterValue;

            break;

        default:
            if(context.depth() == 0)
            {
                context.addValue();
                context.reset();
                context.name() += ch;
                _parse = &SettingsReader::parseName;
                return;
            }

            throw ParseError( "Invalid token", context.line() );
    }
}

void SettingsReader::afterValue(const Pt::Char& ch, ParseContext& context)
{
    if( ch == eof )
    {
        if( context.depth() )
            throw ParseError( "Expected token before EOF", context.line() );

        _parse = &SettingsReader::beginStatement;
        return;
    }

    if( Pt::Unicode::isSpace(ch) || ch == Pt::Char(L'\n') )
        return;

    switch( ch.value() )
    {
        case ',':
            _parse = &SettingsReader::beginStatement;
            break;

        case ')':
            context.leaveNode();

            if( context.depth() == 0 )
                _parse = &SettingsReader::beginStatement;
            else
                _parse = &SettingsReader::endStatement;

            break;
    }
}

void SettingsReader::parseQuotedValue(const Pt::Char& ch, ParseContext& context)
{
    if( ch == eof || ch == Pt::Char(L'\n') || ch == Pt::Char(L'\r') )
        throw ParseError("Expected closing \" token", context.line());

    switch( ch.value() )
    {
        case '\\':
            this->getEscaped(context);
            break;

        case '"':
            _parse = &SettingsReader::finishQuotedValue;
            break;

        default:
            context.value() += ch;
    }
}

void SettingsReader::finishQuotedValue(const Pt::Char& ch, ParseContext& context)
{
    if( ch == eof )
    {
        if(context.depth() > 0)
            throw ParseError( "Expected token before EOF", context.line() );

        context.addValue();
        return;
    }

    if(Pt::Unicode::isSpace(ch) || ch == Pt::Char(L'\n')  )
    {
        return;
    }

    switch( ch.value() )
    {
        case '"':
            _parse = &SettingsReader::parseQuotedValue;
            break;

        case ',':
            context.addValue();
            _parse = &SettingsReader::beginStatement;
            break;

        case ')':
            context.addValue();
            context.leaveNode();

            if( context.depth() == 0 )
                _parse = &SettingsReader::beginStatement;
            else
                _parse = &SettingsReader::endStatement;

            break;

        default:
            if( context.depth() == 0 )
            {
                context.addValue();
                context.reset();
                context.name() += ch;
                _parse = &SettingsReader::parseName;
                return;
            }
            throw ParseError( "Expected closing \')\'", context.line() );
    }
}

void SettingsReader::endStatement(const Pt::Char& ch, ParseContext& context)
{
    if( ch == eof )
    {
        if(context.depth() > 0)
            throw ParseError( "Expected token before EOF", context.line() );

        return;
    }

    if(Pt::Unicode::isSpace(ch) || ch == Pt::Char(L'\n')  )
    {
        return;
    }

    switch( ch.value() )
    {
        case ',':
            _parse = &SettingsReader::beginStatement;
            break;

        case ')':
            context.leaveNode();

            if( context.depth() == 0 )
                _parse = &SettingsReader::beginStatement;

            break;

        default:
            throw ParseError( "Unexpected token", context.line() );
    }
}

void SettingsReader::parseArray(const Pt::Char& ch, ParseContext& context)
{
    if( ch == eof )
    {
        throw ParseError("Incomplete array", context.line());
    }

    if(Pt::Unicode::isSpace(ch) || ch == Pt::Char(L'\n')  )
        return;

    switch( ch.value() )
    {
        case ',':
            throw ParseError("Incomplete array", context.line());

        case '"':
            _parse = &SettingsReader::parseQuotedArrayValue;
            break;

        case '}':
            _parse = &SettingsReader::beginStatement;
            break;

        default:
            context.value() += ch;
            _parse = &SettingsReader::parseArrayValue;
    }
}

void SettingsReader::parseArrayValue(const Pt::Char& ch, ParseContext& context)
{
    if( ch == eof )
        throw ParseError( "Incomplete array", context.line() );


    if( Pt::Unicode::isSpace(ch) || ch == Pt::Char(L'\n') )
    {
        _parse = &SettingsReader::finishArrayValue;
        return;
    }

    switch( ch.value() )
    {
        case ',':
            context.addValue();
            _parse = &SettingsReader::parseArray;
            break;

        case '}':
            context.addValue();
            context.leaveNode();
            _parse = &SettingsReader::beginStatement;
            break;

        default:
            context.value() += ch;
    }
}

void SettingsReader::finishArrayValue(const Pt::Char& ch, ParseContext& context)
{
    if( ch == eof )
        throw ParseError( "Incomplete array", context.line() );

    if( Pt::Unicode::isSpace(ch) || ch == Pt::Char(L'\n') )
        return;

    switch( ch.value() )
    {
        case ',':
            context.addValue();
            _parse = &SettingsReader::parseArray;
            break;

        case '}':
            context.addValue();
            context.leaveNode();
            _parse = &SettingsReader::beginStatement;
            break;
    }
}

void SettingsReader::parseQuotedArrayValue(const Pt::Char& ch, ParseContext& context)
{
    if( ch == eof )
        throw ParseError( "Reached EOF in array element", context.line() );

    switch( ch.value() )
    {
        case '"' :
            _parse = &SettingsReader::finishQuotedArrayValue;
            break;

        case '\\' :
            this->getEscaped(context);
            break;

        default:
            context.value() += ch;
    }
}

void SettingsReader::finishQuotedArrayValue(const Pt::Char& ch, ParseContext& context)
{
    if( ch == eof )
        throw ParseError( "Incomplete array", context.line() );

    if( Pt::Unicode::isSpace(ch) || ch == Pt::Char(L'\n') )
        return;

    if( ch == Pt::Char(L'"') )
    {
        _parse = &SettingsReader::parseQuotedArrayValue;
        return;
    }

    if( ch == Pt::Char(L',') )
    {
        context.addValue();
        _parse = &SettingsReader::parseArray;
        return;
    }

    if( ch == Pt::Char(L'}') )
    {
        context.addValue();
        context.leaveNode();
        _parse = &SettingsReader::beginStatement;
        return;
    }

    throw ParseError( "Unrecognized token in array", context.line() );
}

void SettingsReader::beginSection(const Pt::Char& ch, ParseContext& context)
{
    if( ch == eof )
        throw ParseError("Section not closed", context.line());

    if( Pt::Unicode::isSpace(ch) || ch == Pt::Char(L'\n') )
        return;

    context.section() += ch;
    _parse = &SettingsReader::parseSection;
}

void SettingsReader::parseSection(const Pt::Char& ch, ParseContext& context)
{
    if( ch == eof )
        throw ParseError("Section not closed", context.line());

    if( Pt::Unicode::isSpace(ch) || ch == Pt::Char(L'\n') )
    {
        _parse = &SettingsReader::finishSection;
        return;
    }

    if( ch == Pt::Char(L']') )
    {
        context.reset();
        _parse = &SettingsReader::beginStatement;
        return;
    }

    context.section() += ch;
}

void SettingsReader::finishSection(const Pt::Char& ch, ParseContext& context)
{
    if( ch == eof )
        throw ParseError("Section not closed", context.line());

    if( Pt::Unicode::isSpace(ch) || ch == Pt::Char(L'\n') )
        return;

    if( ch == Pt::Char(L']') )
    {
        context.reset();
        _parse = &SettingsReader::beginStatement;
        return;
    }

    throw ParseError("Invalid section name", context.line());
}

void SettingsReader::getEscaped(ParseContext& context)
{
    Pt::Char ch;
    if( ! _is->get(ch) )
        throw ParseError("Reached EOF within qoute", context.line() );

    switch( ch.value() )
    {
        case 'n':
            context.value() += Pt::Char(L'\n');
            break;

        case 'r':
            context.value() += Pt::Char(L'\r');
            break;

        default:
            throw ParseError("Unknown escaped character", context.line() );
    }
}

}
