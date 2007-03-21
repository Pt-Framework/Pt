/***************************************************************************
 *   Copyright (C) 2007 by Marc Boris Dürner                               *
 *   Copyright (C) 2007 by Laurentiu-Gheorghe Crisan                       *
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
#ifndef Pt_PropertyProxy_h
#define Pt_PropertyProxy_h

#include <Pt/Exception.h>
#include <Pt/TypeInfo.h>
#include <Pt/Any.h>
#include <Pt/Method.h>
#include <Pt/ConstMethod.h>
#include <Pt/Signal.h>


namespace Pt {

class PT_API AbstractProperty
{
    public:
        AbstractProperty()
        {}

        virtual ~AbstractProperty()
        {}

        virtual AbstractProperty* clone() const = 0;

        virtual Pt::Any value()
        { throw std::logic_error("AbstractProperty is not readable" + PT_SOURCEINFO); }

        // Set value and notify all listeners
        virtual void setValue(const Pt::Any& value)
        { throw std::logic_error("AbstractProperty is not writable" + PT_SOURCEINFO); }

        Signal<> onValueChanged;
};


template <typename T>
class ReadPropertyProxy : virtual public AbstractProperty
{
    public:

        template <typename Object, typename ObjectBase>
        ReadPropertyProxy( Object* parent, T (ObjectBase::*getter)() const )
        : AbstractProperty()
        {
            _getter = new Pt::ConstMethod<T, Object>( parent, getter );
        }

        template <typename Object, typename ObjectBase>
        ReadPropertyProxy( Object* parent, T (ObjectBase::*getter)() )
        : AbstractProperty()
        {
            _getter = new Pt::Method<T, Object>( parent, getter );
        }

        ReadPropertyProxy( const ReadPropertyProxy& property )
        : AbstractProperty()
        {
            _getter = property._getter->clone();
        }

        ~ReadPropertyProxy()
        { delete _getter; }

        AbstractProperty* clone() const
        { return new ReadPropertyProxy<T>( *this ); }

        virtual Pt::Any value()
        {
            Pt::Any any;
            any = this->get();
            return any;
        }

        T get() const
        { return _getter->operator()(); }

        T operator()() const
        { return get(); }

    private:
        Pt::Callable<T>* _getter;
};


template <typename T>
class WritePropertyProxy : virtual public AbstractProperty
{
    public:
        template <typename R, typename Object, typename ObjectBase>
        WritePropertyProxy(Object* parent, R (ObjectBase::*setter)(T type) )
        : AbstractProperty()
        {
            _setter = new Pt::Method<R, Object, T>(parent, setter);
        }

        WritePropertyProxy(const WritePropertyProxy& property)
        : AbstractProperty()
        {
            _setter = property._setter->cloneInvokable();
        }

        ~WritePropertyProxy()
        {
            delete _setter;
        }

        AbstractProperty* clone() const
        {
            return new WritePropertyProxy( *this );
        }

        virtual void setValue(const Pt::Any& a)
        {
            typedef typename Pt::TypeInfo<T>::ConstReference ConstRefT ;

            try {
                ConstRefT val = Pt::any_cast<ConstRefT>(a) ;
                this->set( val );
            }
            catch(...) {
                std::cerr << "WritePropertyProxy: Type mismatch: " << a.typeName() << std::endl;
            }
        }

        void operator=(T type)
        { this->set(type); }

        void set(T type)
        {
            _setter->invoke(type);
            onValueChanged.send();
        }

    private:
        Pt::Invokable<T>* _setter;
};


template <typename R, typename A = R>
class PropertyProxy : public ReadPropertyProxy<R>, public WritePropertyProxy<A> {
    public:
        template <typename R2, typename Object, typename ObjectBase>
        PropertyProxy(Object* parent, R (ObjectBase::*getter)() const, R2 (ObjectBase::*setter)(A type) )
        : ReadPropertyProxy<R>(parent, getter)
        , WritePropertyProxy<A>(parent, setter)
        { }

        template <typename R2, typename Object, typename ObjectBase>
        PropertyProxy(Object* parent, R (ObjectBase::*getter)(), R2 (ObjectBase::*setter)(A type) )
        : ReadPropertyProxy<R>(parent, getter)
        , WritePropertyProxy<A>(parent, setter)
        { }

        PropertyProxy(const PropertyProxy& property)
        : AbstractProperty()
        , ReadPropertyProxy<R>(property)
        , WritePropertyProxy<A>(property)
        {}

        AbstractProperty* clone() const
        { return new PropertyProxy(*this);}

        virtual Pt::Any value()
        {
            Pt::Any any;
            any = ReadPropertyProxy<R>::get();
            return any;
        }

        virtual void setValue(const Pt::Any& any)
        { WritePropertyProxy<A>::setValue(any); }
};

} // namespace Pt

#endif
