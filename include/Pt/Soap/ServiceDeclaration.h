/*
 * Copyright (C) 2014 by Dr. Marc Boris Duerner
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

#ifndef Pt_Soap_ServiceDeclaration_h
#define Pt_Soap_ServiceDeclaration_h

#include <Pt/Soap/Api.h>
#include <Pt/NonCopyable.h>
#include <Pt/String.h>
#include <Pt/Types.h>
#include <string>
#include <vector>
#include <map>

namespace Pt {

namespace Soap {

class Parameter;

/** @brief XML schema type for WSDLs.
    @since 1.1.0
*/
class Type : private NonCopyable
{
    public:
        enum TypeId
        {
            Array = 1,
            Struct = 2,
            Bool = 3,
            Int = 4,
            Float = 5,
            String = 6,
            Base64 = 7,
            Dict = 8,
            DictElement = 9
        };
    
    public:
        explicit Type(TypeId typeId)
        : _typeId(typeId)
        {}

        virtual ~Type()
        {}

        TypeId typeId() const
        { return _typeId; }

        virtual bool isSimple() const = 0;

        virtual const Parameter* getParameter(std::size_t n) const = 0;

        virtual const Parameter* getParameter(const std::string& name) const = 0;
                
        virtual const char* name() const = 0;

        virtual std::size_t size() const = 0;

    private:
        TypeId _typeId;    
};


class SimpleType : public Type
{
    public:
        explicit SimpleType(TypeId typeId)
        : Type(typeId)
        {            
        }

        virtual ~SimpleType()
        {}

        virtual bool isSimple() const
        {
            return true;
        }

        virtual const Parameter* getParameter(std::size_t) const
        { return 0; }

        virtual const Parameter* getParameter(const std::string&) const
        { return 0; }

        std::size_t size() const
        {
            return 0;
        }
};


class ComplexType : public Type
{
    public:
        explicit ComplexType(TypeId typeId, const std::string& name)
        : Type(typeId)    
        , _name(name)    
        {            
        }

        virtual ~ComplexType()
        {}

        virtual bool isSimple() const
        {
            return false;
        }

        virtual const char* name() const
        {
            return _name.c_str();
        }

    private:    
        std::string _name;
};


class Parameter
{
    public:
        Parameter()
        : _type(0)
        , _min(1)
        , _max(1)
        {}
        
        Parameter(const std::string& name, const Type& t)
        : _name(name)
        , _type(&t)
        , _min(1)
        , _max(1)
        { }

        virtual ~Parameter()
        {}

        const Type* type() const
        { return _type; }

        const std::string& name() const
        { return _name; }

        void set(const std::string& name, const Type& t)
        {
            _name = name;
            _type = &t;
        }

        int minOccurs() const
        {
          return _min;
        }

        int maxOccurs() const
        {
          return _max;
        }

        void setOccurrence(int min, int max)
        {
          _min = min;
          _max= max;
        }

    private:
        std::string _name;
        const Type* _type;
        int _min;
        int _max;
};


class PT_SOAP_API BooleanType : public SimpleType
{
    public:
        BooleanType();

        virtual ~BooleanType();

        virtual const char* name() const
        {
            return "boolean";
        }
};


class PT_SOAP_API IntegerType : public SimpleType
{
    public:
        IntegerType();

        virtual ~IntegerType();  

        virtual const char* name() const
        {
            return "int";
        }
};


class PT_SOAP_API FloatType : public SimpleType
{
    public:
        FloatType();

        virtual ~FloatType();

        virtual const char* name() const
        {
            return "double";
        }
};


class PT_SOAP_API StringType : public SimpleType
{
    public:
        StringType();

        virtual ~StringType();

        virtual const char* name() const
        {
            return "string";
        }
};


class PT_SOAP_API Base64Type : public SimpleType
{
    public:
        Base64Type();

        virtual ~Base64Type();

        virtual const char* name() const
        {
            return "base64Binary";
        }
};


class PT_SOAP_API StructType : public ComplexType
{
    public:
        StructType(const std::string& name);

        virtual ~StructType();

        void addParameter(const std::string& name, const Type& param, 
                          int minOccurence = 1, int maxOccurence = 1);

        virtual const Parameter* getParameter(std::size_t n) const;

        virtual const Parameter* getParameter(const std::string& name) const;

        virtual std::size_t size() const
        {
            return _paramList.size();
        }

    private:
        typedef std::vector<Parameter> ParameterList;
        ParameterList _paramList;
};


class PT_SOAP_API ArrayType : public ComplexType
{
    public:
        ArrayType(const std::string& name);
        
        ArrayType(const std::string& name, const Type& elem, const std::string& elemName);

        virtual ~ArrayType();

        void setElement(const std::string& elemName, const Type& elem);

        virtual const Parameter* getParameter(std::size_t n) const;

        virtual const Parameter* getParameter(const std::string& name) const;

        virtual std::size_t size() const
        {
            return 1;
        }

    private:
        Parameter _elem;
};


class PT_SOAP_API DictElementType : public ComplexType
{
    public:
        DictElementType(const std::string& name);

        virtual ~DictElementType();

        void setKey(const std::string& name, const Type& param);

        void setValue(const std::string& name, const Type& param);

        virtual const Parameter* getParameter(std::size_t n) const;

        virtual const Parameter* getParameter(const std::string& name) const;

        virtual std::size_t size() const
        {
            return 0;
        }

    private:
        Parameter _key;
        Parameter _value;
};


class PT_SOAP_API DictType : public ComplexType
{
    public:
        DictType(const std::string& typeName, const std::string& elemTypeName);

        virtual ~DictType();

        void setElement(const std::string& elemName,
                        const std::string& keyname, const Type& keyType, 
                        const std::string& valueName, const Type& valueType);

        virtual const Parameter* getParameter(std::size_t n) const;

        virtual const Parameter* getParameter(const std::string& name) const;

        virtual std::size_t size() const
        {
            return 1;
        }

    private:
        DictElementType _elemType;
        Parameter       _elem;
};


PT_SOAP_API const BooleanType& boolType();


PT_SOAP_API const IntegerType& intType();


PT_SOAP_API const FloatType& floatType();


PT_SOAP_API const StringType& stringType();


PT_SOAP_API const Base64Type& base64Type();


class PT_SOAP_API Operation : private NonCopyable
{
    public:
        typedef std::vector<Parameter> ParameterList;

        Operation(const Pt::String& inputName, const Pt::String& outputName);

        virtual ~Operation();

        const Pt::String& inputName() const
        { return _inputName; }

        const Pt::String& outputName() const
        { return _outputName; }

        void addInput(const std::string& name, const Type& param);

        const Parameter* getInput(const std::string& name) const;

        const Parameter* getInput(std::size_t n) const;

        void setOutput(const std::string& name, const Type& param);

        const Parameter* getOutput() const;

        const ParameterList& parameters() const
        {
            return _params;
        }

    private:
        ParameterList _params;
        Parameter _out;
        Pt::String _inputName;
        Pt::String _outputName;
};


class PT_SOAP_API ServiceDeclaration 
{
    public:
        ServiceDeclaration(const std::string& name);

        virtual ~ServiceDeclaration();

        const std::string& name() const
        { return _name; }

        const std::string& targetNamespace() const
        { return _targetNamespace; }

        void setTargetNamespace(const std::string& ns)
        { _targetNamespace = ns; }

        void addOperation(Operation& op);

        const Operation* getOperation(const Pt::String& name) const;
        
        void toWsdl(std::ostream& os) const;

    public:
        // deprecated
        static const BooleanType& boolType();
        
        // deprecated
        static const IntegerType& intType();
        
        // deprecated
        static const FloatType& floatType();
        
        // deprecated
        static const StringType& stringType();

    private:
        static void createComplexTypeList(std::map<std::string,const Type*>& complexTypes, const Type* type);

    private:
        std::string _name;
        std::string _targetNamespace;
        typedef std::vector<Operation*> OperationList;
        OperationList _operations;
};


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

template <typename T>
class BasicParameter : public Type
{
    public:
        BasicParameter()
        { }
};


template <>
class BasicParameter<int> : public IntegerType
{
    public:
        BasicParameter()
        { }
};


template <typename T>
class BasicParameter< std::vector<T> > : public ArrayType
{
    public:
        BasicParameter()
        { 
            setElement(_elem);
        }

    private:
        BasicParameter<T> _elem;
};


template <typename R, typename A1, typename A2>
class BasicProcedureDefinition : public Operation
{
    public:
        BasicProcedureDefinition()
        {}

    private:
        BasicParameter<R> _rDef;
        BasicParameter<A1> _a1Def;
        BasicParameter<A2> _a2Def;
};

} // namespace Soap

} // namespace Pt

#endif // Pt_Soap_ServiceDefinition_h
