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
#ifndef Pt_Xml_InputSource_h
#define Pt_Xml_InputSource_h

#include <Pt/Xml/Api.h>
#include <Pt/Xml/XmlError.h>
#include <Pt/StringStream.h>
#include <Pt/TextBuffer.h>
#include <Pt/TextStream.h>
#include <Pt/Utf8Codec.h>
#include <Pt/String.h>
#include <streambuf>
#include <cstddef>

namespace Pt {

namespace Xml {

/** @brief Input source for the XML reader
*/
class InputSource 
{
    public:
        InputSource(std::size_t refcnt = 0)
        : _refs(refcnt)
        , _line(1)
        , _rdbuf(0)
        {}

        virtual ~InputSource()
        {}

        std::size_t refs() const
        { return _refs; }

        std::size_t line() const
        { return _line; }

        void setLine(std::size_t n)
        { _line = n; }

        std::basic_streambuf<Char>* getSome()
        {
            if(_rdbuf && _rdbuf->in_avail() > 0)
                return _rdbuf;

            _rdbuf = onGetSome();
            return _rdbuf;
        }

        std::basic_streambuf<Char>* get()
        {
            if(_rdbuf)
                return _rdbuf;

            _rdbuf = onGet();
            return _rdbuf;
        }

        std::basic_streambuf<Char>* rdbuf()
        { return _rdbuf; }

        void init(std::basic_streambuf<Char>* sb = 0)
        { _rdbuf = sb; }

    protected:
        virtual std::basic_streambuf<Char>* onGetSome() = 0;

        virtual std::basic_streambuf<Char>* onGet() = 0;

    private:
        std::size_t _refs;
        std::size_t _line;
        std::basic_streambuf<Char>* _rdbuf;
};

class NullInputSource : public InputSource
{
    public:
        explicit NullInputSource(std::size_t refcnt = 0)
        : InputSource(refcnt)
        {
        }

    protected:
        virtual std::basic_streambuf<Char>* onGetSome()
        {      
            return 0;
        }

        virtual std::basic_streambuf<Char>* onGet()
        {
            return 0;
        }
};

/*
    00 00 FE FF  UTF-32, big-endian
    FF FE 00 00  UTF-32, little-endian
    FE FF        UTF-16, big-endian
    FF FE        UTF-16, little-endian
    EF BB BF     UTF-8
*/
class ByteInputSource : public InputSource
{
    public:
        enum BomState
        {
            OnBomBegin = 0,
            
            OnUtf8_0 = 1,
            OnUtf8_1 = 2,
            OnUtf8_2 = 3,

            OnUtf16LE0 = 4,
            OnUtf16LE1 = 5,

            OnUtf16BE0 = 6,
            OnUtf16BE1 = 7,

            OnUtf32LE2 = 8,
            OnUtf16LE3 = 9,
            
            OnUtf32BE0 = 10,
            OnUtf32BE1 = 11,
            OnUtf32BE2 = 12,
            OnUtf32BE3 = 13,

            OnBomEnd = 14,
            OnBomInvalid = 15
        };

    public:
        ByteInputSource(std::istream& is, std::size_t refcnt = 0)
        : InputSource(refcnt)
        , _tbuf(&is, new Utf8Codec)
        , _is(&is)
        , _bomState(OnBomBegin)
        {
        }

        void reset(std::istream& is)
        {
            _tbuf.attach(is);
            _tbuf.setCodec(new Utf8Codec);
            _bomState = OnBomBegin;
        }
        
        void attach(std::istream& is)
        { _tbuf.attach(is); }

        void detach()
        { _tbuf.detach(); }

    protected:
        virtual std::basic_streambuf<Char>* onGetSome()
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
                
            if(_tbuf.in_avail() >= 0)
                return &_tbuf;
            
            if( ! _is->good() )
                return 0;

            return &_tbuf;
        }

        virtual std::basic_streambuf<Char>* onGet()
        {
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

        bool parseBom(unsigned char c)
        {
            switch(_bomState)
            {
                case OnBomBegin:
                    if(c == 0xef)
                        _bomState = OnUtf8_0;
                    else
                        _bomState = OnBomEnd;

                    
                    break;

                case OnUtf8_0:
                    if(c == 0xbb)
                        _bomState = OnUtf8_1;
                    else
                        throw SyntaxError("invalid byte-order mark", 0);
                    
                    break;

                case OnUtf8_1:
                    if(c == 0xbf)
                        _bomState = OnUtf8_2;
                    else
                        throw SyntaxError("invalid byte-order mark", 0);

                    break;

                case OnUtf8_2:
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

    private:
        TextCodec<Char, char>* _codec;
        TextBuffer _tbuf;
        std::istream* _is;
        unsigned char _bomState;
};


class TextInputSource : public InputSource
{
    public:
        explicit TextInputSource(TextIStream& ts, std::size_t refcnt = 0)
        : InputSource(refcnt)
        , _ios(&ts)
        , _tbuf( &ts.buffer() )
        { }

        explicit TextInputSource(TextStream& ts, std::size_t refcnt = 0)
        : InputSource(refcnt)
        , _ios(&ts)
        , _tbuf( &ts.buffer() )
        { }

    protected:
        virtual std::basic_streambuf<Char>* onGetSome()
        {   
            _tbuf->import();

            if(_tbuf->in_avail() > 0 || _ios->good() )
                return _tbuf;

            return 0;
        }

        virtual std::basic_streambuf<Char>* onGet()
        {
            return _tbuf;
        }

    private:
        std::basic_ios<Char>* _ios;
        BasicTextBuffer<Char, char>* _tbuf;
};


class StringInputSource : public InputSource
{
    public:
        StringInputSource(const String& str, std::size_t refcnt = 0)
        : InputSource(refcnt)
        , _sbuf(str)
        {
        }

    protected:
        virtual std::basic_streambuf<Char>* onGetSome()
        {   
            return _sbuf.in_avail() > 0 ? &_sbuf : 0;
        }
        
        virtual std::basic_streambuf<Char>* onGet()
        {
            return &_sbuf;
        }

    private:
        StringBuffer _sbuf;
};

} // namespace Xml

} // namespace Pt

#endif
