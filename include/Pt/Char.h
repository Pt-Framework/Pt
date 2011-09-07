/*
 * Copyright (C) 2005-2011 Marc Boris Duerner
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
#ifndef PT_CHAR_H
#define PT_CHAR_H

#include <Pt/Api.h>
#include <Pt/Types.h>
#include <string>
#include <cstring>
#include <cctype>
 
namespace Pt {

/**
 * @brief A lightweight Character class (32 bits).
 *
 * Unicode characters are 32-bit entities. This class represents such an entity. It is lightweight, so it
 * can be used everywhere. Most compilers treat it like an unsigned int of 32 bits.
 *
 * This class provides methods for testing/classification, converting to and from other formats, comparing
 * and case-converting. To convert a character or number to a instance of this class use one of the
 * constructors provided. To check the type of the character use one of the method starting with "is", like
 * isLetter() or isDigit(). To compare lower- or upper-case use isUpper() and isLower(). To compare two
 * characters the corresponding operators are overloaded accordingly. Addition and substraction is supported
 * as well. Comparison of numeric values (>, <, ==) is supported when using these operators.
 *
 * The classification methods operate on the full range of Unicode characters. All methods return $true$
 * if the character is a certain type of character. These methods are are wrappers around category() which
 * return the Unicode-defined category of each character.
 *
 * Comparison is critical in Unicode as it covers the characters of the entire world where characters which
 * look the same may be different in the thinking of numeric values (aka positions in the Unicode table)
 * Comparing characters will compare based purely on the numeric Unicode value (code point) of the characters.
 * Upper- and lower-casing using upper() and lower() will only work if the character has a well-defined
 * upper/lower-case equivalent.
 *
 * @see Category
 */
class Char
{
    public:
        //! Constructs a character with a value of 0.
        Char()
        : _value(0)
        {}

        //! Constructs a character using the given char as base for the character value.
        Char(char ch)
        : _value( (uint32_t)((unsigned char)ch) )
        {}

        //! Constructs a character using the given 8-bit char as base for the character value.
        Char(signed char ch)
        : _value( (uint32_t)((unsigned char)ch) )
        {}

        //! Constructs a character using the given char as base for the character value.
        Char(unsigned char ch)
        : _value( (uint32_t)(ch) )
        {}

        //! Constructs a character using the given wide-char as base for the character value.
        //Char(wchar_t ch)
        //: _value(ch)
        //{}

        //! Constructs a character using the given 16-bit integer as base for the character value.
        Char(short val)
        : _value( (uint32_t)((unsigned short)val) )
        {}

        //! Constructs a character using the given 16-bit integer as base for the character value.
        Char(unsigned short val)
        : _value( (uint32_t)(val) )
        {}

        //! Constructs a character using the given 32-bit integer as base for the character value.
        Char(int val)
        : _value( (uint32_t)(val) )
        {}

        //! Constructs a character using the given 32-bit integer as base for the character value.
        Char(unsigned int val)
        : _value(val)
        {}

        //! Constructs a character using the given long as base for the character value.
        Char(long value)
        : _value( (uint32_t)((unsigned long)value) )
        {}

        //! Constructs a character using the given long as base for the character value.
        Char(unsigned long val)
        : _value(val)
        {}

        /** @brief Narrows this character into an 8-bit char if possible.
         
            If the character can not be converted into an 8-bit char because its value is
            greater than 255, the defaultCharacter which is passed to this method is returned.

            If this character is equal or lower than 255 the character is cast to char.

            @param def The default character which is returned if this character can not be narrowed
            @return An 8-bit char which is a narrowed representation of this character object or
                    the default character if this character object's value is out of range (>255).
        */
        char narrow(char def = '?') const
        {
            if( _value <= 0xff )
                return (char)_value;

            if( _value == this->eofval() )
                return std::char_traits<char>::eof();

            return def;
        }

        /**
         * @brief Assigns the given uint32_t as new value for this character.
         * @param value The new 32-bit value for this character.
         * @return A reference to this object to allow concatination of operations.
         */
        Char& operator=(uint32_t value)
        { _value = value; return *this; }

        Char& operator=(const Char& ch)
        { _value = ch._value; return *this; }

        /**
         * @brief Returns the internal value (unsigned 32 bits) of this character.
         * @return The 32-bit-value of this character.
         */
        uint32_t value() const
        { return _value; }
        
        /**
         * @brief This conversion operator converts the internal value of this character to unsigned 32 bits.
         *
         * As the internal value also is an unsigned 32-bit value, the internal value of this character
         * ist returned.
         *
         * @return The character converted to unsigned 32-bit.
         */
        operator int() const
        { return _value; }

        /**
         * @brief Substracts the numeric value of this character and the numeric value of the given character and
         * stores the result in this chracater class.
         *
         * @param value This character's numeric value is subtracted from this' character numeric value.
         * @return A reference to this character class.
         */
        Char& operator-=(const Char& value)
        {
            this->_value -= value._value;
            return *this;
        }

        /**
         * @brief Sums the numeric value of this character and the numeric value of the given character and
         * stores the result in this chracater class.
         *
         * @param value This character's numeric value is added to this' character numeric value.
         * @return A reference to this character class.
         */
        Char& operator+=(const Char& value)
        {
            this->_value += value._value;
            return *this;
        }

        Char& operator>>=(const Char& value)
        {
            this->_value >>= value._value;
            return *this;
        }

        Char& operator<<=(const Char& value)
        {
            this->_value <<= value._value;
            return *this;
        }

        //! @brief Returns $true$ if the a and b are the same character; $false$ otherwise.
        //! @return $true$ if the a and b are the same character; $false$ otherwise.
        friend bool operator==(const Char& a, const Char& b)
        { return a.value() == b.value(); }

        //! @brief Returns $true$ if the a and b are the same character; $false$ otherwise.
        //! @return $true$ if the a and b are the same character; $false$ otherwise.
        friend bool operator==(const Char& a, char b)
        { return a.value() == (unsigned char)b; }

        //! @brief Returns $true$ if the a and b are the same character; $false$ otherwise.
        //! @return $true$ if the a and b are the same character; $false$ otherwise.
        friend bool operator==(const Char& a, int b)
        { return a.value() == (unsigned int)b; }

        //! @brief Returns $true$ if the a and b are not the same character; $false$ otherwise.
        //! @return $true$ if the a and b are not the same character; $false$ otherwise.
        friend bool operator!=(const Char& a, const Char& b)
        { return a.value() != b.value(); }

        //! @brief Returns $true$ if the a and b are the same character; $false$ otherwise.
        //! @return $true$ if the a and b are the same character; $false$ otherwise.
        friend bool operator!=(const Char& a, char b)
        { return a.value() != (unsigned char)b; }

        //! @brief Returns $true$ if the a and b are the same character; $false$ otherwise.
        //! @return $true$ if the a and b are the same character; $false$ otherwise.
        friend bool operator!=(const Char& a, wchar_t b)
        { return a.value() != (unsigned int)b; }

        //! @brief Returns $true$ if the a and b are the same character; $false$ otherwise.
        //! @return $true$ if the a and b are the same character; $false$ otherwise.
        friend bool operator!=(const Char& a, int b)
        { return a.value() != (unsigned int)b; }

        //! @brief Returns $true$ if the numeric value of a is less than the numeric value of b; $false$ otherwise.
        //! @return $true$ if the numeric value of a is less than the numeric value of b; $false$ otherwise.
        friend bool operator<(const Char& a, const Char& b)
        { return a.value() < b.value(); }

        //! @brief Returns $true$ if the numeric value of a is greater than the numeric value of b; $false$ otherwise.
        //! @return $true$ if the numeric value of a is greater than the numeric value of b; $false$ otherwise.
        friend bool operator>(const Char& a, const Char& b)
        { return a.value() > b.value(); }

        //! @brief Returns $true$ if the numeric value of a is equal or less than the numeric value of b; $false$ otherwise.
        //! @return $true$ if the numeric value of a is equal or less than the numeric value of b; $false$ otherwise.
        friend  bool operator<=(const Char& a, const Char& b)
        { return a.value() <= b.value(); }

        friend  bool operator<=(const Char& a, int b)
        { return a.value() <= static_cast<uint32_t>(b); }

        //! @brief Returns $true$ if the numeric value of a is equals or greater than the numeric value of b; $false$ otherwise.
        //! @return $true$ if the numeric value of a is equals or greater than the numeric value of b; $false$ otherwise.
        friend  bool operator>=(const Char& a, const Char& b)
        { return a.value() >= b.value(); }

        friend  bool operator>=(const Char& a, int b)
        { return a.value() >= static_cast<uint32_t>(b); }

        //! @brief Sums the numeric value of a and the numeric value of b and returns the sum.
        //! @return The sum of the numeric values of a and b.
        friend Char operator+(const Char& a, const Char& b)
        { return a.value() + b.value(); }

        //! @brief Sums the numeric value of a and the numeric value of b and returns the sum.
        //! @return The sum of the numeric values of a and b.
        friend Char operator+(const Char& a, char ch)
        { return a.value() + ch; }

        friend Char operator+(const Char& a, int ch)
        { return a.value() + ch; }

        //! @brief Subtracts the numeric value of b from the numeric value of b and returns the result.
        //! @return The substraction of the numeric values of b from a.
        friend Char operator-(const Char& a, const Char& b)
        { return a.value() - b.value(); }

        //! @brief Subtracts the numeric value of b from the numeric value of b and returns the result.
        //! @return The substraction of the numeric values of b from a.
        friend Char operator-(const Char& a, int ch)
        { return a.value() - ch; }

        //! @brief Subtracts the numeric value of b from the numeric value of b and returns the result.
        //! @return The substraction of the numeric values of b from a.
        friend Char operator-(const Char& a, char ch)
        { return a.value() - ch; }

        //! @brief Does an OR-combination of the numeric value of a and b and returns the result.
        //! @return The OR-combination of the numeric values of a and b.
        friend Char operator|(const Char& a, const Char& b)
        { return a.value() | b.value(); }

        //! @brief Does an AND-combination of the numeric value of a and b and returns the result.
        //! @return The AND-combination of the numeric values of a and b.
        friend Char operator&(const Char& a, const Char& b)
        { return a.value() & b.value(); }

        friend Char operator&(const Char& a, int b)
        { return a.value() & b; }

        friend Char operator<<(const Char& a, int b)
        { return a.value() << b; }

        static Char null()
        {
            return Char(0);
        }

        static Char eof()
        {
            return Pt::Char( Pt::uint32_t(-1) );
        }

        static Pt::uint32_t eofval()
        {
            return Pt::uint32_t(-1);
        }

    private:
        Pt::uint32_t _value;
};

struct MBState
{
    MBState()
    : n(0)
    {}

    int n;
    union {
        Pt::uint32_t wchars[4];
        char mbytes[16];
    } value;
};

} // namespace Pt

namespace std {

/// @cond INTERNAL
template<>
struct char_traits<Pt::Char>
{
    typedef Pt::Char char_type;
    typedef Pt::uint32_t int_type;
    typedef streamoff off_type;
    typedef streampos pos_type;
    typedef Pt::MBState state_type;

    inline static void assign(char_type& c1, const char_type& c2);

    inline static bool eq(const char_type& c1, const char_type& c2);

    inline static bool lt(const char_type& c1, const char_type& c2);

    inline static int compare(const char_type* c1, const char_type* c2, size_t n);

    inline static size_t length(const char_type* s);

    inline static const char_type* find(const char_type* s, size_t n, const char_type& a);

    inline static char_type* move(char_type* s1, const char_type* s2, size_t n);

    inline static char_type* copy(char_type* s1, const char_type* s2, size_t n);

    inline static char_type* assign(char_type* s, size_t n, char_type a);

    inline static char_type to_char_type(const int_type& c);

    inline static int_type to_int_type(const char_type& c);

    inline static bool eq_int_type(const int_type& c1, const int_type& c2);

    inline static int_type eof();

    inline static int_type not_eof(const int_type& c);
};

inline void char_traits<Pt::Char>::assign(char_type& c1, const char_type& c2)
{
    c1 = c2;
}

inline bool char_traits<Pt::Char>::eq(const char_type& c1, const char_type& c2)
{
    return c1 == c2;
}

inline bool char_traits<Pt::Char>::lt(const char_type& c1, const char_type& c2)
{
    return c1 < c2;
}

inline int char_traits<Pt::Char>::compare(const char_type* s1, const char_type* s2, size_t n)
{
    while(n-- > 0)
    {
        if( !eq(*s1, *s2) )
            return lt(*s1, *s2) ? -1 : +1;

        ++s1;
        ++s2;
    }

    return 0;
}

inline size_t char_traits<Pt::Char>::length(const char_type* s)
{
    static const Pt::Char term(0);
    std::size_t n = 0;
    while( !eq(s[n], term) )
        ++n;

    return n;
}

inline const char_traits<Pt::Char>::char_type*
char_traits<Pt::Char>::find(const char_type* s, size_t n, const char_type& a)
{
    while(n-- > 0) {
        if (*s == a)
            return s;
        ++s;
    }

    return 0;
}

inline char_traits<Pt::Char>::char_type*
char_traits<Pt::Char>::move(char_type* s1, const char_type* s2, size_t n)
{
    return (Pt::Char*)std::memmove(s1, s2, n * sizeof(Pt::Char));
}

inline char_traits<Pt::Char>::char_type*
char_traits<Pt::Char>::copy(char_type* s1, const char_type* s2, size_t n)
{
    return (Pt::Char*)std::memcpy(s1, s2, n * sizeof(Pt::Char));
}

inline char_traits<Pt::Char>::char_type*
char_traits<Pt::Char>::assign(char_type* s, size_t n, char_type a)
{
    while(n-- > 0) {
        *(s++) = a;
    }

    return s;
}

inline char_traits<Pt::Char>::char_type
char_traits<Pt::Char>::to_char_type(const int_type& c)
{
    return char_type(c);
}

inline char_traits<Pt::Char>::int_type
char_traits<Pt::Char>::to_int_type(const char_type& c)
{
    return c.value();
}

inline bool char_traits<Pt::Char>::eq_int_type(const int_type& c1, const int_type& c2)
{
    return c1 == c2;
}

inline char_traits<Pt::Char>::int_type char_traits<Pt::Char>::eof()
{
    return Pt::Char::eof().value();
}

inline char_traits<Pt::Char>::int_type char_traits<Pt::Char>::not_eof(const int_type& c)
{
    return eq_int_type(c, eof()) ? 0 : c;
}

} // namespace std

#include <Pt/Locale.h>

namespace Pt {

PT_API std::ctype_base::mask ctypeMask(const Char& ch);

inline int isalpha(const Char& ch)
{
    return ctypeMask(ch) & std::ctype_base::alpha;
}

inline int isalnum(const Char& ch)
{
    return ctypeMask(ch) & std::ctype_base::alnum;
}

inline int ispunct(const Char& ch)
{
    return ctypeMask(ch) & std::ctype_base::punct;
}

inline int iscntrl(const Char& ch)
{
    return ctypeMask(ch) & std::ctype_base::cntrl;
}

inline int isdigit(const Pt::Char& ch)
{
    return ctypeMask(ch) & std::ctype_base::digit;
}

inline int isxdigit(const Char& ch)
{
    return ctypeMask(ch) & std::ctype_base::xdigit;
}

inline int isgraph(const Char& ch)
{
    return ctypeMask(ch) & std::ctype_base::graph;
}

inline int islower(const Pt::Char& ch)
{
    return ctypeMask(ch) & std::ctype_base::lower;
}

inline int isupper(const Char& ch)
{
    return ctypeMask(ch) & std::ctype_base::upper;
}

inline int isprint(const Char& ch)
{
    return ctypeMask(ch) & std::ctype_base::print;
}

inline int isspace(const Char& ch)
{
    return ctypeMask(ch) & std::ctype_base::space;
}

PT_API Pt::Char tolower(const Pt::Char& ch);

PT_API Pt::Char toupper(const Pt::Char& ch);

} // namespace Pt

#include <Pt/String.h>

#endif
