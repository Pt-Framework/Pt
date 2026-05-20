/*
 * Copyright (C) 2020-2026 by Marc Boris Duerner
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

#ifndef PT_MCP_TYPE_H
#define PT_MCP_TYPE_H

#include <Pt/Mcp/Api.h>
#include <Pt/NonCopyable.h>
#include <string>
#include <vector>
#include <iosfwd>

namespace Pt {

namespace Mcp {

/** @brief JSON Schema type descriptor for MCP tool parameters.
*/
class PT_MCP_API Type : private NonCopyable
{
  public:
    enum TypeId
    {
        Null = 0,
        Integer = 1,
        Number = 2,
        String = 3,
        Boolean = 4,
        Object = 5,
        Array = 6
    };

    explicit Type(TypeId id)
    : _id(id)
    {}

    virtual ~Type();

    TypeId typeId() const
    { return _id; }

    /** @brief Writes the JSON Schema representation to the stream.
    */
    virtual void toSchema(std::ostream& os,
                          const std::string& description = "") const;

  private:
    TypeId _id;
};


PT_MCP_API const Type& nullType();

PT_MCP_API const Type& integerType();

PT_MCP_API const Type& numberType();

PT_MCP_API const Type& stringType();

PT_MCP_API const Type& booleanType();


/** @brief Property of an ObjectType.
*/
class PT_MCP_API Property
{
  public:
    Property(const std::string& name, const Type& type,
             const std::string& description = "");

    const std::string& name() const
    { return _name; }

    const Type& type() const
    { return *_type; }

    const std::string& description() const
    { return _description; }

    bool isRequired() const
    { return _required; }

    void setOptional()
    { _required = false; }

  private:
    std::string _name;
    const Type* _type;
    std::string _description;
    bool _required;
};


/** @brief Object type with named properties.

    Describes a JSON object schema with properties, each having
    a name, type, and required flag.
*/
class PT_MCP_API ObjectType : public Type
{
  public:
    ObjectType();

    ~ObjectType();

    ObjectType& addProperty(const std::string& name, const Type& type,
                            const std::string& description = "");

    ObjectType& setOptional(const std::string& name);

    /** @brief Disallow properties not listed in the schema.
    */
    ObjectType& setStrict();

    const std::vector<Property>& properties() const
    { return _properties; }

    bool isStrict() const
    { return _strict; }

    void toSchema(std::ostream& os,
                  const std::string& description = "") const override;

  private:
    std::vector<Property> _properties;
    bool _strict;
};


/** @brief Array type with element type.

    Describes a JSON array schema with a single items type.
*/
class PT_MCP_API ArrayType : public Type
{
  public:
    explicit ArrayType(const Type& items);

    ~ArrayType();

    const Type& items() const
    { return *_items; }

    void toSchema(std::ostream& os,
                  const std::string& description = "") const override;

  private:
    const Type* _items;
};


/** @brief Enum type with a fixed set of allowed string values.
*/
class PT_MCP_API EnumType : public Type
{
  public:
    EnumType();

    ~EnumType();

    EnumType& addValue(const std::string& value);

    const std::vector<std::string>& values() const
    { return _values; }

    void toSchema(std::ostream& os,
                  const std::string& description = "") const override;

  private:
    std::vector<std::string> _values;
};


/** @brief Nullable wrapper that allows null as an alternative value.

    Outputs a schema with type as an array, e.g. {"type":["string","null"]}.
*/
class PT_MCP_API NullableType : public Type
{
  public:
    explicit NullableType(const Type& inner);

    ~NullableType();

    const Type& inner() const
    { return *_inner; }

    void toSchema(std::ostream& os,
                  const std::string& description = "") const override;

  private:
    const Type* _inner;
};

} // namespace Mcp

} // namespace Pt

#endif // PT_MCP_TYPE_H
