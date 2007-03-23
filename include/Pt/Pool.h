/***************************************************************************
 *   Copyright (C) 2007 Marc Boris Dürner                                  *
 *   Copyright (C) 2007 Laurentiu-Gheorghe Crisan                          * 
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
#ifndef PT_POOL_H
#define PT_POOL_H
#include <Pt/Types.h>
#include <Pt/Exception.h>
#include <queue>

namespace Pt {

template<typename T, typename AllocatorT = std::allocator<T> >
class Pool 
{
public:
    Pool( size_t size )
    : _allocCount( 0 )
    {
        for( size_t i = 0; i < size; i++ )
            _items.push( this->allocate() );            
    }

    ~Pool()
    {
        while( !_items.empty() )
        {
            T* p = _items.front();
            _allocator.deallocate(p, 1);
            _items.pop();
        }
        
        if( _allocCount != 0 )
            std::runtime_error("Memory leaks detected" + PT_SOURCEINFO );        
    }

    T* alloc()
    {
        T* item;

        if( _items.empty() )
        {
            item = this->allocate();
        }
        else
        {
            item = _items.front();
            _items.pop();
        }     
        _allocCount++;
        return item;
    }

    void release(T* item)
    { 
        _allocCount--;
        _items.push( item ); 
    }

    size_t size() const
    { return _items.size(); }

    AllocatorT& allocator()
    {
        return _allocator;
    }

private:

    T* allocate()
    {
        T* item = _allocator.allocate(1);
        return item;
    }

    std::queue<T*> _items;
    AllocatorT     _allocator;
    Pt::ssize_t     _allocCount;
};

} //namespace Pt

#endif
