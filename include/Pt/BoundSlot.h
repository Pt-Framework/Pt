/* Copyright (C) 2016 Marc Boris Duerner
  
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
  
  As a special exception, you may use this file as part of a free
  software library without restriction. Specifically, if other files
  instantiate templates or use macros or inline functions from this
  file, or you compile this file and link it with other files to
  produce an executable, this file does not by itself cause the
  resulting executable to be covered by the GNU General Public
  License. This exception does not however invalidate any other
  reasons why the executable file might be covered by the GNU Library
  General Public License.
  
  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.
  
  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
  MA 02110-1301 USA
*/

#ifndef Pt_BoundSlot_h
#define Pt_BoundSlot_h

#include <Pt/Api.h>
#include <Pt/Void.h>

namespace Pt {

class BindAdaptorBase
{
    public:
        BindAdaptorBase(const Slot& slot)
        : _slot( slot.clone() )
        { }
        
        BindAdaptorBase(const BindAdaptorBase& c)
        : _slot( c._slot->clone() )
        { }
        
        ~BindAdaptorBase()
        { delete _slot; }
        
        Slot& slot()
        { return *_slot; }
        
        const Slot& slot() const
        { return *_slot; }

    private:
        Slot* _slot;
};


template <typename R, typename A1, typename A2 = Void>
class BindAdaptor : public Callable<R, A1>
                  , public BindAdaptorBase
{   
    public:
        typedef BasicSlot<R, A1> SlotBase;

    public:
        BindAdaptor(const BasicSlot<R, A1, A2>& slot, const A2& a)
        : BindAdaptorBase(slot)
        , _a(a)
        { }
        
        BindAdaptor(const BindAdaptor& c)
        : BindAdaptorBase(c)
        , _a(c._a)
        { }
        
        virtual Callable<R, A1>* clone() const
        { return new BindAdaptor(*this); }

        virtual R operator()(A1 a1) const
        { return static_cast< const Callable<R, A1, A2>* >( slot().callable() )->call(a1, _a); }
    
    private:
        BindAdaptor& operator=(const BindAdaptor&);
    
    private:
        A2 _a;
};


template <typename R, typename A1>
class BindAdaptor<R, A1, Void> : public Callable<R>
                               , public BindAdaptorBase
{   
    public:
        typedef BasicSlot<R> SlotBase;

    public:
        BindAdaptor(const BasicSlot<R, A1>& slot, const A1& a)
        : BindAdaptorBase(slot)
        , _a(a)
        { }
        
        BindAdaptor(const BindAdaptor& c)
        : BindAdaptorBase(c)
        , _a(c._a)
        { }
        
        virtual Callable<R>* clone() const
        { return new BindAdaptor(*this); }

        virtual R operator()() const
        { return static_cast< const Callable<R, A1>* >( slot().callable() )->call(_a); }
    
    private:
        BindAdaptor& operator=(const BindAdaptor&);
    
    private:
        A1 _a;
};


template <typename R, typename A1, typename A2>
class BoundSlot : public BindAdaptor<R, A1, A2>::SlotBase
{
    public:
        template <typename T>
        BoundSlot(const BasicSlot<R, A1, A2>& slot, const T& a)
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
        BindAdaptor<R, A1, A2> _adaptor;     
};


template < typename R, typename A1, typename A2, typename T>
BoundSlot<R, A1, A2> slot(const BasicSlot<R, A1, A2>& slot, const T& a)
{
    return BoundSlot<R, A1, A2>(slot, a);
}

} // namespace Pt

#endif
