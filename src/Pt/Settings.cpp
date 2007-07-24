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
        if( const SerializationEntry* entry = it->toEntry() )
        {
            this->writeEntry( entry->name(), entry->value().str(), entry->typeName() );
            *_os << std::endl;
        }
        else if(const SerializationData* subdata = it->toData() )
        {
            this->writeSection( subdata->name() );
            this->writeParent( *subdata );
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
            this->writeEntry( entry->name(), entry->value().str(), entry->typeName() );
            *_os << std::endl;
        }
        else if(const SerializationData* subdata = it->toData() )
        {
            *_os << subdata->name() << Pt::String(L" = ") << subdata->typeName() << Pt::String(L"( ");
            this->writeChild(*subdata);     
            *_os << Pt::String(L" )") << std::endl;
        }
    }
}


void SettingsWriter::writeChild(const SerializationData& sd)
{
    bool separate = false;
    SerializationData::ConstNodeIterator it;
    for(it = sd.begin(); it != sd.end(); ++it)
    {
        if(separate)
            *_os << Pt::String(L", ");
        
        if(const SerializationEntry* entry = it->toEntry() )
        {
            this->writeEntry( entry->name(), entry->value().str(), entry->typeName() );
        }
        else if(const SerializationData* subdata = it->toData() )
        {
           
            *_os << subdata->name() << Pt::String(L" = ") << subdata->typeName() << Pt::String(L"( ");
            this->writeChild(*subdata);     
            *_os << Pt::String(L" )");
        }
        
        separate = true;
    }
}


void SettingsWriter::writeEntry(const Pt::String& name, const Pt::String& value, const Pt::String& type)
{
    if( type.empty() )
    {
        *_os << name << Pt::String(L"=\"") << value << Pt::String(L"\"");
        return;
    }
    
    *_os << name << Pt::String(L" = ") << type<< Pt::String(L"(\"") << value << Pt::String(L"\")");
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
        , _hasPrev(false)
        , _isDotted(false)
        {}

        void reset()
        {
            if( _section.empty() )
                _name.clear();
            else
                _name = _section + Pt::Char(L'.');

            _value.clear();
            _type.clear();
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

        void enter()
        {
            _prevName = _name;
            _prevValue = _value;
            _prevType = _value;
            _hasPrev = true;

            _name.clear();
            _value.clear();
            _type.clear();
        }

        void popValue()
        {
            _value = _name;
            _name = _prevName;
            _type = _prevValue;
            
            _prevName.clear();
            _prevValue.clear();
            _prevType.clear();
            _hasPrev = false;
            
            this->addValue();
        }

        void leave()
        {
            //std::cerr << "left: " << _data->name().narrow() << std::endl;

            assert(_depth > 0);

            _data = _data->parent();
            --_depth;
            
            if(_depth == 1 && _isDotted)
            {
                _data = _data->parent();
                _isDotted = false;
                --_depth;
            }
        }

        void addValue()
        {
            this->popNode();

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

        void popNode()
        {
            if(_hasPrev)
                this->pushNode();

            _prevName.clear();
            _prevValue.clear();
            _prevType.clear();
            _hasPrev = false;
        }

    protected:
        void pushNode()
        {
            //std::cerr << "pushed: " << "(" << _prevType.narrow() << ") " << _prevName.narrow() << std::endl;
            
            size_t pos  = _prevName.rfind( Pt::Char(L'.') );
            
            if(pos != Pt::String::npos)
            { 
                Pt::SerializationData* data = _data->getData( _prevName.substr( 0, pos ) );
                if(data == 0)
                    data = &( _data->addData( _prevName.substr( 0, pos ) ) );
                    
                _data = data;
                ++_depth;
                _isDotted = true;
                _prevName = _prevName.substr( ++pos );
            }

            Pt::SerializationData* data = _data->getData( _prevName );
            if(data == 0)
                data = &( _data->addData( _prevName ) );

            _data = data;
            _data->setTypeName(_prevType);
            ++_depth;
        }

    private:
        Pt::SerializationData* _data;
        unsigned _line;
        unsigned _depth;
        Pt::String _section;
        Pt::String _name;
        Pt::String _type;
        Pt::String _value;
        Pt::String _prevName;
        Pt::String _prevValue;
        Pt::String _prevType;
        bool _hasPrev;
        bool _isDotted;
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
            if(context.depth() == 0)
                throw ParseError( "Invalid closing ')'", context.line() );
            
            context.leave();

            if( context.depth() == 0 )
                _parse = &SettingsReader::beginStatement;
            else
                _parse = &SettingsReader::afterValue;

            break;

        case '(':
        case '=':
            throw ParseError( "Unexpected token " + ch.narrow(' '), context.line() );

        case '"':
            _parse = &SettingsReader::parseQoutedName;
            break;

        default:
            context.name() += ch;
            _parse = &SettingsReader::parseName;
    }
}


void SettingsReader::parseName(const Pt::Char& ch, ParseContext& context)
{
    switch( ch.value() )
    {
        case Pt::uint32_t(-1):
            throw ParseError("Expected \'=\' token", context.line());
        
        case '\n':
        case '\r':
        case '\t':
        case ' ':
            _parse = &SettingsReader::afterName;
            break;

        case '=':
            _parse = &SettingsReader::onEqual;
            break;

        case ')':
            context.popValue();
            _parse = &SettingsReader::afterValue;
            break;

        default:
            context.name() += ch;
    }
}


void SettingsReader::parseQoutedName(const Pt::Char& ch, ParseContext& context)
{
    switch( ch.value() )
    {
        case Pt::uint32_t(-1):
            throw ParseError("Expected \')\' token", context.line());

        case '"':
            _parse = &SettingsReader::afterName;
            break;

        default:
            context.name() += ch;
    }
}


void SettingsReader::afterName(const Pt::Char& ch, ParseContext& context)
{
    switch( ch.value() )
    {
        case Pt::uint32_t(-1):
            throw ParseError("Expected \'=\' token", context.line());

        case '\n':
        case '\r':
        case '\t':
        case ' ':
            break;

        case '=':
            _parse = &SettingsReader::onEqual;
            break;

        case ')':
            context.popValue();
            _parse = &SettingsReader::afterValue;
            break;

        default:
            throw ParseError("Expected \'=\' token", context.line());
    }
}


void SettingsReader::onEqual(const Pt::Char& ch, ParseContext& context)
{
    switch( ch.value() )
    {
        case Pt::uint32_t(-1):
            throw ParseError("Expected \'=\' token", context.line());

        case '\n':
        case '\r':
        case '\t':
        case ' ':
            break;
    
        case '=':
            throw ParseError("Expected token before \'=\'", context.line());

        case '"':
            _parse = &SettingsReader::parseQuotedValue;
            break;

        case '{':
            context.popNode();
            context.enter();
            _parse = &SettingsReader::parseArray;
            break;

        case '(':
            context.popNode();
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
        _parse = &SettingsReader::finishValue;
        return;
    }

    switch( ch.value() )
    {
        case ',':
            context.addValue();
            _parse = &SettingsReader::beginStatement;
            break;

        case ')':
            context.popNode();
            context.addValue();
            context.leave();

            if( context.depth() == 0 )
                _parse = &SettingsReader::beginStatement;
            else
                _parse = &SettingsReader::afterValue;

            break;

        case '(':
            context.popNode();
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
            context.popNode();
            context.enter();
            _parse = &SettingsReader::beginStatement;
            break;

        case ',':
            context.addValue();
            _parse = &SettingsReader::beginStatement;
            break;

        case ')':
            context.popNode();
            context.addValue();
            context.leave();

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
            context.leave();

            if( context.depth() == 0 )
                _parse = &SettingsReader::beginStatement;
            else
                _parse = &SettingsReader::endStatement;

            break;
            
        default:
            if(context.depth() == 0)
            {
                context.reset();
                context.name() += ch;
                _parse = &SettingsReader::parseName;
                return;
            }

            throw ParseError( "Invalid token", context.line() );
    }
}

void SettingsReader::parseQuotedValue(const Pt::Char& ch, ParseContext& context)
{
    if( ch == eof || ch == Pt::Char(L'\n') || ch == Pt::Char(L'\r') )
        throw ParseError("Expected closing \" token", context.line());

    switch( ch.value() )
    {
        case '\\':
        {
            bool success = this->getEscaped( context.value() );
            if(!success)
                throw ParseError("Invalid escaped character", context.line() );
            break;
        }
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
            context.leave();

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
            context.leave();

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
            context.leave();
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
            context.leave();
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
        {
            bool success = this->getEscaped( context.value() );
            if(!success)
                throw ParseError("Invalid escaped character", context.line() );
                
            break;
        }

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
        context.leave();
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

bool SettingsReader::getEscaped(Pt::String& s)
{
    Pt::Char ch;
    if( ! _is->get(ch) )
        return false;

    switch( ch.value() )
    {
        case 'n':
            s += Pt::Char(L'\n');
            break;

        case 'r':
            s += Pt::Char(L'\r');
            break;

        default:
            return false;
    }
    
    return true;
}

}
