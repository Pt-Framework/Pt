/***************************************************************************
 *   Copyright (C) 2004-2006 by Marc Boris Drner                          *
 *   Copyright (C) 2004-2006 by Stepan Beal                                *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef Pt_Variant_h
#define Pt_Variant_h

#include <Pt/VariantTraits.h>

#include <string>


namespace Pt {

    class Variant {
        public:
            Variant() throw()
            {}

            ~Variant() throw()
            {}

            template <typename T>
            Variant(const T& value) throw()
            { this->set(value); }

            Variant(const Variant & variant) throw()
            : _data(variant._data)
            { }

            Variant(const void* blob, int byteCount) throw()
            {
                _data.assign((const char*)blob, byteCount);
            }

            void assign(const void* blob, int byteCount) throw()
            {
                _data.assign((const char*)blob, byteCount);
            }

            template <typename T>
            bool get(T& type) const throw()
            { return VariantTraits<T>::fromData(type , _data); }

            template <typename T>
            void set(const T& value) throw()
            { VariantTraits<T>::toData(_data, value); }

            void clear()
            { this->_data.clear(); }

            inline bool empty() const
            { return this->_data.empty(); }

            template <typename T>
            inline Variant& operator=(const T& type) throw()
            {
                this->set(type);
                return *this;
            }

            inline Variant& operator=(const Variant & rhs) throw()
            {
                if(&rhs != this)
                    this->_data = rhs._data;

                return *this;
            }

            inline std::string& str() throw()
            { return this->_data; }

            inline const std::string& str() const throw()
            { return  this->_data; }

            inline bool operator<(const Variant& rhs) const
            { return this->str() < rhs.str(); }

            inline bool operator>(const Variant& rhs) const
            { return this->str() > rhs.str(); }

            template <typename T>
            inline bool operator==(const T & rhs) const
            {
                T type;
                this->get(type);
                return type == rhs;
            }

            inline bool operator==(const Variant & rhs) const
            {
                return this->str() == rhs.str();
            }

            inline bool operator==(const char * rhs) const
            {
                if( !rhs ) return this->empty();
                return this->str() == std::string(rhs);
            }

            inline bool operator==(const std::string & rhs) const
            {
                return this->str() == rhs;
            }

        private:
            std::string _data;
    };


    inline std::ostream& operator<<(std::ostream & os, const Variant& var)
    {
        os << var.str();
        return os;
    }


    inline std::istream& operator>>(std::istream & is, Variant& var)
    {
        std::getline( is, var.str(), static_cast<std::istream::char_type>(std::istream::traits_type::eof()) );
        return is;
    }


} // namespace Pt


#endif 

