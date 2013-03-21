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

void TextInputSource::reset(std::basic_istream<Char>& ios)
{
    // reset bomState
    _ios = &ios;

    // TODO: init()
    clear();
}


std::basic_streambuf<Char>* TextInputSource::onGetSome()
{   
    if( ! _ios || ( _ios->rdbuf() && _ios->rdbuf()->in_avail() <= 0 ) )
    {
        _ios = onGetSomeText();
    }

    if( _ios && _ios->rdbuf() && _ios->good() )
    {
        return _ios->rdbuf();
    }

    return 0;
}


std::basic_streambuf<Char>* TextInputSource::onGet()
{
    if( ! _ios)
    {
        _ios = onGetText();
    }

    if( ! _ios->rdbuf() )
    {
        return 0;
    }

    if( _ios->good() )
    {
        return _ios->rdbuf();
    }

    return 0;
}


std::basic_istream<Char>* TextInputSource::onGetText()
{   
    if( _ios && ( ! _ios->rdbuf() || ! _ios->good() ) )
        return 0;

    // NOTE: on many compilers, stringbuf::in_avail never returns -1 
    //       even if it is empty
    return _ios;
}


std::basic_istream<Char>* TextInputSource::onGetSomeText()
{
    if( _ios && ( ! _ios->rdbuf() || ! _ios->good() ) )
        return 0;

    return _ios;
}


std::basic_istream<Char>* StringInputSource::onGetSomeText()
{   
    // NOTE: on some systems stringbuf::in_avail never returns -1, 
    //       even if no more characters are available
    
    return _ss.rdbuf()->in_avail() > 0 ? &_ss : 0;
}


std::basic_istream<Char>* StringInputSource::onGetText()
{   
    return &_ss;
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

    OnBomEnd = 255,
};


BinaryInputSource::BinaryInputSource()
: InputSource()
, _is(0)
, _utf8Codec(1)
, _tbuf(&_utf8Codec)
//, _bufsize(0)
, _bomState(OnBomBegin)
{ }


BinaryInputSource::BinaryInputSource(std::istream& is)
: InputSource()
, _is(&is)
, _utf8Codec(1)
, _tbuf(&is, &_utf8Codec)
//, _bufsize(0)
, _bomState(OnBomBegin)
{ }


void BinaryInputSource::reset(std::istream& is)
{ 
    _tbuf.attach(is); 
    _tbuf.setCodec(&_utf8Codec);

    _bomState = OnBomBegin;
    _is = &is;
    //_bufsize = 0;

    // TODO: init(&_tbuf);
    clear();
}


std::basic_streambuf<Char>* BinaryInputSource::onGetSome()
{
    // TODO: check if attach/detach affects peformance

    // TODO: return early when _tbuf has available

    if( ! _is || ! _is->rdbuf() )
    {
        return 0;
    }
    
    if( _is->rdbuf()->in_avail() <= 0 )
    {
        _is = onGetSomeBytes();
    }

    if( isBegin() )
    {
        std::streambuf* sb = _is->rdbuf();
        std::char_traits<char>::int_type c = 0;
        std::streamsize avail = sb->in_avail();
        
        for( ; ; --avail)
        {
            //if(_bufsize == MaxBufSize)
            //{
            //    _tbuf.import( _buf, _bufsize );
            //    _bufsize = 0;
            //    return &_tbuf;
            //}
            
            if(avail <= 0)
            {
                return &_tbuf;
            }

            c = sb->sbumpc();
            
            char ch = std::char_traits<char>::to_char_type(c);

            //_buf[_bufsize] = ch;
            //++_bufsize;
            
            if( ! parseBom(ch) )
                break;
        }

        //_tbuf.import( _buf, _bufsize );
        //_bufsize = 0;
    }

    _tbuf.import();
    return &_tbuf;
}


std::istream* BinaryInputSource::onGetSomeBytes()
{
    if( ! _is || ! _is->rdbuf() || ! _is->good() )
        return 0;

    return _is;
}


std::basic_streambuf<Char>* BinaryInputSource::onGet()
{
    if( ! _is || ! _is->rdbuf() )
    {
        return 0;
        /*std::istream* is = onGetBytes();

        if( ! is || ! is->rdbuf() )
        {           
            if(_is)
            {
                _is = 0;
                _tbuf.detach();
            }
            
            return 0;
        }

        if(_is != is)
        {
            _is = is;
            _tbuf.attach(*_is);
        }*/
    }

    if( isBegin() )
    {        
        std::char_traits<char>::int_type c = 0;
        std::streambuf* sb = _is->rdbuf();
        std::char_traits<char>::int_type eofval = std::char_traits<char>::eof();

        for( ; ; )
        {
            //if(_bufsize == MaxBufSize)
            //{
            //    _tbuf.import( _buf, _bufsize );
            //    _bufsize = 0;
            //    return &_tbuf;
            //}
            
            c = sb->sbumpc();
            
            if( std::char_traits<char>::eq_int_type(c, eofval) )
            {
                return &_tbuf;
            }

            char ch = std::char_traits<char>::to_char_type(c);

            //_buf[_bufsize] = ch;
            //++_bufsize;
            
            if( ! parseBom(ch) )
                break;
        }

        //_tbuf.import( _buf, _bufsize );
        //_bufsize = 0;
    }

    return &_tbuf;
}


std::istream* BinaryInputSource::onGetBytes()
{
    if( ! _is || ! _is->rdbuf() || ! _is->good() )
        return 0;

    return _is;
}


bool BinaryInputSource::isBegin() const
{
    return _bomState != OnBomEnd;
}


// TODO: parse encoding name and call virtual function so use can 
//       return pointer to codec: 
//
//       Codec* onEncoding(const char* name)
//
//       This function could also be part of the XmlResolver or some
//       context class that would be useful in other situations too...
bool BinaryInputSource::parseBom(unsigned char c)
{
    char buf[16];

    switch(_bomState)
    {
        case OnBomBegin:
            if(c == 0xef)
                _bomState = OnBomUtf8_0;
            else if(c == '<')
                _bomState = On8Bit;
            else
                _bomState = OnBomEnd;

            break;

        case OnBomUtf8_0:
            if(c == 0xbb)
                _bomState = OnBomUtf8_1;
            else
                _bomState = OnBomEnd;
                    
            break;

        case OnBomUtf8_1:
            if(c == 0xbf)
                _tbuf.setCodec(&_utf8Codec);

            _bomState = OnBomEnd;
            break;

        case On8Bit:
            if(c == '?')
            {
                _bomState = On8Bit_1;
                break;
            }
            
            buf[0] = '<';
            buf[1] = c;
            _tbuf.import(buf, 2);
            _bomState = OnBomEnd;
            break;

        case On8Bit_1:
            if(c == 'x')
            {
                _bomState = On8Bit_2;
                break;
            }

            buf[0] = '<';
            buf[1] = '?';
            buf[2] = c;
            _tbuf.import(buf, 3);
            _bomState = OnBomEnd;
            break;
            
        case On8Bit_2:
            if(c == 'm')
            {
                _bomState = On8Bit_3;
                break;
            }

            buf[0] = '<';
            buf[1] = '?';
            buf[2] = 'x';
            buf[3] = c;
            _tbuf.import(buf, 3);
            _bomState = OnBomEnd;
            break;

        case On8Bit_3:
            if(c == 'l')
            {
                _bomState = On8Bit_4;
                break;
            }

            buf[0] = '<';
            buf[1] = '?';
            buf[2] = 'x';
            buf[3] = 'm';
            buf[4] = c;
            _tbuf.import(buf, 4);
            _bomState = OnBomEnd;
            break;

        case On8Bit_4:
            if(c == ' ' || c == '\t' || c == '\r' || c == '\n')
            {
                _bomState = On8Bit_5;
                break;
            }

            buf[0] = '<';
            buf[1] = '?';
            buf[2] = 'x';
            buf[3] = 'm';
            buf[4] = 'l';
            buf[5] = c;
            _tbuf.import(buf, 5);
            _bomState = OnBomEnd;
            break;

        case On8Bit_5:
            if(c == '>')
                _bomState = OnBomEnd;

            break;

        case OnBomEnd:
            assert(false);
            break;

        default:
            break;
    }

    return _bomState != OnBomEnd;
}


ByteInputSource::ByteInputSource()
: BinaryInputSource()
{ 
}


ByteInputSource::ByteInputSource(std::istream& is)
: BinaryInputSource(is)
{ 
}


void ByteInputSource::reset(std::istream& is)
{ 
    BinaryInputSource::reset(is);
}

/*
std::istream* ByteInputSource::onGetSomeBytes()
{
    if( ! _is || ! _is->rdbuf() || ! _is->good() )
        return 0;

    return _is;
}


std::istream* ByteInputSource::onGetBytes()
{
    if( ! _is || ! _is->rdbuf() || ! _is->good() )
        return 0;

    return _is;
}
*/
} // namespace Xml

} // namespace Pt
