/*
 * Copyright (C) 2008 by Marc Boris Duerner
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
#ifndef Pt_SerializationContext_h
#define Pt_SerializationContext_h

#include <Pt/Api.h>
#include <Pt/SerializationInfo.h>
#include <typeinfo>
#include <string>

namespace Pt {

class SerializationInfo;
class SerializationNode;
class SerializationCache;

class SerializationSurrogate
{
    public:
        virtual ~SerializationSurrogate()
        {}

        virtual void pack(SerializationInfo& it) const = 0;

        virtual void unpack(SerializationInfo& to, const SerializationInfo& from) const = 0;

    protected:
        SerializationSurrogate()
        {}
};


class PT_API SerializationContext
{
    public:
        typedef void (*FixupHandler)(void* fixme,
                                     void* target, const std::type_info& targetType);

    public:
        SerializationContext();

        virtual ~SerializationContext();

        virtual void reset();

        virtual bool beginSave(const void* p, const std::string& name);

        virtual void finishSave();

        virtual void prepareId(const void* p);

        virtual const char* getId(const void* p);

        virtual const char* makeId(const void* p)
        { return 0; }

    public:
        virtual void beginLoad(void* obj, const std::type_info& fixupInfo,
                               const std::string& name, const std::string& id);

        virtual void finishLoad();

        virtual void prepareFixup(void* obj, const std::string& id, FixupHandler);

        virtual void* getFixup(void* ref, const char* name) const
        { return 0; }

        virtual void fixup();

    public:
        void setLimit(size_t n);

        size_t limit() const;

        SerializationInfo* get();

        void push(SerializationInfo* si);

        SerializationNode* get(SerializationInfo::Category category);

        void push(SerializationNode* node);

        void addSurrogate(const char* name, SerializationSurrogate* surrogate);

        SerializationSurrogate* surrogate(const char* name) const;

    private:
        SerializationCache* _cache;
};


template <typename T>
void operator <<=(Pt::SerializationContext& ctx, const T& type)
{
    // export symbols, if any are reachable
    // prepare id's if any
}


struct SymbolInfo
{
    SymbolInfo(SerializationContext& ctx, const char* n)
    : context(&ctx)
    , name(n)
    {}

    template <typename T>
    bool save(const T& t)
    {
        if( context->beginSave(&t, name) )
        {
            *context <<= t;
            context->finishSave();
            return true;
        }

        return false;
    }

    SerializationContext* context;
    const char* name;
};


template <typename T>
struct Symbol
{
    Symbol(const T& t, const char* n)
    : type(&t)
    , name(n)
    {}

    const T* type;
    const char* name;
};


struct sym
{
    sym(const char* n)
    : name(n)
    {}

    const char* name;
};


template <typename T>
Symbol<T> operator <<=(const sym& s, const T& t)
{
    return Symbol<T>(t, s.name);
}


template <typename T>
void operator <<=(SymbolInfo& s, const T& t)
{
    s.save(t);
}


template <typename T>
inline void operator <<=(SerializationContext& context, const Symbol<T>& s)
{
    SymbolInfo si(context, s.name);
    si <<= *(s.type);
}

} // namespace Pt

#endif
