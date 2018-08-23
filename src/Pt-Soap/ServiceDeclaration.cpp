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

#include <Pt/Soap/ServiceDeclaration.h>

namespace Pt {

namespace Soap {

struct ServiceDeclarationStaticInit
{
    ServiceDeclarationStaticInit()
    {
        boolType();
        intType();
        floatType();
        stringType();
        base64Type();
    }
};


const BooleanType& boolType()
{
    static const BooleanType type;
    return type;
}


const IntegerType& intType()
{
    static const IntegerType type;
    return type;
}


const FloatType& floatType()
{
    static const FloatType type;
    return type;
}


const StringType& stringType()
{
    static const StringType type;
    return type;
}


const Base64Type& base64Type()
{
    static const Base64Type type;
    return type;
}

///////////////////////////////////////////////////////////////////////////////
// BooleanType
///////////////////////////////////////////////////////////////////////////////

BooleanType::BooleanType()
: SimpleType(Type::Bool)
{ 
}


BooleanType::~BooleanType()
{
}



///////////////////////////////////////////////////////////////////////////////
// IntegerType
///////////////////////////////////////////////////////////////////////////////

IntegerType::IntegerType()
: SimpleType(Type::Int)
{ 
}


IntegerType::~IntegerType()
{
}

///////////////////////////////////////////////////////////////////////////////
// StringType
///////////////////////////////////////////////////////////////////////////////

FloatType::FloatType()
: SimpleType(Type::Float)
{ 
}


FloatType::~FloatType()
{
}

///////////////////////////////////////////////////////////////////////////////
// StringType
///////////////////////////////////////////////////////////////////////////////

StringType::StringType()
: SimpleType(Type::String)
{ 
}


StringType::~StringType()
{
}

///////////////////////////////////////////////////////////////////////////////
// Base64Type
///////////////////////////////////////////////////////////////////////////////

Base64Type::Base64Type()
: SimpleType(Type::Base64)
{ 
}


Base64Type::~Base64Type()
{
}

///////////////////////////////////////////////////////////////////////////////
// StructType
///////////////////////////////////////////////////////////////////////////////

StructType::StructType(const std::string& name)
: ComplexType(Type::Struct, name)
{ 
}


StructType::~StructType()
{

}


void StructType::addParameter(const std::string& name, const Type& t, 
                              int minOccurence, int maxOccurence)
{
    Parameter param(name, t);
    param.setOccurrence(minOccurence, maxOccurence);
    
    _paramList.push_back(param);
}


const Parameter* StructType::getParameter(std::size_t n) const
{ 
    return n >= _paramList.size() ? 0 : &_paramList[n]; 
}


const Parameter* StructType::getParameter(const std::string& name) const
{ 
    ParameterList::const_iterator it;
    for(it = _paramList.begin(); it != _paramList.end(); ++it)
    {
        if(it->name() == name)
            return &*it;
    }
    
    return 0; 
}

///////////////////////////////////////////////////////////////////////////////
// ArrayType
///////////////////////////////////////////////////////////////////////////////

ArrayType::ArrayType(const std::string& name)
: ComplexType(Type::Array, name)
{ 
}


ArrayType::ArrayType(const std::string& name, const Type& elem, const std::string& elemName)
: ComplexType(Type::Array, name)
, _elem(elemName, elem)
{
}


ArrayType::~ArrayType()
{
}


void ArrayType::setElement(const std::string& name, const Type& elem)
{
    _elem.set(name, elem);
}


const Parameter* ArrayType::getParameter(std::size_t n) const
{ 
    return &_elem; 
}


const Parameter* ArrayType::getParameter(const std::string& name) const
{ 
    return &_elem; 
}


///////////////////////////////////////////////////////////////////////////////
// DictElementType
///////////////////////////////////////////////////////////////////////////////

DictElementType::DictElementType(const std::string& name)
: ComplexType(Type::DictElement, name)
{ 
}


DictElementType::~DictElementType()
{
}


void DictElementType::setKey(const std::string& name, const Type& type)
{
  _key.set(name, type);
}


void DictElementType::setValue(const std::string& name, const Type& type)
{
  _value.set(name, type);
}


const Parameter* DictElementType::getParameter(std::size_t n) const
{ 
    return n == 0 ? &_key :
           n == 1 ? &_value : 0; 
}


const Parameter* DictElementType::getParameter(const std::string& name) const
{
    if(_key.name() == name)
        return &_key;

    if(_value.name() == name)
        return &_value;

    return 0; 
}

///////////////////////////////////////////////////////////////////////////////
// DictType
///////////////////////////////////////////////////////////////////////////////

DictType::DictType(const std::string& typeName, const std::string& elemTypeName)
: ComplexType(Type::Dict, typeName)
, _elemType(elemTypeName)
{ 
}


DictType::~DictType()
{
}


void DictType::setElement(const std::string& elemName,
                          const std::string& keyName, const Type& keyType, 
                          const std::string& valueName, const Type& valueType)
{
  _elemType.setKey(keyName, keyType);
  _elemType.setValue(valueName, valueType);

  _elem.set(elemName, _elemType);
}


const Parameter* DictType::getParameter(std::size_t n) const
{ 
    return &_elem; 
}


const Parameter* DictType::getParameter(const std::string& name) const
{ 
    return &_elem; 
}


///////////////////////////////////////////////////////////////////////////////
// Operation
///////////////////////////////////////////////////////////////////////////////

Operation::Operation(const Pt::String& inputName, const Pt::String& outputName)
: _inputName(inputName)
, _outputName(outputName)
{ 
}


Operation::~Operation()
{

}


void Operation::addInput(const std::string& name, const Type& t)
{
    Parameter param(name, t);
    _params.push_back(param);
}


const Parameter* Operation::getInput(const std::string& name) const
{ 
    ParameterList::const_iterator it;
    for(it = _params.begin(); it != _params.end(); ++it)
    {
        if(it->name() == name)
            return &*it;
    }
    
    return 0; 
}


const Parameter* Operation::getInput(std::size_t n) const
{   
    return n >= _params.size() ? 0 : &_params[n]; 
}


void Operation::setOutput(const std::string& name, const Type& type)
{
    _out.set(name, type);
}


const Parameter* Operation::getOutput() const
{
    return &_out;
}

///////////////////////////////////////////////////////////////////////////////
// SoapServiceDefinition
///////////////////////////////////////////////////////////////////////////////

ServiceDeclaration::ServiceDeclaration(const std::string& name)
: _name(name)
{ 
}


ServiceDeclaration::~ServiceDeclaration()
{
}


void ServiceDeclaration::addOperation(Operation& op)
{
    //System::MutexLock lock( mutex() );
    _operations.push_back( &op );
}


typedef std::map<std::string, const Type*> ComplexTypesMap;


void ServiceDeclaration::createComplexTypeList(ComplexTypesMap& complexTypes, const Type* type)
{
    ComplexTypesMap::iterator it = complexTypes.find( type->name() );
    
    if( it != complexTypes.end() )
        return;
        
    complexTypes[type->name()] = type;

    for( size_t i = 0; i < type->size(); ++i)
    {
        const Type* elemType = type->getParameter(i)->type();
        
        if(elemType->isSimple())
            continue;

        createComplexTypeList(complexTypes, type->getParameter(i)->type());
    }    
}

void ServiceDeclaration::toWsdl( std::ostream& os) const
{
    os << "<?xml version=\"1.0\" encoding=\"utf-8\"?>" << std::endl
       << "<wsdl:definitions xmlns:soapenc=\"http://schemas.xmlsoap.org/soap/encoding/\" " << std::endl
       << "    xmlns:tns=\"" <<  targetNamespace() << "\" " << std::endl
       << "    xmlns:soap=\"http://schemas.xmlsoap.org/wsdl/soap12/\" " << std::endl
       << "    xmlns:s=\"http://www.w3.org/2001/XMLSchema\" " << std::endl
       << "    xmlns:http=\"http://schemas.xmlsoap.org/wsdl/http/\" " << std::endl
       << "    targetNamespace=\""<<  targetNamespace() << "\" " << std::endl
       << "    xmlns:wsdl=\"http://schemas.xmlsoap.org/wsdl/\">" << std::endl;
  
    os << "<wsdl:types>" << std::endl;

    os << "<s:schema elementFormDefault=\"qualified\" targetNamespace=\"" << targetNamespace() << "\">" << std::endl;

    ComplexTypesMap complexTypes;
                    
    // operations
    for( size_t i = 0; i < _operations.size(); ++i)
    {        
        const Operation* operation = _operations[i];
        
        os << "<s:element name=\"" << operation->inputName().narrow() << "\">" << std::endl;
        os << "<s:complexType>" << std::endl;
        os << "<s:sequence>" << std::endl;                    

        for(size_t j = 0; j < operation->parameters().size(); ++j)
        {
            const Parameter& param = operation->parameters()[j];                        
                                                
            if( param.type()->isSimple() )
            {
                os << "<s:element minOccurs=\"1\" maxOccurs=\"1\" name=\""
                   << param.name() <<"\" type=\"s:" << param.type()->name() << "\" />"<<std::endl;
            }
            else
            {
                os << "<s:element minOccurs=\"1\" maxOccurs=\"1\" name=\"" 
                   << param.name() << "\" type=\"tns:" << param.type()->name() << "\" />"<<std::endl;
                
                createComplexTypeList(complexTypes, param.type());
            }
        }

        os << "</s:sequence>" << std::endl;
        os << "</s:complexType>" << std::endl;
        os << "</s:element>" << std::endl;

        os << "<s:element name=\"" << operation->outputName().narrow() << "\">"<<std::endl;
        os << "<s:complexType>" << std::endl;
        const Parameter* param = operation->getOutput();

        if(param != 0)
        {
          os<<"<s:sequence>"<<std::endl;                    

              if( param->type()->isSimple() )
              {
                  os <<"<s:element minOccurs=\"1\" maxOccurs=\"1\" name=\"" << param->name() 
                     << "\" type=\"s:"<< param->type()->name()<<"\" />" << std::endl;
              }
              else
              {
                  os <<"<s:element minOccurs=\"1\" maxOccurs=\"1\" name=\"" << param->name() 
                      << "\" type=\"tns:" << param->type()->name() << "\" />" << std::endl;
                  
                  createComplexTypeList(complexTypes, param->type());
              }
          }

        os<<"</s:sequence>"<<std::endl;
        os<<"</s:complexType>"<<std::endl;
        os<<"</s:element>"<<std::endl;
    }

    // complex types
    ComplexTypesMap::iterator it = complexTypes.begin(); 

    for(; it != complexTypes.end(); ++it)
    {
        os << "<s:complexType name=\"" << it->first << "\">" <<std::endl;
        os << "<s:sequence>" << std::endl;

        const Type* type = it->second;

        for(std::size_t i = 0; i < type->size(); ++i)
        {
            const Parameter* member = type->getParameter(i);           

            if( type->typeId() == Type::Array )
            {
                os << "<s:element minOccurs=\"0\" maxOccurs=\"unbounded\" name=\"" 
                   << member->name() << "\" ";
            }
            else
            {
                os << "<s:element minOccurs=\"";
                if(member->minOccurs() < 0)
                  os << "unbounded";
                else
                  os << member->minOccurs();
               
                os << "\" maxOccurs=\"";
                
                if(member->maxOccurs() < 0)
                  os << "unbounded";
                else
                  os << member->maxOccurs(); 
                
                os << "\" name=\"" << member->name() << "\" ";
            }

            if( member->type()->isSimple() )
                os << "type=\"s:"<< member->type()->name() << "\" />" << std::endl;
            else
                os << "type=\"tns:"<< member->type()->name() <<"\" />" << std::endl;
        }

        os << "</s:sequence>" << std::endl;
        os << "</s:complexType>" << std::endl;
    }
    
    os << "</s:schema>" << std::endl;
    os << "</wsdl:types>" << std::endl;

    // messages
    for( size_t i = 0; i < _operations.size(); ++i)
    {        
        const Operation* operation = _operations[i];
        
        //Request message
        os << "<wsdl:message name=\"" << operation->inputName().narrow() << "\">" << std::endl;
        os << "    <wsdl:part name=\"parameters\" element=\"tns:" << operation->inputName().narrow()<<"\"/>" << std::endl;
        os << "</wsdl:message>" << std::endl;

        //Response message
        os << "<wsdl:message name=\"" << operation->outputName().narrow() << "\">" << std::endl;
        os << "    <wsdl:part name=\"parameters\" element=\"tns:" << operation->outputName().narrow() << "\"/>" << std::endl;
        os << "</wsdl:message>" << std::endl;
    }

    // ports
    os << "<wsdl:portType name=\"" << _name << "\">" << std::endl;
        
    for( size_t i = 0; i < _operations.size(); ++i)
    {        
        const Operation* operation = _operations[i];
        
        os << "<wsdl:operation name=\"" << operation->inputName().narrow() << "\">" << std::endl;
            os << "<wsdl:input message=\"tns:" << operation->inputName().narrow() << "\" />" << std::endl;
            os << "<wsdl:output message=\"tns:" << operation->outputName().narrow() << "\" />" << std::endl;            
        os << "</wsdl:operation>" << std::endl;
    }
    os << "</wsdl:portType>" << std::endl;
  
    os << "<wsdl:binding name=\""  <<  _name  <<  "Soap12\" type=\"tns:"  <<  _name  <<  "\">" << std::endl;
    os << "<soap:binding transport=\"http://schemas.xmlsoap.org/soap/http\" />" << std::endl;

    for( size_t i = 0; i < _operations.size(); ++i)
    {        
        const Operation* operation = _operations[i];
        
          os << "<wsdl:operation name=\"" << operation->inputName().narrow() << "\" >" << std::endl;
          os << "<soap:operation soapAction=\"" << targetNamespace() << operation->inputName().narrow() << "\" style=\"document\" />" << std::endl;
          os << "<wsdl:input>" << std::endl;
            os << "<soap:body use=\"literal\" />" << std::endl;
          os << "</wsdl:input>" << std::endl;
          os << "<wsdl:output>" << std::endl;
            os << "<soap:body use=\"literal\" />" << std::endl;
          os << "</wsdl:output>" << std::endl;
        os << "</wsdl:operation>" << std::endl;
    }
    
    os << "</wsdl:binding>" << std::endl;

    os << "<wsdl:service name=\"" << _name << "\">" << std::endl;
    os << "  <wsdl:port name=\"" <<  _name << "Soap12\" binding=\"tns:" << _name << "Soap12\">" << std::endl;
    os << "  </wsdl:port>" << std::endl;
    os << "</wsdl:service>" << std::endl;
    
    os << "</wsdl:definitions>" << std::endl;
}


const BooleanType& ServiceDeclaration::boolType()
{
    return Pt::Soap::boolType();
}


const IntegerType& ServiceDeclaration::intType()
{
    return Pt::Soap::intType();
}


const FloatType& ServiceDeclaration::floatType()
{
    return Pt::Soap::floatType();
}


const StringType& ServiceDeclaration::stringType()
{
    return Pt::Soap::stringType();
}


const Operation* ServiceDeclaration::getOperation(const Pt::String& name) const
{
    //System::MutexLock lock( mutex() );

    OperationList::const_iterator it;
    for(it = _operations.begin(); it != _operations.end(); ++it)
    {
        if((*it)->inputName() == name)
            return *it;
    }

    return 0;
}

} // namespace Soap

} // namespace Pt
