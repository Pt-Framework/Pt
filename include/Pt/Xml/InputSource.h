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
#include <Pt/NonCopyable.h>
#include <Pt/TextBuffer.h>
#include <Pt/TextStream.h>
#include <Pt/Utf8Codec.h>
#include <Pt/StringStream.h>
#include <Pt/String.h>
#include <streambuf>
#include <iostream>
#include <cstddef>

namespace Pt {

namespace Xml {

/** @brief Input source for the XML reader
*/
class InputSource : private NonCopyable
{
    public:
        typedef std::char_traits<Char>::int_type int_type;

    public:
        InputSource()
        : _rdbuf(0)
        , _line(1)
        {}

        virtual ~InputSource()
        {}

        std::size_t line() const
        { return _line; }

        void setLine(std::size_t n)
        { _line = n; }

        // TODO: do not return buffer pointer

        // NULL is EOF
        std::basic_streambuf<Char>* getSome()
        {
            if(_rdbuf && _rdbuf->in_avail() > 0)
                return _rdbuf;

            _rdbuf = onGetSome();
            return _rdbuf;
        }

        // TODO: do not return buffer pointer
        std::basic_streambuf<Char>* get()
        {
            //if(_rdbuf)
            //    return _rdbuf;

            _rdbuf = onGet();
            return _rdbuf;
        }

        std::streamsize getSome2()
        { 
            std::streamsize r = 0;
            if( _rdbuf && (r =_rdbuf->in_avail()) > 0)
                return r;

            // TODO: it is enough to return bool here
            std::basic_streambuf<Char>* ok = onGetSome();
            return _rdbuf && ok ? _rdbuf->in_avail() : -1;
        }
        
        int_type get2()
        {
            if( _rdbuf )
                return _rdbuf->sbumpc();

            _rdbuf = onGet();
            return _rdbuf ? _rdbuf->sbumpc() : std::char_traits<Char>::eof();
        }

        std::basic_streambuf<Char>* rdbuf()
        { return _rdbuf; }

    protected:
        void init(std::basic_streambuf<Char>* rdbuf = 0)
        {
            _line = 0;
            _rdbuf = rdbuf;
        }

        virtual std::basic_streambuf<Char>* onGetSome() = 0;

        virtual std::basic_streambuf<Char>* onGet() = 0;

    private:
        std::basic_streambuf<Char>* _rdbuf;
        std::size_t _line;
};


class PT_XML_API TextInputSource : public InputSource
{
    public:
        TextInputSource();

        explicit TextInputSource(std::basic_istream<Char>& is);

        void reset(std::basic_istream<Char>& ios);     

        virtual std::basic_streambuf<Char>* onGetSome();

        virtual std::basic_streambuf<Char>* onGet();

    protected:
        virtual bool onGetSomeText();

    private:
        std::basic_istream<Char>* _ios;
};


class PT_XML_API StringInputSource : public TextInputSource
{
    public:
        StringInputSource(const String& str);

    protected:
        virtual bool onGetSomeText();

    private:
        StringStream _ss;
};


class PT_XML_API BinaryInputSource : public InputSource
{
    public:
        BinaryInputSource();

        explicit BinaryInputSource(std::istream& is);

        void reset(std::istream& is);

    protected:
        virtual std::basic_streambuf<Char>* onGetSome();

        virtual std::basic_streambuf<Char>* onGet();

    protected:
        virtual bool onGetSomeData();

    private:
        bool parseBom(unsigned char c);

        bool isBegin() const;

    private:
        std::istream* _is;
        Utf8Codec _utf8Codec;
        TextBuffer _tbuf;
        unsigned char _state;
};


class NullInputSource : public InputSource
{
    public:
        explicit NullInputSource()
        : InputSource()
        { }

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

} // namespace Xml

} // namespace Pt

#endif
