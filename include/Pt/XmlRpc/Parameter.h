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
#ifndef Pt_XmlRpc_Parameter_h
#define Pt_XmlRpc_Parameter_h

#include <Pt/XmlRpc/Api.h>
#include <Pt/XmlRpc/TypeHandler.h>
#include <Pt/Xml/StartElement.h>
#include <Pt/Xml/EndElement.h>
#include <Pt/Xml/Characters.h>
#include <string>

namespace Pt {

namespace XmlRpc {

template <typename T>
class Parameter
{
    enum State
    {
        OnParamBegin,
        OnValueBegin,
        OnValueEnd,
        OnParamEnd,

        OnScalarBegin,
        OnScalar,
        OnScalarEnd,

        OnStructBegin,
        OnMemberBegin,
        OnNameBegin,
        OnName,
        OnNameEnd,
        OnMemberEnd,
        OnStructEnd,

        OnArrayBegin,
        OnDataBegin,
        OnDataEnd,
        OnArrayEnd
    };

    public:
        Parameter()
        : _builder()
        , _current(&_builder)
        , _state(OnParamBegin)
        {
            _builder.begin(_value);
        }

        bool compose(const Xml::Node& node)
        {
            switch(_state)
            {
                case OnParamBegin:
                { //std::cerr << "OnParamBegin" << std::endl;
                    if(node.type() == Xml::Node::StartElement) // value
                    {
                        _state = OnValueBegin;
                    }
                    break;
                }

                case OnValueBegin:
                { //std::cerr << "OnValueBegin" << std::endl;
                    if(node.type() == Xml::Node::StartElement) // i4, struct, array...
                    {
                        const Xml::StartElement& se = static_cast<const Xml::StartElement&>(node);
                        //std::cerr << se.name().narrow() << std::endl;
                        if(se.name() == L"struct")
                        {
                            _state = OnStructBegin;
                        }
                        else if(se.name() == L"array")
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
                        if(ee.name() == L"member")
                        { //std::cerr << "OnValueEnd member" << std::endl;
                            _current = _current->leaveMember();
                            _state = OnStructBegin;
                        }
                        else if(ee.name() == L"data")
                        { //std::cerr << "OnValueEnd data" << std::endl;
                            _current = _current->leaveMember();
                            _state = OnDataEnd;
                        }
                        else if(ee.name() == L"param")
                        { //std::cerr << "OnValueEnd data other " << ee.name().narrow() << std::endl;
                            _current->finish();
                            _state = OnParamEnd;
                            return true;
                        }
                    }
                    else if(node.type() == Xml::Node::StartElement)
                    {
                        const Xml::StartElement& se = static_cast<const Xml::StartElement&>(node);
                        if(se.name() == L"value")
                        { //std::cerr << "OnValueEnd data value" << std::endl;
                            _current = _current->leaveMember();
                            _current = _current->beginMember("");
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

                        _current = _current->beginMember(name);

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
                        _current = _current->beginMember("");
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
                    throw std::runtime_error("invalid parameters");
                }

            }

            return false;
        }

        const T& get() const
        { return _value; }

    private:
        T _value;
        TypeHandler<T> _builder;
        ITypeHandler* _current;
        State _state;
};

}

}

#endif
