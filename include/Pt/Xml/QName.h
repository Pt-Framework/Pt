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
#ifndef Pt_Xml_QName_h
#define Pt_Xml_QName_h

#include <Pt/Xml/Api.h>
#include <Pt/Xml/Namespace.h>
#include <Pt/String.h>
#include <Pt/NonCopyable.h>
#include <vector>

namespace Pt {

namespace Xml {

/** @brief A namespace qualified XML name
*/
class QName 
{
    public:
        /** @brief Constructs an empty qualified name.
        */
        QName()
        {}

        /** @brief Clears all content.
        */
        void clear()
        {
            _name.clear();
            _prefix.clear();
        }

        template <typename T>
        bool equals(const T* name) const
        {
            return _name == name;
        }

        template <typename T>
        bool equals(const T* prefix, const T* name) const
        {
            return _name == name && _prefix == prefix;
        }

        /** @brief Returns the namespace prefix.
        */
        String& prefix() 
        { return _prefix; }

        /** @brief Returns the namespace prefix.
        */
        const String& prefix() const
        { return _prefix; }

        /** @brief Sets the namespace prefix.
        */
        void setPrefix(const String& prefix)
        { _prefix = prefix; }

        /** @brief Returns the local name.
        */
        String& name() 
        { return _name; }

        /** @brief Returns the local name.
        */
        const String& name() const
        { return _name; }

        /** @brief Sets the local name.
        */
        void setName(const String& name)
        { _name = name; }

        //! @internal
        void addName(Char ch)
        { _name += ch; }

        //! @internal
        void clearName()
        { _name.clear(); }

    private:
        String _prefix;
        String _name;
};


inline bool operator ==(const QName& a, const QName& b)
{
    return a.prefix() == b.prefix() && a.name() == b.name();
}


inline bool operator !=(const QName& a, const QName& b)
{
    return a.prefix() != b.prefix() || a.name() != b.name();
}


inline bool operator<(const QName& a, const QName& b)
{
	  return a.prefix() < b.prefix() ||
           ! (b.prefix() < a.prefix()) && a.name() < b.name();
}


class QNameStack
{
    static const unsigned int BufSize = 16;

    public:
        inline QNameStack()
        : _cur(0)
        {
            _cur = &_names[0]; 
        }

        inline void clear()
        {
            _cur->clear();

            while(! empty() )
                pop();
        }

        inline void pushChar(Char ch)
        {                        
            _cur->name() += ch;
        }

        inline bool pushPrefix()
        {
            if( _cur->prefix().empty() )
            {
                // TODO: use swap
                _cur->setPrefix( _cur->name() );
                _cur->name().clear();
                return true;
            }

            return false;
        }
            
        inline std::size_t pushName()
        {
            if( _cur >= _names && _cur < &_names[BufSize-1] )
            {
                ++_cur;
            }
            else
            {
                _extra.push_back( QName() );
                _cur = &_extra.back();
            }
                    
            return 0;
        }

        inline std::size_t pop()
        {
            if( _extra.empty() )
            {
                --_cur;
            }
            else
            {
                _extra.pop_back();
                _cur = _extra.empty() ? &_names[BufSize-1]
                                        : &_extra.back();
            }

            _cur->clear();
                    
            return 0;
        }

        inline const QName& top() const
        {
            return _extra.size() == 1 ? _names[BufSize-1]
                                      : *(_cur - 1);
        }
                
        inline bool empty() const
        {
            return _cur == _names;
        }

        inline std::size_t size() const
        {
            return _extra.empty() ? _cur - _names : BufSize + _extra.size();
        }

    private:
        QName* _cur;
        QName _names[BufSize];
        std::vector<QName> _extra;
};

} // namespace Xml

} // namespace Pt

#endif
