/*
 * Copyright (C) 2008 Marc Boris Duerner
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

#ifndef Pt_Slot_h
#define Pt_Slot_h

#include <Pt/Api.h>
#include <Pt/Void.h>
#include <Pt/Callable.h>

namespace Pt {

class Connection;

class Slot 
{
    public:
        virtual ~Slot() {}

        virtual Slot* clone() const = 0;

        virtual const void* callable() const = 0;

        virtual void onConnect(const Connection& c) = 0;

        virtual void onDisconnect(const Connection& c) = 0;

        virtual bool equals(const Slot& slot) const = 0;
};

template < typename R, typename A1 = Void,  typename A2 = Void,
                       typename A3 = Void,  typename A4 = Void,
                       typename A5 = Void,  typename A6 = Void,
                       typename A7 = Void,  typename A8 = Void,
                       typename A9 = Void,  typename A10 = Void >
class BasicSlot : public Slot 
{
    public:
        virtual Slot* clone() const = 0;
};


template <typename T>
class BindAdaptorBase
{
    public:
        BindAdaptorBase(const Slot& s, const T& a)
        : _slot( s.clone() )
        , _a(a)
        { }
        
        BindAdaptorBase(const BindAdaptorBase& c)
        : _slot( c._slot->clone() )
        , _a(c._a)
        { }
        
        ~BindAdaptorBase()
        { delete _slot; }
        
        BindAdaptorBase& operator=(const BindAdaptorBase& b)
        {
            if(this == &b)
              return *this;

            Slot* s = b.slot().clone();
            delete _slot;
            _slot = s;
            
            _a = b.a;
            return *this;
        }
        
        Slot& slot()
        { return *_slot; }
        
        const Slot& slot() const
        { return *_slot; }

        const T& arg() const
        { return _a; }

    private:
        Slot* _slot;
        T     _a;
};


template <typename R, typename A1, 
          typename A2 = Void, typename A3 = Void, 
          typename A4 = Void>
class BindAdaptor : public Callable<R, A1, A2, A3>
                  , public BindAdaptorBase<A4>
{   
    public:
        typedef BasicSlot<R, A1, A2, A3> SlotBase;

    public:
        BindAdaptor(const BasicSlot<R, A1, A2, A3, A4>& slot, const A4& a)
        : BindAdaptorBase<A4>(slot, a)
        { }
        
        virtual Callable<R, A1, A2, A3>* clone() const
        { return new BindAdaptor(*this); }

        virtual R operator()(A1 a1, A2 a2, A3 a3) const
        { 
            const Callable<R, A1, A2, A3, A4>* cb = 
                static_cast< const Callable<R, A1, A2, A3, A4>* >( this->slot().callable() );
            
            return cb->call( a1, a2, a3, this->arg() ); 
        }
};


template <typename R, typename A1, 
          typename A2, typename A3>
class BindAdaptor<R, A1, A2, A3, Void> : public Callable<R, A1, A2>
                                       , public BindAdaptorBase<A3>
{   
    public:
        typedef BasicSlot<R, A1, A2> SlotBase;

    public:
        BindAdaptor(const BasicSlot<R, A1, A2, A3>& slot, const A3& a)
        : BindAdaptorBase<A3>(slot, a)
        { }
        
        virtual Callable<R, A1, A2>* clone() const
        { return new BindAdaptor(*this); }

        virtual R operator()(A1 a1, A2 a2) const
        { 
            const Callable<R, A1, A2, A3>* cb = 
                static_cast< const Callable<R, A1, A2, A3>* >( this->slot().callable() );
            
            return cb->call( a1, a2, this->arg() ); 
        }
};



template <typename R, typename A1, typename A2>
class BindAdaptor<R, A1, A2, Void, Void> : public Callable<R, A1>
                                         , public BindAdaptorBase<A2>
{   
    public:
        typedef BasicSlot<R, A1> SlotBase;

    public:
        BindAdaptor(const BasicSlot<R, A1, A2>& slot, const A2& a)
        : BindAdaptorBase<A2>(slot, a)
        { }
        
        virtual Callable<R, A1>* clone() const
        { return new BindAdaptor(*this); }

        virtual R operator()(A1 a1) const
        { 
            const Callable<R, A1, A2>* cb = 
                static_cast< const Callable<R, A1, A2>* >( this->slot().callable() );
            
            return cb->call( a1, this->arg() ); 
        }
};


template <typename R, typename A1>
class BindAdaptor<R, A1, Void, Void, Void> : public Callable<R>
                                           , public BindAdaptorBase<A1>
{   
    public:
        typedef BasicSlot<R> SlotBase;

    public:
        BindAdaptor(const BasicSlot<R, A1>& slot, const A1& a)
        : BindAdaptorBase<A1>(slot, a)
        { }
        
        virtual Callable<R>* clone() const
        { return new BindAdaptor(*this); }

        virtual R operator()() const
        { 
            const Callable<R, A1>* cb = 
                static_cast< const Callable<R, A1>* >( this->slot().callable() );
            
            return cb->call( this->arg() ); 
        }
};


template <typename R, typename A1, typename A2, typename A3, typename A4>
class BoundSlot : public BindAdaptor<R, A1, A2, A3, A4>::SlotBase
{
    public:
        template <typename T>
        BoundSlot(const BasicSlot<R, A1, A2, A3, A4>& slot, const T& a)
        : _adaptor(slot, a)
        { }

        Slot* clone() const
        { 
            return new BoundSlot(*this); 
        }
        
        virtual const void* callable() const
        { 
            return &_adaptor; 
        }

        virtual void onConnect(const Connection& c)
        {
            _adaptor.slot().onConnect(c);
        }

        virtual void onDisconnect(const Connection& c)
        {
            _adaptor.slot().onDisconnect(c);
        }

        virtual bool equals(const Slot& slot) const
        {
            return _adaptor.slot().equals(slot);
        }

    private:
        BindAdaptor<R, A1, A2, A3, A4> _adaptor;     
};


template < typename R, typename A1, typename A2, typename A3, typename A4, typename T>
BoundSlot<R, A1, A2, A3, A4> slot(const BasicSlot<R, A1, A2, A3, A4>& slot, const T& a)
{
    return BoundSlot<R, A1, A2, A3, A4>(slot, a);
}

} // namespace Pt

#endif
