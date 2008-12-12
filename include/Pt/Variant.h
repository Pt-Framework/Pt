/*
 * Copyright (C) 2004-2006 by Marc Boris Duerner
 * Copyright (C) 2004-2006 by Stepan Beal
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
#ifndef Pt_Variant_h
#define Pt_Variant_h

#include <Pt/Convert.h>
#include <string>

namespace Pt {

    /**
           Variant provides a really convenient way to lexically cast
           strings and other streamable types to/from each other.

           All parameterized types used by this type must be:

           - i/o streamable. The operators must complement each other.

           - Assignable.

           - Default Constructable.

           This type is fairly light-weight, with only one std::string
           data member, so it should copy quickly and implicitely use
           std::string's CoW and reference counting features. Adding
           reference counting to this class would be of no benefit,
           and would probably hurt performance, considering that
           std::string's are optimized in these ways, and this type
           is simply a proxy for a std::string.

           For some uses the variant type can replace the requirement
           for returning a proxy type from a type's operator[](), as
           discussed in Scott Meyers' <em>More Effective C++</em>, Item
           30. This class originally was such a proxy, and then
           evolved into a generic solution for POD-based types, which
           inherently also covers most i/ostreamable types. It is less
           efficient than specialized proxies for, e.g. (char &), but
           it is also extremely easy to use, as shown here:


    <pre>
        Variant lex = 17;
        int bogo = lex;
        ulong bogol = bogo * static_cast<long>(lex);
        lex = "bogus string";

        typedef std::map<Variant,Variant> LMap;
        LMap map;

        map[4] = "one";
        map["one"] = 4;
        map[123] = "eat this";
        map['x'] = "marks the spot";
        map["fred"] = 94.3 * static_cast<double>( map["one"] );
        map["fred"] = 10 * static_cast<double>( map["fred"] );
        map["123"] = "this was re-set";
        int myint = map["one"];

    </pre>

        Finally, Perl-ish type flexibility in C++. :)

    */
    class Variant {
        public:
            /**
                Constructs an empty object.
            */
            Variant()
            {}

            ~Variant() throw()
            {}

            /**
                Lexically casts value to a string.
            */
            template <typename T>
            Variant(const T& value)
            {
                this->set(value);
            }

            /**
            Deeply copies rhs.
            */
            Variant(const Variant & rhs)
            : _data(rhs._data)
            { }

            /**
                Lexically casts this->str() to a T
                and assigns that to tgt.
            */
            template <typename T>
            void get(T& tgt) const throw()
            {
                convert(tgt , _data);
            }

            /**
                Lexically casts value to a string
                and sets this->str().
            */
            template <typename T>
            void set(const T& value) throw()
            {
                convert(_data, value);
            }

            /**
                Clears all data in this object.
            */
            void clear()
            {
                this->_data.clear();
            }

            /**
                Returns true if this object has no content (e.g., was
                default constructed, clear() was called, or it contains
                an empty string value). Note that there is no way to
                differentiate between a "null" value and an empty value
                (e.g., by calling str("") or operator=("")).
            */
            inline bool empty() const
            { return this->_data.empty(); }

            /**
                lexically casts val to a string and returns
                this object. this->str() holds the lexically-cast
                value of val.
            */
            template <typename T>
            inline Variant& operator=(const T& val) throw()
            {
                this->set(val);
                return *this;
            }

            /**
                Deeply copies rhs.
            */
            inline Variant& operator=(const Variant & rhs) throw()
            {
                if(&rhs != this)
                    this->_data = rhs._data;

                return *this;
            }

            /**
                Returns this object's MUTABLE string data.
            */
            Pt::String& str()
            { return this->_data; }

            /**
                Returns this object's IMMUTABLE string data.
            */
            const Pt::String& str() const
            { return  this->_data; }

            /**
                Returns (this->str() < rhs.str()).
            */
            inline bool operator<(const Variant& rhs) const
            { return this->str() < rhs.str(); }

            /**
                Returns (this->str() > rhs.str()).
            */
            inline bool operator>(const Variant& rhs) const
            { return this->str() > rhs.str(); }

            /**
                Lexically casts this->str() to
                a T object and returns operator==(thatT,rhs).
            */
            template <typename T>
            inline bool operator==(const T & rhs) const
            {
                T type;
                this->get(type);
                return type == rhs;
            }

            /**
                returns this->str() == rhs.str()
            */
            inline bool operator==(const Variant & rhs) const
            {
                return this->str() == rhs.str();
            }

            /**
                returns this->str() == rhs

                If (!rhs) then this function returns (this->empty()).
            */
            inline bool operator==(const wchar_t* rhs) const
            {
                if( !rhs ) return this->empty();
                return this->str() == rhs;
            }

            /**
                returns this->str() == rhs
            */
            inline bool operator==(const Pt::String& rhs) const
            {
                return this->str() == rhs;
            }

        private:
            Pt::String _data;
    };


    /**
       Copies var to os and returns os.
    */
    inline std::ostream& operator<<(std::ostream & os, const Variant& var)
    {
        os << var.str().narrow();
        return os;
    }


    /**
       Reads from is UNTIL END OF FILE, stores all of the data
       in var, and returns is.
    */
    inline std::istream& operator>>(std::istream & is, Variant& var)
    {
        std::string s;
        std::getline( is, s, static_cast<std::istream::char_type>(std::istream::traits_type::eof()) );
        var.set(s);
        return is;
    }
    
    
    inline std::basic_ostream<Pt::Char>& operator<<(std::basic_ostream<Pt::Char>& os, const Variant& var)
    {
        os << var.str();
        return os;
    }
    
    
    inline std::basic_istream<Pt::Char>& operator>>(std::basic_istream<Pt::Char>& is, Variant& var)
    {
        Pt::String s;
        std::getline( is, s, static_cast<std::basic_istream<Pt::Char>::char_type>(std::basic_istream<Pt::Char>::traits_type::eof()));
        var.set(s);
        return is;
    }

} // namespace Pt

#endif
