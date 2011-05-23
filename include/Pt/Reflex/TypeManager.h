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
#ifndef PT_REFLEX_TYPEMANAGER_H
#define PT_REFLEX_TYPEMANAGER_H

#include <Pt/Reflex/Api.h>
#include <map>
#include <vector>
#include <string>

namespace Pt {

namespace Reflex {

class Type;
class Argument;
class ArgumentList;
class GenericType;
class FunctionInfo;
class TypeSpecifier;

class PT_REFLEX_API FunctionTable
{
    typedef std::vector<Pt::Reflex::FunctionInfo*> Container;

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

                explicit Iterator(Pt::Reflex::FunctionInfo** pos)
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

                Pt::Reflex::FunctionInfo& operator*()
                { return **_current; }

                Pt::Reflex::FunctionInfo* operator->()
                { return *_current; }

                bool operator!=(const Iterator& other) const
                { return _current != other._current; }

                bool operator==(const Iterator& other) const
                { return _current == other._current; }

            private:
                Pt::Reflex::FunctionInfo** _current;
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

                explicit ConstIterator(const Pt::Reflex::FunctionInfo* const* pos)
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

                const Pt::Reflex::FunctionInfo& operator*()
                { return **_current; }

                const Pt::Reflex::FunctionInfo* operator->()
                { return *_current; }

                bool operator!=(const ConstIterator& other) const
                { return _current != other._current; }

                bool operator==(const ConstIterator& other) const
                { return _current == other._current; }

            private:
                const Pt::Reflex::FunctionInfo* const* _current;
        };

        FunctionTable();

        ~FunctionTable();

        Iterator begin();

        Iterator end();

        ConstIterator begin() const;

        ConstIterator end() const;

        unsigned size() const;

        Pt::Reflex::FunctionInfo* find(const std::string& name);

        Pt::Reflex::FunctionInfo* find(const std::string& name, const Pt::Reflex::ArgumentList& args);

        Pt::Reflex::FunctionInfo* find(const std::string& name, Pt::Reflex::Type** args, size_t nargs);

        bool insert(Pt::Reflex::FunctionInfo* fi);

        bool remove(Pt::Reflex::FunctionInfo* fi);

    private:
        std::vector< Pt::Reflex::FunctionInfo* > _entries;
};


class PT_REFLEX_API TypeTable
{
    typedef std::vector<Pt::Reflex::Type*> Container;

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

                explicit Iterator(Pt::Reflex::Type** pos)
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

                Pt::Reflex::Type& operator*()
                { return **_current; }

                Pt::Reflex::Type* operator->()
                { return *_current; }

                bool operator!=(const Iterator& other) const
                { return _current != other._current; }

                bool operator==(const Iterator& other) const
                { return _current == other._current; }

            private:
                Pt::Reflex::Type** _current;
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

                explicit ConstIterator(const Pt::Reflex::Type* const* pos)
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

                const Pt::Reflex::Type& operator*()
                { return **_current; }

                const Pt::Reflex::Type* operator->()
                { return *_current; }

                bool operator!=(const ConstIterator& other) const
                { return _current != other._current; }

                bool operator==(const ConstIterator& other) const
                { return _current == other._current; }

            private:
                const Pt::Reflex::Type* const* _current;
        };

        TypeTable();

        ~TypeTable();

        Iterator begin();

        Iterator end();

        ConstIterator begin() const;

        ConstIterator end() const;

        unsigned size() const;

        Pt::Reflex::Type* find(const std::type_info& ti);

        Pt::Reflex::Type* find(const std::string& name);

        bool insert(Pt::Reflex::Type* fi);

        bool remove(Pt::Reflex::Type* fi);

    private:
        std::vector< Pt::Reflex::Type* > _entries;
};


class PT_REFLEX_API TypeManager
{
    typedef std::map<std::string, GenericType*> GenericsMap;

    public:
        TypeManager();

        TypeManager(TypeManager& parent);

        virtual ~TypeManager();

        virtual void* alloc(size_t bytes) = 0;

        virtual void dealloc(void* p, size_t bytes) = 0;

        Type* getType(const std::type_info& ti);

        Type* getType(const std::string& typeName);

        Type* getType(const TypeSpecifier& typeSpec);

        GenericType* getGeneric(const std::string& baseName);

        bool registerType(Type& type);

        void registerType(Type& type, GenericType& generic, Type& t1);

        bool unregisterType(Type& type);

        void registerType(GenericType& type);

        // template <typename R, typename A1, typename A2>
        // void registerFunction( const char* name, R (*func)(A1, A2) );

        template <typename A1>
        void registerFunction( const char* name, void (*func)(A1) );

        bool registerFunction(Pt::Reflex::FunctionInfo* fi);

        bool unregisterFunction(Pt::Reflex::FunctionInfo* fi);

        Pt::Reflex::FunctionInfo* function(const std::string& name);

        Pt::Reflex::FunctionInfo* function(const std::string& name, const Pt::Reflex::ArgumentList& args);

        Pt::Reflex::FunctionInfo* function(const std::string& name, Pt::Reflex::Type** args, size_t nargs);

        TypeTable& types()
        { return _ttab; }

        const TypeTable& types() const
        { return _ttab; }

        FunctionTable& functions()
        { return _functions; }

        const FunctionTable& functions() const
        { return _functions; }

    private:
        TypeManager* _parent;
        GenericsMap _gtypes;
        TypeTable _ttab;
        FunctionTable _functions;
};

}

}

#include <Pt/Reflex/FunctionProxy.h>

namespace Pt {

namespace Reflex {

// template <typename R, typename A1, typename A2>
// inline void TypeManager::registerFunction( const char* name, R (*proxy)(A1, A2) )
// {
//     FunctionProxy<R, A1, A2>* fi = new FunctionProxy<R, A1, A2>(*this, name, proxy);
//     if ( ! TypeManager::registerFunction(fi) )
//     {
//         delete fi;
//     }
// }
/*
template <typename A1>
inline void TypeManager::registerFunction( const char* name, void (*proxy)(A1) )
{
    FunctionProxy<A1>* fi = new FunctionProxy<A1>(*this, name, proxy);
    if ( ! TypeManager::registerFunction(fi) )
    {
        delete fi;
    }
}
*/
}

}

#endif
