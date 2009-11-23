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


class ISerializationInfo
{
    public:
        virtual ~ISerializationInfo()
        {}

        virtual void saveReference(const void* p) = 0;

        virtual ISerializationInfo& addMember(const std::string& name) = 0;

        virtual void setTypeName(const std::string& type) = 0;

        virtual bool beginSave(const void* p, const std::string& name = "") = 0;

        virtual void finishSave() = 0;
};


template <typename T>
void operator <<=(Pt::ISerializationInfo& si, const T& type)
{
    // export symbols, if any are reachable
    // prepare id's if any
}


template <typename T>
inline void operator <<=(ISerializationInfo& si, const T* ptr)
{
    si.saveReference( ptr );
    //std::cerr << "saveReference " << ptr << std::endl;
    si.setTypeName("reference");
}


class ISaveInfo
{
    public:
        explicit ISaveInfo(ISerializationInfo& info)
        : si(&info)
        {}

        template <typename T>
        bool save(const T& type)
        {
            bool first = si->beginSave( &type );
            if(first)
            {
                *si <<= type;
                 si->finishSave();
            }

            return first;
        }

        template <typename T>
        void put(const T& type)
        {
            *si <<= type;
        }

        ISerializationInfo* si;
};


template <typename T>
inline void operator<<= (ISerializationInfo& si, const Save<T>& sv)
{
    ISaveInfo info(si);
    info <<= *(sv.type);
}


template <typename T>
inline void operator<<= (ISaveInfo& si, const T& type)
{
    si.save( type );
}


class PT_API SerializationContext : public ISerializationInfo
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

        void saveReference(const void* p)
        { this->prepareId(p); }

        ISerializationInfo& addMember(const std::string& name)
        { return *this; }

        void setTypeName(const std::string&)
        {}

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

/*
struct BreakDown
{
    BreakDown(SerializationContext& ctx)
    : _ctx(ctx)
    {}

    template <typename T>
    bool save(const T& t, const char* name)
    {
        if( _ctx.beginSave(&t, name) )
        {
            breakdown(*this, t);
            _ctx.finishSave();
            return true;
        }

        return false;
    }

    template <typename T>
    void prepareId(const T* t)
    {
        _ctx.prepareId(t);
    }

    SerializationContext& _ctx;
};


template <typename T>
void breakdown(Pt::BreakDown& b, const T& type)
{
    // export symbols, if any are reachable
    // prepare id's if any
}


template <typename T>
void breakdown(Pt::BreakDown& b, const T* t)
{
    b.prepareId(t);
}


template <typename T>
void symbolize(Pt::BreakDown& b, const T& type, const char* name)
{
    b.save(type, name);
}
*/
/*
template <typename T>
void operator <<=(Pt::SerializationContext& ctx, const T& type)
{
    // export symbols, if any are reachable
    // prepare id's if any
}


template <typename T>
void operator <<=(Pt::SerializationContext& ctx, const T* type)
{
    ctx.prepareId(type);
}


struct sym
{
    sym(const char* n)
    : name(n)
    {}

    const char* name;
};


template <>
struct SaveInfo<sym>
{
    SaveInfo(SerializationContext& ctx, const char* n)
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

    template <typename T>
    void put(const T& t)
    {
        *context <<= t;
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


template <typename T>
Symbol<T> operator <<=(const sym& s, const T& t)
{
    return Symbol<T>(t, s.name);
}


template <typename T>
inline void operator <<=(SerializationContext& context, const Symbol<T>& s)
{
    SaveInfo<sym> si(context, s.name);
    si <<= *(s.type);
}
*/
} // namespace Pt

#endif
