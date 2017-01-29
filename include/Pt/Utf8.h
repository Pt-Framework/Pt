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

class PT_API Utf8Iterator
{
    public:
        typedef Char value_type;
        typedef std::ptrdiff_t difference_type;
        typedef Char* pointer;
        typedef const Char& reference;
        typedef std::input_iterator_tag iterator_category;

    public:
        Utf8Iterator()
        : _utf8(0)
        , _n(0)
        { }

        explicit Utf8Iterator(const char* utf8, std::size_t n)
        : _utf8(utf8)
        , _n(n)
        {
            decode();
        }

        explicit Utf8Iterator(const std::string& bytes)
        : _utf8(bytes.data())
        , _n(bytes.size())
        {
            decode();
        }

        Utf8Iterator(const Utf8Iterator& other)
        : _utf8(other._utf8)
        , _value(other._value)
        , _n(other._n)
        {}

        Utf8Iterator& operator=(const Utf8Iterator& other)
        {
            _utf8 = other._utf8;
            _value = other._value;
            _n = other._n;
            return *this;
        }

        Utf8Iterator& operator++()
        {
            if(_n == 0)
                _utf8 = 0;
            else
                decode();
            
            return *this;
        }

	    Utf8Iterator operator++(int)
		{
		    Utf8Iterator tmp = *this;
		    ++*this;
		    return tmp;
		}

        const Char& operator*() const
        { return _value; }

        bool operator!=(const Utf8Iterator& other) const
        { return _utf8 != other._utf8; }

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

class PT_API Utf8Appender
{
    public:
        typedef Char value_type;
        typedef std::ptrdiff_t difference_type;
        typedef Char* pointer;
        typedef const Char& reference;
        typedef std::output_iterator_tag iterator_category;

    public:
        explicit Utf8Appender(std::string& str)
        : _str(&str)
        { }

        Utf8Appender(const Utf8Appender& other)
        : _str(other._str)
        { }
        
        Utf8Appender& operator=(const Utf8Appender& other)
        { 
            _str = other._str;
            return *this;
        }

        Utf8Appender& operator=(const Char& ch)
        {
            encode(ch);
            return *this;
        }

        Utf8Appender& operator*()
        {
            return *this;
        }

        Utf8Appender& operator++()
        {
            return *this;
        }

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

class PT_API Utf8Convert
{
    public:
        typedef TextCodec<Char, char> CodecType;

    public:
        explicit Utf8Convert(TextCodec<Char, char>* codec);
        
        ~Utf8Convert();
        
        std::string toBytes(const char* utf8, std::size_t n);

        std::string toBytes(const std::string& bytes)
        {
            return toBytes(bytes.data(), bytes.size());
        }
        
        std::string fromBytes(const char* bytes, std::size_t n);

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
