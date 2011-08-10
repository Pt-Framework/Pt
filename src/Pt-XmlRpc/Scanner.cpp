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
#include <Pt/XmlRpc/Scanner.h>
#include <Pt/Xml/StartElement.h>
#include <Pt/Xml/EndElement.h>
#include <Pt/Xml/Characters.h>

#define log_define(e)
#define log_debug(e)
log_define("Pt.XmlRpc.scanner")

namespace Pt {

namespace XmlRpc {

namespace
{
    void throwSerializationError(const char* msg = "invalid XML-RPC parameter")
    {
        throw SerializationError(msg);
    }
}

bool Scanner::advance(const Pt::Xml::Node& node)
{
    switch(_state)
    {
        case OnParam:
        {
            log_debug("OnParam");
            if(node.type() == Xml::Node::StartElement) // i4, struct, array...
            {
                const Xml::StartElement& se = static_cast<const Xml::StartElement&>(node);

                if(se.name() != L"value")
                    throwSerializationError();

                _state = OnValueBegin;
            }
            else if(node.type() == Xml::Node::EndElement)
            {
                throwSerializationError();
            }

            break;
        }

        case OnValueBegin:
        {
            log_debug("OnValueBegin, node type " << node.type());
            if(node.type() == Xml::Node::StartElement) // i4, struct, array...
            {
                const Xml::StartElement& se = static_cast<const Xml::StartElement&>(node);

                log_debug("-> found type " << se.name().narrow());
                if(se.name() == L"struct")
                {
                    _state = OnStructBegin;
                }
                else if(se.name() == L"array")
                {
                    _state = OnArrayBegin;
                }
                else if(se.name() == L"int" || se.name() == L"i4")
                {
                    _state = OnIntBegin;
                }
                else if(se.name() == L"boolean")
                {
                    _state = OnBoolBegin;
                }
                else if(se.name() == L"double")
                {
                    _state = OnDoubleBegin;
                }
                else
                {
                    _state = OnStringBegin;
                }

                _value.clear();
            }
            else if(node.type() == Xml::Node::Characters)
            {
                // maybe <value>...<type>...</type>...</value>  (case 1)
                //    or <value>...</value>                     (case 2)
                const Xml::Characters& chars = static_cast<const Xml::Characters&>(node);
                _value = chars.content();
            }
            else if(node.type() == Xml::Node::EndElement)
            {
                const Xml::EndElement& ee = static_cast<const Xml::EndElement&>(node);
                if(ee.name() != L"value")
                    throwSerializationError();

                // is always type string
                _current->setValue( _value );
                _value.clear();

                _state = OnValueEnd;
            }
            else
            {
                throwSerializationError();
            }

            break;
        }

        case OnValueEnd:
        {
            log_debug("OnValueEnd, node type " << node.type());

            if(node.type() == Xml::Node::EndElement)
            {
                const Xml::EndElement& ee = static_cast<const Xml::EndElement&>(node);

                if(ee.name() == L"member")
                {
                    log_debug("OnValueEnd member");
                    _current = _current->finish();
                    if( ! _current )
                        throwSerializationError("invalid XML-RPC struct");

                    _state = OnStructBegin;
                }
                else if(ee.name() == L"data")
                {
                    log_debug("OnValueEnd data");
                    _current = _current->finish();
                    if( ! _current )
                        throwSerializationError("invalid XML-RPC array");

                    _state = OnDataEnd;
                }
                else if(ee.name() == L"param")
                {
                    log_debug("OnValueEnd data other " << ee.name().narrow());
                    if( 0 != _current->finish() )
                        throwSerializationError();

                    _state = OnValueEnd;
                    return true;
                }
                else if(ee.name() == L"fault")
                {
                    log_debug("OnValueEnd data other " << ee.name().narrow());
                    if( 0 != _current->finish() )
                        throwSerializationError("invalid XML-RPC fault");

                    _state = OnValueEnd;
                    return true;
                }
                else
                {
                    throwSerializationError();
                }
            }
            else if(node.type() == Xml::Node::StartElement)
            {
                const Xml::StartElement& se = static_cast<const Xml::StartElement&>(node);
                if(se.name() == L"value")
                {
                    log_debug("OnValueEnd data value");
                    _current = _current->finish();

                    if( ! _current )
                        throwSerializationError("invalid XML-RPC element");

                    _current = _current->beginElement();
                    _state = OnValueBegin;
                }
                else
                {
                    throwSerializationError();
                }
            }

            break;
        }

        case OnStructBegin:
        {
            log_debug("OnStructBegin");
            if(node.type() == Xml::Node::StartElement) // <member>
            {
                const Xml::StartElement& se = static_cast<const Xml::StartElement&>(node);

                if(se.name() != L"member")
                    throwSerializationError();

                _state = OnMemberBegin;
            }
            else if(node.type() == Xml::Node::EndElement) // </struct>
            {
                _state = OnStructEnd;
            }
            break;
        }

        case OnStructEnd:
        {
            log_debug("OnStructEnd");
            if(node.type() == Xml::Node::EndElement) // </value>
            {
                const Xml::EndElement& ee = static_cast<const Xml::EndElement&>(node);

                if(ee.name() != L"value")
                    throwSerializationError();

                _state = OnValueEnd;
            }
            else if(node.type() == Xml::Node::StartElement)
            {
                throwSerializationError();
            }

            break;
        }

        case OnMemberBegin:
        {
            log_debug("OnMemberBegin");
            if(node.type() == Xml::Node::StartElement) // name
            {
                const Xml::StartElement& se = static_cast<const Xml::StartElement&>(node);

                if(se.name() != L"name")
                    throwSerializationError();

                _state = OnNameBegin;
            }
            else if(node.type() == Xml::Node::EndElement)
            {
                throwSerializationError();
            }

            break;
        }

        case OnNameBegin:
        {
            log_debug("OnNameBegin");
            if(node.type() == Xml::Node::Characters) // member-name
            {
                const Xml::Characters& chars = static_cast<const Xml::Characters&>(node);
                const std::string& name = chars.content().narrow();

                _current = _current->beginMember(name);

                _state = OnName;
            }
            else
            {
                throwSerializationError();
            }

            break;
        }

        case OnName:
        {
            log_debug("OnName");
            if(node.type() == Xml::Node::EndElement) // </name>
            {
                const Xml::EndElement& ee = static_cast<const Xml::EndElement&>(node);

                if(ee.name() != L"name")
                    throwSerializationError();

                _state = OnNameEnd;
            }
            else if(node.type() == Xml::Node::StartElement)
            {
                throwSerializationError();
            }

            break;
        }

        case OnNameEnd:
        {
            log_debug("OnNameEnd");
            if(node.type() == Xml::Node::StartElement) // <value>
            {
                const Xml::StartElement& se = static_cast<const Xml::StartElement&>(node);

                if(se.name() != L"value")
                    throwSerializationError();

                _state = OnValueBegin;
            }
            else if(node.type() == Xml::Node::EndElement)
            {
                throwSerializationError();
            }

            break;
        }

        case OnBoolBegin:
        {
            log_debug("OnBoolBegin ");
            if(node.type() == Xml::Node::Characters)
            {
                const Xml::Characters& chars = static_cast<const Xml::Characters&>(node);
                log_debug("-> found bool " << chars.content().narrow());

                bool found = false;
                bool value = false;
                const Pt::String& strval = chars.content();
                for(Pt::String::const_iterator it = strval.begin(); it != strval.end(); ++it)
                {
                    if( Pt::isspace(*it) )
                        continue;

                    switch(*it)
                    {
                        case '0':
                            if(found) 
                                throwSerializationError();

                            value = false;
                            found = true;
                            break;

                        case '1': 
                            if(found) 
                                throwSerializationError();

                            value = true;
                            found = true;
                            break;

                        default:
                            throwSerializationError();
                            break;
                    }
                }

                _current->setBool(value);
                _state = OnScalar;
            }
            else
            {
                throwSerializationError();
            }

            break;
        }

        case OnIntBegin:
        {
            log_debug("OnIntBegin ");
            if(node.type() == Xml::Node::Characters)
            {
                const Xml::Characters& chars = static_cast<const Xml::Characters&>(node);
                log_debug("-> found int " << chars.content().narrow());

                bool neg = false;
                long number = 0;
                const Pt::String& numstr = chars.content();
                for(Pt::String::const_iterator it = numstr.begin(); it != numstr.end(); ++it)
                {
                    if( Pt::isspace(*it) )
                        continue;

                    switch(*it)
                    {
                        case '+':
                            break;

                        case '-': 
                            if(neg) throwSerializationError();
                            neg = true; 
                            break;

                        case '0':
                        case '1':
                        case '2':
                        case '3':
                        case '4':
                        case '5':
                        case '6':
                        case '7':
                        case '8':
                        case '9':
                            number *= 10;
                            number += static_cast<int>(*it) - 48;
                            break;

                        default:
                            throwSerializationError();
                            break;
                    }
                }

                if(neg)
                    number *= -1;

                _current->setInt(number);
                _state = OnScalar;
            }
            else
            {
                throwSerializationError();
            }

            break;
        }

        case OnDoubleBegin:
        {
            log_debug("OnDoubleBegin ");
            if(node.type() == Xml::Node::Characters)
            {
                const Xml::Characters& chars = static_cast<const Xml::Characters&>(node);
                log_debug("-> found double " << chars.content().narrow());

                bool neg = false;
                double number = 0.0;
                const Pt::String& numstr = chars.content();
                Pt::String::const_iterator it;
                for(it = numstr.begin(); it != numstr.end(); ++it)
                {
                    if( Pt::isspace(*it) )
                        continue;

                    if( *it == '.' )
                    {
                        ++it;
                        break;
                    }

                    switch(*it)
                    {
                        case '+':
                            break;

                        case '-': 
                            if(neg) throwSerializationError();
                            neg = true; 
                            break;

                        case '0':
                        case '1':
                        case '2':
                        case '3':
                        case '4':
                        case '5':
                        case '6':
                        case '7':
                        case '8':
                        case '9':
                            number *= 10;
                            number += static_cast<int>(*it) - 48;
                            break;

                        default:
                            throwSerializationError();
                            break;
                    }

                }

                log_debug("-> intpart " << number);

                unsigned digits = 0;
                double fraction = 0.0;
                for(; it != numstr.end(); ++it)
                {
                    if( Pt::isspace(*it) )
                        break;

                    switch(*it)
                    {
                        case '0':
                        case '1':
                        case '2':
                        case '3':
                        case '4':
                        case '5':
                        case '6':
                        case '7':
                        case '8':
                        case '9':
                            fraction *= 10;
                            fraction += static_cast<int>(*it) - 48;
                            ++digits;
                            break;

                        default:
                            throwSerializationError();
                            break;
                    }
                }

                log_debug("-> fraction  " << fraction);

                // do not allow characters other than whitespace at end
                for(; it != numstr.end(); ++it)
                {
                    if( 0 == Pt::isspace(*it) )
                        throwSerializationError();
                }

                fraction /= (digits * 10);
                number += fraction;

                if(neg)
                    number *= -1;

                _current->setFloat(number);
                log_debug("-> parsed double " << number);
                _state = OnScalar;
            }
            else
            {
                throwSerializationError();
            }

            break;
        }

        case OnStringBegin:
        {
            log_debug("OnStringBegin ");
            if(node.type() == Xml::Node::Characters)
            {
                const Xml::Characters& chars = static_cast<const Xml::Characters&>(node);
                _state = OnScalar;

                log_debug("-> found string " << chars.content().narrow());
                _current->setValue( chars.content() );
            }
            else if(node.type() == Xml::Node::EndElement) // no content, for example empty strings
            {
                log_debug("-> found empty value ");
                _current->setValue( Pt::String() );
                _state = OnScalarEnd;
            }
            else
            {
                throwSerializationError();
            }

            break;
        }

        case OnScalar:
        {
            log_debug("OnScalar");
            if(node.type() == Xml::Node::EndElement) // </int>, boolean ...
            {
                _state = OnScalarEnd;
            }
            else if(node.type() == Xml::Node::StartElement)
            {
                throwSerializationError();
            }

            break;
        }

        case OnScalarEnd:
        {
            log_debug("OnScalarEnd");
            if(node.type() == Xml::Node::EndElement) // </value>
            {
                const Xml::EndElement& ee = static_cast<const Xml::EndElement&>(node);

                if(ee.name() != L"value")
                    throwSerializationError();

                _state = OnValueEnd;
            }
            else if(node.type() == Xml::Node::StartElement)
            {
                throwSerializationError();
            }

            break;
        }

        case OnArrayBegin:
        {
            log_debug("OnArrayBegin");
            if(node.type() == Xml::Node::StartElement) // <data>
            {
                const Xml::StartElement& se = static_cast<const Xml::StartElement&>(node);

                if(se.name() != L"data")
                    throwSerializationError();

                _state = OnDataBegin;
            }
            else if(node.type() == Xml::Node::EndElement)
            {
                throwSerializationError();
            }

            break;
        }

        case OnDataBegin:
        {
            log_debug("OnDataBegin");
            if(node.type() == Xml::Node::StartElement) // value
            {
                log_debug(_current);
                _current = _current->beginElement();
                _state = OnValueBegin;
            }
            else if(node.type() == Xml::Node::EndElement) // empty array
            {
                const Xml::EndElement& ee = static_cast<const Xml::EndElement&>(node);
                if(ee.name() != L"data")
                    throwSerializationError();

                _state = OnDataEnd;
            }

            break;
        }

        case OnDataEnd:
        {
            log_debug("OnDataEnd");
            if(node.type() == Xml::Node::EndElement) // </array>
            {
                const Xml::EndElement& ee = static_cast<const Xml::EndElement&>(node);

                if(ee.name() != L"array")
                    throwSerializationError();

                _state = OnArrayEnd;
            }
            else if(node.type() == Xml::Node::StartElement)
            {
                throwSerializationError();
            }

            break;
        }

        case OnArrayEnd:
        {
            log_debug("OnArrayEnd");
            if(node.type() == Xml::Node::EndElement) // </value>
            {
                const Xml::EndElement& ee = static_cast<const Xml::EndElement&>(node);

                if(ee.name() != L"value")
                    throwSerializationError();

                _state = OnValueEnd;
            }
            else if(node.type() == Xml::Node::StartElement)
            {
                throwSerializationError();
            }

            break;
        }
    }

    return false;
}

}

}
