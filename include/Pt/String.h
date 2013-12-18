/*
 * Copyright (C) 2004-2007 Marc Boris Duerner
 * Copyright (C) 2011 Tommi Maekitalo
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

#ifndef PT_STRING_H
#define PT_STRING_H

#include <Pt/Api.h>
#include <Pt/Types.h>
#include <Pt/Locale.h>
#include <string>
#include <iterator>
#include <stdexcept>
#include <cstring>
#include <cctype>
#include <cstddef>

namespace Pt {

/** @brief Unicode character type.
 
    @ingroup Unicode
 */
class Char
{
    public:
        //! @brief Default Constructor.
        Char()
        : _value(0)
        {}

        //! @brief Construct from char.
        Char(char ch)
        : _value( (unsigned char)ch )
        {}

        //! @brief Construct from int.
        Char(int val)
        : _value( (unsigned)(val) )
        {}

        //! @brief Construct from unsigned 32-bit integer.
        Char(uint32_t val)
        : _value(val)
        {}

        //! @brief Returns the unicode value.
        uint32_t value() const
        { return _value; }
        
        //! @brief Returns the unicode value.
        operator uint32_t() const
        { return _value; }

        //! @brief Assignment operator.
        Char& operator=(const Char& ch)
        { 
            _value = ch._value; 
            return *this; 
        }

        /** @brief Narrows the character to 8-bit.
         
            The default character \a def is returned if the unicode value
            is too large to be narrowed to char i.e. greater than 255. 
        */
        char narrow(char def = '?') const
        {
            return _value > 0xff ? def : static_cast<char>(_value);
        }

    private:
        Pt::uint32_t _value;
};

//inline bool operator ==(const Char& a, const Char& b)
//{ return a.value() == b.value(); }

//inline bool operator !=(const Char& a, const Char& b)
//{ return a.value() != b.value(); }

//inline bool operator >(const Char& a, const Char& b)
//{ return a.value() > b.value(); }

//nline bool operator >=(const Char& a, const Char& b)
//{ return a.value() >= b.value(); }

//inline bool operator <(const Char& a, const Char& b)
//{ return a.value() < b.value(); }

//inline bool operator <=(const Char& a, const Char& b)
//{ return a.value() <= b.value(); }

//inline Pt::uint32_t operator -(const Char& a, const Char& b)
//{ return a.value() - b.value(); }

//! @internal @brief Returns the ctype mask for the \a ch.
PT_API std::ctype_base::mask ctypeMask(const Char& ch);

/** @brief Checks whether @a ch is a alphabetic character.

    @ingroup Unicode
*/
inline int isalpha(const Char& ch)
{
    return ctypeMask(ch) & std::ctype_base::alpha;
}

/** @brief Checks whether @a ch is a alphanumeric character.

    @ingroup Unicode
*/
inline int isalnum(const Char& ch)
{
    return ctypeMask(ch) & std::ctype_base::alnum;
}

/** @brief Checks whether @a ch is a punctuation character.

    @ingroup Unicode
*/
inline int ispunct(const Char& ch)
{
    return ctypeMask(ch) & std::ctype_base::punct;
}

/** @brief Checks whether @a ch is a control character.

    @ingroup Unicode
*/
inline int iscntrl(const Char& ch)
{
    return ctypeMask(ch) & std::ctype_base::cntrl;
}

/** @brief Checks whether @a ch is a decimal digit.

    @ingroup Unicode
*/
inline int isdigit(const Pt::Char& ch)
{
    return ctypeMask(ch) & std::ctype_base::digit;
}

/** @brief Checks whether @a ch is a hexadecimal digit.

    @ingroup Unicode
*/
inline int isxdigit(const Char& ch)
{
    return ctypeMask(ch) & std::ctype_base::xdigit;
}

/** @brief Checks whether @a ch is a graphical character.

    @ingroup Unicode
*/
inline int isgraph(const Char& ch)
{
    return ctypeMask(ch) & std::ctype_base::graph;
}

/** @brief Checks whether @a ch is lower case.

    @ingroup Unicode
*/
inline int islower(const Pt::Char& ch)
{
    return ctypeMask(ch) & std::ctype_base::lower;
}

/** @brief Checks whether @a ch is upper case.

    @ingroup Unicode
*/
inline int isupper(const Char& ch)
{
    return ctypeMask(ch) & std::ctype_base::upper;
}

/** @brief Checks whether @a ch is a printable character.

    @ingroup Unicode
*/
inline int isprint(const Char& ch)
{
    return ctypeMask(ch) & std::ctype_base::print;
}

/** @brief Checks whether @a ch is a whitespace character.

    @ingroup Unicode
*/
inline int isspace(const Char& ch)
{
    return ctypeMask(ch) & std::ctype_base::space;
}

/** @brief Convert a character to lower case.

    @ingroup Unicode
*/
PT_API Pt::Char tolower(const Pt::Char& ch);

/** @brief Convert a character to upper case.

    @ingroup Unicode
*/
PT_API Pt::Char toupper(const Pt::Char& ch);


/** @internal @brief Multi-byte conversion state.
*/
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

/** @internal @brief Character traits for the basic string specialization
*/
template<>
struct char_traits<Pt::Char>
{
    typedef Pt::Char char_type;
    typedef Pt::uint32_t int_type;
    typedef streamoff off_type;
    typedef streampos pos_type;
    typedef Pt::MBState state_type;

    inline static void assign(char_type& c1, const char_type& c2)
    {
        c1 = c2;
    }

    inline static bool eq(const char_type& c1, const char_type& c2)
    {
        return c1 == c2;
    }

    inline static bool lt(const char_type& c1, const char_type& c2)
    {
        return c1 < c2;
    }

    inline static int compare(const char_type* s1, const char_type* s2, std::size_t n)
    {
        while(n-- > 0)
        {
            if( ! eq(*s1, *s2) )
                return lt(*s1, *s2) ? -1 : +1;

            ++s1;
            ++s2;
        }

        return 0;
    }

    inline static std::size_t length(const char_type* s)
    {
        const Pt::Char term(0);
        
        std::size_t n = 0;
        while( ! eq(s[n], term) )
            ++n;

        return n;
    }

    inline static const char_type* find(const char_type* s, std::size_t n, const char_type& a)
    {
        while(n-- > 0) 
        {
            if (*s == a)
                return s;

            ++s;
        }

        return 0;
    }

    inline static char_type* move(char_type* s1, const char_type* s2, std::size_t n)
    {
        return (Pt::Char*) std::memmove(s1, s2, n * sizeof(Pt::Char));
    }

    inline static char_type* copy(char_type* s1, const char_type* s2, std::size_t n)
    {
        return (Pt::Char*) std::memcpy(s1, s2, n * sizeof(Pt::Char));
    }

    inline static char_type* assign(char_type* s, std::size_t n, char_type a)
    {
        while(n-- > 0)
            *(s++) = a;

        return s;
    }

    inline static char_type to_char_type(const int_type& c)
    {
        return char_type(c);
    }

    inline static int_type to_int_type(const char_type& c)
    {
        return c.value();
    }

    inline static bool eq_int_type(const int_type& c1, const int_type& c2)
    {
        return c1 == c2;
    }

    inline static int_type eof()
    {
        return Pt::uint32_t(-1);
    }

    inline static int_type not_eof(const int_type& c)
    {
        return eq_int_type(c, eof()) ? 0 : c;
    }
};

/** @brief Unicode capable strings.

    @ingroup Unicode
*/
template <>
class PT_API basic_string<Pt::Char> 
{
    public:
        typedef Pt::Char value_type;
        typedef std::size_t size_type;
        typedef char_traits< Pt::Char > traits_type;
        typedef std::allocator<Pt::Char> allocator_type;
        typedef allocator_type::difference_type difference_type;
        typedef allocator_type::reference reference;
        typedef allocator_type::const_reference const_reference;
        typedef allocator_type::pointer pointer;
        typedef allocator_type::const_pointer const_pointer;
        typedef value_type* iterator;
        typedef const value_type* const_iterator;

#ifdef _RWSTD_NO_CLASS_PARTIAL_SPEC
        typedef std::reverse_iterator<const_iterator,
                                      random_access_iterator_tag, value_type>
                                      const_reverse_iterator;

        typedef std::reverse_iterator<iterator,
                                      random_access_iterator_tag, value_type>
                                      reverse_iterator;
#else
        typedef std::reverse_iterator<iterator> reverse_iterator;
        typedef const std::reverse_iterator<const_iterator> const_reverse_iterator;
#endif

        static const size_type npos = static_cast<size_type>(-1);

    public:
        explicit basic_string( const allocator_type& a = allocator_type());

        basic_string(const Pt::Char* str, const allocator_type& a = allocator_type());

        basic_string(const Pt::Char* str, size_type n, const allocator_type& a = allocator_type());

        basic_string(const wchar_t* str, const allocator_type& a = allocator_type());

        basic_string(const wchar_t* str, size_type n, const allocator_type& a = allocator_type());

        basic_string(const char* str, const allocator_type& a = allocator_type());

        basic_string(const char* str, size_type n, const allocator_type& a = allocator_type());

        basic_string(size_type n, Pt::Char c, const allocator_type& a = allocator_type());

        basic_string(const basic_string& str);

        basic_string(const basic_string& str, const allocator_type& a);

        basic_string(const basic_string& str, size_type pos, size_type n = npos, const allocator_type& a = allocator_type());

        basic_string(const Pt::Char* begin, const Pt::Char* end, const allocator_type& a = allocator_type());

        template <typename InputIterator>
        basic_string(InputIterator begin, InputIterator end, const allocator_type& a = allocator_type());

        ~basic_string();

    public:
        iterator begin()
        { return privdata_rw(); }

        iterator end()
        { return privdata_rw() + length(); }

        const_iterator begin() const
        { return privdata_ro(); }

        const_iterator end() const
        { return privdata_ro() + length(); }

        reverse_iterator rbegin()
        { return reverse_iterator( this->end() ); }

        reverse_iterator rend()
        { return reverse_iterator( this->begin() ); }

        const_reverse_iterator rbegin() const
        { return const_reverse_iterator( this->end() ); }

        const_reverse_iterator rend()   const
        { return const_reverse_iterator( this->begin() ); }

        reference operator[](size_type n)
        { return privdata_rw()[n]; }

        const_reference operator[](size_type n) const
        { return privdata_ro()[n]; }

        reference at(size_type n)
        { 
            if( n >= size() )
                throw out_of_range("at");

            return privdata_rw()[n]; 
        }

        const_reference at(size_type n) const
        { 
            if( n >= size() )
                throw out_of_range("at");
            
            return privdata_ro()[n]; 
        }

    public:
        void push_back(Pt::Char ch)
        { (*this) += ch; }

        void resize( std::size_t n, Pt::Char ch = value_type() );

        void reserve(std::size_t n = 0);

        void swap(basic_string& str);

        allocator_type get_allocator() const
        { return _d; }

        size_type copy(Pt::Char* a, size_type n, size_type pos = 0) const;

        basic_string substr(size_type pos = 0, size_type n = npos) const
        { return basic_string(*this, pos, n); }

    public:
        size_type length() const
        { return isShortString() ? shortStringLength() : longStringLength(); }

        size_type size() const
        { return length(); }

        bool empty() const
        { return length() == 0; }

        size_type max_size() const
        { return ( size_type(-1) / sizeof(Pt::Char) ) - 1; }

        size_type capacity() const
        { return isShortString() ? shortStringCapacity() : longStringCapacity(); }

        const Pt::Char* data() const
        { return privdata_ro(); }

        const Pt::Char* c_str() const
        { return privdata_ro(); }

        basic_string& assign(const basic_string& str);

        basic_string& assign(const basic_string& str, size_type pos, size_type n);

        basic_string& assign(const wchar_t* str);

        basic_string& assign(const wchar_t* str, size_type n);

        basic_string& assign(const Pt::Char* str);

        basic_string& assign(const Pt::Char* str, size_type length);

        basic_string& assign(const char* str);

        basic_string& assign(const char* str, size_type length);

        basic_string& assign(size_type n, Pt::Char c);

        template <typename InputIterator>
        basic_string& assign(InputIterator begin, InputIterator end);

        basic_string& append(const Pt::Char* str);

        basic_string& append(const Pt::Char* str, size_type n);

        basic_string& append(size_type n, Pt::Char ch);

        basic_string& append(const basic_string& str);

        basic_string& append(const basic_string& str, size_type pos, size_type n);

        template <typename InputIterator>
        basic_string& append(InputIterator begin, InputIterator end);

        basic_string& append(const Pt::Char* begin, const Pt::Char* end);

        basic_string& insert(size_type pos, const Pt::Char* str);

        basic_string& insert(size_type pos, const Pt::Char* str, size_type n);

        basic_string& insert(size_type pos, size_type n, Pt::Char ch);

        basic_string& insert(size_type pos, const basic_string& str);

        basic_string& insert(size_type pos, const basic_string& str, size_type pos2, size_type n);

        basic_string& insert(iterator p, Pt::Char ch);

        basic_string& insert(iterator p, size_type n, Pt::Char ch);

        // unimplemented
        //template <typename InputIterator>
        //basic_string& insert(iterator p, InputIterator first, InputIterator last);

        void clear()
        { setLength(0); }

        basic_string& erase(size_type pos = 0, size_type n = npos);

        iterator erase(iterator pos);

        iterator erase(iterator first, iterator last);

        basic_string& replace(size_type pos, size_type n, const Pt::Char* str);

        basic_string& replace(size_type pos, size_type n, const Pt::Char* str, size_type n2);

        basic_string& replace(size_type pos, size_type n, size_type n2, Pt::Char ch);

        basic_string& replace(size_type pos, size_type n, const basic_string& str);

        basic_string& replace(size_type pos, size_type n, const basic_string& str, size_type pos2, size_type n2);

        basic_string& replace(iterator i1, iterator i2, const Pt::Char* str);

        basic_string& replace(iterator i1, iterator i2, const Pt::Char* str, size_type n);

        basic_string& replace(iterator i1, iterator i2, size_type n, Pt::Char ch);

        basic_string& replace(iterator i1, iterator i2, const basic_string& str);

        int compare(const basic_string& str) const;

        int compare(const Pt::Char* str) const;

        int compare(const Pt::Char* str, size_type n) const;

        int compare(const wchar_t* str) const;

        int compare(const wchar_t* str, size_type n) const;

        int compare(const char* str) const;

        int compare(const char* str, size_type n) const;

        int compare(size_type pos, size_type n, const basic_string& str) const;

        int compare(size_type pos, size_type n, const basic_string& str, size_type pos2, size_type n2) const;

        int compare(size_type pos, size_type n, const Pt::Char* str) const;

        int compare(size_type pos, size_type n, const Pt::Char* str, size_type n2) const;

        size_type find(const basic_string& str, size_type pos = 0) const;

        size_type find(const Pt::Char* str, size_type pos, size_type n) const;

        size_type find(const Pt::Char* str, size_type pos = 0) const;

        size_type find(Pt::Char ch, size_type pos = 0) const;

        size_type rfind(const basic_string& str, size_type pos = npos) const;

        size_type rfind(const Pt::Char* str, size_type pos, size_type n) const;

        size_type rfind(const Pt::Char* str, size_type pos = npos) const;

        size_type rfind(Pt::Char ch, size_type pos = npos) const;

        size_type find_first_of(const basic_string& str, size_type pos = 0) const
        { return this->find_first_of( str.data(), pos, str.size() ); }

        size_type find_first_of(const Pt::Char* s, size_type pos, size_type n) const;

        size_type find_first_of(const Pt::Char* str, size_type pos = 0) const
        { return this->find_first_of( str, pos, traits_type::length(str) ); }

        size_type find_first_of(const Pt::Char ch, size_type pos = 0) const
        { return this->find(ch, pos); }

        size_type find_last_of(const basic_string& str, size_type pos = npos) const
        { return this->find_last_of( str.data(), pos, str.size() ); }

        size_type find_last_of(const Pt::Char* s, size_type pos, size_type n) const;

        size_type find_last_of(const Pt::Char* str, size_type pos = npos) const
        { return this->find_last_of( str, pos, traits_type::length(str) ); }

        size_type find_last_of(const Pt::Char ch, size_type pos = npos) const
        { return this->rfind(ch, pos); }

        size_type find_first_not_of(const basic_string& str, size_type pos = 0) const
        { return this->find_first_not_of( str.data(), pos, str.size() ); }

        size_type find_first_not_of(const Pt::Char* s, size_type pos, size_type n) const;

        size_type find_first_not_of(const Pt::Char* str, size_type pos = 0) const
        { return this->find_first_not_of( str, pos, traits_type::length(str) ); }

        size_type find_first_not_of(const Pt::Char ch, size_type pos = 0) const;

        size_type find_last_not_of(const basic_string& str, size_type pos = npos) const
        { return this->find_last_not_of( str.data(), pos, str.size() ); }

        size_type find_last_not_of(const Pt::Char* tok, size_type pos, size_type n) const;

        size_type find_last_not_of(const Pt::Char* str, size_type pos = npos) const
        { return this->find_last_not_of( str, pos, traits_type::length(str) ); }

        size_type find_last_not_of(Pt::Char ch, size_type pos = npos) const;

    public:
        std::string narrow(char dfault = '?') const;

        static basic_string widen(const char* str);

        static basic_string widen(const std::string& str);

        template <typename OutIterT>
        OutIterT toUtf16(OutIterT to) const;

        template <typename InIterT>
        static basic_string fromUtf16(InIterT from, InIterT fromEnd);

    public:
        basic_string& operator=(const basic_string& str)
        { return this->assign(str); }

        basic_string& operator=(const wchar_t* str)
        { return this->assign(str); }

        basic_string& operator=(const char* str)
        { return this->assign(str); }

        basic_string& operator=(const Pt::Char* str)
        { return this->assign(str); }

        basic_string& operator=(Pt::Char ch)
        {
            // no privreserve(1), short string capacity is large enough

            Pt::Char* p = privdata_rw();
            p[0] = ch;
            setLength(1);
            return *this;
        }

        basic_string& operator+=(const basic_string& str)
        { return this->append(str); }

        basic_string& operator+=(const Pt::Char* str)
        { return this->append(str); }

        basic_string& operator+=(Pt::Char c);

    private:
        struct Ptr
        {
            Pt::Char* _begin;
            Pt::Char* _end;
            Pt::Char* _capacity;
        };

        // minimum possible short string character count
        static const unsigned _minN = (sizeof(Ptr) / sizeof(Pt::Char)) + 1;

        // short string character count
        static const unsigned _nN = _minN < 8 ? 8 : _minN;

        // short string raw storage size in bytes
        static const unsigned _nS = _nN * sizeof(Pt::Char);

        struct Data : public allocator_type
        {
            Data(const allocator_type& a)
            : allocator_type(a)
            {
                Pt::Char* str = reinterpret_cast<Pt::Char*>(&_u._s[0]);
                *str = 0;
                
                _u._s[_nS - 1] = _nN - 1;
            }

            union
            {
                Ptr _p;
                unsigned char _s[_nS];
            } _u;
        } _d;

    private:
        const Pt::Char* privdata_ro() const
        { return isShortString() ? shortStringData() : longStringData(); }
        
        Pt::Char* privdata_rw()
        { return isShortString() ? shortStringData() : longStringData(); }

        void privreserve(std::size_t n);

        bool isShortString() const                    
        { return shortStringMagic() != 0xff; }
        
        void markLongString()                         
        { shortStringMagic() = 0xff; }
        
        const Pt::Char* shortStringData() const       
        { return reinterpret_cast<const Pt::Char*>(&_d._u._s[0]); }
        
        Pt::Char* shortStringData()                   
        { return reinterpret_cast<Pt::Char*>(&_d._u._s[0]); }
        
        unsigned char shortStringMagic() const        
        { return _d._u._s[_nS - 1]; }
        
        unsigned char& shortStringMagic()              
        { return _d._u._s[_nS - 1]; }
        
        size_type shortStringLength() const           
        { return _nN - 1 - shortStringMagic(); }
        
        size_type shortStringCapacity() const         
        { return _nN - 1; }
        
        void setShortStringLength(size_type n)        
        { 
            shortStringData()[n] = Pt::Char(0); 
            shortStringMagic() = static_cast<unsigned char>(_nN - n - 1); 
        }
        
        void shortStringAssign(const Pt::Char* str, size_type n)
        {
            traits_type::copy(shortStringData(), str, n);
            shortStringData()[n] = Pt::Char(0);
            shortStringMagic() = static_cast<unsigned char>(_nN - n - 1);
        }
        void shortStringAssign(const wchar_t* str, size_type n)
        {
            for (size_type nn = 0; nn < n; ++nn)
                shortStringData()[nn] = str[nn];
            shortStringData()[n] = Pt::Char(0);
            shortStringMagic() = static_cast<unsigned char>(_nN - n - 1);
        }

        const Pt::Char* longStringData() const          
        { return _d._u._p._begin; }
        
        Pt::Char* longStringData()                      
        { return _d._u._p._begin; }
        
        size_type longStringLength() const              
        { return _d._u._p._end - _d._u._p._begin; }
        
        size_type longStringCapacity() const            
        { return _d._u._p._capacity - _d._u._p._begin; }
        
        void setLength(size_type n)
        {
            if (isShortString())
                setShortStringLength(n);
            else
            {
                _d._u._p._end = _d._u._p._begin + n;
                _d._u._p._begin[n] = 0;
            }
        }
};

// swap
inline void swap(basic_string<Pt::Char>& a, basic_string<Pt::Char>& b)
{ a.swap(b); }

// operator +
inline basic_string<Pt::Char> operator+(const basic_string<Pt::Char>& a, const basic_string<Pt::Char>& b)
{ basic_string<Pt::Char> temp; temp += a; temp += b; return temp; }

inline basic_string<Pt::Char> operator+(const basic_string<Pt::Char>& a, const Pt::Char* b)
{ basic_string<Pt::Char> temp; temp += a; temp += b; return temp; }

inline basic_string<Pt::Char> operator+(const Pt::Char* a, const basic_string<Pt::Char>& b)
{ basic_string<Pt::Char> temp; temp += a; temp += b; return temp; }

inline basic_string<Pt::Char> operator+(const basic_string<Pt::Char>& a, Pt::Char b)
{ basic_string<Pt::Char> temp; temp += a; temp += b; return temp; }

inline basic_string<Pt::Char> operator+(Pt::Char a, const basic_string<Pt::Char>& b)
{ basic_string<Pt::Char> temp; temp += a; temp += b; return temp; }

// operator ==
inline bool operator==(const basic_string<Pt::Char>& a, const basic_string<Pt::Char>& b)
{ return a.compare(b) == 0; }

inline bool operator==(const Pt::Char* a, const basic_string<Pt::Char>& b)
{ return b.compare(a) == 0; }

inline bool operator==(const basic_string<Pt::Char>& a, const Pt::Char* b)
{ return a.compare(b) == 0; }

inline bool operator==(const basic_string<Pt::Char>& a, const wchar_t* b)
{ return a.compare(b) == 0; }

inline bool operator==(const wchar_t* b, const basic_string<Pt::Char>& a)
{ return a.compare(b) == 0; }

inline bool operator==(const basic_string<Pt::Char>& a, const char* b)
{ return a.compare(b) == 0; }

inline bool operator==(const char* b, const basic_string<Pt::Char>& a)
{ return a.compare(b) == 0; }

// operator !=
inline bool operator!=(const basic_string<Pt::Char>& a, const basic_string<Pt::Char>& b)
{ return a.compare(b) != 0; }

inline bool operator!=(const Pt::Char* a, const basic_string<Pt::Char>& b)
{ return b.compare(a) != 0; }

inline bool operator!=(const basic_string<Pt::Char>& a, const Pt::Char* b)
{ return a.compare(b) != 0; }

inline bool operator!=(const basic_string<Pt::Char>& a, const wchar_t* b)
{ return a.compare(b) != 0; }

inline bool operator!=(const wchar_t* b, const basic_string<Pt::Char>& a)
{ return a.compare(b) != 0; }

inline bool operator!=(const basic_string<Pt::Char>& a, const char* b)
{ return a.compare(b) != 0; }

inline bool operator!=(const char* b, const basic_string<Pt::Char>& a)
{ return a.compare(b) != 0; }

// operator <
inline bool operator<(const basic_string<Pt::Char>& a, const basic_string<Pt::Char>& b)
{ return a.compare(b) < 0; }

inline bool operator<(const Pt::Char* a, const basic_string<Pt::Char>& b)
{ return b.compare(a) > 0; }

inline bool operator<(const basic_string<Pt::Char>& a, const Pt::Char* b)
{ return a.compare(b) < 0; }

inline bool operator<(const basic_string<Pt::Char>& a, const wchar_t* b)
{ return a.compare(b) < 0; }

inline bool operator<(const wchar_t* b, const basic_string<Pt::Char>& a)
{ return a.compare(b) > 0; }

inline bool operator<(const basic_string<Pt::Char>& a, const char* b)
{ return a.compare(b) < 0; }

inline bool operator<(const char* b, const basic_string<Pt::Char>& a)
{ return a.compare(b) > 0; }

// operator <=
inline bool operator<=(const basic_string<Pt::Char>& a, const basic_string<Pt::Char>& b)
{ return a.compare(b) <= 0; }

inline bool operator<=(const Pt::Char* a, const basic_string<Pt::Char>& b)
{ return b.compare(a) >= 0; }

inline bool operator<=(const basic_string<Pt::Char>& a, const Pt::Char* b)
{ return a.compare(b) <= 0; }

inline bool operator<=(const basic_string<Pt::Char>& a, const wchar_t* b)
{ return a.compare(b) <= 0; }

inline bool operator<=(const wchar_t* b, const basic_string<Pt::Char>& a)
{ return a.compare(b) >= 0; }

inline bool operator<=(const basic_string<Pt::Char>& a, const char* b)
{ return a.compare(b) <= 0; }

inline bool operator<=(const char* b, const basic_string<Pt::Char>& a)
{ return a.compare(b) >= 0; }

// operator >
inline bool operator>(const basic_string<Pt::Char>& a, const basic_string<Pt::Char>& b)
{ return a.compare(b) > 0; }

inline bool operator>(const Pt::Char* a, const basic_string<Pt::Char>& b)
{ return b.compare(a) < 0; }

inline bool operator>(const basic_string<Pt::Char>& a, const Pt::Char* b)
{ return a.compare(b) > 0; }

inline bool operator>(const basic_string<Pt::Char>& a, const wchar_t* b)
{ return a.compare(b) > 0; }

inline bool operator>(const wchar_t* b, const basic_string<Pt::Char>& a)
{ return a.compare(b) < 0; }

inline bool operator>(const basic_string<Pt::Char>& a, const char* b)
{ return a.compare(b) > 0; }

inline bool operator>(const char* b, const basic_string<Pt::Char>& a)
{ return a.compare(b) < 0; }

// operator >=
inline bool operator>=(const basic_string<Pt::Char>& a, const basic_string<Pt::Char>& b)
{ return a.compare(b) >= 0; }

inline bool operator>=(const Pt::Char* a, const basic_string<Pt::Char>& b)
{ return b.compare(a) <= 0; }

inline bool operator>=(const basic_string<Pt::Char>& a, const Pt::Char* b)
{ return a.compare(b) >= 0; }

inline bool operator>=(const basic_string<Pt::Char>& a, const wchar_t* b)
{ return a.compare(b) >= 0; }

inline bool operator>=(const wchar_t* b, const basic_string<Pt::Char>& a)
{ return a.compare(b) <= 0; }

inline bool operator>=(const basic_string<Pt::Char>& a, const char* b)
{ return a.compare(b) >= 0; }

inline bool operator>=(const char* b, const basic_string<Pt::Char>& a)
{ return a.compare(b) <= 0; }

// operator <<
PT_API ostream& operator<< (ostream& out, const basic_string<Pt::Char>& str);

} // namespace std

namespace Pt {

/** @brief Unicode capable strings
    @ingroup Unicode
*/
typedef std::basic_string<Pt::Char> String;

}

#ifdef PT_WITH_STD_LOCALE
#include <Pt/Facets.h>
#endif

// Include the implementation header
#include <Pt/String.tpp>

#endif
