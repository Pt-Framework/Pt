/* Copyright (C) 2020-2026 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
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

/** @brief JSON Schema type for an MCP tool parameter.

    %Type is the schema node the parameter-schema group described. It
    is either a primitive from %integerType() and the other factories,
    or a composed type the caller owns. %typeId() is the JSON Schema
    kind. %toSchema() writes that kind as a JSON object, with an
    optional description. Composed types override %toSchema() to add
    properties, items, enum values, or null.

    The type does not own other types. An %ObjectType, %ArrayType, or
    %NullableType stores pointers to the types it names, and those
    types must outlive it. %Type is not copyable.

    @ingroup Pt-Mcp-Types
*/
class PT_MCP_API Type : private NonCopyable
{
  public:
    /** @brief JSON Schema kind of a %Type.
    */
    enum TypeId
    {
        Null = 0,     //!< JSON null
        Integer = 1,  //!< JSON integer
        Number = 2,   //!< JSON number
        String = 3,   //!< JSON string
        Boolean = 4,  //!< JSON boolean
        Object = 5,   //!< JSON object
        Array = 6     //!< JSON array
    };

    /** @brief Creates a type with schema kind @a id.
    */
    explicit Type(TypeId id)
    : _id(id)
    {}

    /** @brief Destroys the type.
    */
    virtual ~Type();

    /** @brief Returns the JSON Schema kind of this type.
    */
    TypeId typeId() const
    { return _id; }

    /** @brief Writes this type as a JSON Schema object to @a os.

        When @a description is not empty, it is written as the schema
        description member.
    */
    virtual void toSchema(std::ostream& os,
                          const std::string& description = "") const;

  private:
    TypeId _id;
};


/** @brief Returns the process-wide JSON null schema type.

    @ingroup Pt-Mcp-Types
*/
PT_MCP_API const Type& nullType();

/** @brief Returns the process-wide JSON integer schema type.

    @ingroup Pt-Mcp-Types
*/
PT_MCP_API const Type& integerType();

/** @brief Returns the process-wide JSON number schema type.

    @ingroup Pt-Mcp-Types
*/
PT_MCP_API const Type& numberType();

/** @brief Returns the process-wide JSON string schema type.

    @ingroup Pt-Mcp-Types
*/
PT_MCP_API const Type& stringType();

/** @brief Returns the process-wide JSON boolean schema type.

    @ingroup Pt-Mcp-Types
*/
PT_MCP_API const Type& booleanType();


/** @brief Named slot of an object schema or a tool parameter.

    %Property is the name, type, description, and required flag the
    schema group uses for both %ObjectType members and %Tool
    parameters. The type is not owned; it must outlive the property.
    A new property is required. %setOptional() clears that flag.

    @ingroup Pt-Mcp-Types
*/
class PT_MCP_API Property
{
  public:
    /** @brief Creates a required property named @a name of @a type.

        @a description is the schema description of this property.
    */
    Property(const std::string& name, const Type& type,
             const std::string& description = "");

    /** @brief Returns the property name.
    */
    const std::string& name() const
    { return _name; }

    /** @brief Returns the schema type of this property.
    */
    const Type& type() const
    { return *_type; }

    /** @brief Returns the schema description of this property.
    */
    const std::string& description() const
    { return _description; }

    /** @brief Returns true when the property is required.
    */
    bool isRequired() const
    { return _required; }

    /** @brief Marks the property as optional.
    */
    void setOptional()
    { _required = false; }

  private:
    std::string _name;
    const Type* _type;
    std::string _description;
    bool _required;
};


/** @brief JSON object schema with named properties.

    %ObjectType is a composed %Type whose schema is a JSON object.
    %addProperty() appends a required property. %setOptional() makes
    an existing property optional by name; a name that is not present
    is ignored. %setStrict() writes additionalProperties as false so
    undeclared members are rejected.

    Each property stores a pointer to its %Type. Those types must
    outlive this object type. The object type must outlive every
    %Tool that uses it as a parameter.

    @ingroup Pt-Mcp-Types
*/
class PT_MCP_API ObjectType : public Type
{
  public:
    /** @brief Creates an empty object schema.
    */
    ObjectType();

    /** @brief Destroys the object schema.
    */
    ~ObjectType();

    /** @brief Adds a required property named @a name of @a type.

        @a description is the schema description of the property.
    */
    ObjectType& addProperty(const std::string& name, const Type& type,
                            const std::string& description = "");

    /** @brief Marks the property named @a name as optional.
    */
    ObjectType& setOptional(const std::string& name);

    /** @brief Forbids properties that are not listed in the schema.
    */
    ObjectType& setStrict();

    /** @brief Returns the properties of this object schema.
    */
    const std::vector<Property>& properties() const
    { return _properties; }

    /** @brief Returns true when additional properties are forbidden.
    */
    bool isStrict() const
    { return _strict; }

    /** @brief Writes this object as a JSON Schema object to @a os.
    */
    void toSchema(std::ostream& os,
                  const std::string& description = "") const override;

  private:
    std::vector<Property> _properties;
    bool _strict;
};


/** @brief JSON array schema with one element type.

    %ArrayType is a composed %Type whose schema is a JSON array. The
    items type is the schema of every element. It is not owned; it
    must outlive this array type.

    @ingroup Pt-Mcp-Types
*/
class PT_MCP_API ArrayType : public Type
{
  public:
    /** @brief Creates an array schema whose items have type @a items.
    */
    explicit ArrayType(const Type& items);

    /** @brief Destroys the array schema.
    */
    ~ArrayType();

    /** @brief Returns the schema type of the array elements.
    */
    const Type& items() const
    { return *_items; }

    /** @brief Writes this array as a JSON Schema object to @a os.
    */
    void toSchema(std::ostream& os,
                  const std::string& description = "") const override;

  private:
    const Type* _items;
};


/** @brief String schema restricted to a fixed set of values.

    %EnumType is a composed %Type whose schema is a JSON string with
    an enum list. %addValue() appends an allowed string. The type id
    is %Type::String. The caller owns the enum type and must keep it
    alive while a tool names it.

    @ingroup Pt-Mcp-Types
*/
class PT_MCP_API EnumType : public Type
{
  public:
    /** @brief Creates an enum schema with no values.
    */
    EnumType();

    /** @brief Destroys the enum schema.
    */
    ~EnumType();

    /** @brief Adds @a value to the set of allowed strings.
    */
    EnumType& addValue(const std::string& value);

    /** @brief Returns the allowed string values.
    */
    const std::vector<std::string>& values() const
    { return _values; }

    /** @brief Writes this enum as a JSON Schema object to @a os.
    */
    void toSchema(std::ostream& os,
                  const std::string& description = "") const override;

  private:
    std::vector<std::string> _values;
};


/** @brief Schema that accepts a type or JSON null.

    %NullableType wraps another %Type so the schema allows null as
    well as that inner type. For primitives the schema uses a type
    array, for example {"type":["string","null"]}. For objects and
    arrays it uses oneOf. The inner type is not owned; it must
    outlive this wrapper.

    @ingroup Pt-Mcp-Types
*/
class PT_MCP_API NullableType : public Type
{
  public:
    /** @brief Creates a nullable wrapper around @a inner.
    */
    explicit NullableType(const Type& inner);

    /** @brief Destroys the nullable wrapper.
    */
    ~NullableType();

    /** @brief Returns the wrapped schema type.
    */
    const Type& inner() const
    { return *_inner; }

    /** @brief Writes this nullable type as a JSON Schema object to @a os.
    */
    void toSchema(std::ostream& os,
                  const std::string& description = "") const override;

  private:
    const Type* _inner;
};

} // namespace Mcp

} // namespace Pt

#endif // PT_MCP_TYPE_H
