/*
 * Copyright (C) 2009 by Dr. Marc Boris Duerner
 * Copyright (C) 2009 by Tommi Meakitalo
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
#include "Pt/XmlRpc/Service.h"
#include "Pt/XmlRpc/Formatter.h"
#include "Pt/Utf8Codec.h"

namespace Pt {

namespace XmlRpc {

Service::Service()
{
}


Service::~Service()
{
    ProcedureMap::iterator it;
    for(it = _procedures.begin(); it != _procedures.end(); ++it)
    {
        delete it->second;
    }
}


ServiceProcedure* Service::procedure(const std::string& name)
{
    ProcedureMap::iterator it = _procedures.find( name );
    if( it == _procedures.end() )
    {
        return 0;
    }

    return it->second;
}


void Service::registerProcedure(const std::string& name, ServiceProcedure* proc)
{
    std::pair<const std::string, ServiceProcedure*> p( name, proc );
    _procedures.insert( p );
}


Net::HttpResponder* Service::createResponder()
{
    return new HttpXmlRpcResponder(*this);
}


void Service::releaseResponder(Net::HttpResponder* resp)
{
    delete resp;
}


HttpXmlRpcResponder::HttpXmlRpcResponder(Service& service)
: Net::HttpResponder(service)
, _state(OnBegin)
, _ts(new Utf8Codec)
, _reader(0)
, _service(0)
, _proc(0)
, _current(0)
{ }


HttpXmlRpcResponder::~HttpXmlRpcResponder()
{
    delete _reader;
}


std::size_t HttpXmlRpcResponder::advance(std::istream& is)
{
    if( ! _reader )
    {
        _ts.attach(is);
        _reader = new Xml::XmlReader(_ts);
    }

    std::size_t n = _ts.buffer().import();
    if(n == 0)
        return n;

    while( _reader->advance() )
    {
        const Pt::Xml::Node& node = _reader->get();
        switch(_state)
        {
            case OnBegin:
            {
                if(node.type() == Xml::Node::StartElement)
                {
                    _state = OnMethodCallBegin;
                }
                break;
            }

            case OnMethodCallBegin:
            {
                if(node.type() == Xml::Node::StartElement)
                {
                    _state = OnMethodNameBegin;
                }
                break;
            }

            case OnMethodNameBegin:
            {
                if(node.type() == Xml::Node::Characters)
                {
                    const Xml::Characters& chars = static_cast<const Xml::Characters&>(node);

                    _proc = _service->procedure( chars.content().narrow() );
                    if( ! _proc )
                        throw std::runtime_error("no such procedure");

                    _state = OnMethodName;
                }
                break;
            }

            case OnMethodName:
            {
                if(node.type() == Xml::Node::EndElement)
                {
                    _state = OnMethodNameEnd;
                }
                break;
            }

            case OnMethodNameEnd:
            {
                if(node.type() == Xml::Node::StartElement)
                {
                    _state = OnParams;
                }
                break;
            }
/*
            case OnParams:
            {
                if( ! _args )
                    throw std::runtime_error("invalid XML-RPC request");

                bool finished = _args->compose(node);
                if(finished)
                {
                    _state = OnParamsEnd;
                }

                break;
            }
*/
            case OnParamsEnd:
            { //std::cerr << "OnParamsEnd" << std::endl;
                if(node.type() == Xml::Node::EndElement)
                {
                    _state = OnMethodCallEnd;
                }
                break;
            }

            case OnMethodCallEnd:
            { //std::cerr << "OnMethodCallEnd" << std::endl;
                if(node.type() == Xml::Node::EndDocument)
                {
                    _state = OnMethodCallEnd;
                }
                break;
            }

///
///
///

            case OnParams:
            {
                if(node.type() == Xml::Node::StartElement)
                {
                    const Xml::StartElement& se = static_cast<const Xml::StartElement&>(node);

                    if(se.name() == "param")
                    {
                        _state = OnParamBegin;
                    }
                }
                else if(node.type() == Xml::Node::EndElement)
                {
                    const Xml::EndElement& ee = static_cast<const Xml::EndElement&>(node);
                    if(ee.name() == "params")
                    {
                        _state = OnParamsEnd;
                    }
                }

                break;
            }

            case OnParamBegin:
            { //std::cerr << "OnParamBegin" << std::endl;
                if(node.type() == Xml::Node::StartElement) // value
                {
                    _argv.push_back( Pt::SerializationInfo() );
                    _current = &_argv.back();
                    _state = OnValueBegin;
                }
                break;
            }

            case OnParamEnd:
            { //std::cerr << "OnParamEnd" << std::endl;

                if(node.type() == Xml::Node::StartElement)
                {
                    const Xml::StartElement& se = static_cast<const Xml::StartElement&>(node);

                    if(se.name() == "param")
                    {
                        _state = OnParamBegin;
                    }
                }
                else if(node.type() == Xml::Node::EndElement)
                {
                    const Xml::EndElement& ee = static_cast<const Xml::EndElement&>(node);
                    if(ee.name() == "params")
                    {
                        _state = OnParamsEnd;
                    }
                }
                break;
            }

            case OnValueBegin:
            { //std::cerr << "OnValueBegin" << std::endl;
                if(node.type() == Xml::Node::StartElement) // i4, struct, array...
                {
                    const Xml::StartElement& se = static_cast<const Xml::StartElement&>(node);
                    //std::cerr << se.name().narrow() << std::endl;
                    if(se.name() == "struct")
                    {
                        _state = OnStructBegin;
                    }
                    else if(se.name() == "array")
                    {
                        _state = OnArrayBegin;
                    }
                    else
                    {
                        _state = OnScalarBegin;
                    }
                }
                break;
            }

            case OnValueEnd:
            { //std::cerr << "OnValueEnd" << std::endl;

                if(node.type() == Xml::Node::EndElement)
                {
                    const Xml::EndElement& ee = static_cast<const Xml::EndElement&>(node);
                    if(ee.name() == "member")
                    { //std::cerr << "OnValueEnd member" << std::endl;
                        _current = _current->parent();
                        _state = OnStructBegin;
                    }
                    else if(ee.name() == "data")
                    { //std::cerr << "OnValueEnd data" << std::endl;
                        _current = _current->parent();
                        _state = OnDataEnd;
                    }
                    else if(ee.name() == "param")
                    { //std::cerr << "OnValueEnd data other " << ee.name().narrow() << std::endl;
                        _state = OnParamEnd;
                    }
                }
                else if(node.type() == Xml::Node::StartElement)
                {
                    const Xml::StartElement& se = static_cast<const Xml::StartElement&>(node);
                    if(se.name() == "value")
                    { //std::cerr << "OnValueEnd data value" << std::endl;
                        _current = _current->parent();

                        SerializationInfo& child = _current->addMember("");
                        _current = &child;

                        _state = OnValueBegin;
                    }
                }

                break;
            }

            case OnStructBegin:
            { //std::cerr << "OnStructBegin" << std::endl;
                if(node.type() == Xml::Node::StartElement) // <member>
                {
                    _state = OnMemberBegin;
                }
                else if(node.type() == Xml::Node::EndElement) // </struct>
                {
                    _state = OnStructEnd;
                }
                break;
            }

            case OnStructEnd:
            {//std::cerr << "OnStructEnd" << std::endl;
                if(node.type() == Xml::Node::EndElement) // </value>
                {
                    _state = OnValueEnd;
                }
                break;
            }

            case OnMemberBegin:
            { //std::cerr << "OnMemberBegin" << std::endl;
                if(node.type() == Xml::Node::StartElement) // name
                {
                    _state = OnNameBegin;
                }
                break;
            }

            case OnNameBegin:
            { //std::cerr << "OnNameBegin" << std::endl;
                if(node.type() == Xml::Node::Characters) // member-name
                {
                    const Xml::Characters& chars = static_cast<const Xml::Characters&>(node);
                    const std::string& name = chars.content().narrow();

                    SerializationInfo& child = _current->addMember(name);
                    _current = &child;

                    _state = OnName;
                }
                break;
            }

            case OnName:
            { //std::cerr << "OnName" << std::endl;
                if(node.type() == Xml::Node::EndElement) // </name>
                {
                    _state = OnNameEnd;
                }
                break;
            }

            case OnNameEnd:
            { //std::cerr << "OnNameEnd" << std::endl;
                if(node.type() == Xml::Node::StartElement) // <value>
                {
                    _state = OnValueBegin;
                }
                break;
            }

            case OnScalarBegin:
            { //std::cerr << "OnScalarBegin " << std::endl;
                if(node.type() == Xml::Node::Characters)
                {
                    const Xml::Characters& chars = static_cast<const Xml::Characters&>(node);
                    _state = OnScalar;
                    _current->setValue( chars.content() );
                }
                break;
            }

            case OnScalar:
            { //std::cerr << "OnScalar" << std::endl;
                if(node.type() == Xml::Node::EndElement) // </int>, boolean ...
                {
                    _state = OnScalarEnd;
                }
                break;
            }

            case OnScalarEnd:
            { //std::cerr << "OnScalarEnd" << std::endl;
                if(node.type() == Xml::Node::EndElement) // </value>
                {
                    _state = OnValueEnd;
                }
                break;
            }

            case OnArrayBegin:
            { //std::cerr << "OnArrayBegin" << std::endl;
                if(node.type() == Xml::Node::StartElement) // data
                {
                    _state = OnDataBegin;
                }
                break;
            }

            case OnDataBegin:
            { //std::cerr << "OnDataBegin" << std::endl;
                if(node.type() == Xml::Node::StartElement) // value
                {
                    //std::cerr << _current << std::endl;
                    SerializationInfo& child = _current->addMember("");
                    _current = &child;
                    _state = OnValueBegin;
                }
                break;
            }

            case OnDataEnd:
            { //std::cerr << "OnDataEnd" << std::endl;
                if(node.type() == Xml::Node::EndElement) // </array>
                {
                    _state = OnArrayEnd;
                }
                break;
            }

            case OnArrayEnd:
            { //std::cerr << "OnArrayEnd" << std::endl;
                if(node.type() == Xml::Node::EndElement) // </value>
                {
                    _state = OnValueEnd;
                }
                break;
            }

            default:
            {
                throw std::runtime_error("invalid XML-RPC request");
            }
        }
    }

    return n;
}


void HttpXmlRpcResponder::finish(std::ostream& os)
{
    delete _reader;
    _state = OnBegin;

    SerializationInfo ret;
    _proc->run( ret, &_argv[0], _argv.size() );

    ResponseFormatter formatter(os);
    formatResult(ret, formatter);
    formatter.finish();
}


void HttpXmlRpcResponder::formatResult(const Pt::SerializationInfo& si, Formatter& formatter)
{
    if(si.category() == SerializationInfo::Value)
    {
        // TODO use formatter to adapt typenames to protocol specific typenames

        formatter.addValue( si.typeName(), si.toString() );
    }
    else if(si.category() == SerializationInfo::Object)
    {
        formatter.beginObject();

        SerializationInfo::ConstIterator it;
        for(it = si.begin(); it != si.end(); ++it)
        {
            formatter.beginMember( it->name() );
            formatResult(*it, formatter);
            formatter.finishMember();
        }

        formatter.finishObject();
    }

    //TODO arrays should use SerializationInfo Array
}

}

}
