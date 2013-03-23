/*
 * Copyright (C) 2012 Marc Boris Duerner
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */
 
#include <Pt/Xml/InputSource.h>
#include <Pt/Xml/XmlError.h>
#include <cassert>

namespace Pt {

namespace Xml {

TextInputSource::TextInputSource()
: InputSource()
, _ios(0)
{ 
}


TextInputSource::TextInputSource(std::basic_istream<Char>& is)
: InputSource()
, _ios(&is)
{ 
}


void TextInputSource::reset(std::basic_istream<Char>& ios)
{
    // reset bomState
    _ios = &ios;

    init(0);
}


std::streamsize TextInputSource::onGetSome()
{   
    if( ! _ios || ! _ios->rdbuf() )
    {
        return -1;
    }
    
    if( _ios->rdbuf()->in_avail() <= 0 )
    {
        bool r = onGetSomeText();
        if( ! r)
            return -1;
    }

    // parse XML dclaration

    init( _ios->rdbuf() );
    return _ios->rdbuf()->in_avail();
}


InputSource::int_type TextInputSource::onGet()
{
    if( ! _ios || ! _ios->rdbuf() )
    {
        return std::char_traits<Char>::eof();
    }

    // parse XML dclaration

    init( _ios->rdbuf() );
    return _ios->rdbuf()->sbumpc();
}


bool TextInputSource::onGetSomeText()
{
    if( ! _ios || ! _ios->rdbuf() || ! _ios->good() )
        return false;

    return true;
}


StringInputSource::StringInputSource(const String& str)
: TextInputSource()
, _ss(str)
{ 
    reset(_ss);
}


bool StringInputSource::onGetSomeText()
{   
    // NOTE: on some systems stringbuf::in_avail never returns -1, 
    //       even if no more characters are available
    
    return _ss.rdbuf()->in_avail() > 0;
}


/*
    00 00 FE FF  UTF-32, big-endian
    FF FE 00 00  UTF-32, little-endian
    FE FF        UTF-16, big-endian
    FF FE        UTF-16, little-endian
    EF BB BF     UTF-8
*/
enum BomParseState
{
    OnBomBegin = 0,
            
    OnBomUtf8_0 = 1,
    OnBomUtf8_1 = 2,

    On8Bit = 3,
    On8Bit_1 = 4,
    On8Bit_2 = 5,
    On8Bit_3 = 6,
    On8Bit_4 = 7,
    On8Bit_5 = 8,

    OnBomEnd = 64,

    OnXmlDocBegin = 100,
    OnXmlDeclBegin = 101,
    OnXmlDeclBeginQuest= 102,
    OnXmlDeclBegin_X= 103,
    OnXmlDeclBegin_M= 104,
    OnXmlDeclBegin_L= 105,
    OnXmlDecl= 106,
    OnXmlDeclEnd= 107,
    OnNoXmlDecl= 108
};




BinaryInputSource::BinaryInputSource()
: InputSource()
, _is(0)
, _utf8Codec(1)
, _tbuf(&_utf8Codec)
, _state(OnBomBegin)
, _mbSize(1)
, _mbPos(0)
, _bufsize(0)
{ }


BinaryInputSource::BinaryInputSource(std::istream& is)
: InputSource()
, _is(&is)
, _utf8Codec(1)
, _tbuf(&is, &_utf8Codec)
, _state(OnBomBegin)
, _mbSize(1)
, _mbPos(0)
, _bufsize(0)
{ 
}


void BinaryInputSource::reset(std::istream& is)
{ 
    _is = &is;
    _tbuf.attach(is); 
    _tbuf.setCodec(&_utf8Codec);
    _state = OnBomBegin;
    _mbSize = 1;
    _mbPos = 0;
    _bufsize = 0;
    
    init(0);
}


std::streamsize BinaryInputSource::onGetSome()
{
    if( ! _is || ! _is->rdbuf() )
    {
        return -1;
    }
    
    if( _is->rdbuf()->in_avail() <= 0 )
    {
        bool r = onGetSomeData();
        if( ! r)
        {
            return -1;
        }
    }

    if( isBegin() )
    {
        std::streambuf* sb = _is->rdbuf();
        std::char_traits<char>::int_type c = 0;
        std::streamsize avail = sb->in_avail();
        
        for( ; ; --avail)
        {            
            if(avail <= 0)
            {
                return true;
            }

            c = sb->sbumpc();
            
            char ch = std::char_traits<char>::to_char_type(c);
            
            if( ! parseBom(ch, _buf, _bufsize) )
            {               
                break;
            }
        }
    }

    _tbuf.import();
    init(&_tbuf);
    return _tbuf.in_avail();
}


InputSource::int_type BinaryInputSource::onGet()
{
    if( ! _is || ! _is->rdbuf() )
    {
        return std::char_traits<Char>::eof();
    }

    if( isBegin() )
    {        
        std::char_traits<char>::int_type c = 0;
        std::streambuf* sb = _is->rdbuf();
        std::char_traits<char>::int_type eofval = std::char_traits<char>::eof();

        for( ; ; )
        {            
            c = sb->sbumpc();
            
            if( std::char_traits<char>::eq_int_type(c, eofval) )
            {
                break;    
            }

            char ch = 0;
            if(_mbSize == 1)
            {
                ch = std::char_traits<char>::to_char_type(c);
            }
            else if(_mbSize == 2)
            {
                if(_mbPos == 0)
                    continue;

                ch = std::char_traits<char>::to_char_type(c);
            }
            else
            {
                break;
            }

            if( ! parseBom(ch, _buf, _bufsize) )
            {
                if(_bufsize == 1)
                {
                    return _buf[0];
                }

                if(_bufsize > 1)
                {
                    init(_buf+1, _buf + _bufsize);
                    return _buf[0];
                }
                
                break;
            }
        }
    }

    init( &_tbuf );
    return _tbuf.sbumpc();
}


bool BinaryInputSource::onGetSomeData()
{
    if( ! _is || ! _is->rdbuf() || ! _is->good() )
        return false;

    return true;
}


bool BinaryInputSource::isBegin() const
{
    return _state != OnBomEnd;
}



bool BinaryInputSource::parseDeclaration(unsigned char c)
{
    switch(_state)
    {
        case OnXmlDocBegin:
            if(c == '<')
            {
                _state = OnXmlDeclBegin;
                break;
            }
            
            _state = OnNoXmlDecl;
            break;

        case OnXmlDeclBegin:
            if(c == '?')
            {
                _state = OnXmlDeclBeginQuest;
                break;
            }
            
            _state = OnNoXmlDecl;
            break;

        case OnXmlDeclBeginQuest:
            if(c == 'x')
            {
                _state = OnXmlDeclBegin_X;
                break;
            }
            
            _state = OnNoXmlDecl;
            break;
        
        case OnXmlDeclBegin_X:
            if(c == 'm')
            {
                _state = OnXmlDeclBegin_M;
                break;
            }
            
            _state = OnNoXmlDecl;
            break;

        case OnXmlDeclBegin_M:
            if(c == 'l')
            {
                _state = OnXmlDeclBegin_L;
                break;
            }
            
            _state = OnNoXmlDecl;
            break;

        case OnXmlDeclBegin_L:
            if(c == ' ' || c == '\t' || c == '\r' || c== '\n')
            {
                _state = OnXmlDecl;
                break;
            }
            
            _state = OnNoXmlDecl;
            break;

        case OnXmlDecl:
            if(c == '>')
            {
                _state = OnXmlDeclEnd;
                break;
            }
            
            break;
    }

    return _state != OnXmlDeclEnd && _state != OnNoXmlDecl;
}


// TODO: parse encoding name and call virtual function so use can 
//       return pointer to codec: 
//
//       Codec* onEncoding(const char* name)
//
//       This function could also be part of the XmlResolver or some
//       context class that would be useful in other situations too...
bool BinaryInputSource::parseBom(unsigned char c, Pt::Char* buf, std::size_t& bufsize)
{
    if(bufsize < 8)
    {
        buf[bufsize++] = c;
    }
    
    switch(_state)
    {
        case OnBomBegin:
            if(c == 0xef)
                _state = OnBomUtf8_0;
            else if(c == '<')
                _state = On8Bit;
            else
                _state = OnBomEnd;

            break;

        case OnBomUtf8_0:
            if(c == 0xbb)
                _state = OnBomUtf8_1;
            else
                _state = OnBomEnd;
                    
            break;

        case OnBomUtf8_1:
            if(c == 0xbf)
            {
                bufsize = 0;
                _tbuf.setCodec(&_utf8Codec);
            }

            _state = OnBomEnd;
            break;

        case On8Bit:
            if(c == '?')
            {
                _state = On8Bit_1;
                break;
            }
                
            _state = OnBomEnd;
            break;

        case On8Bit_1:
            if(c == 'x')
            {
                _state = On8Bit_2;
                break;
            }

            _state = OnBomEnd;
            break;
            
        case On8Bit_2:
            if(c == 'm')
            {
                _state = On8Bit_3;
                break;
            }

            _state = OnBomEnd;
            break;

        case On8Bit_3:
            if(c == 'l')
            {
                _state = On8Bit_4;
                break;
            }

            _state = OnBomEnd;
            break;

        case On8Bit_4:
            if(c == ' ' || c == '\t' || c == '\r' || c == '\n')
            {
                _bufsize = 0;
                _state = On8Bit_5;
                break;
            }

            _state = OnBomEnd;
            break;

        case On8Bit_5:
            if(c == '>')
            {
                _bufsize = 0;
                _state = OnBomEnd;
            }

            break;

        case OnBomEnd:
            assert(false);
            break;

        default:
            break;
    }

    return _state != OnBomEnd;
}

} // namespace Xml

} // namespace Pt
