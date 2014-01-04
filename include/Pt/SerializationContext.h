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
#include <Pt/FixupInfo.h>
#include <Pt/SerializationInfo.h>
#include <Pt/SerializationSurrogate.h>
#include <typeinfo>
#include <string>

namespace Pt {

class SerializationContextImpl;

/** @brief Context for the serialization of types.

    @ingroup Serialization
*/
class PT_API SerializationContext : public SerializationInfo
{
    public:
        SerializationContext();

        virtual ~SerializationContext();

        inline void enableReferencing(bool enabled)
        { _refsEnabled = enabled; }

        inline bool isReferencing() const
        { return _refsEnabled; }

        SerializationInfo* get();

        void push(SerializationInfo* si);

        template <typename T>
        void registerSurrogate( const std::string& typeName,
                                void (*compose)(const Pt::SerializationInfo& si, T& type),
                                void (*decompose)(Pt::SerializationInfo& si, const T& type) );

        template <typename T>
        void registerSurrogate( const char* typeName,
                                void (*compose)(const Pt::SerializationInfo& si, T& type),
                                void (*decompose)(Pt::SerializationInfo& si, const T& type) );

        template <typename T>
        bool compose(const SerializationInfo& si, T& type) const;

        template <typename T>
        bool decompose(SerializationInfo& si, const T& type);

        template <typename T>
        const BasicSerializationSurrogate<T>* getSurrogate() const;

        /** @internal Workaround for some compilers (GCC 3.x).
        */
        template <typename T>
        friend const BasicSerializationSurrogate<T>* getSurrogate(SerializationContext*);

        const SerializationSurrogate* getSurrogate(const std::type_info& ti) const;

    public:
        /** @brief Clears all content.
        */
        void clear()
        { onClear(); }

        bool beginSave(const void* p, const char* name)
        { return onBeginSave(p, name); }

        void finishSave()
        { onFinishSave(); }

        void prepareId(const void* p)
        { onPrepareId(p); }

        const char* getId(const void* p)
        { return onGetId(p); }

        const char* makeId(const void* p)
        { return onMakeId(p); }

    public:     
        void beginLoad(void* obj, const std::type_info& ti,
                       const char* name, const char* id)
        { onBeginLoad(obj, ti, name, id); }
        
        void finishLoad()
        { onFinishLoad(); }

        void rebindTarget(const char* id, void* obj)
        { onRebindTarget(id, obj); }

        void rebindFixup(const char* id, void* obj, void* prev)
        { onRebindFixup(id, obj, prev); }

        void prepareFixup(void* obj, const char* id, FixupInfo::FixupHandler fh, unsigned mid)
        { onPrepareFixup(obj, id, fh, mid); }

        void fixup()
        { onFixup(); }

    protected:
        virtual void onClear();

        virtual bool onBeginSave(const void* p, const char* name);

        virtual void onFinishSave();

        virtual void onPrepareId(const void* p);

        virtual const char* onGetId(const void* p);

        virtual const char* onMakeId(const void* p);

        virtual void onBeginLoad(void* obj, const std::type_info& fixupInfo,
                                 const char* name, const char* id);

        virtual void onFinishLoad();

        virtual void onRebindTarget(const char* id, void* obj);

        virtual void onRebindFixup(const char* id, void* obj, void* prev);

        virtual void onPrepareFixup(void* obj, const char* id, FixupInfo::FixupHandler fh, unsigned mid);

        virtual void onFixup();

    private:
        void registerSurrogate(const std::type_info& ti, SerializationSurrogate* surrogate);

        SerializationContext(const SerializationContext& si)
        {}

        SerializationContext& operator=(const SerializationContext& si)
        { return *this; }

    private:
        SerializationContextImpl* _cache;
        bool _refsEnabled;
};


template <typename T>
inline bool SerializationContext::compose(const SerializationInfo& si, T& type) const
{
    const BasicSerializationSurrogate<T>* surr = this->getSurrogate<T>();
    if( ! surr )
        return false;

    surr->compose(si, type);
    return true;
}


template <typename T>
inline bool SerializationContext::decompose(SerializationInfo& si, const T& type)
{
    const BasicSerializationSurrogate<T>* surr = this->getSurrogate<T>();
    if( ! surr )
        return false;

    surr->decompose(si, type);
    si.setTypeName( surr->typeName() );
    return true;
}


template <typename T>
inline const BasicSerializationSurrogate<T>* SerializationContext::getSurrogate() const
{
    const SerializationSurrogate* surr = this->getSurrogate( typeid(T) );
    if( ! surr )
        return 0;

    return static_cast<const BasicSerializationSurrogate<T>*>(surr);
}


template <typename T>
inline void SerializationContext::registerSurrogate( const std::string& typeName,
                                                     void (*compose)(const Pt::SerializationInfo& si, T& type),
                                                     void (*decompose)(Pt::SerializationInfo& si, const T& type) )
{
    SerializationSurrogate* surr = new BasicSerializationSurrogate<T>(typeName, compose, decompose);
    registerSurrogate(typeid(T), surr);
}


template <typename T>
inline void SerializationContext::registerSurrogate( const char* typeName,
                                                     void (*compose)(const Pt::SerializationInfo& si, T& type),
                                                     void (*decompose)(Pt::SerializationInfo& si, const T& type) )
{
    SerializationSurrogate* surr = new BasicSerializationSurrogate<T>(typeName, compose, decompose);
    registerSurrogate(typeid(T), surr);
}

} // namespace Pt

#endif // Pt_SerializationContext_h
