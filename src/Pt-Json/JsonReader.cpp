/*
   Copyright (C) 2015-2023 by Dr. Marc Boris Duerner
  
   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.
   
   As a special exception, you may use this file as part of a free
   software library without restriction. Specifically, if other files
   instantiate templates or use macros or inline functions from this
   file, or you compile this file and link it with other files to
   produce an executable, this file does not by itself cause the
   resulting executable to be covered by the GNU General Public
   License. This exception does not however invalidate any other
   reasons why the executable file might be covered by the GNU Library
   General Public License.
   
   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.
   
   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
   MA 02110-1301 USA
*/

#include <Pt/Json/JsonReader.h>
#include <Pt/Json/JsonError.h>
//#include <Pt/Json/StartDocument.h>
#include <Pt/Json/EndDocument.h>
#include <Pt/Json/Null.h>
#include <Pt/Json/Boolean.h>
#include <Pt/Json/String.h>
#include <Pt/Json/Integer.h>
#include <Pt/Json/Float.h>
#include <Pt/Json/StartObject.h>
#include <Pt/Json/Member.h>
#include <Pt/Json/EndObject.h>
#include <Pt/Json/StartArray.h>
#include <Pt/Json/EndArray.h>
#include <Pt/System/Logger.h>
#include <Pt/Convert.h>

#include <stack>
#include <vector>
#include <cassert>

PT_LOG_DEFINE("Pt.Json.JsonReader")

namespace Pt {

namespace Json {

class JsonReaderImpl
{
    typedef std::char_traits<Char>::int_type int_type;

    typedef void (JsonReaderImpl::*ParseFunc)(int_type);

    private:
        void onDocument(int_type c)
        {
            if( c == std::char_traits<Char>::eof() )
            {
                _current.push_back(&_endDoc);
                return;
            }

            Char ch = c;

            if( isSpace(ch) )
                return;

            pushParseState(&JsonReaderImpl::onDocumentEnd);
            onElement(c);
        }

        void onDocumentEnd(int_type c)
        {
            Char ch = c;

            if( c == std::char_traits<Char>::eof() )
            {
                _current.push_back(&_endDoc);
                return;
            }
            
            if( ! isSpace(ch) )
            {
                throw SyntaxError("JSON syntax error", line());
            }
        }

        void onObject(int_type c)
        {
            Char ch = notEof(c);

            if( isSpace(ch) )
                return;

            if(ch == '}')
            {
                _current.push_back(&_endObject);
                popParseState();
            }
            else if(ch == '"')
            {
                _member.name().clear();
                _parse = &JsonReaderImpl::onMemberName;
            }
            else
            {
                throw SyntaxError("JSON syntax error", line());
            }
        }

        void onMemberName(int_type c)
        {
            Char ch = notEof(c);

            if(ch == '"')
            {
                _current.push_back(&_member);
                _parse = &JsonReaderImpl::onMemberNameEnd;
                return;
            }

            _member.name() += ch;
        }

        void onMemberNameEnd(int_type c)
        {
            Char ch = c;

            if( isSpace(ch) )
                return;

            if(ch == ':')
            {
                _parse = &JsonReaderImpl::onMemberAssign;
            }
            else
            {
                throw SyntaxError("JSON syntax error", line());
            }
        }

        void onMemberAssign(int_type c)
        {
            Char ch = c;

            if( isSpace(ch) )
                return;

            pushParseState( &JsonReaderImpl::onMemberEnd);
            onElement(c);
        }

        void onMemberEnd(int_type c)
        {
            Char ch = c;

            if( isSpace(ch) )
                return;

            if(ch == '}')
            {
                _current.push_back(&_endObject);
                popParseState();
            }
            else if(ch == ',')
            {
                _parse = &JsonReaderImpl::onObject;
            }
            else
            {
                throw SyntaxError("JSON syntax error", line());
            }
        }

        void onArray(int_type c)
        {
            Char ch = notEof(c);

            if( isSpace(ch) )
                return;

            if(ch == ']')
            {
                _current.push_back(&_endArray);
                popParseState();
                return;
            }
            
            pushParseState(&JsonReaderImpl::onArrayElementEnd);
            onElement(c);
        }

        void onArrayElementEnd(int_type c)
        {
            Char ch = c;

            if( isSpace(ch) )
                return;

            if(ch == ']')
            {
                _current.push_back(&_endArray);
                popParseState();
            }
            else if(ch == ',')
            {
                _parse = &JsonReaderImpl::onArray;
            }
            else
            {
                throw SyntaxError("JSON syntax error", line());
            }
        }

        void onElement(int_type c)
        {
            Char ch = c;

            if( isSpace(ch) )
                return;

            if(ch == '{')
            {
                _current.push_back(&_startObject);
                _parse = &JsonReaderImpl::onObject;
            }
            else if(ch == '[')
            {
                _current.push_back(&_startArray);
                _parse = &JsonReaderImpl::onArray;
            }
            else if(ch == '"')
            {
                _string.clear();
                _parse = &JsonReaderImpl::onString;
            }
            else if(ch == 'n')
            {
                _token.clear();
                _parse = &JsonReaderImpl::onLiteral;
                (this->*_parse)(c);
            }
            else if(ch == 't')
            {
                _token.clear();
                _parse = &JsonReaderImpl::onLiteral;
                (this->*_parse)(c);
            }
            else if(ch == 'f')
            {
                _token.clear();
                _parse = &JsonReaderImpl::onLiteral;
                (this->*_parse)(c);
            }
            else if( isdigit(ch) || ch == '-' )
            {
                _token.clear();
                _parse = &JsonReaderImpl::onNumber;
                (this->*_parse)(c);
            }
            else
            {
                throw SyntaxError("JSON syntax error", line());
            }
        }

        void onLiteral(int_type c)
        {
            bool isEof = c == std::char_traits<Char>::eof();
            
            Char ch = c;
            
            if( isalpha(ch) && ! isEof )
            {
                _token += ch;
                return;
            }

            if(_token == "null")
            {
                _current.push_back(&_null);
            }
            else if(_token == "true")
            {
                _boolean.setValue(true);
                _current.push_back(&_boolean);
            }
            else if(_token == "false")
            {
                _boolean.setValue(false);
                _current.push_back(&_boolean);
            }
            else
            {
                throw SyntaxError("JSON syntax error", line());
            }
            
            popParseState();
            (this->*_parse)(c);
        }

        void onString(int_type c)
        {
            Char ch = notEof(c);

            if(ch == '"')
            {
                _current.push_back(&_string);
                popParseState();
                return;
            }

            _string.value() += ch;
        }

        void onNumber(int_type c)
        {
            bool isEof = c == std::char_traits<Char>::eof();
            
            Char ch = c;
            
            if( isNumber(ch) && ! isEof )
            {
                _token += ch;
                return;
            }

            if(ch == '.')
            {
                _token += ch;
                _parse = &JsonReaderImpl::onFraction;
                return;
            }

            int n = 0;
            Pt::parseInt(_token.begin(), _token.end(), n);

            _integer.setValue(n);
            _current.push_back(&_integer);
            
            popParseState();
            (this->*_parse)(c);
        }

        void onFraction(int_type c)
        {
            bool isEof = c == std::char_traits<Char>::eof();
            
            Char ch = c;
            
            if( isNumber(ch) && ! isEof )
            {
                _token += ch;
                return;
            }

            double n = 0;
            Pt::parseFloat(_token.begin(), _token.end(), n);
            
            _float.setValue(n);
            _current.push_back(&_float);
            
            popParseState();
            (this->*_parse)(c);
        }

    private:
        inline Char notEof(int_type c) const
        {
            if( c == std::char_traits<Char>::eof() )
            {
                throw SyntaxError("JSON syntax error", line());
            }

            return Char(c);
        }

        inline bool isSpace(Char ch) const
        {
            return ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n';
        }

        inline bool isNumber(Char ch) const
        {
            return isdigit(ch) || ch == '-' || ch == '+' || ch == 'e' || ch == 'E';
        }

        inline void pushParseState(ParseFunc parse)
        {
            // TODO: push state should add to _usedSize...
            _parseStack.push(parse);
        }

        inline void popParseState()
        {
            // TODO: push state should shrink _usedSize...
            if( _parseStack.empty() )
                throw SyntaxError("JSON syntax error", line());

            _parse = _parseStack.top();
            _parseStack.pop();
        }

    public:
        JsonReaderImpl()
        : _is(0)
        , _parse(0)
        , _line(1)
        , _usedSize(0)
        , _maxSize(2048)
        , _chunkSize(0) 
        , _maxChunkSize(2048)
        {
            _parse = &JsonReaderImpl::onDocument;
        }

        JsonReaderImpl(std::basic_istream<Pt::Char>& is)
        : _is(&is)
        , _parse(0)
        , _line(1)
        , _usedSize(0)
        , _maxSize(2048)
        , _chunkSize(0)
        , _maxChunkSize(2048)
        {
            _parse = &JsonReaderImpl::onDocument;
        }

        ~JsonReaderImpl()
        { }
        
        void setMaxSize(std::size_t n)
        {
            _maxSize = n;
        }

        std::size_t maxSize() const
        {
            return _maxSize;
        }

        std::size_t usedSize() const
        {
            return _usedSize;
        }

        void setChunkSize(std::size_t n)
        {
            _maxChunkSize = n;
        }

        void reset()
        {
            _is = 0;
           
            _line = 1;
            _usedSize = 0;
            _chunkSize = 0;

            _parse = &JsonReaderImpl::onDocument;

            while( ! _parseStack.empty() )
                _parseStack.pop();

            _token.clear(); 
            _current.clear();
            _member.clear();
            _string.clear();
        }

        std::basic_istream<Pt::Char>* input()
        {
            return _is;
        }

        void reset(std::basic_istream<Pt::Char>& is)
        {
            reset();
            _is = &is;
        }

        void attach(std::basic_istream<Pt::Char>& is)
        { 
            _is = &is;
        }

        std::size_t line() const
        { 
            return _line; 
        }

        void bumpLine()
        {
            ++_line;
        }

        Node& get()
        {
            if( _current.empty() )
            {
                this->next();
            }

            assert( ! _current.empty() );

            return *_current.front();
        }

        Node& next()
        {
            if( ! _current.empty() )
                _current.erase( _current.begin() );

            if( ! _current.empty() )
                return *_current.front();

            if( ! _is )
                throw JsonError("no input");

            while( _current.empty() )
            {
                std::char_traits<Char>::int_type c = _is->get();

                (this->*_parse)(c);

                if(c == '\n')
                    bumpLine();
            }

            return *_current.front();
        }

        Node* advance()
        {
            if( ! _current.empty() )
                _current.erase( _current.begin() );

            if( ! _current.empty() )
                return _current.front();

            if( ! _is || ! _is->rdbuf() )
                throw JsonError("no input");

            do
            {
                std::streamsize n = _is->rdbuf()->in_avail();  
                if(n == 0)
                  break;
          
                if(n > 0)
                {
                    std::char_traits<Char>::int_type c = _is->get();
                    
                    (this->*_parse)(c);

                    if(c == '\n')
                        bumpLine();
                }
                else if(n < 0)
                {
                    (this->*_parse)( std::char_traits<Char>::eof() );
                }
            } 
            while( _current.empty() );

            if( ! _current.empty() )
                return _current.front();

            return 0;
        }

    private:
        std::basic_istream<Pt::Char>*  _is;

        ParseFunc             _parse;
        std::stack<ParseFunc> _parseStack;

        Pt::String   _token;
        std::size_t  _line;
        std::size_t  _usedSize;
        std::size_t  _maxSize;
        std::size_t  _chunkSize;
        std::size_t  _maxChunkSize;

        std::vector<Node*> _current;
 
        EndDocument _endDoc;
        Null        _null;
        Boolean     _boolean;
        String      _string;
        Integer     _integer;
        Float       _float;
        StartObject _startObject;
        Member      _member;
        EndObject   _endObject;
        StartArray  _startArray;
        EndArray    _endArray;
};


JsonReader::JsonReader()
: _impl(0)
{
    _impl = new JsonReaderImpl();
}


JsonReader::JsonReader(std::basic_istream<Pt::Char>& is)
: _impl(0)
{
    _impl = new JsonReaderImpl(is);
}


JsonReader::~JsonReader()
{
    delete _impl;
}


std::basic_istream<Pt::Char>* JsonReader::input()
{
    return _impl->input();
}


void JsonReader::reset()
{
    _impl->reset();
}


void JsonReader::reset(std::basic_istream<Pt::Char>& is)
{
    _impl->reset(is);
}


void JsonReader::attach(std::basic_istream<Pt::Char>& in)
{
    _impl->attach(in);
}


void JsonReader::setMaxSize(std::size_t n)
{
    return _impl->setMaxSize(n);
}


std::size_t JsonReader::maxSize() const
{
    return _impl->maxSize();
}


std::size_t JsonReader::usedSize() const
{
    return _impl->usedSize();
}


void JsonReader::setChunkSize(std::size_t n)
{
    return _impl->setChunkSize(n);
}


std::size_t JsonReader::line() const
{
    return _impl->line();
}


Node& JsonReader::get()
{
    return _impl->get();
}


Node& JsonReader::next()
{
    return _impl->next();
}


Node* JsonReader::advance()
{
    return _impl->advance();
}

} // namespace

} // namespace
