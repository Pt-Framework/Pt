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

#ifndef Pt_SerializationSurrogate_h
#define Pt_SerializationSurrogate_h

#include <Pt/Api.h>
#include <Pt/SerializationInfo.h>
#include <string>

namespace Pt {

class SerializationInfo;

/** @brief Customizes type serialization.

    @ingroup Serialization
*/
class SerializationSurrogate
{
    public:
        virtual ~SerializationSurrogate()
        {}

        const std::string& typeName() const
        { return _typeName; }

    protected:
        SerializationSurrogate(const std::string& typeName)
        : _typeName(typeName)
        {}

        SerializationSurrogate(const char* typeName)
        : _typeName(typeName)
        {}

    private:
        std::string _typeName;
};


template <typename T>
class BasicSerializationSurrogate : public SerializationSurrogate
{
    public:
        typedef void (*Compose)(const Pt::SerializationInfo& si, T& type);
        typedef void (*Decompose)(Pt::SerializationInfo& si, const T& type);

        BasicSerializationSurrogate(const std::string& typeName, Compose c, Decompose d)
        : SerializationSurrogate( typeName )
        , _decompose(d)
        , _compose(c)
        {}

        void decompose(Pt::SerializationInfo& si, const T& type) const
        { _decompose(si, type); }

        void compose(const Pt::SerializationInfo& si, T& type) const
        { _compose(si, type); }

    private:
        Decompose _decompose;
        Compose _compose;
};

} // namespace Pt

#endif // Pt_SerializationSurrogate_h
