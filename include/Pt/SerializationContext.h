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
#include <Pt/TypeInfo.h>
#include <Pt/SerializationInfo.h>
#include <typeinfo>
#include <string>

namespace Pt {

class SerializationNode;
class SerializationCache;
class SerializationSurrogate;

class ISurrogate
{
    public:
        virtual ~ISurrogate()
        {}

        const std::type_info& valueType() const
        {   return *_valueType; }

        virtual void compose(void* type, const SerializationInfo::Value& value) = 0;

        virtual SerializationInfo::Value* decompose(const void* type) = 0;

    protected:
        ISurrogate(const std::type_info& valtype)
        : _valueType(&valtype)
        {}

    private:
        const std::type_info* _valueType;
};

template <typename T, typename V>
class BasicSurrogate : public ISurrogate
{
    public:
        BasicSurrogate()
        : ISurrogate( typeid(T) )
        {}

        virtual void compose(void* type, const SerializationInfo::Value& value)
        {}

        virtual SerializationInfo::Value* decompose(const void* type)
        {
            const T* type = reinterpret_cast<const T*>(type);
            V* value = new V;
            return value;
        }
};


class PT_API SerializationContext
{
    public:
        typedef SerializationInfo::FixupHandler FixupHandler;

        typedef void (*Deflate)(SerializationInfo& si);

        typedef void (*Inflate)(SerializationInfo& to, const SerializationInfo& from);

    public:
        SerializationContext();

        virtual ~SerializationContext();

        inline void enableReferencing(bool enabled)
        {
            _refsEnabled = enabled;
        }

        inline bool referencingEnabled() const
        {
            return _refsEnabled;
        }

        virtual void reset();

        virtual bool beginSave(const void* p, const std::string& name);

        virtual void finishSave();

        virtual void prepareId(const void* p);

        virtual const char* getId(const void* p);

        virtual const char* makeId(const void* p);

    public:
        virtual void beginLoad(void* obj, const std::type_info& fixupInfo,
                               const std::string& name, const std::string& id);

        virtual void finishLoad();

        virtual void rebindTarget(const std::string& id, void* obj);

        virtual void rebindFixup(const std::string& id, void* obj, void* prev);

        virtual void prepareFixup(void* obj, const std::string& id, FixupHandler, unsigned mid);

        virtual void fixup();

    public:
        void setLimit(size_t n);

        size_t limit() const;

        SerializationInfo* get();

        void push(SerializationInfo* si);

        void setSurrogates(const char* name, Deflate def, Inflate inf);

        SerializationSurrogate getSurrogate(const char* name);

        void registerSurrogate(const std::type_info& ti, ISurrogate* surrogate)
        {
            _surrmap[ti] = surrogate;
        }

        template <typename T>
        SerializationInfo::Value* decompose(const T& type)
        {
            Pt::TypeInfo toType = typeid(T);

            if( _surrmap.find( toType ) != _surrmap.end() )
            {
                return _surrmap[toType]->decompose(&type);
            }

            return 0;
        }

        template <typename T>
        bool compose(const SerializationInfo::Value& value, T& type)
        {
            Pt::TypeInfo toType = typeid(T);
            Pt::TypeInfo valueType = value.typeInfo();

            if( _surrmap.find( toType ) != _surrmap.end() )
            {
                if( _surrmap[toType]->valueType() == valueType)
                {
                    _surrmap[toType]->compose(&type, &value);
                    return true;
                }
            }

            return false;
        }

    private:
        SerializationCache* _cache;
        std::map<Pt::TypeInfo, ISurrogate*> _surrmap;
        bool _refsEnabled;
};

} // namespace Pt

#endif
