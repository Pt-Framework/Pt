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
#include <Pt/Xml/ByteorderMark.h>
#include <Pt/Xml/XmlDeclaration.h>
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

/** @brief Input source for the XML reader.

    The %InputSource allows the XML readers to read input as an unicode
    character stream. It converts the encoding, if required, and supports
    blocking and non-blocking operation. Line information and the XML or text
    declaration are provided by each input source. Input sources are not only
    used for the main document, but also for entites and external DTD subsets.
*/
class InputSource : private NonCopyable
{
    public:
        typedef std::char_traits<Char>::int_type int_type;

    public:
        /** @brief Construct an empty %InputSource.
        */
        InputSource()
        : _rdbuf(0)
        , _line(1)
        , _decl(0)
        {}

        /** @brief Destructor.
        */
        virtual ~InputSource()
        {}

        /** @brief Current line count.
        */
        std::size_t line() const
        { return _line; }

        /** @brief Sets the current line count.
        */
        void setLine(std::size_t n)
        { _line = n; }

        /** @brief Import available bytes into buffer.

            If no more characters can be read directly from the input stream
            buffer without blocking read operations, the virtual method
            onInput() is called. The number of available characters is
            returned, which can be 0 if no data is available.
        */
        inline std::streamsize import()
        { 
            std::streamsize r = 0;
            return  (_rdbuf && (r =_rdbuf->in_avail()) > 0) ? r 
                                                            : onImport();
        }

        /** @brief Returns the next character or EOF.
        */
        inline int_type get()
        {
            return _rdbuf ? _rdbuf->sbumpc() 
                          : onGet();
        }

        /** @brief Returns the XML declaration or a nullptr if none was read.
        */
        const XmlDeclaration* declaration() const
        { return _decl; }

        /** @brief Returns the ID of the input stream.
        */
        const Pt::String& id() const
        { return onId(); }

    protected:
        /** @brief Initialize to use an input buffer and XML declaration.

            The derived input sources must call this method once the XML or
            text declaration is parsed or if none was found. Normally, this is
            done in the virtual methods onGet() and onImport(), which are 
            called by the public interface methods get() and import() when no
            buffer was set yet. The passed buffer and declaration are owned by
            the derived class.
        */
        void init(std::basic_streambuf<Char>* rdbuf = 0, XmlDeclaration* decl = 0)
        {
            _line = 0;
            _rdbuf = rdbuf;
            _decl = decl;
        }
        
        /** @brief Refills the stream buffer.
        */
        virtual std::streamsize onImport() = 0;

        /** @brief Returns the next character or EOF.
        */
        virtual int_type onGet() = 0;
        
        // NOTE: EntityStream returns a reference to the entity name
        /** @brief Returns the ID of the input stream.
        */
        virtual const Pt::String& onId() const = 0;

    private:
        std::basic_streambuf<Char>* _rdbuf;
        std::size_t _line;
        XmlDeclaration* _decl;
};

/** @brief Text input source for the XML reader.

    This input source can read characters from an unicode based input stream,
    so it does not convert the encoding, but only parses the XML declaration.
*/
class PT_XML_API TextInputSource : public InputSource
{
    public:
        /** @brief Construct an empty %TextInputSource.
        */
        TextInputSource();

        /** @brief Construct with input stream.
        */
        explicit TextInputSource(std::basic_istream<Char>& is);

        /** @brief Destructor.
        */
        ~TextInputSource();

        /** @brief Resets the input source and removes the input stream.
        */
        void reset();

        /** @brief Resets the input source to begin reading from a stream.
        */
        void reset(std::basic_istream<Char>& ios);     

        /** @brief Sets the ID.
        */
        void setId(const Pt::String& id);
        
    protected:
        // inherit docs
        virtual std::streamsize onImport();

        // inherit docs
        virtual int_type onGet();

        // inherit docs
        const Pt::String& onId() const;

        /** @brief Refill the character buffer.
        */
        virtual bool onImportText();

    private:
        //! @internal
        bool onParseXml(int_type c);

    private:
        std::basic_istream<Char>* _ios;
        XmlDeclaration _xmlDecl;
        Pt::String _id;
        unsigned char _xmlState;
        const char* _pbBegin;
        const char* _pbEnd;
};

/** @brief String input source for the XML reader

    This input source can read characters from an unicode string, so no
    character conversion is performed, but the XML declaration is parsed.
*/
class PT_XML_API StringInputSource : public TextInputSource
{
    public:
        /** @brief Construct from a string.
        */
        StringInputSource(const String& str);

        /** @brief Destructor.
        */
        ~StringInputSource();

    protected:
        // inherit docs
        virtual bool onImportText();

    private:
        StringStream _ss;
};

/** @brief Binary input source for the XML reader

    This input source reads from a byte stream and converts from the external
    character encoding to unicode depending on the byte order mark (BOM) and
    the encoding specified in the XML declaration. The XmlResolver is used
    to resolve encoding strings to text codecs. The underlying input stream
    and resolver must 
*/
class PT_XML_API BinaryInputSource : public InputSource
{  
    public:
        /** @brief Construct an empty input source.
        */
        BinaryInputSource();

        /** @brief Construct from byte stream.
        */
        explicit BinaryInputSource(std::istream& is);

        /** @brief Construct with resolver.
        */
        explicit BinaryInputSource(XmlResolver& resolver);

        /** @brief Construct with resolver and input stream.
        */
        BinaryInputSource(XmlResolver& resolver, std::istream& is);

        /** @brief Destructor.
        */
        ~BinaryInputSource();

        /** @brief Resets the input source and removes the input stream.
        */
        void reset();

        /** @brief Resets the input source to begin reading from a stream.
        */
        void reset(std::istream& is);

        /** @brief Sets the ID.
        */
        void setId(const Pt::String& id);

    protected:
        // inherit docs
        virtual std::streamsize onImport();

        // inherit docs
        virtual int_type onGet();

        // inherit docs
        const Pt::String& onId() const;

        /** @brief Refill the input buffer.
        */
        virtual bool onImportData();

    private:
        //! @internal
        bool onParseBom(unsigned char c);

        //! @internal
        bool onParseXml(int c);

        //! @internal
        void onDeclaration();

    private:
        XmlResolver* _resolver;
        std::istream* _is;
        Utf8Codec _utf8Codec;
        MBState _mbState;
        TextBuffer _tbuf;
        Pt::String _id;
        ByteorderMark _bom;
        XmlDeclaration _xmlDecl;
        unsigned char _bomState;
        unsigned char _xmlState;
        const char* _pbBegin;
        const char* _pbEnd;
};

/** @internal @brief Null input source for the XML reader.
*/
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

        const Pt::String& onId() const
        { return _id; }

    private:
        Pt::String _id;
};

} // namespace Xml

} // namespace Pt

#endif // Pt_Xml_InputSource_h
