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
        std::basic_streambuf<Char>* getSome()
        {
            //if(_rdbuf && _rdbuf->in_avail() > 0)
            //    return _rdbuf;

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

        std::basic_streambuf<Char>* rdbuf()
        { return _rdbuf; }

    protected:
        // TODO: void init(std::basic_streambuf<Char>* rdbuf = 0)
        void clear()
        {
            _line = 0;
            _rdbuf = 0;
        }
        
        virtual std::basic_streambuf<Char>* onGetSome() = 0;

        virtual std::basic_streambuf<Char>* onGet() = 0;

    private:
        std::basic_streambuf<Char>* _rdbuf;
        std::size_t _line;
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


class PT_XML_API TextInputSource : public InputSource
{
    public:
        TextInputSource()
        : InputSource()
        , _ios(0)
        { }

    protected:
        explicit TextInputSource(std::basic_istream<Char>& is)
        : InputSource()
        , _ios(&is)
        { }

        void reset(std::basic_istream<Char>& ios);

        virtual std::basic_streambuf<Char>* onGetSome();

        virtual std::basic_streambuf<Char>* onGet();

        virtual std::basic_istream<Char>* onGetText();

        virtual std::basic_istream<Char>* onGetSomeText();

    private:
        std::basic_istream<Char>* _ios;
};


class PT_XML_API StringInputSource : public TextInputSource
{
    public:
        StringInputSource(const String& str)
        : TextInputSource()
        , _ss(str)
        { }

    protected:
        virtual std::basic_istream<Char>* onGetText();

        virtual std::basic_istream<Char>* onGetSomeText();

    private:
        StringStream _ss;
};


class PT_XML_API BinaryInputSource : public InputSource
{
    public:
        BinaryInputSource();

    protected:
        explicit BinaryInputSource(std::istream& is);

        void reset(std::istream& is);

        virtual std::basic_streambuf<Char>* onGetSome();

        virtual std::istream* onGetSomeBytes();

        virtual std::basic_streambuf<Char>* onGet();

        virtual std::istream* onGetBytes();

    private:
        bool parseBom(unsigned char c);

        bool isBegin() const;

    private:
        std::istream* _is;
        Utf8Codec _utf8Codec;
        TextBuffer _tbuf;
        
        //static const unsigned MaxBufSize = 32;
        //char _buf[MaxBufSize];
        //std::size_t _bufsize;
        unsigned char _bomState;
};


class PT_XML_API ByteInputSource : public BinaryInputSource
{
    public:
        ByteInputSource();
        
        explicit ByteInputSource(std::istream& is);
        
        void reset(std::istream& is);
};

} // namespace Xml

} // namespace Pt

#endif
