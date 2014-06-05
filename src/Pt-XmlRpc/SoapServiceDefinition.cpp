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

#include <Pt/XmlRpc/SoapServiceDefinition.h>

namespace Pt {

namespace XmlRpc {

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
// StructType
///////////////////////////////////////////////////////////////////////////////

StructType::StructType(const std::string& name)
: ComplexType(Type::Struct, name)
{ 
}


StructType::~StructType()
{

}


void StructType::addParameter(const std::string& name, Type& t)
{
    Parameter param(name, t);
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
// StructType
///////////////////////////////////////////////////////////////////////////////

ArrayType::ArrayType(const std::string& name)
: ComplexType(Type::Array, name)
{ 
}


ArrayType::ArrayType(const std::string& name, Type& elem, const std::string& elemName)
: ComplexType(Type::Array, name)
, _elem(elemName, elem)
{
}


ArrayType::~ArrayType()
{
}


void ArrayType::setElement(const std::string& name, Type& elem)
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


void Operation::addInput(const std::string& name, Type& t)
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


void Operation::setOutput(const std::string& name, Type& type)
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

SoapServiceDeclaration::SoapServiceDeclaration(const std::string& name)
: _name(name)
{ 
}


SoapServiceDeclaration::~SoapServiceDeclaration()
{
}


void SoapServiceDeclaration::addOperation(Operation& op)
{
    //System::MutexLock lock( mutex() );
    _operations.push_back( &op );
}


typedef std::map<std::string, const Type*> ComplexTypesMap;


void SoapServiceDeclaration::createComplexTypeList(ComplexTypesMap& complexTypes, const Type* type)
{
    ComplexTypesMap::iterator it = complexTypes.find( type->name() );
    
    if( it != complexTypes.end())
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

void SoapServiceDeclaration::toWsdl( std::ostream& os) const
{
    os << "<?xml version=\"1.0\" encoding=\"utf-8\"?>" << std::endl
       << "<wsdl:definitions xmlns:soapenc=\"http://schemas.xmlsoap.org/soap/encoding/\" " << std::endl
       << "    xmlns:tns=\"" <<  targetNamespace() << "\" " << std::endl
       << "    xmlns:soap=\"http://schemas.xmlsoap.org/wsdl/soap/\" " << std::endl
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
                                                
            if(param.type()->isSimple())
            {
                os << "<s:element minOccurs=\"0\" maxOccurs=\"1\" name=\""
                   << param.name() <<"\" type=\"s:" << param.type()->name() << "\" />"<<std::endl;
            }
            else
            {
                os << "<s:element minOccurs=\"0\" maxOccurs=\"1\" name=\"" 
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

              if(param->type()->isSimple())
              {
                  os <<"<s:element minOccurs=\"0\" maxOccurs=\"1\" name=\"" << param->name() 
                     << "\" type=\"s:"<< param->type()->name()<<"\" />" << std::endl;
              }
              else
              {
                  os <<"<s:element minOccurs=\"0\" maxOccurs=\"1\" name=\"" << param->name() 
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
        os<<"<s:complexType name=\""<<it->first <<"\">"<<std::endl;
        os<<"<s:sequence>"<<std::endl;

        const Type* type = it->second;

        for(std::size_t i = 0; i < type->size(); ++i)
        {
            if(type->getParameter(i)->type()->isSimple())
                os << "<s:element minOccurs=\"0\" maxOccurs=\"unbounded\" name=\"" << type->getParameter(i)->name()
                   << "\" type=\"s:"<<type->getParameter(i)->type()->name() << "\" />"<<std::endl;
            else
                os << "<s:element minOccurs=\"0\" maxOccurs=\"unbounded\" name=\"" << type->getParameter(i)->name()
                    << "\" type=\"tns:"<<type->getParameter(i)->type()->name() <<"\" />"<<std::endl;
        }

        os<<"</s:sequence>"<<std::endl;
        os<<"</s:complexType>"<<std::endl;
    }
    os<<"</s:schema>"<<std::endl;
    os<<"</wsdl:types>"<<std::endl;

    // messages
    for( size_t i = 0; i < _operations.size(); ++i)
    {        
        const Operation* operation = _operations[i];
        
        //Request message
        os << "<wsdl:message name=\"" << operation->inputName().narrow() << "\">" << std::endl;
        os << "    <wsdl:part name=\"parameters\" element=\"tns:" << operation->inputName().narrow()<<"\"/>" << std::endl;
        os << "</wsdl:message>" << std::endl;

        //Responce message
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
  
    os << "<wsdl:binding name=\""  <<  _name  <<  "Soap11\" type=\"tns:"  <<  _name  <<  "\">" << std::endl;
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
    os << "  <wsdl:port name=\"" <<  _name << "Soap11\" binding=\"tns:" << _name << "Soap11\">" << std::endl;
    os << "  </wsdl:port>" << std::endl;
    os << "</wsdl:service>" << std::endl;
    
    os << "</wsdl:definitions>" << std::endl;
}

const Operation* SoapServiceDeclaration::getOperation(const Pt::String& name) const
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

///////////////////////////////////////////////////////////////////////////////
// SoapServiceDefinition
///////////////////////////////////////////////////////////////////////////////

SoapServiceDefinition::SoapServiceDefinition(const SoapServiceDeclaration& decl)
: _decl(decl)
{ 
}


SoapServiceDefinition::~SoapServiceDefinition()
{
}

} // namespace XmlRpc

} // namespace Pt
