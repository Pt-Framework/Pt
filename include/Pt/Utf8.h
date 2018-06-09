/* Copyright (C) 2016 Marc Boris Duerner 

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
   MA  02110-1301  USA
*/

#ifndef Pt_Utf8_h
#define Pt_Utf8_h

#include <Pt/Api.h>
#include <Pt/Utf8Codec.h>
#include <Pt/String.h>
#include <iterator>
#include <string>

namespace Pt {

/** @brief UTF-8 string input iterator.

    The %Utf8Iterator is a read-only input iterator, to iterate an UTF-8
    string as if it were a sequence of unicode characters. When used with
    standard C++ algorithms, the default constructed iterator serves as
    an iterator pointing to the end of the logic sequence.

    @code
    std::string utf8 = "Hell\303\266 W\303\266rld!";

    Pt::Utf8Iterator it(utf8);
    Pt::Utf8Iterator end;

    Char oe = 0366;
    int n = std::count(it, end, oe);
    assert(n == 2);
    @endcode
*/
class PT_API Utf8Iterator
{
    public:
        typedef Char value_type;
        typedef std::ptrdiff_t difference_type;
        typedef Char* pointer;
        typedef const Char& reference;
        typedef std::input_iterator_tag iterator_category;

    public:
        /** @brief Construct end iterator.
        */
        Utf8Iterator()
        : _utf8(0)
        , _n(0)
        { }

        /** @brief Construct from UTF-8 string data.
        */
        explicit Utf8Iterator(const char* utf8, std::size_t n)
        : _utf8(utf8)
        , _n(n)
        {
            decode();
        }
        
        /** @brief Construct from UTF-8 encoded string.
        */
        explicit Utf8Iterator(const std::string& bytes)
        : _utf8(bytes.data())
        , _n(bytes.size())
        {
            decode();
        }

        /** @brief Copy constructor.
        */
        Utf8Iterator(const Utf8Iterator& other)
        : _utf8(other._utf8)
        , _n(other._n)
        , _value(other._value)
        {}

        /** @brief Assignment operator.
        */
        Utf8Iterator& operator=(const Utf8Iterator& other)
        {
            _utf8 = other._utf8;
            _value = other._value;
            _n = other._n;
            return *this;
        }

        /** @brief Decodes next character.
        */
        Utf8Iterator& operator++()
        {
            if(_n == 0)
                _utf8 = 0;
            else
                decode();
            
            return *this;
        }
        
        /** @brief Decodes next character.
        */
	      Utf8Iterator operator++(int)
		    {
		        Utf8Iterator tmp = *this;
		        ++*this;
		        return tmp;
		    }

        /** @brief Returns current character.
        */
        const Char& operator*() const
        { return _value; }

        /** @brief Inequality comparison.
        */
        bool operator!=(const Utf8Iterator& other) const
        { return _utf8 != other._utf8; }

        /** @brief Equality comparison.
        */
        bool operator==(const Utf8Iterator& other) const
        { return _utf8 == other._utf8; }

    private:
        void decode();

    private:
        Utf8Codec   _codec;
        const char* _utf8;
        std::size_t _n;
        Char        _value;
};

/** @brief UTF-8 string output iterator.

    The %Utf8Appender is a single-pass output iterator, to append unicode
    characters to an UTF-8 encoded string. It can be used with standard C++
    algorithms.

    @code
    std::string s;
    Pt::Utf8Appender a(s);

    Char oe = 0366;
    std::fill_n(a, 3, oe);
    assert(s, "\303\266\303\266\303\266");
    @endcode

    @ingroup Unicode
*/
class PT_API Utf8Appender
{
    public:
        typedef Char value_type;
        typedef std::ptrdiff_t difference_type;
        typedef Char* pointer;
        typedef const Char& reference;
        typedef std::output_iterator_tag iterator_category;

    public:
        /** @brief Construct from UTF-8 encoded string.
        */
        explicit Utf8Appender(std::string& str)
        : _str(&str)
        { }

        /** @brief Copy constructor.
        */
        Utf8Appender(const Utf8Appender& other)
        : _str(other._str)
        { }
        
        /** @brief Assignment operator.
        */
        Utf8Appender& operator=(const Utf8Appender& other)
        { 
            _str = other._str;
            return *this;
        }

        /** @brief Encodes a unicode character to the target string.
        */
        Utf8Appender& operator=(const Char& ch)
        {
            encode(ch);
            return *this;
        }

        /** @brief No-op.
        */
        Utf8Appender& operator*()
        {
            return *this;
        }
        
        /** @brief No-op.
        */
        Utf8Appender& operator++()
        {
            return *this;
        }
        
        /** @brief No-op.
        */
        Utf8Appender operator++(int)
        {
            return *this;
        }

    private:
        void encode(const Char& ch);

    private:
        Utf8Codec   _codec;
        std::string* _str;
};

/** @brief UTF-8 string converter.

    %Utf8Convert converts between strings in external encodings and UTF-8.
    It uses a fixed internal buffer for the conversion, so temporary allocations
    are not performed. A Wave::TextCodec facet is used for the actual conversion.
    It can either be default constructed with new and ownership is passed to the
    %Utf8Convert or with a refcount greater than 0, which indicates ownership by
    the caller.

    @code
    // converter deletes codec
    Pt::Utf8Convert conv(new Pt::Latin1Codec);

    std::string latin1 = ...;
    std::string utf8 = conv.fromBytes(latin1);
    @endcode
    
    @code
    // converter doe not delete codec
    Pt::Latin1Codec codec(1);
    Pt::Utf8Convert conv(codec);

    string utf8 = ...;
    std::string latin1 = conv.toBytes(utf8);
    @endcode

    @ingroup Unicode
*/
class PT_API Utf8Convert
{
    public:
        typedef TextCodec<Char, char> CodecType;

    public:
        /** @brief Construct with codec.
        */
        explicit Utf8Convert(TextCodec<Char, char>* codec);
        
        /** @brief Destructor.
        */
        ~Utf8Convert();
        
        /** @brief Encode to external encoding.
        */
        std::string toBytes(const char* utf8, std::size_t n);

        /** @brief Encode to external encoding.
        */
        std::string toBytes(const std::string& bytes)
        {
            return toBytes(bytes.data(), bytes.size());
        }
        
        /** @brief Decode from external encoding.
        */
        std::string fromBytes(const char* bytes, std::size_t n);

        /** @brief Decode from external encoding.
        */
        std::string fromBytes(const std::string& bytes)
        {
            return fromBytes(bytes.data(), bytes.size());
        }

    private:
        Utf8Convert(const Utf8Convert&);
        Utf8Convert& operator=(const Utf8Convert&);
    
    private:
        TextCodec<Char, char>*   _codec;
        
        static const std::size_t _ibufSize = 16;
        Char                     _ibuf[_ibufSize];

        static const std::size_t _ebufSize = 32;
        char                     _ebuf[_ebufSize];
};

} //namespace Pt

#endif // include guard
