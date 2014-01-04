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
#ifndef Pt_Xml_QNameStack_h
#define Pt_Xml_QNameStack_h

#include <Pt/Xml/Api.h>
#include <Pt/Xml/QName.h>
#include <vector>
#include <cstddef>

namespace Pt {

namespace Xml {

//! @internal Stack of QNames.
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

            while( ! empty() )
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
                _cur->prefix().swap( _cur->name() );
                return true;
            }

            return false;
        }
            
        void pushName()
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
        }

        std::size_t pop()
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

            std::size_t n = _cur->name().size() + _cur->prefix().size();
            _cur->clear(); 
            return n;
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
