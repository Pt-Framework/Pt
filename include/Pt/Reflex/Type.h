/*
 * Copyright (C) 2004-2010 by Marc Boris Duerner
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
#ifndef PT_REFLEX_TYPE_H
#define PT_REFLEX_TYPE_H

#include <Pt/Reflex/Api.h>
#include <Pt/Reflex/ConstructorInfo.h>
#include <Pt/Reflex/MethodInfo.h>
#include <Pt/Reflex/PropertyInfo.h>
#include <string>
#include <vector>
#include <map>

namespace Pt {

namespace Reflex {

class Argument;
class TypeManager;


class PT_REFLEX_API ConstructorTable
{
    typedef std::vector<Pt::Reflex::ConstructorInfo*> Container;

    public:
        class Iterator
        {
            public:
                Iterator()
                : _current(0)
                { }

                Iterator(const Iterator& other)
                : _current(other._current)
                {}

                explicit Iterator(Pt::Reflex::ConstructorInfo** pos)
                : _current(pos)
                { }

                Iterator& operator=(const Iterator& other)
                {
                    _current = other._current;
                    return *this;
                }

                Iterator& operator++()
                {
                    _current++;
                    return *this;
                }

                Pt::Reflex::ConstructorInfo& operator*()
                { return **_current; }

                Pt::Reflex::ConstructorInfo* operator->()
                { return *_current; }

                bool operator!=(const Iterator& other) const
                { return _current != other._current; }

                bool operator==(const Iterator& other) const
                { return _current == other._current; }

            private:
                Pt::Reflex::ConstructorInfo** _current;
        };

        class ConstIterator
        {
            public:
                ConstIterator()
                : _current(0)
                { }

                ConstIterator(const ConstIterator& other)
                : _current(other._current)
                {}

                explicit ConstIterator(const Pt::Reflex::ConstructorInfo* const* pos)
                : _current(pos)
                { }

                ConstIterator& operator=(const ConstIterator& other)
                {
                    _current = other._current;
                    return *this;
                }

                ConstIterator& operator++()
                {
                    _current++;
                    return *this;
                }

                const Pt::Reflex::ConstructorInfo& operator*()
                { return **_current; }

                const Pt::Reflex::ConstructorInfo* operator->()
                { return *_current; }

                bool operator!=(const ConstIterator& other) const
                { return _current != other._current; }

                bool operator==(const ConstIterator& other) const
                { return _current == other._current; }

            private:
                const Pt::Reflex::ConstructorInfo* const* _current;
        };

        ConstructorTable();

        ~ConstructorTable();

        Iterator begin();

        Iterator end();

        ConstIterator begin() const;

        ConstIterator end() const;

        std::size_t size() const;

        Pt::Reflex::ConstructorInfo* find(const Pt::Reflex::ArgumentList& args);

        Pt::Reflex::ConstructorInfo* find(Pt::Reflex::Type** args, std::size_t nargs);

        bool insert(Pt::Reflex::ConstructorInfo* ci);

        bool remove(Pt::Reflex::ConstructorInfo* ci);

    private:
        std::vector< Pt::Reflex::ConstructorInfo* > _entries;
};


class PT_REFLEX_API MethodTable
{
    typedef std::vector<Pt::Reflex::MethodInfo*> Container;

    public:
        class Iterator
        {
            public:
                Iterator()
                : _current(0)
                { }

                Iterator(const Iterator& other)
                : _current(other._current)
                {}

                explicit Iterator(Pt::Reflex::MethodInfo** pos)
                : _current(pos)
                { }

                Iterator& operator=(const Iterator& other)
                {
                    _current = other._current;
                    return *this;
                }

                Iterator& operator++()
                {
                    _current++;
                    return *this;
                }

                Pt::Reflex::MethodInfo& operator*()
                { return **_current; }

                Pt::Reflex::MethodInfo* operator->()
                { return *_current; }

                bool operator!=(const Iterator& other) const
                { return _current != other._current; }

                bool operator==(const Iterator& other) const
                { return _current == other._current; }

            private:
                Pt::Reflex::MethodInfo** _current;
        };

        class ConstIterator
        {
            public:
                ConstIterator()
                : _current(0)
                { }

                ConstIterator(const ConstIterator& other)
                : _current(other._current)
                {}

                explicit ConstIterator(const Pt::Reflex::MethodInfo* const* pos)
                : _current(pos)
                { }

                ConstIterator& operator=(const ConstIterator& other)
                {
                    _current = other._current;
                    return *this;
                }

                ConstIterator& operator++()
                {
                    _current++;
                    return *this;
                }

                const Pt::Reflex::MethodInfo& operator*()
                { return **_current; }

                const Pt::Reflex::MethodInfo* operator->()
                { return *_current; }

                bool operator!=(const ConstIterator& other) const
                { return _current != other._current; }

                bool operator==(const ConstIterator& other) const
                { return _current == other._current; }

            private:
                const Pt::Reflex::MethodInfo* const* _current;
        };

        MethodTable();

        ~MethodTable();

        Iterator begin();

        Iterator end();

        ConstIterator begin() const;

        ConstIterator end() const;

        std::size_t size() const;

        Pt::Reflex::MethodInfo* find(const std::string& name, const ArgumentList& args);

        Pt::Reflex::MethodInfo* find(const std::string& name, Type** args, std::size_t nargs);

        Pt::Reflex::MethodInfo* find(unsigned id);

        unsigned findId(const std::string& name, Type** args, std::size_t nargs);

        void insert(Pt::Reflex::MethodInfo* fi);

        bool remove(Pt::Reflex::MethodInfo* fi);

    private:
        std::vector< Pt::Reflex::MethodInfo* > _entries;
};


class PT_REFLEX_API PropertyTable
{
    typedef std::vector<Pt::Reflex::PropertyInfo*> Container;

    public:
        class Iterator
        {
            public:
                Iterator()
                : _current(0)
                { }

                Iterator(const Iterator& other)
                : _current(other._current)
                {}

                explicit Iterator(Pt::Reflex::PropertyInfo** pos)
                : _current(pos)
                { }

                Iterator& operator=(const Iterator& other)
                {
                    _current = other._current;
                    return *this;
                }

                Iterator& operator++()
                {
                    _current++;
                    return *this;
                }

                Pt::Reflex::PropertyInfo& operator*()
                { return **_current; }

                Pt::Reflex::PropertyInfo* operator->()
                { return *_current; }

                bool operator!=(const Iterator& other) const
                { return _current != other._current; }

                bool operator==(const Iterator& other) const
                { return _current == other._current; }

            private:
                Pt::Reflex::PropertyInfo** _current;
        };

        class ConstIterator
        {
            public:
                ConstIterator()
                : _current(0)
                { }

                ConstIterator(const ConstIterator& other)
                : _current(other._current)
                {}

                explicit ConstIterator(const Pt::Reflex::PropertyInfo* const* pos)
                : _current(pos)
                { }

                ConstIterator& operator=(const ConstIterator& other)
                {
                    _current = other._current;
                    return *this;
                }

                ConstIterator& operator++()
                {
                    _current++;
                    return *this;
                }

                const Pt::Reflex::PropertyInfo& operator*()
                { return **_current; }

                const Pt::Reflex::PropertyInfo* operator->()
                { return *_current; }

                bool operator!=(const ConstIterator& other) const
                { return _current != other._current; }

                bool operator==(const ConstIterator& other) const
                { return _current == other._current; }

            private:
                const Pt::Reflex::PropertyInfo* const* _current;
        };

        PropertyTable();

        ~PropertyTable();

        Iterator begin();

        Iterator end();

        ConstIterator begin() const;

        ConstIterator end() const;

        std::size_t size() const;

        Pt::Reflex::PropertyInfo* find(const std::string& name);

        bool insert(Pt::Reflex::PropertyInfo* fi);

        bool remove(Pt::Reflex::PropertyInfo* fi);

    private:
        std::vector< Pt::Reflex::PropertyInfo* > _entries;
};


class PT_REFLEX_API Type
{
    friend class TypeManager;

    public:
        static const unsigned InvalidMethodId = static_cast<unsigned>(-1);

    public:
        Type(const std::string& name);

        Type(const std::type_info& ti, const std::string& name);

        virtual ~Type();

        virtual void define(TypeManager& context)
        {}

        const std::type_info* id() const
        { return _id; }

        const std::string& name() const
        { return _name; }

        Type* base() const;

        void inherit(TypeManager& ctx, Type& base);

        bool isTypeOf(const Type& type) const
        {
            const std::string& name = type.name();

            const Type* t = this;
            while(t != 0)
            {
                if( t->_name == name )
                {
                    return true;
                }

                t = t->base();
            }

            return false;
        }

        bool isTypeOf(const char* typeName) const
        {
            const Type* t = this;
            while(t != 0)
            {
                if( t->_name == typeName )
                {
                    return true;
                }

                t = t->base();
            }

            return false;
        }

        bool isTypeOf(const std::type_info& id) const
        {
            const Type* t = this;

            while(t != 0)
            {
                if( t->_id && *t->_id == id )
                {
                    return true;
                }

                t = t->base();
            }

            return false;
        }

        virtual std::size_t size() const = 0;

        ConstructorInfo* constructor( const Pt::Reflex::ArgumentList& args);

        ConstructorInfo* constructor( Pt::Reflex::Type** args, std::size_t nargs);

        ConstructorTable& constructors()
        { return _ctab; }

        const ConstructorTable& constructors() const
        { return _ctab; }

        MethodInfo* method(const char* name, const ArgumentList& args);

        MethodInfo* method(unsigned id);

        unsigned methodId(const char* name, Pt::Reflex::Type** args, std::size_t nargs);

        MethodTable& methods()
        { return _mtab; }

        const MethodTable& methods() const
        { return _mtab; }

        PropertyInfo* property(const char* name);

        PropertyTable& properties()
        { return _ptab; }

        const PropertyTable& properties() const
        { return _ptab; }

        template <typename T>
        void registerConstructor( TypeManager& tm, T& type, void (T::*method)(void*) );

        template <typename T, typename A1>
        void registerConstructor( TypeManager& tm, T& type, void (T::*method)(void*, A1) );

        template <typename T, typename A1, typename A2>
        void registerConstructor( TypeManager& tm, T& type, void (T::*method)(void*, A1, A2) );

        template <typename T, typename A1, typename A2, typename A3>
        void registerConstructor( TypeManager& tm, T& type, void (T::*method)(void*, A1, A2, A3) );

        template <typename T>
        void registerMethod(const char* name, Pt::Any (*proxy)(T&),
                            Pt::Reflex::Type& rtype);

        template <typename T>
        void registerMethod(const char* name, Pt::Any (*proxy)(T&, Pt::Reflex::Argument&),
                            Pt::Reflex::Type& rtype, Pt::Reflex::Type& t1);

        template <typename T>
        void registerMethod(const char* name, Pt::Any (*proxy)(T&, Pt::Reflex::Argument&, Pt::Reflex::Argument&),
                            Pt::Reflex::Type& rtype, Pt::Reflex::Type& t1, Pt::Reflex::Type& t2);

        template <typename R, typename T>
        void registerMethod( TypeManager& context, const char* name, R (*method)(T&) );

        template <typename R, typename T, typename A1>
        void registerMethod( TypeManager& context, const char* name, R (*method)(T&, A1) );

        template <typename R, typename T, typename A1, typename A2>
        void registerMethod( TypeManager& context, const char* name, R (*method)(T&, A1, A2) );

        template <typename R, typename T>
        void registerMethod( TypeManager& context, const char* name, R (T::*method)() );

        template <typename R, typename T, typename A1>
        void registerMethod( TypeManager& context, const char* name, R (T::*method)(A1) );

        template <typename R, typename T, typename A1, typename A2>
        void registerMethod( TypeManager& context, const char* name, R (T::*method)(A1, A2) );

        template <typename R, typename T, typename A1, typename A2, typename A3>
        void registerMethod( TypeManager& context, const char* name, R (T::*method)(A1, A2, A3) );

        template <typename R, typename T, typename A1, typename A2, typename A3, typename A4>
        void registerMethod( TypeManager& context, const char* name, R (T::*method)(A1, A2, A3, A4) );

        template <typename C, typename T>
        void registerProperty( TypeManager& context, const char* name, T (*getter)(C&), void (*setter)(C&, T) );

        template <typename C, typename T>
        void registerProperty( TypeManager& context, const char* name, T (C::*getter)() const, void (C::*setter)(T) );

        bool registerConstructor(ConstructorInfo* mi);

        bool registerProperty(PropertyInfo* pi);

        void registerMethod(MethodInfo* mi);

        unsigned refs() const
        { return _refs; }

        TypeManager* parent()
        { return _tm; }

    protected:
        virtual PropertyInfo* onProperty(const char* name);

        void setParent(TypeManager* tm)
        { _tm = tm; }

    private:
        Type(const Type&);
        Type& operator=(const Type&);

    private:
        TypeManager* _tm;
        unsigned _refs;
        std::string _name;
        const std::type_info* _id;
        Type* _base;
        ConstructorTable _ctab;
        PropertyTable _ptab;
        MethodTable _mtab;
};


template <typename T>
class BasicType : public Reflex::Type
{
    public:
        BasicType(const std::string& name)
        : Type(typeid(T), name)
        {
        }

        std::size_t size() const 
        { return sizeof(T); }
};

}

}

#include <Pt/Reflex/ConstructorProxy.h>
#include <Pt/Reflex/MethodProxy.h>
#include <Pt/Reflex/PropertyProxy.h>
#include <Pt/Reflex/Property.h>

namespace Pt {

namespace Reflex {

template <typename T>
inline void Type::registerConstructor( TypeManager& tm, T& type, void (T::*proxy)(void*) )
{
    ConstructorProxy<T>* ci = new ConstructorProxy<T>(tm, type, proxy);
    bool ok = Type::registerConstructor(ci);
    if( ! ok )
        delete ci;
}


template <typename T, typename A1>
inline void Type::registerConstructor( TypeManager& tm, T& type, void (T::*proxy)(void*, A1) )
{
    ConstructorProxy<T, A1>* ci = new ConstructorProxy<T, A1>(tm, type, proxy);
    bool ok = Type::registerConstructor(ci);
    if( ! ok )
        delete ci;
}


template <typename T, typename A1, typename A2>
inline void Type::registerConstructor( TypeManager& tm, T& type, void (T::*proxy)(void*, A1, A2) )
{
    ConstructorProxy<T, A1, A2>* ci = new ConstructorProxy<T, A1, A2>(tm, type, proxy);
    bool ok = Type::registerConstructor(ci);
    if( ! ok )
        delete ci;
}


template <typename T, typename A1, typename A2,typename A3>
inline void Type::registerConstructor( TypeManager& tm, T& type, void (T::*proxy)(void*, A1, A2, A3) )
{
    ConstructorProxy<T, A1, A2, A3>* ci = new ConstructorProxy<T, A1, A2, A3>(tm, type, proxy);
    bool ok = Type::registerConstructor(ci);
    if( ! ok )
        delete ci;
}


template <typename T>
void Type::registerMethod(const char* name, Pt::Any (*proxy)(T&),
                         Pt::Reflex::Type& rtype)
{
    GenericMethod0<T>* gm =  new GenericMethod0<T>(proxy, name, rtype);
    this->registerMethod( gm );
}


template <typename T>
void Type::registerMethod(const char* name, Pt::Any (*proxy)(T&, Pt::Reflex::Argument&),
                         Pt::Reflex::Type& rtype, Pt::Reflex::Type& t1)
{
    GenericMethod1<T>* gm = new GenericMethod1<T>(proxy, name, rtype, t1);
    this->registerMethod( gm );
}


template <typename T>
void Type::registerMethod(const char* name, Pt::Any (*proxy)(T&, Pt::Reflex::Argument&, Pt::Reflex::Argument&),
                         Pt::Reflex::Type& rtype, Pt::Reflex::Type& t1, Pt::Reflex::Type& t2)
{
    GenericMethod2<T>* gm = new GenericMethod2<T>(proxy, name, rtype, t1, t2);
    this->registerMethod( gm );
}


template <typename R, typename T>
inline void Type::registerMethod( TypeManager& context, const char* name, R (*proxy)(T&) )
{
    MethodProxy<R, T>* mi = new MethodProxy<R, T>(context, name, proxy);
    Type::registerMethod(mi);
}


template <typename R, typename T, typename A1>
inline void Type::registerMethod( TypeManager& context, const char* name, R (*proxy)(T&, A1) )
{
    MethodProxy<R, T, A1>* mi = new MethodProxy<R, T, A1>(context, name, proxy);
    Type::registerMethod(mi);
}


template <typename R, typename T, typename A1, typename A2>
inline void Type::registerMethod( TypeManager& context, const char* name, R (*proxy)(T&, A1, A2) )
{
    MethodProxy<R, T, A1, A2>* mi = new MethodProxy<R, T, A1, A2>(context, name, proxy);
    Type::registerMethod(mi);
}


template <typename R, typename T>
inline void Type::registerMethod( TypeManager& context, const char* name, R (T::*proxy)() )
{
    Method<R, T>* mi = new Method<R, T>(context, name, proxy);
    Type::registerMethod(mi);
}


template <typename R, typename T, typename A1>
inline void Type::registerMethod( TypeManager& context, const char* name, R (T::*proxy)(A1) )
{
    Method<R, T, A1>* mi = new Method<R, T, A1>(context, name, proxy);
    Type::registerMethod(mi);
}


template <typename R, typename T, typename A1, typename A2>
inline void Type::registerMethod( TypeManager& context, const char* name, R (T::*proxy)(A1, A2) )
{
    Method<R, T, A1, A2>* mi = new Method<R, T, A1, A2>(context, name, proxy);
    Type::registerMethod(mi);
}


template <typename R, typename T, typename A1, typename A2, typename A3>
inline void Type::registerMethod( TypeManager& context, const char* name, R (T::*proxy)(A1, A2, A3) )
{
    Method<R, T, A1, A2, A3>* mi = new Method<R, T, A1, A2, A3>(context, name, proxy);
    Type::registerMethod(mi);
}


template <typename R, typename T, typename A1, typename A2, typename A3, typename A4>
inline void Type::registerMethod( TypeManager& context, const char* name, R (T::*proxy)(A1, A2, A3, A4) )
{
    Method<R, T, A1, A2, A3, A4>* mi = new Method<R, T, A1, A2, A3, A4>(context, name, proxy);
    Type::registerMethod(mi);
}


template <typename C, typename T>
void Type::registerProperty( TypeManager& tm, const char* name, 
                             T (*getter)(C&), void (*setter)(C&, T) )
{
    PropertyInfo* pi = new ReadWritePropertyProxy<C, T>(tm, name, getter, setter);
    if ( ! Type::registerProperty(pi) )
    {
        delete pi;
    }
}


template <typename C, typename T>
void Type::registerProperty( TypeManager& tm, const char* name, 
                             T (C::*getter)() const, void (C::*setter)(T) )
{
    PropertyInfo* pi = new Property<C, T>(tm, name, getter, setter);
    if ( ! Type::registerProperty(pi) )
    {
        delete pi;
    }
}

}

}

#endif
