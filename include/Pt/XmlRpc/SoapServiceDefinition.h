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

#ifndef Pt_XmlRpc_SoapServiceDefinition_h
#define Pt_XmlRpc_SoapServiceDefinition_h

#include <Pt/XmlRpc/Api.h>
#include <Pt/XmlRpc/ServiceDefinition.h>
#include <Pt/NonCopyable.h>
#include <Pt/Types.h>
#include <string>
#include <vector>
#include <map>

namespace Pt {

namespace XmlRpc {

class Parameter;

class Type : private NonCopyable
{
    public:
        enum TypeId
        {
            Array = 1,
            Struct = 2,
            Int = 3,
            String = 4
        };
    
    public:
        explicit Type(TypeId typeId)
        : _typeId(typeId)
        {}

        virtual ~Type()
        {}

        TypeId typeId() const
        { return _typeId; }

        virtual const Parameter* getParameter(std::size_t n) const = 0;

        virtual const Parameter* getParameter(const std::string& name) const = 0;

    private:
        TypeId _typeId;
};


class Parameter
{
    public:
        Parameter()
        : _type(0)
        {}
        
        Parameter(const std::string& name, Type& t)
        : _name(name)
        , _type(&t)
        { }

        virtual ~Parameter()
        {}

        void set(const std::string& name, Type& t)
        {
            _name = name;
            _type = &t;
        }

        const Type* type() const
        { return _type; }

        const std::string& name() const
        { return _name; }

    private:
        std::string _name;
        Type* _type;
};


class PT_XMLRPC_API IntegerType : public Type
{
    public:
        IntegerType();

        virtual ~IntegerType();

        virtual const Parameter* getParameter(std::size_t n) const
        { return 0; }

        virtual const Parameter* getParameter(const std::string& name) const
        { return 0; }
};


class PT_XMLRPC_API StringType : public Type
{
    public:
        StringType();

        virtual ~StringType();

        virtual const Parameter* getParameter(std::size_t n) const
        { return 0; }

        virtual const Parameter* getParameter(const std::string& name) const
        { return 0; }
};


class PT_XMLRPC_API StructType : public Type
{
    public:
        StructType();

        virtual ~StructType();

        void addParameter(const std::string& name, Type& param);

        virtual const Parameter* getParameter(std::size_t n) const;

        virtual const Parameter* getParameter(const std::string& name) const;

    private:
        typedef std::map<std::string, Type*> ParameterMap;
        ParameterMap _params;

        typedef std::vector<Parameter> ParameterList;
        ParameterList _paramList;
};


class PT_XMLRPC_API ArrayType : public Type
{
    public:
        ArrayType();
        
        ArrayType(Type& elem, const std::string& elemName);

        virtual ~ArrayType();

        void setElement(const std::string& elemName, Type& elem);

        virtual const Parameter* getParameter(std::size_t n) const;

        virtual const Parameter* getParameter(const std::string& name) const;

    private:
        Parameter _elem;
};

class PT_XMLRPC_API PortType : private NonCopyable
{
    public:
        PortType(const std::string& inputName, const std::string& outputName);

        virtual ~PortType();

        const std::string& inputName() const
        { return _inputName; }

        const std::string& outputName() const
        { return _outputName; }

        void addInput(const std::string& name, Type& param);

        const Parameter* getInput(const std::string& name) const;

        void setOutput(const std::string& name, Type& param);

        const Parameter* getOutput() const;

    private:
        typedef std::vector<Parameter> ParameterList;
        ParameterList _params;
        Parameter _out;
        std::string _inputName;
        std::string _outputName;
};


class PT_XMLRPC_API SoapServiceDefinition : public ServiceDefinition
{
    public:
        SoapServiceDefinition();

        virtual ~SoapServiceDefinition();

        const std::string& targetNamespace() const
        { return _targetNamespace; }

        void setTargetNamespace(const std::string& ns)
        { _targetNamespace = ns; }

        void addPort(PortType& p);

        const PortType* getPort(const std::string& name) const;

    private:
        std::string _targetNamespace;
        typedef std::vector<PortType*> PortList;
        PortList _ports;
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
class BasicProcedureDefinition : public PortType
{
    public:
        BasicProcedureDefinition()
        {}

    private:
        BasicParameter<R> _rDef;
        BasicParameter<A1> _a1Def;
        BasicParameter<A2> _a2Def;
};

} // namespace XmlRpc

} // namespace Pt

#endif // Pt_XmlRpc_SoapServiceDefinition_h
