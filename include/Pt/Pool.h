#ifndef PTV_POOL_H
#define PTV_POOL_H

#include <queue>


namespace Pt {

template<typename T, typename AllocatorT = std::allocator<T> >
class Pool {

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
						_allocator.destroy(p);
						_allocator.deallocate(p, 1);
            _items.pop();
				}
    }

    T* alloc()
    {
        if( _items.empty() )
            return this->allocate();

        T* item = _items.front();
        _items.pop();

        return item;
    }

    void release(T* item)
    {  _items.push( item );  }

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
        ::new ( item ) T();
        return item;
    }

    std::queue<T*> _items;
    AllocatorT     _allocator;
};

}

#endif
