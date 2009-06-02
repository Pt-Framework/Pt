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
#include <vector>
#include <map>
#include <set>

namespace Pt {

class ValueNode;
class ObjectNode;
class ValueNode;

class PT_API SerializationContext
{
    public:
        SerializationContext();

        virtual ~SerializationContext();

        virtual void beginUnlinkTarget(const std::string& name, const void* p);

        virtual void finishUnlinkTarget();

        virtual void prepareUnlink(const void* p);

        virtual bool isUnlinked(const void* p);

        virtual std::string getUnlinkId(const void* p);

    public:
        virtual void beginLinkTarget(const std::string& name, const std::string& id,
                                     void* obj, const std::type_info& fixupInfo);

        virtual void finishLinkTarget();

        virtual void prepareLink(const std::string& id, void* obj, const std::type_info& fixupInfo);

        virtual bool checkLink(const std::type_info& from, const std::type_info& to);

        virtual void link();

    public:
        SerializationInfo* get();

        void push(SerializationInfo::Node* node);

        void push(SerializationInfo* si);

        ValueNode* getScalarData();

        SerializationInfo::Node* getObjectData();

    private:
        std::vector<SerializationInfo*> _infos;
        std::vector<ValueNode*> _scalars;
        std::vector<ObjectNode*> _objects;
        std::vector<ReferenceNode*> _refs;

};


class SerializationBinder
{
    public:
        SerializationBinder()
        {}

        virtual ~SerializationBinder()
        {}

        virtual void beginBindTarget(const std::string& name, const std::string& id,
                                     void* obj, const std::type_info& fixupInfo) = 0;

        virtual void finishBindTarget() = 0;

        virtual void prepareBind(const std::string& id, void* obj, const std::type_info& fixupInfo) = 0;

        virtual bool checkBinding(const std::type_info& from, const std::type_info& to) = 0;

        virtual void bind() = 0;
};

} // namespace Pt

#endif
