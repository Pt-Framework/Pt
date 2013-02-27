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
 
#include "Pt/Xml/InputSource.h"

namespace Pt {

namespace Xml {

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
    OnBomUtf8_2 = 3,

    OnBomEnd = 255,
};


ByteInputSource::ByteInputSource(std::istream& is, std::size_t refcnt)
: InputSource(refcnt)
, _tbuf(&is, new Utf8Codec)
, _is(&is)
, _bomState(OnBomBegin)
{ }


void ByteInputSource::reset(std::istream& is)
{
    _tbuf.attach(is);
    _tbuf.setCodec(new Utf8Codec);
    _bomState = OnBomBegin;
}

        
void ByteInputSource::attach(std::istream& is)
{ 
    _tbuf.attach(is); 
}


void ByteInputSource::detach()
{ 
    _tbuf.detach(); 
}


std::basic_streambuf<Char>* ByteInputSource::onGetSome()
{
    if( ! _is || ! _is->rdbuf() || ! _is->good() )
            return 0;

    if(_bomState != OnBomEnd)
    {
        std::streambuf* sb = _is->rdbuf();
        std::char_traits<char>::int_type c = 0;
        for(;;)
        {
            if(sb->in_avail() <= 0)
                return &_tbuf;

            c = sb->sgetc();
            char ch = std::char_traits<char>::to_char_type(c);

            if( ! parseBom(ch) )
                break;

            c = sb->sbumpc();
        }                   
    }

    _tbuf.import();

    //TODO: review all streambufs. _tbuf.import() will not set the underlying
    //      stream _is to EOF. All proxy streambufs should read from basic_ios
    //      so that they can propagate EOF.

    // NOTE: stringbuf::in_avail never returns -1 even if it is empty !!!
                
    if(_tbuf.in_avail() >= 0)
        return &_tbuf;
            
    if( ! _is->good() )
        return 0;

    return &_tbuf;
}


std::basic_streambuf<Char>* ByteInputSource::onGet()
{
    if( ! _is || ! _is->rdbuf() )
            return 0;

    if(_bomState != OnBomEnd)
    {        
        std::char_traits<char>::int_type c = 0;
        std::streambuf* sb = _is->rdbuf();
        std::char_traits<char>::int_type eofval = std::char_traits<char>::eof();
                
        for(c = sb->sgetc(); ! std::char_traits<char>::eq_int_type(c, eofval); c = sb->sbumpc() )
        {
            char ch = std::char_traits<char>::to_char_type(c);

            if( ! parseBom(ch) )
                break;
        }
    }
            
    return &_tbuf;
}


bool ByteInputSource::parseBom(unsigned char c)
{
    switch(_bomState)
    {
        case OnBomBegin:
            if(c == 0xef)
                _bomState = OnBomUtf8_0;
            else
                _bomState = OnBomEnd;

            break;

        case OnBomUtf8_0:
            if(c == 0xbb)
                _bomState = OnBomUtf8_1;
            else
                throw SyntaxError("invalid byte-order mark", 0);
                    
            break;

        case OnBomUtf8_1:
            if(c == 0xbf)
                _bomState = OnBomUtf8_2;
            else
                throw SyntaxError("invalid byte-order mark", 0);

            break;

        case OnBomUtf8_2:
            _bomState = OnBomEnd;
            _tbuf.setCodec(new Utf8Codec);
            break;

        case OnBomEnd:
            throw SyntaxError("invalid byte-order mark", 0);
            break;

        default:
            break;
    }

    return _bomState != OnBomEnd;
}

} // namespace Xml

} // namespace Pt
