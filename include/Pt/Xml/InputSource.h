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
#include <string>
#include <cstddef>

namespace Pt {

namespace Xml {

class XmlResolver;

class XmlDeclaration
{
    public:
        XmlDeclaration()
        : _standalone(false)
        {}

        ~XmlDeclaration()
        {}
        
        void clear()
        {
            _version.clear();
            _encoding.clear();
            _standalone = false;
        }
        
        const std::string& version() const
        { return _version; }

        std::string& version()
        { return _version; }

        const std::string& encoding() const
        { return _encoding; }

        std::string& encoding()
        { return _encoding;}

        bool isStandalone() const
        { return _standalone; }
        
        void setStandalone(bool value)
        { _standalone = value; }

    private:
        std::string _version;
        std::string _encoding;
        bool _standalone;
};

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
        , _decl(0)
        {}

        virtual ~InputSource()
        {}

        std::size_t line() const
        { return _line; }

        void setLine(std::size_t n)
        { _line = n; }

        inline std::streamsize import()
        { 
            std::streamsize r = 0;
            return  (_rdbuf && (r =_rdbuf->in_avail()) > 0) ? r 
                                                            : onImport();
        }

        inline int_type get()
        {
            return _rdbuf ? _rdbuf->sbumpc() 
                          : onGet();
        }

        const XmlDeclaration* declaration() const
        { return _decl; }

    protected:
        // InputSource does not take ownership of rdbuf and decl
        void init(std::basic_streambuf<Char>* rdbuf = 0, XmlDeclaration* decl = 0)
        {
            _line = 0;
            _rdbuf = rdbuf;
            _decl = decl;
        }

        virtual std::streamsize onImport() = 0;

        virtual int_type onGet() = 0;

    private:
        std::basic_streambuf<Char>* _rdbuf;
        std::size_t _line;
        XmlDeclaration* _decl;
        Pt::String _id; // maybe virtual???
};


class PT_XML_API TextInputSource : public InputSource
{
    public:
        TextInputSource();

        explicit TextInputSource(std::basic_istream<Char>& is);

        void reset(std::basic_istream<Char>& ios);     

    protected:
        virtual std::streamsize onImport();

        virtual int_type onGet();

        virtual bool onImportText();

    private:
        bool onParseXml(int_type c);

    private:
        std::basic_istream<Char>* _ios;
        XmlDeclaration _xmlDecl;
        unsigned char _xmlState;
        const char* _pbBegin;
        const char* _pbEnd;
};


class PT_XML_API StringInputSource : public TextInputSource
{
    public:
        StringInputSource(const String& str);

    protected:
        virtual bool onImportText();

    private:
        StringStream _ss;
};


class PT_XML_API BinaryInputSource : public InputSource
{  
    public:
        BinaryInputSource();

        explicit BinaryInputSource(std::istream& is);

        explicit BinaryInputSource(XmlResolver& resolver);

        BinaryInputSource(XmlResolver& resolver, std::istream& is);

        void reset(std::istream& is);

    protected:
        virtual std::streamsize onImport();

        virtual int_type onGet();

        virtual bool onImportData();

    private:
        bool onParseBom(unsigned char c);

        bool onParseXml(int c);

        void onDeclaration();

    private:
        XmlResolver* _resolver;
        std::istream* _is;
        Utf8Codec _utf8Codec;
        TextBuffer _tbuf;
        XmlDeclaration _xmlDecl;
        MBState _mbState;
        unsigned char _bomState;
        unsigned char _bomEncoding;
        unsigned char _xmlState;
        const char* _pbBegin;
        const char* _pbEnd;
};


class NullInputSource : public InputSource
{
    public:
        explicit NullInputSource()
        : InputSource()
        { }

    protected:
        virtual std::streamsize onImport()
        { return -1; }

        virtual int_type onGet()
        { return std::char_traits<Char>::eof(); }
};

} // namespace Xml

} // namespace Pt

#endif
