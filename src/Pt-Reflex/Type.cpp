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
#include "Pt/Reflex/Type.h"
#include "Pt/Reflex/TypeManager.h"
#include <stdexcept>
#include <iostream>

namespace Pt {

namespace Reflex {

ConstructorTable::ConstructorTable()
{
}


ConstructorTable::~ConstructorTable()
{
    for(Iterator it = begin(); it != end(); ++it)
    {
        Pt::Reflex::ConstructorInfo& ci = *it;

        if( ci.unref() == 0 )
        {
            delete &ci;
        }
    }
}


ConstructorTable::Iterator ConstructorTable::begin()
{
    if( _entries.empty() )
        return Iterator();

    return Iterator( &_entries[0] );
}


ConstructorTable::Iterator ConstructorTable::end()
{
    if( _entries.empty() )
        return Iterator();

    Pt::Reflex::ConstructorInfo** it = & _entries.back();
    return Iterator( ++it );
}


ConstructorTable::ConstIterator ConstructorTable::begin() const
{
    if( _entries.empty() )
        return ConstIterator();

    return ConstIterator( &_entries[0] );
}


ConstructorTable::ConstIterator ConstructorTable::end() const
{
    if( _entries.empty() )
        return ConstIterator();

    const Pt::Reflex::ConstructorInfo* const* it = & _entries.back();
    return ConstIterator( ++it );
}


unsigned ConstructorTable::size() const
{
    return _entries.size();

}


Pt::Reflex::ConstructorInfo* ConstructorTable::find(const Pt::Reflex::ArgumentList& args)
{
    size_t nargs = args.size();

    Container::iterator it;
    for(it = _entries.begin(); it != _entries.end(); ++it)
    {
        Pt::Reflex::ConstructorInfo* ci = *it;

        if( nargs == ci->psize() )
        {
            if(nargs == 0)
                return ci;

            size_t n = 0;
            Pt::Reflex::Type** param = ci->params();
            ArgumentIterator iter = args.begin();
            for(; n < nargs; ++n, ++iter, ++param)
            {
                if( ! iter->type().isTypeOf(**param) )
                    break;
            }

            if(n == nargs)
                return ci;
        }
    }

    return 0;
}


Pt::Reflex::ConstructorInfo* ConstructorTable::find(Pt::Reflex::Type** args, size_t nargs)
{
    Container::iterator it;
    for(it = _entries.begin(); it != _entries.end(); ++it)
    {
        Pt::Reflex::ConstructorInfo* ci = *it;

        if( nargs == ci->psize() )
        {
            if(nargs == 0)
                return ci;

            Pt::Reflex::Type** param = ci->params();
            size_t n = 0;
            for( ; n < nargs; ++n, ++param)
            {
                if( ! args[n]->isTypeOf(**param) )
                    break;
            }

            if(n == nargs)
                return ci;
        }
    }

    return 0;
}


bool ConstructorTable::insert(Pt::Reflex::ConstructorInfo* ci)
{
    unsigned nargs = ci->psize();

    Container::iterator it;
    for(it = _entries.begin(); it != _entries.end(); ++it)
    {
        if( nargs == (*it)->psize() )
        {
            if(nargs == 0)
                return false;

            size_t n = 0;
            Pt::Reflex::Type** param = ci->params();
            Pt::Reflex::Type** bparam = (*it)->params();

            for( ; n < nargs; ++n, ++param, ++bparam)
            {
                if( ! (*param)->isTypeOf(**bparam) )
                    break;
            }

            if(n == nargs)
                return false;
        }
    }

    _entries.push_back(ci);
    ci->ref();

    return true;
}


bool ConstructorTable::remove(Pt::Reflex::ConstructorInfo* ci)
{
    Container::iterator it;
    for(it = _entries.begin(); it != _entries.end(); ++it)
    {
        if( *it == ci )
        {
            _entries.erase(it);

            if( ci->unref() == 0 )
            {
                delete &ci;
            }

            return true;
        }
    }

    return false;
}


MethodTable::MethodTable()
{
}


MethodTable::~MethodTable()
{
    Iterator fit;
    for(fit = begin(); fit != end(); ++fit)
    {
        Pt::Reflex::MethodInfo& mi = *fit;
        if( mi.unref() == 0 )
        {
            delete &mi;
        }
    }
}


MethodTable::Iterator MethodTable::begin()
{
    if( _entries.empty() )
        return Iterator();

    return Iterator( &_entries[0] );
}


MethodTable::Iterator MethodTable::end()
{
    if( _entries.empty() )
        return Iterator();

    Pt::Reflex::MethodInfo** it = & _entries.back();
    return Iterator( ++it );
}


MethodTable::ConstIterator MethodTable::begin() const
{
    if( _entries.empty() )
        return ConstIterator();

    return ConstIterator( &_entries[0] );
}


MethodTable::ConstIterator MethodTable::end() const
{
    if( _entries.empty() )
        return ConstIterator();

    const Pt::Reflex::MethodInfo* const* it = & _entries.back();
    return ConstIterator( ++it );
}


unsigned MethodTable::size() const
{
    return _entries.size();

}


Pt::Reflex::MethodInfo* MethodTable::find(const std::string& name, const ArgumentList& args)
{
    Container::iterator it;
    for(it = _entries.begin(); it != _entries.end(); ++it)
    {
        Pt::Reflex::MethodInfo* mi = *it;

        if( name == (*it)->name() && args.size() == mi->psize() )
        {
            if(args.size() == 0)
                return mi;

            Pt::Reflex::ArgumentIterator iter = args.begin();
            Pt::Reflex::Type** param = mi->params();

            for(size_t n = 0; n < args.size(); ++param, ++iter, ++n)
            {
                Pt::Reflex::Type& arg = iter->type();

                if( arg.isTypeOf(**param) )
                    return mi;
            }
        }
    }

    return 0;
}


Pt::Reflex::MethodInfo* MethodTable::find(const std::string& name, Pt::Reflex::Type** args, size_t nargs)
{
    Container::iterator it;
    for(it = _entries.begin(); it != _entries.end(); ++it)
    {
        Pt::Reflex::MethodInfo* mi = *it;

        if( name == (*it)->name() && nargs == mi->psize() )
        {
            if(nargs == 0)
                return mi;

            Pt::Reflex::Type** param = mi->params();

            size_t n = 0;
            for( ; n < nargs; ++n, ++param)
            {
                Pt::Reflex::Type* arg = args[n];

                if( ! arg->isTypeOf(**param) )
                    break;
            }

            if(n == nargs)
                return mi;
        }
    }

    return 0;
}


Pt::Reflex::MethodInfo* MethodTable::find(unsigned id)
{
    if( id >= _entries.size() )
        return 0;

    return _entries[id];
}


unsigned MethodTable::findId(const std::string& name, Pt::Reflex::Type** args, size_t nargs)
{
    unsigned id = 0;

    for(Container::iterator it = _entries.begin(); it != _entries.end(); ++it)
    {
        Pt::Reflex::MethodInfo* mi = *it;

        if( name == (*it)->name() && nargs == mi->psize() )
        {
            if(nargs == 0)
                return id;

            Pt::Reflex::Type** param = mi->params();

            size_t n = 0;
            for( ;n < nargs; ++n, ++param)
            {
                if( ! args[n]->isTypeOf(**param) )
                    break;
            }

            if(n == nargs)
                return id;
        }

        ++id;
    }

    return Pt::Reflex::Type::InvalidMethodId;
}


void MethodTable::insert(Pt::Reflex::MethodInfo* mi)
{
    unsigned nargs = mi->psize();

    //
    //  overwrite method if found
    //
    Container::iterator it;
    for(it = _entries.begin(); it != _entries.end(); ++it)
    {
        if( 0 != strcmp( mi->name(), (*it)->name() ) ||
            nargs != (*it)->psize() )
            continue;

        bool match = true;
        Pt::Reflex::Type** param = mi->params();
        Pt::Reflex::Type** bparam = (*it)->params();

        for(size_t n = 0; n < nargs; ++n, ++param, ++bparam)
        {
            if( ! (*param)->isTypeOf(**bparam) )
            {
                match = false;
                break;
            }
        }

        if(match)
            break;
    }

    if( it != _entries.end() )
    {
        if( (*it)->unref() == 0 )
            delete *it;

        *it = mi;
        mi ->ref();
        return;
    }

    _entries.push_back(mi);
    mi->ref();
}


bool MethodTable::remove(Pt::Reflex::MethodInfo* mi)
{
    Container::iterator it;
    for(it = _entries.begin(); it != _entries.end(); ++it)
    {
        if( *it == mi )
        {
            _entries.erase(it);

            if( mi->unref() == 0 )
            {
                delete &mi;
            }

            return true;
        }
    }

    return false;
}


PropertyTable::PropertyTable()
{
}


PropertyTable::~PropertyTable()
{
    Iterator fit;
    for(fit = begin(); fit != end(); ++fit)
    {
        Pt::Reflex::PropertyInfo& fi = *fit;
        if( fi.refs() == 0 )
        {
            delete &fi;
        }
    }
}


PropertyTable::Iterator PropertyTable::begin()
{
    if( _entries.empty() )
        return Iterator();

    return Iterator( &_entries[0] );
}


PropertyTable::Iterator PropertyTable::end()
{
    if( _entries.empty() )
        return Iterator();

    Pt::Reflex::PropertyInfo** it = & _entries.back();
    return Iterator( ++it );
}


PropertyTable::ConstIterator PropertyTable::begin() const
{
    if( _entries.empty() )
        return ConstIterator();

    return ConstIterator( &_entries[0] );
}


PropertyTable::ConstIterator PropertyTable::end() const
{
    if( _entries.empty() )
        return ConstIterator();

    const Pt::Reflex::PropertyInfo* const* it = & _entries.back();
    return ConstIterator( ++it );
}


unsigned PropertyTable::size() const
{
    return _entries.size();

}


Pt::Reflex::PropertyInfo* PropertyTable::find(const std::string& name)
{
    Container::iterator it;
    for(it = _entries.begin(); it != _entries.end(); ++it)
    {
        if(name == (*it)->name())
            return *it;
    }

    return 0;
}


bool PropertyTable::insert(Pt::Reflex::PropertyInfo* pi)
{
    if( 0 != this->find( pi->name() ) )
    {
        return false;
    }

    _entries.push_back(pi);
    pi->ref();
    return true;
}


bool PropertyTable::remove(Pt::Reflex::PropertyInfo* fi)
{
    Container::iterator it;
    for(it = _entries.begin(); it != _entries.end(); ++it)
    {
        if( *it == fi )
        {
            _entries.erase(it);

            if( fi->unref() == 0 )
            {
                delete &fi;
            }

            return true;
        }
    }

    return false;
}


Type::Type(const std::string& name)
: _tm(0)
, _refs(1)
, _name(name)
, _id(0)
, _base(0)
{
}


Type::Type(const std::type_info& ti, const std::string& name)
: _tm(0)
, _refs(1)
, _name(name)
, _id(&ti)
, _base(0)
{
}


Type::~Type()
{
    if(_tm)
    {
        _tm->unregisterType(*this);
    }
}


void Type::inherit(TypeManager& ctx, Type& base)
{
    base.define(ctx);

    if( this->methods().size() == 0 )
    {
        Pt::Reflex::MethodTable::Iterator it;
        for( it = base.methods().begin(); it != base.methods().end(); ++it)
        {
            Pt::Reflex::MethodInfo& mi = *it;
            this->registerMethod(&mi);
        }
    }

    if( this->properties().size() == 0 )
    {
        Pt::Reflex::PropertyTable::Iterator it;
        for( it = base.properties().begin(); it != base.properties().end(); ++it)
        {
            Pt::Reflex::PropertyInfo& pi = *it;
            this->registerProperty(&pi);
        }
    }

    _base = &base;
}


bool Type::registerConstructor(ConstructorInfo* ci)
{
    return _ctab.insert(ci);
}


ConstructorInfo* Type::constructor( const Pt::Reflex::ArgumentList& args)
{
    return _ctab.find(args);
}


ConstructorInfo* Type::constructor( Pt::Reflex::Type** args, size_t nargs)
{
    return _ctab.find(args, nargs);
}


MethodInfo* Type::method(const char* method, const ArgumentList& args)
{
    return _mtab.find(method, args);
}


MethodInfo* Type::method(unsigned id)
{
    return  _mtab.find(id);
}


unsigned Type::methodId(const char* name, Pt::Reflex::Type** args, size_t nargs)
{
    return _mtab.findId(name, args, nargs);
}


void Type::registerMethod(MethodInfo* mi)
{
    _mtab.insert(mi);
}


PropertyInfo* Type::property(const char* name)
{
    PropertyInfo* pi = _ptab.find(name);

    if( pi )
        return pi;

    return this->onProperty(name);
}


PropertyInfo* Type::onProperty(const char* name)
{
    return 0;
}


bool Type::registerProperty(PropertyInfo* pi)
{
    return _ptab.insert(pi);
}


Type* Type::base() const
{
    return _base;
}

}

}
