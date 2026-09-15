/* Copyright (C) 2008 Marc Boris Duerner

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  As a special exception, you may use this file as part of a free
  software library without restriction. Specifically, if other files
  instantiate templates or use macros or inline functions from this
  file, or you compile this file and link it with other files to
  produce an executable, this file does not by itself cause the
  resulting executable to be covered by the GNU General Public
  License. This exception does not however invalidate any other
  reasons why the executable file might be covered by the GNU Library
  General Public License.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
  MA 02110-1301 USA
*/

#ifndef PT_API_SERIALIZATION_H
#define PT_API_SERIALIZATION_H

/** @addtogroup Pt-Serialization

    @brief Composing and decomposing types to text or binary formats.

    Data structures and types can be serialized to text or binary formats using
    %Pt's serialization. This is used within the framework to load and store
    data or to implement remote procedure calls. It is extensible to work with
    all kinds of types, including STL containers, PODs (plain old data types),
    builtin language types or custom data types. The framework separates the
    process of composing and decomposing types from the formatting stage,
    resulting in a two-phase serialization process. This also allows to resolve
    and fixup shared pointers or references.

    A type is serializable, if two operators are implemented to compose and
    decompose it to a SerializationInfo. The SerializationContext provides
    improved memory management, a mechanism to generate IDs for shared pointers
    and a way to further customize or override serialization for a type.
    Alternatively, performance can be increased by implementing a Composer
    or Decomposer for the type, however it is more complicated to do so.

    Various formats are supported by implementing Formatters. Other modules
    of the framework also implement Formatters, for example to support
    serialization to XML. The Serializer and Deserializer combine a Formatter
    and a SerializationContext, manage composition and decomposition and thus
    form the high-level interface for the serialization of a set of types.

    The %Pt framework already provides serialization support for the C++
    builtin types and the types provided by the C++ standard library, like
    std::string or std::vector. To make custom types serializable, the
    serialization operators have to be implemented. The next example shows a
    simple data type and the declarations of the serialization operators:

    @code
    struct Address
    {
        Address()
        : code(0)
        {}

        std::string country;
        std::string city;
        std::string street;
        unsigned code;
    };

    void operator >>=(const Pt::SerializationInfo& si, Address& address);
    void operator <<=(Pt::SerializationInfo& si, const Address& address);
    @endcode

    Similar to the insertion and extraction operators for standard C++
    iostreams, one operator has to be overloaded to serialize a type and
    another one to deserialize it. The types of operators (<<= and >>=)
    indicate that this is an assignment operation. Each type has to be
    composed from or decomposed to Pt::SerializationInfo objects, which form
    a tree representing the object graph. It contains all meta information
    so composition and decomposition can be separated from formatting and
    parsing. Building up the tree is highly optimized and, for example,
    requires only very few allocations. The next example shows the
    definition of the serialization operator:

    @code
    void operator<<=(Pt::SerializationInfo& si, const Address& address)
    {
        si.addMember("country") <<= address.country;
        si.addMember("city")    <<= address.city;
        si.addMember("street")  <<= address.street;
        si.addMember("code").setUInt32(address.code);
        si.setTypeName("Address");
    }
    @endcode

    The SerializationInfo passed by reference to the operator is meant to
    represent an Address object in the object graph. SerializationInfo child
    nodes are added for each member variable using
    Pt::SerializationInfo::addMember(), which also assigns the name. Member
    types can be serialized to the returned SerializationInfo using their
    specific overload of the operator. For builtin integer types it is
    recommended to use a setter (here Pt::SerializationInfo::setUInt32())
    instead of the serialization operator, to be specific on the type.
    Integer types could potentially be serialized differently depending on
    the platform. For example, a @c long could be serialized as a 32-bit or a
    64-bit integer type. Finally, the type name is set for the parent node,
    representing the Address object.

    The deserialization operator performs the same process, just in reverse.
    The SerializationInfo object passed to it contains the meta information
    for all members. SerializationInfo child nodes can be obtained by name
    using Pt::SerializationInfo::getMember() and members can be deserialized
    with their overloads of the deserialization operators. The following
    example illustrates this:

    @code
    void operator>>=(const Pt::SerializationInfo& si, Address& address)
    {
        si.getMember("country") >>= address.country;
        si.getMember("city")    >>= address.city;
        si.getMember("street")  >>= address.street;
        si.getMember("code")    >>= address.code;
    }
    @endcode
*/

#endif
