#ifndef PT_POOL_H
#define PT_POOL_H

#include <queue>

namespace Pt {

template<typename T, typename AllocatorT = std::allocator<T> >
class Pool 
{

public:
    Pool( size_t size )
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

        ::new ( item ) T();        

        return item;
    }

    void release(T* item)
    { 
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
};

} //namespace Pt

#endif
