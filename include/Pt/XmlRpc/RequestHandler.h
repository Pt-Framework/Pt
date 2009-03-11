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
#ifndef Pt_XmlRpc_RequestReader_h
#define Pt_XmlRpc_RequestReader_h

#include <Pt/XmlRpc/Api.h>
#include <Pt/Void.h>
#include <Pt/SerializationInfo.h>
#include <Pt/Method.h>
#include "Pt/TextStream.h"
#include "Pt/Utf8Codec.h"
#include <Pt/Xml/XmlReader.h>
#include <Pt/Xml/StartElement.h>
#include <Pt/Xml/Characters.h>
#include <string>
#include <vector>
#include <map>
#include <cstddef>

namespace Pt {

namespace XmlRpc {

class RequestHandler
{
    enum State
    {
        OnBegin,
        OnMethodCallBegin,
        OnMethodNameBegin,
        OnMethodName,
        OnMethodNameEnd,
        OnParams,
        OnParamsEnd,
        OnMethodCallEnd
    };

    public:
        RequestHandler(Service& service, std::istream& is)
        : _state(OnBegin)
        , _ts(is, new Pt::Utf8Codec)
        , _reader(_ts)
        , _service(&service)
        , _proc(0)
        , _args(0)
        { }

        ~RequestHandler()
        {
            delete _args;
        }

        std::size_t advance()
        {
            std::size_t n = _ts.buffer().import();
            if(n == 0)
                return n;

            while( _reader.advance() )
            {
                const Pt::Xml::Node& node = _reader.get();
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

                            _args = _proc->createArgs();
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

                    case OnParamsEnd:
                    {
                        if(node.type() == Xml::Node::EndElement)
                        {
                            _state = OnMethodCallEnd;
                        }
                        break;
                    }

                    case OnMethodCallEnd:
                    {
                        if(node.type() == Xml::Node::EndDocument)
                        {
                            _state = OnMethodCallEnd;
                        }
                        break;
                    }
                }
            }

            return n;
        }

        void finish(std::ostream& out)
        {
            std::string res;

            if( ! _args || ! _proc )
                throw std::runtime_error("invalid XML-RPC request");

            _proc->exec(out, *_args);
        }

        Args* args()
        { return _args; }

    private:
       State _state;
       Pt::TextIStream _ts;
       Pt::Xml::XmlReader _reader;
       Service* _service;
       ServiceProcedure* _proc;
       Args* _args;
};

}

}

#endif
