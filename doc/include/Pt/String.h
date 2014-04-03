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

//
// Note copy from Pt/String.h and replace
// String -> String
// basic_string<Char>     -> String
// basic_string           -> String
//

/** @brief Unicode capable basic_string.

    This class Pt::String is not yet another unicode string class, but it
    is a specialization of the std::basic_string template for the unicode
    character type Pt::Char:

    @code
    typedef std::basic_string<Pt::Char> String;
    @endcode

    It offers all the functionality of the std::basic_string template. This has
    the advantage, that all generic algorithms that work with std::basic_string
    should also work with Pt::String. Please refer to a standard c++ manual for
    a complete overview.
    Additional methods make it easier to work with other character types. For
    example, the relational operators are also overloaded for char and wchar_t.  

    @ingroup Unicode
*/
class String 
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
        /** @brief Default Constructor.
        */
        explicit String( const allocator_type& a = allocator_type());

        /** @brief Constructor.
        */
        String(const Pt::Char* str, const allocator_type& a = allocator_type());
        
        /** @brief Constructor.
        */
        String(const Pt::Char* str, size_type n, const allocator_type& a = allocator_type());
        
        /** @brief Constructor.
        */
        String(const wchar_t* str, const allocator_type& a = allocator_type());

        /** @brief Constructor.
        */
        String(const wchar_t* str, size_type n, const allocator_type& a = allocator_type());

        /** @brief Constructor.
        */
        String(const char* str, const allocator_type& a = allocator_type());

        /**  @brief Constructor.
        */
        String(const char* str, size_type n, const allocator_type& a = allocator_type());

        /** @brief Constructor.
        */
        String(size_type n, Pt::Char c, const allocator_type& a = allocator_type());

        /** @brief Copy Constructor.
        */
        String(const String& str);

        /**  @brief Constructor.
        */
        String(const String& str, const allocator_type& a);

      /** @brief Constructor.
      */
        String(const String& str, size_type pos, size_type n = npos, const allocator_type& a = allocator_type());

        /** @brief Constructor.
        */
        String(const Pt::Char* begin, const Pt::Char* end, const allocator_type& a = allocator_type());

        template <typename InputIterator>
        String(InputIterator begin, InputIterator end, const allocator_type& a = allocator_type());

        /** @brief Destructor.
        */
        ~String();

    public:
        /** @brief Returns an iterator to the begin of the string.
        */
        iterator begin()
        { return privdata_rw(); }
        
        /**  @brief Returns an iterator to the end of the string.
        */
        iterator end()
        { return privdata_rw() + length(); }
        
        /** @brief Returns an iterator to the begin of the string.
        */
        const_iterator begin() const
        { return privdata_ro(); }
        
        /** @brief Returns an iterator to the end of the string.
        */
        const_iterator end() const
        { return privdata_ro() + length(); }

        /** @brief Returns an reverse iterator to the end of the string.
        */
        reverse_iterator rbegin()
        { return reverse_iterator( this->end() ); }

        /** @brief Returns an reverse iterator to the begin of the string.
        */
        reverse_iterator rend()
        { return reverse_iterator( this->begin() ); }

        /** @brief Returns an reverse iterator to the end of the string.
        */
        const_reverse_iterator rbegin() const
        { return const_reverse_iterator( this->end() ); }

        /** @brief Returns an reverse iterator to the begin of the string.
        */
        const_reverse_iterator rend()   const
        { return const_reverse_iterator( this->begin() ); }

        /** @brief Random access to characters.
        */
        reference operator[](size_type n)
        { return privdata_rw()[n]; }

        /** @brief Random access to characters.
        */
        const_reference operator[](size_type n) const
        { return privdata_ro()[n]; }

        /** @brief Random access to characters.
        */
        reference at(size_type n)
        { 
            if( n >= size() )
                throw out_of_range("at");

            return privdata_rw()[n]; 
        }

        /** @brief Random access to characters.
        */
        const_reference at(size_type n) const
        { 
            if( n >= size() )
                throw out_of_range("at");
            
            return privdata_ro()[n]; 
        }

    public:
        /** @brief Append a character.
        */
        void push_back(Pt::Char ch)
        { (*this) += ch; }

        /** @brief Resizes the string.
        */
        void resize( std::size_t n, Pt::Char ch = value_type() );

        /** @brief Reserves space.
        */
        void reserve(std::size_t n = 0);

        /** @brief Swaps with another string.
        */
        void swap(String& str);

        /** @brief Returns the used allocator.
        */
        allocator_type get_allocator() const
        { return _d; }

        /** @brief Copy characters to a buffer.
        */
        size_type copy(Pt::Char* a, size_type n, size_type pos = 0) const;

        /** @brief Returns a substring.
        */
        String substr(size_type pos = 0, size_type n = npos) const
        { return String(*this, pos, n); }

    public:
        /** @brief Returns the length of the string.
        */
        size_type length() const
        { return isShortString() ? shortStringLength() : longStringLength(); }

        /** @brief Returns the length of the string.
        */
        size_type size() const
        { return length(); }

        /** @brief Returns true if empty.
        */
        bool empty() const
        { return length() == 0; }

        /** @brief Returns the maximum possible length.
        */
        size_type max_size() const
        { return ( size_type(-1) / sizeof(Pt::Char) ) - 1; }

        /** @brief Returns the capacity.
        */
        size_type capacity() const
        { return isShortString() ? shortStringCapacity() : longStringCapacity(); }

        /** @brief Returns the string character buffer.
        */
        const Pt::Char* data() const
        { return privdata_ro(); }

        /** @brief Returns a null terminated C string.
        */
        const Pt::Char* c_str() const
        { return privdata_ro(); }

        /** @brief Assign content to the string.
        */
        String& assign(const String& str);

        /** @brief Assign content to the string.
        */
        String& assign(const String& str, size_type pos, size_type n);

        /** @brief Assign content to the string.
        */
        String& assign(const wchar_t* str);

        /** @brief Assign content to the string.
        */
        String& assign(const wchar_t* str, size_type n);

        /** @brief Assign content to the string.
        */
        String& assign(const Pt::Char* str);

        /** @brief Assign content to the string.
        */
        String& assign(const Pt::Char* str, size_type length);

        /** @brief Assign content to the string.
        */
        String& assign(const char* str);

        /** @brief Assign content to the string.
        */
        String& assign(const char* str, size_type length);

        /** @brief Assign content to the string.
        */
        String& assign(size_type n, Pt::Char c);

        template <typename InputIterator>
        String& assign(InputIterator begin, InputIterator end);

        /** @brief Append content to the string.
        */
        String& append(const Pt::Char* str);

        /** @brief Append content to the string.
        */
        String& append(const Pt::Char* str, size_type n);

        /** @brief Append content to the string.
        */
        String& append(size_type n, Pt::Char ch);

        /** @brief Append content to the string.
        */
        String& append(const String& str);

        /** @brief Append content to the string.
        */
        String& append(const String& str, size_type pos, size_type n);

        template <typename InputIterator>
        String& append(InputIterator begin, InputIterator end);

        /** @brief Append content to the string.
        */
        String& append(const Pt::Char* begin, const Pt::Char* end);

        /** @brief Insert into string.
        */
        String& insert(size_type pos, const Pt::Char* str);

        /** @brief Insert into string.
        */
        String& insert(size_type pos, const Pt::Char* str, size_type n);

        /** @brief Insert into string.
        */
        String& insert(size_type pos, size_type n, Pt::Char ch);

        /** @brief Insert into string.
        */
        String& insert(size_type pos, const String& str);

        /** @brief Insert into string.
        */
        String& insert(size_type pos, const String& str, size_type pos2, size_type n);

        /** @brief Insert into string.
        */
        String& insert(iterator p, Pt::Char ch);

        /** @brief Insert into string.
        */
        String& insert(iterator p, size_type n, Pt::Char ch);

        // unimplemented
        //template <typename InputIterator>
        //String& insert(iterator p, InputIterator first, InputIterator last);

        /** @brief Clears the string.
        */
        void clear()
        { setLength(0); }

        /** @brief Erase characters from the string.
        */
        String& erase(size_type pos = 0, size_type n = npos);

        /** @brief Erase characters from the string.
        */
        iterator erase(iterator pos);

        /** @brief Erase characters from the string.
        */
        iterator erase(iterator first, iterator last);

        /** @brief Replace portion of the string.
        */
        String& replace(size_type pos, size_type n, const Pt::Char* str);

        /** @brief Replace portion of the string.
        */
        String& replace(size_type pos, size_type n, const Pt::Char* str, size_type n2);

        /** @brief Replace portion of the string.
        */
        String& replace(size_type pos, size_type n, size_type n2, Pt::Char ch);

        /** @brief Replace portion of the string.
        */
        String& replace(size_type pos, size_type n, const String& str);

        /** @brief Replace portion of the string.
        */
        String& replace(size_type pos, size_type n, const String& str, size_type pos2, size_type n2);

        /** @brief Replace portion of the string.
        */
        String& replace(iterator i1, iterator i2, const Pt::Char* str);

        /** @brief Replace portion of the string.
        */
        String& replace(iterator i1, iterator i2, const Pt::Char* str, size_type n);

        /** @brief Replace portion of the string.
        */
        String& replace(iterator i1, iterator i2, size_type n, Pt::Char ch);

        /** @brief Replace portion of the string.
        */
        String& replace(iterator i1, iterator i2, const String& str);

        /** @brief Compare strings.
        */
        int compare(const String& str) const;

        /** @brief Compare strings.
        */
        int compare(const Pt::Char* str) const;

        /** @brief Compare strings.
        */
        int compare(const Pt::Char* str, size_type n) const;

        /** @brief Compare strings.
        */
        int compare(const wchar_t* str) const;

        /** @brief Compare strings.
        */
        int compare(const wchar_t* str, size_type n) const;

        /** @brief Compare strings.
        */
        int compare(const char* str) const;

        /** @brief Compare strings.
        */
        int compare(const char* str, size_type n) const;

        /** @brief Compare strings.
        */
        int compare(size_type pos, size_type n, const String& str) const;

        /** @brief Compare strings.
        */
        int compare(size_type pos, size_type n, const String& str, size_type pos2, size_type n2) const;

        /** @brief Compare strings.
        */
        int compare(size_type pos, size_type n, const Pt::Char* str) const;

        /** @brief Compare strings.
        */
        int compare(size_type pos, size_type n, const Pt::Char* str, size_type n2) const;

        /** @brief Find content in the string.
        */
        size_type find(const String& str, size_type pos = 0) const;

        /** @brief Find content in the string.
        */
        size_type find(const Pt::Char* str, size_type pos, size_type n) const;

        /** @brief Find content in the string.
        */
        size_type find(const Pt::Char* str, size_type pos = 0) const;

        /** @brief Find content in the string.
        */
        size_type find(Pt::Char ch, size_type pos = 0) const;

        /** @brief Find content in the string.
        */
        size_type rfind(const String& str, size_type pos = npos) const;

        /** @brief Find content in the string.
        */
        size_type rfind(const Pt::Char* str, size_type pos, size_type n) const;

        /** @brief Find content in the string.
        */
        size_type rfind(const Pt::Char* str, size_type pos = npos) const;

        /** @brief Find content in the string.
        */
        size_type rfind(Pt::Char ch, size_type pos = npos) const;

        /** @brief Find content in the string.
        */
        size_type find_first_of(const String& str, size_type pos = 0) const
        { return this->find_first_of( str.data(), pos, str.size() ); }

        /** @brief Find content in the string.
        */
        size_type find_first_of(const Pt::Char* s, size_type pos, size_type n) const;

        /** @brief Find content in the string.
        */
        size_type find_first_of(const Pt::Char* str, size_type pos = 0) const
        { return this->find_first_of( str, pos, traits_type::length(str) ); }

        /** @brief Find content in the string.
        */
        size_type find_first_of(const Pt::Char ch, size_type pos = 0) const
        { return this->find(ch, pos); }

        /** @brief Find content in the string.
        */
        size_type find_last_of(const String& str, size_type pos = npos) const
        { return this->find_last_of( str.data(), pos, str.size() ); }

        /** @brief Find content in the string.
        */
        size_type find_last_of(const Pt::Char* s, size_type pos, size_type n) const;

        /** @brief Find content in the string.
        */
        size_type find_last_of(const Pt::Char* str, size_type pos = npos) const
        { return this->find_last_of( str, pos, traits_type::length(str) ); }

        /** @brief Find content in the string.
        */
        size_type find_last_of(const Pt::Char ch, size_type pos = npos) const
        { return this->rfind(ch, pos); }

        /** @brief Find content in the string.
        */
        size_type find_first_not_of(const String& str, size_type pos = 0) const
        { return this->find_first_not_of( str.data(), pos, str.size() ); }

        /** @brief Find content in the string.
        */
        size_type find_first_not_of(const Pt::Char* s, size_type pos, size_type n) const;

        /** @brief Find content in the string.
        */
        size_type find_first_not_of(const Pt::Char* str, size_type pos = 0) const
        { return this->find_first_not_of( str, pos, traits_type::length(str) ); }

        /** @brief Find content in the string.
        */
        size_type find_first_not_of(const Pt::Char ch, size_type pos = 0) const;

        /** @brief Find content in the string.
        */
        size_type find_last_not_of(const String& str, size_type pos = npos) const
        { return this->find_last_not_of( str.data(), pos, str.size() ); }

        /** @brief Find content in the string.
        */
        size_type find_last_not_of(const Pt::Char* tok, size_type pos, size_type n) const;

        /** @brief Find content in the string.
        */
        size_type find_last_not_of(const Pt::Char* str, size_type pos = npos) const
        { return this->find_last_not_of( str, pos, traits_type::length(str) ); }

        /** @brief Find content in the string.
        */
        size_type find_last_not_of(Pt::Char ch, size_type pos = npos) const;

    public:
        /** @brief Narrow string to 8-bit.
        */
        std::string narrow(char dfault = '?') const;

        /** @brief Widen 8-bit string .
        */
        static String widen(const char* str);

        /** @brief Widen 8-bit string .
        */
        static String widen(const std::string& str);

        /** @brief Convert to UTF-16.
        */
        template <typename OutIterT>
        OutIterT toUtf16(OutIterT to) const;

        /** @brief Convert from UTF-16.
        */
        template <typename InIterT>
        static String fromUtf16(InIterT from, InIterT fromEnd);

    public:
        /** @brief Assignment operator.
        */
        String& operator=(const String& str)
        { return this->assign(str); }

        /** @brief Assignment operator.
        */
        String& operator=(const wchar_t* str)
        { return this->assign(str); }

        /** @brief Assignment operator.
        */
        String& operator=(const char* str)
        { return this->assign(str); }

        /** @brief Assignment operator.
        */
        String& operator=(const Pt::Char* str)
        { return this->assign(str); }

        /** @brief Assignment operator.
        */
        String& operator=(Pt::Char ch)
        {
            // no privreserve(1), short string capacity is large enough

            Pt::Char* p = privdata_rw();
            p[0] = ch;
            setLength(1);
            return *this;
        }

        /** @brief Append a string.
        */
        String& operator+=(const String& str)
        { return this->append(str); }

        /** @brief Append a string.
        */
        String& operator+=(const Pt::Char* str)
        { return this->append(str); }

        /** @brief Append a character.
        */
        String& operator+=(Pt::Char c);

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

/** @brief Swaps two strings.

    @related String
*/
inline void swap(String& a, String& b)
{ a.swap(b); }

// operator +

/** @brief Concatenates two strings.

    @related String
*/
inline String operator+(const String& a, const String& b)
{ String temp; temp += a; temp += b; return temp; }

/** @brief Concatenates two strings.

    @related String
*/
inline String operator+(const String& a, const Pt::Char* b)
{ String temp; temp += a; temp += b; return temp; }

/** @brief Concatenates two strings.

    @related String
*/
inline String operator+(const Pt::Char* a, const String& b)
{ String temp; temp += a; temp += b; return temp; }

/** @brief Concatenates two strings.

    @related String
*/
inline String operator+(const String& a, Pt::Char b)
{ String temp; temp += a; temp += b; return temp; }

/** @brief Concatenates two strings.

    @related String
*/
inline String operator+(Pt::Char a, const String& b)
{ String temp; temp += a; temp += b; return temp; }

// operator ==

/** @brief Compares two strings.

    @related String
*/
inline bool operator==(const String& a, const String& b)
{ return a.compare(b) == 0; }

/** @brief Compares two strings.

    @related String
*/
inline bool operator==(const Pt::Char* a, const String& b)
{ return b.compare(a) == 0; }

/** @brief Compares two strings.

    @related String
*/
inline bool operator==(const String& a, const Pt::Char* b)
{ return a.compare(b) == 0; }

/** @brief Compares two strings.

    @related String
*/
inline bool operator==(const String& a, const wchar_t* b)
{ return a.compare(b) == 0; }

/** @brief Compares two strings.

    @related String
*/
inline bool operator==(const wchar_t* b, const String& a)
{ return a.compare(b) == 0; }

/** @brief Compares two strings.

    @related String
*/
inline bool operator==(const String& a, const char* b)
{ return a.compare(b) == 0; }

/** @brief Compares two strings.

    @related String
*/
inline bool operator==(const char* b, const String& a)
{ return a.compare(b) == 0; }

// operator !=

/** @brief Compares two strings.

    @related String
*/
inline bool operator!=(const String& a, const String& b)
{ return a.compare(b) != 0; }

/** @brief Compares two strings.

    @related String
*/
inline bool operator!=(const Pt::Char* a, const String& b)
{ return b.compare(a) != 0; }

/** @brief Compares two strings.

    @related String
*/
inline bool operator!=(const String& a, const Pt::Char* b)
{ return a.compare(b) != 0; }

/** @brief Compares two strings.

    @related String
*/
inline bool operator!=(const String& a, const wchar_t* b)
{ return a.compare(b) != 0; }

/** @brief Compares two strings.

    @related String
*/
inline bool operator!=(const wchar_t* b, const String& a)
{ return a.compare(b) != 0; }

/** @brief Compares two strings.

    @related String
*/
inline bool operator!=(const String& a, const char* b)
{ return a.compare(b) != 0; }

/** @brief Compares two strings.

    @related String
*/
inline bool operator!=(const char* b, const String& a)
{ return a.compare(b) != 0; }

// operator <

/** @brief Compares two strings.

    @related String
*/
inline bool operator<(const String& a, const String& b)
{ return a.compare(b) < 0; }

/** @brief Compares two strings.

    @related String
*/
inline bool operator<(const Pt::Char* a, const String& b)
{ return b.compare(a) > 0; }

/** @brief Compares two strings.

    @related String
*/
inline bool operator<(const String& a, const Pt::Char* b)
{ return a.compare(b) < 0; }

/** @brief Compares two strings.

    @related String
*/
inline bool operator<(const String& a, const wchar_t* b)
{ return a.compare(b) < 0; }

/** @brief Compares two strings.

    @related String
*/
inline bool operator<(const wchar_t* b, const String& a)
{ return a.compare(b) > 0; }

/** @brief Compares two strings.

    @related String
*/
inline bool operator<(const String& a, const char* b)
{ return a.compare(b) < 0; }

/** @brief Compares two strings.

    @related String
*/
inline bool operator<(const char* b, const String& a)
{ return a.compare(b) > 0; }

// operator <=

/** @brief Compares two strings.

    @related String
*/
inline bool operator<=(const String& a, const String& b)
{ return a.compare(b) <= 0; }

/** @brief Compares two strings.

    @related String
*/
inline bool operator<=(const Pt::Char* a, const String& b)
{ return b.compare(a) >= 0; }

/** @brief Compares two strings.

    @related String
*/
inline bool operator<=(const String& a, const Pt::Char* b)
{ return a.compare(b) <= 0; }

/** @brief Compares two strings.

    @related String
*/
inline bool operator<=(const String& a, const wchar_t* b)
{ return a.compare(b) <= 0; }

/** @brief Compares two strings.

    @related String
*/
inline bool operator<=(const wchar_t* b, const String& a)
{ return a.compare(b) >= 0; }

/** @brief Compares two strings.

    @related String
*/
inline bool operator<=(const String& a, const char* b)
{ return a.compare(b) <= 0; }

/** @brief Compares two strings.

    @related String
*/
inline bool operator<=(const char* b, const String& a)
{ return a.compare(b) >= 0; }

// operator >

/** @brief Compares two strings.

    @related String
*/
inline bool operator>(const String& a, const String& b)
{ return a.compare(b) > 0; }

/** @brief Compares two strings.

    @related String
*/
inline bool operator>(const Pt::Char* a, const String& b)
{ return b.compare(a) < 0; }

/** @brief Compares two strings.

    @related String
*/
inline bool operator>(const String& a, const Pt::Char* b)
{ return a.compare(b) > 0; }

/** @brief Compares two strings.

    @related String
*/
inline bool operator>(const String& a, const wchar_t* b)
{ return a.compare(b) > 0; }

/** @brief Compares two strings.

    @related String
*/
inline bool operator>(const wchar_t* b, const String& a)
{ return a.compare(b) < 0; }

/** @brief Compares two strings.

    @related String
*/
inline bool operator>(const String& a, const char* b)
{ return a.compare(b) > 0; }

/** @brief Compares two strings.

    @related String
*/
inline bool operator>(const char* b, const String& a)
{ return a.compare(b) < 0; }

// operator >=

/** @brief Compares two strings.

    @related String
*/
inline bool operator>=(const String& a, const String& b)
{ return a.compare(b) >= 0; }

/** @brief Compares two strings.

    @related String
*/
inline bool operator>=(const Pt::Char* a, const String& b)
{ return b.compare(a) <= 0; }

/** @brief Compares two strings.

    @related String
*/
inline bool operator>=(const String& a, const Pt::Char* b)
{ return a.compare(b) >= 0; }

/** @brief Compares two strings.

    @related String
*/
inline bool operator>=(const String& a, const wchar_t* b)
{ return a.compare(b) >= 0; }

/** @brief Compares two strings.

    @related String
*/
inline bool operator>=(const wchar_t* b, const String& a)
{ return a.compare(b) <= 0; }

/** @brief Compares two strings.

    @related String
*/
inline bool operator>=(const String& a, const char* b)
{ return a.compare(b) >= 0; }

/** @brief Compares two strings.

    @related String
*/
inline bool operator>=(const char* b, const String& a)
{ return a.compare(b) <= 0; }

/** @brief Stream insertion operator.

    @related String
*/
PT_API ostream& operator<< (ostream& out, const String& str);

} // namespace Pt


#endif
