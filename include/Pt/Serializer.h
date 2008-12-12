/*
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
#ifndef Pt_Serializer_h
#define Pt_Serializer_h

#include <Pt/Api.h>
#include <Pt/SerializationInfo.h>
#include <map>
#include <list>

namespace Pt {

class PT_API Serializer
{
    public:
        virtual ~Serializer();

        /** @brief Serialize an object to XML

            The serializer will serialize the object \a type as
            XML to the assigned stream. The string \a name will be used
            as the instance name of \a type and appear as the name of the
            XML element. The type must be serializable.
        */
        template <typename T>
        void serialize(const T& type, const std::string& name)
        {
            SerializationInfo& si = this->push(&type);
            si.setName(name);
            si <<= type;
        }

        /** @brief Serialize objects to XML

            Writes all serialized objects.
        */
        void finish();

    protected:
        Serializer();

        virtual void write(const SerializationInfo& si) = 0;

    private:
        SerializationInfo& push(const void* obj);

        //! @internal
        void fixdown(Pt::SerializationInfo& si);

        //! @internal
        std::list<Pt::SerializationInfo> _stack;

        //! @internal
        std::map<const void*, Pt::SerializationInfo*> _objects;
};

} // namespace Pt

#endif
