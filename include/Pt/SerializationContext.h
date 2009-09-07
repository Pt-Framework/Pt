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

class PT_API SerializationContext
{
    public:
        typedef void (*FixupHandler)(void* fixme,
                                     void* target, const std::type_info& targetType);

        typedef void (*Serialize)(SerializationInfo& si);

    public:
        SerializationContext();

        virtual ~SerializationContext();

        virtual void reset();

        virtual bool beginSave(const void* p, const std::string& name);

        virtual void finishSave();

        virtual void prepareId(const void* p);

        virtual const char* getId(const void* p);

    public:
        virtual void beginLoad(void* obj, const std::type_info& fixupInfo,
                               const std::string& name, const std::string& id);

        virtual void finishLoad();

        virtual void prepareFixup(void* obj, const std::string& id, FixupHandler);

        virtual void fixup();

    public:
        void setLimit(size_t n);

        size_t limit() const;

        SerializationInfo* get();

        void push(SerializationInfo* si);

        SerializationNode* get(SerializationInfo::Category category);

        void push(SerializationNode* node);

    private:
        SerializationCache* _cache;
};

} // namespace Pt

#endif
