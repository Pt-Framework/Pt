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
#include "Pt/Reflex/TypeManager.h"
#include "Pt/Reflex/Type.h"
#include "Pt/Reflex/GenericType.h"
#include "Pt/Reflex/FunctionInfo.h"
#include "Pt/Reflex/TypeSpecifier.h"
#include <stdexcept>
#include <iostream>

namespace Pt {

namespace Reflex {

FunctionTable::FunctionTable()
{
}


FunctionTable::~FunctionTable()
{
    for(Iterator fit = begin(); fit != end(); ++fit)
    {
        Pt::Reflex::FunctionInfo& fi = *fit;

        if( fi.refs() == 0 )
        {
            delete &fi;
        }
    }
}


FunctionTable::Iterator FunctionTable::begin()
{
    if( _entries.empty() )
        return Iterator();

    return Iterator( &_entries[0] );
}


FunctionTable::Iterator FunctionTable::end()
{
    if( _entries.empty() )
        return Iterator();

    Pt::Reflex::FunctionInfo** it = & _entries.back();
    return Iterator( ++it );
}


FunctionTable::ConstIterator FunctionTable::begin() const
{
    if( _entries.empty() )
        return ConstIterator();

    return ConstIterator( &_entries[0] );
}


FunctionTable::ConstIterator FunctionTable::end() const
{
    if( _entries.empty() )
        return ConstIterator();

    const Pt::Reflex::FunctionInfo* const* it = & _entries.back();
    return ConstIterator( ++it );
}


unsigned FunctionTable::size() const
{
    return _entries.size();

}


Pt::Reflex::FunctionInfo* FunctionTable::find(const std::string& name)
{
    Container::iterator it;
    for(it = _entries.begin(); it != _entries.end(); ++it)
    {
        if(name == (*it)->name())
            return *it;
    }

    return 0;
}


Pt::Reflex::FunctionInfo* FunctionTable::find(const std::string& name, const Pt::Reflex::ArgumentList& args)
{
    size_t nargs = args.size();

    Container::iterator it;
    for(it = _entries.begin(); it != _entries.end(); ++it)
    {
        Pt::Reflex::FunctionInfo* fi = *it;

        if( name == (*it)->name() && nargs == fi->psize() )
        {
            if(nargs == 0)
                return fi;

            size_t n = 0;
            Pt::Reflex::Type** param = fi->params();
            ArgumentIterator iter = args.begin();
            for(; n < nargs; ++n, ++iter, ++param)
            {
                if( ! iter->type().isTypeOf(**param) )
                    break;
            }

            if(n == nargs)
                return fi;
        }
    }

    return 0;
}


Pt::Reflex::FunctionInfo* FunctionTable::find(const std::string& name, Pt::Reflex::Type** args, size_t nargs)
{
    Container::iterator it;
    for(it = _entries.begin(); it != _entries.end(); ++it)
    {
        Pt::Reflex::FunctionInfo* fi = *it;

        if( name == fi->name() && nargs == fi->psize() )
        {
            if(nargs == 0)
                return fi;

            Pt::Reflex::Type** param = fi->params();
            size_t n = 0;
            for( ; n < nargs; ++n, ++param)
            {
                if( ! args[n]->isTypeOf(**param) )
                    break;
            }

            if(n == nargs)
                return fi;
        }
    }

    return 0;
}


bool FunctionTable::insert(Pt::Reflex::FunctionInfo* fi)
{
    unsigned nargs = fi->psize();

    Container::iterator it;
    for(it = _entries.begin(); it != _entries.end(); ++it)
    {
        if( 0 == strcmp( fi->name(), (*it)->name() ) && nargs == (*it)->psize() )
        {
            if(nargs == 0)
                return false;

            size_t n = 0;
            Pt::Reflex::Type** param = fi->params();
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

    _entries.push_back(fi);
    return true;
}


bool FunctionTable::remove(Pt::Reflex::FunctionInfo* fi)
{
    Container::iterator it;
    for(it = _entries.begin(); it != _entries.end(); ++it)
    {
        if( *it == fi )
        {
            _entries.erase(it);

            if( fi->refs() == 0 )
            {
                delete &fi;
            }

            return true;
        }
    }

    return false;
}


TypeTable::TypeTable()
{
}


TypeTable::~TypeTable()
{
    for(Iterator it = begin(); it != end(); ++it)
    {
        Pt::Reflex::Type& type = *it;

        if( type.refs() == 0 )
        {
            delete &type;
        }
    }
}


TypeTable::Iterator TypeTable::begin()
{
    if( _entries.empty() )
        return Iterator();

    return Iterator( &_entries[0] );
}


TypeTable::Iterator TypeTable::end()
{
    if( _entries.empty() )
        return Iterator();

    Pt::Reflex::Type** it = & _entries.back();
    return Iterator( ++it );
}


TypeTable::ConstIterator TypeTable::begin() const
{
    if( _entries.empty() )
        return ConstIterator();

    return ConstIterator( &_entries[0] );
}


TypeTable::ConstIterator TypeTable::end() const
{
    if( _entries.empty() )
        return ConstIterator();

    const Pt::Reflex::Type* const* it = & _entries.back();
    return ConstIterator( ++it );
}


unsigned TypeTable::size() const
{
    return _entries.size();

}


Pt::Reflex::Type* TypeTable::find(const std::type_info& ti)
{
    Container::iterator it;
    for(it = _entries.begin(); it != _entries.end(); ++it)
    {
        const std::type_info* tinfo = (*it)->id();
        if( tinfo && ti == *tinfo )
            return *it;
    }

    return 0;
}


Pt::Reflex::Type* TypeTable::find(const std::string& name)
{
    Container::iterator it;
    for(it = _entries.begin(); it != _entries.end(); ++it)
    {
        if( name == (*it)->name() )
            return *it;
    }

    return 0;
}


bool TypeTable::insert(Pt::Reflex::Type* fi)
{
    if( 0 != this->find( fi->name() ) )
    {
        return false;
    }

    _entries.push_back(fi);
    return true;
}


bool TypeTable::remove(Pt::Reflex::Type* type)
{
    Container::iterator it;
    for(it = _entries.begin(); it != _entries.end(); ++it)
    {
        if( *it == type )
        {
            _entries.erase(it);

            if( type->refs() == 0 )
            {
                delete &type;
            }

            return true;
        }
    }

    return false;
}


TypeManager::TypeManager()
: _parent(0)
{
}


TypeManager::TypeManager(TypeManager& parent)
: _parent(&parent)
{
}


TypeManager::~TypeManager()
{
    Pt::Reflex::FunctionTable::Iterator it;
    for( it = _functions.begin(); it != _functions.end(); ++it)
    {
        it->setParent(0);
    }

    Pt::Reflex::TypeTable::Iterator tit;
    for( tit = _ttab.begin(); tit != _ttab.end(); ++tit)
    {
        tit->setParent(0);
    }
}


bool TypeManager::registerType(Type& type)
{
    if( this == type.parent() )
    {
        return false;
    }

    bool ok = _ttab.insert(&type);

    if( ok  )
    {
        if( type.parent() )
            type.parent()->unregisterType(type); // nothrow

        type.setParent(this);
    }

    return ok;
}


bool TypeManager::unregisterType(Type& type)
{
    if( this != type.parent() )
    {
        return false;
    }

    bool ok = _ttab.remove(&type);
    if(ok)
    {
        type.setParent(0);
    }

    return ok;
}


void TypeManager::registerType(GenericType& type)
{
    _gtypes[ type.name() ] = &type;
}


Type* TypeManager::getType(const std::type_info& ti)
{
    Type* type = _ttab.find(ti);
    if( ! type )
    {
        if(_parent)
            return _parent->getType(ti);

        throw std::invalid_argument(std::string("no such type: ") + ti.name());
    }

    return type;
}


Type* TypeManager::getType(const std::string& typeName)
{
    Type* type = _ttab.find(typeName);
    if( ! type )
    {
        if(_parent)
            return _parent->getType(typeName);

        throw std::invalid_argument( "no such type: " + typeName );
    }

    return type;
}


Type* TypeManager::getType(const TypeSpecifier& typeSpec)
{
    if( typeSpec.params().empty() )
        return this->getType(typeSpec.name());

    GenericType* generic = this->getGeneric(typeSpec.name());

    if( typeSpec.params().size() > 1 )
        throw std::logic_error( "multiple type params not yet supported" );

    Type* t1 = this->getType( typeSpec.params()[0] );

    Type* type =  generic->getType( *t1);

    registerType(*type);
    type->define(*this);

    return type;
}


GenericType* TypeManager::getGeneric(const std::string& baseName)
{
    GenericsMap::iterator it = _gtypes.find(baseName);
    if( it == _gtypes.end() )
    {
        if(_parent)
            return _parent->getGeneric(baseName);

        throw std::invalid_argument("no such type " + std::string(baseName) );
    }

    return  it->second;
}


void TypeManager::registerType(Type& type, GenericType& generic, Type& t1)
{
    generic.setType(t1, type);
    registerType(type);
}


bool TypeManager::registerFunction(Pt::Reflex::FunctionInfo* fi)
{
    if( this == fi->parent() )
    {
        return false;
    }

    bool ok = _functions.insert(fi);

    if( ok  )
    {
        if( fi->parent() )
            fi->parent()->unregisterFunction(fi); // nothrow

        fi->setParent(this);
    }

    return ok;
}


bool TypeManager::unregisterFunction(Pt::Reflex::FunctionInfo* fi)
{
    if( this != fi->parent() )
    {
        return false;
    }

    bool ok = _functions.remove(fi);
    if(ok)
    {
        fi->setParent(0);
    }

    return ok;
}


Pt::Reflex::FunctionInfo* TypeManager::function(const std::string& name)
{
    return _functions.find(name);
}


Pt::Reflex::FunctionInfo* TypeManager::function(const std::string& name, const Pt::Reflex::ArgumentList& args)
{
    return _functions.find(name, args);
}


Pt::Reflex::FunctionInfo* TypeManager::function(const std::string& name, Pt::Reflex::Type** args, size_t nargs)
{
    return _functions.find(name, args, nargs);
}

}

}
