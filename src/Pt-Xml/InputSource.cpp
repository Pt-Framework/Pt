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

enum XmlParseState
{
    OnXmlBegin = 0,
    OnXmlDeclBegin,
    OnXmlDeclBeginQuest,
    OnXmlDeclBegin_x,
    OnXmlDeclBegin_m,
    OnXmlDeclBegin_l,
    OnXmlDecl,
    OnXmlDeclClose,
    OnXmlDeclEnd = 64
};


bool isXmlBegin(unsigned xmlState)
{
    return xmlState != OnXmlDeclEnd;
}


bool parseXml(unsigned char& state, unsigned char c, std::size_t& putback)
{
    switch(state)
    {
        case OnXmlBegin:
            if(c == '<')
            {
                state = OnXmlDeclBegin;
            }
            else
            {
                state = OnXmlDeclEnd;
            }

            break;

        case OnXmlDeclBegin:
            if(c == '?')
            {
                state = OnXmlDeclBeginQuest;
            }
            else
            {
                putback = 1;
                state = OnXmlDeclEnd;
            }

            break;

        case OnXmlDeclBeginQuest:
            if(c == 'x')
            {
                state = OnXmlDeclBegin_x;
            }
            else
            {
                putback = 2;
                state = OnXmlDeclEnd;
            }

            break;

        case OnXmlDeclBegin_x:
            if(c == 'm')
            {
                state = OnXmlDeclBegin_m;
            }
            else
            {
                putback = 3;
                state = OnXmlDeclEnd;
            }

            break;

        case OnXmlDeclBegin_m:
            if(c == 'l')
            {
                state = OnXmlDeclBegin_l;
            }
            else
            {
                putback = 4;
                state = OnXmlDeclEnd;
            }

            break;

        case OnXmlDeclBegin_l:
            if(c == ' ' || c == '\t' || c == '\r' || c == '\n')
            {
                state = OnXmlDecl;
            }
            else
            {
                putback = 5;
                state = OnXmlDeclEnd;
            }

            break;

        case OnXmlDecl:
            if(c == '>')
            {
                state = OnXmlDeclClose;
            }

            break;

        case OnXmlDeclClose:
            state = OnXmlDeclEnd;
            break;

        case OnXmlDeclEnd:
            assert(false);
            break;

        default:
            break;
    }

    return state != OnXmlDeclEnd;
}


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

    // parse XML declaration

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
            
    OnBomUtf8_0,
    OnBomUtf8_1,
    OnBomUtf8_2,

    OnBomEnd = 64,
};


BinaryInputSource::BinaryInputSource()
: InputSource()
, _is(0)
, _utf8Codec(1)
, _tbuf(&_utf8Codec)
, _state(OnBomBegin)
, _xmlState(OnXmlBegin)
, _mbSize(1)
, _mbPos(0)
, _putback(0)
{ }


BinaryInputSource::BinaryInputSource(std::istream& is)
: InputSource()
, _is(&is)
, _utf8Codec(1)
, _tbuf(&is, &_utf8Codec)
, _state(OnBomBegin)
, _xmlState(OnXmlBegin)
, _mbSize(1)
, _mbPos(0)
, _putback(0)
{ 
}


void BinaryInputSource::reset(std::istream& is)
{ 
    _is = &is;
    _tbuf.attach(is); 
    _tbuf.setCodec(&_utf8Codec);
    _state = OnBomBegin;
    _xmlState = OnXmlBegin;
    _mbSize = 1;
    _mbPos = 0;
    _putback = 0;
    
    init(0);
}


bool BinaryInputSource::onParseXml(int c)
{ 
    char ch = std::char_traits<char>::to_char_type(c);

    if(_mbSize == 2)
    {
        if(_mbPos == 0)
        {
            ++_mbPos;
            return true;
        }

        _mbPos = 0;
        ch = std::char_traits<char>::to_char_type(c);
    }
            
    if( ! parseXml(_xmlState, ch, _putback) )
    {
        encoding() = L"UTF-8";
        version() = L"1.0";
                
        _putback = _putback < 8 ? _putback : 0;
        const char* pbtxt = "<?xml     ";
        const char* pb = pbtxt + _putback;

        for(unsigned n = 0; pb != pbtxt; ++n)
        {
            _putbackBuffer[n] = *--pb;
        }
   
        return false;
    }

    return true;
}


bool BinaryInputSource::isBomBegin() const
{
    return _state != OnBomEnd;
}


// TODO: parse encoding name and call virtual function so use can 
//       return pointer to codec: 
//
//       Codec* onEncoding(const char* name)
//
//       This function could also be part of the XmlResolver or some
//       context class that would be useful in other situations too...
bool BinaryInputSource::onParseBom(unsigned char c)
{    
    switch(_state)
    {
        case OnBomBegin:
            if(c == 0xef)
                _state = OnBomUtf8_0;
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
                _tbuf.setCodec(&_utf8Codec);
                _state = OnBomUtf8_2;
                break;
            }

            _state = OnBomEnd;
            break;

        case OnBomUtf8_2:
            _state = OnBomEnd;
            break;

        case OnBomEnd:
            assert(false);
            break;

        default:
            break;
    }

    return _state != OnBomEnd;
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

    if( isBomBegin() )
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

            c = sb->sgetc();
            char ch = std::char_traits<char>::to_char_type(c);

            if( ! onParseBom(ch) )
            {
                break;
            }

            sb->sbumpc();
        }
    }

    if( isXmlBegin(_xmlState) )
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
            
            c = sb->sgetc();
            
            bool ok = onParseXml(c);
            if( ! ok)
                break;

            sb->sbumpc();
        }
    }

    if(_putback > 0)
    {
        return _putbackBuffer[--_putback];
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

    if( isBomBegin() )
    {
        std::streambuf* sb = _is->rdbuf();
        std::char_traits<char>::int_type c = 0;
        std::char_traits<char>::int_type eofval = std::char_traits<char>::eof();
        
        for( ; ; )
        {           
            c = sb->sgetc();
 
            if( std::char_traits<char>::eq_int_type(c, eofval) )
            {
                break;    
            }

            char ch = std::char_traits<char>::to_char_type(c);
            
            if( ! onParseBom(ch) )
            {
                break;
            }

            sb->sbumpc();
        }
    }

    if( isXmlBegin(_xmlState) )
    {
        std::streambuf* sb = _is->rdbuf();
        std::char_traits<char>::int_type c = 0;
        std::char_traits<char>::int_type eofval = std::char_traits<char>::eof();
        
        for( ; ; )
        {            
            c = sb->sgetc();
 
            if( std::char_traits<char>::eq_int_type(c, eofval) )
            {
                break;    
            }
            
            bool ok = onParseXml(c);
            if( ! ok)
            {
                break;
            }

            sb->sbumpc();
        }
    }
    
    if(_putback > 0)
    {
        return _putbackBuffer[--_putback];
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

} // namespace Xml

} // namespace Pt
