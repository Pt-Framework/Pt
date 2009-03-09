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
#ifndef Pt_XmlRpc_RequestHandler_h
#define Pt_XmlRpc_RequestHandler_h

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

class PT_XMLRPC_API RequestHandler
{
    enum State
    {
        Default,
        BeforeMethodName,
        BeforeArgument
    };

    public:
        RequestHandler()
        : _ts(0)
        , _reader(0)
        , _state(Default)
        , _proc(0)
        , _service(0)
        {
        }

        void begin(std::istream& is, Service& service)
        {
            _args.clear();
            _service = &service;
            _state = Default;
            _proc = 0;
            _ts = new TextIStream(is, new Utf8Codec);
            _reader = new Xml::XmlReader(*_ts);
        }

        std::size_t advance()
        {
            std::size_t n = _ts->buffer().import();
            if(n)
            {
                while( _reader->advance() )
                {
                    const Xml::Node& node = _reader->get();
                    if(node.type() == Xml::Node::StartElement)
                    {
                        const Xml::StartElement& se = static_cast<const Xml::StartElement&>(node);
                        if(se.name() == L"methodName")
                        {
                            _state = BeforeMethodName;
                            continue;
                        }
                        if(se.name() == L"i4")
                        {
                            _state = BeforeArgument;
                            continue;
                        }
                    }

                    if(node.type() == Xml::Node::Characters)
                    {
                        const Xml::Characters& chars = static_cast<const Xml::Characters&>(node);
                        if( _state == BeforeMethodName )
                        {
                            _proc = _service->procedure( chars.content().narrow() );
                        }
                        else if( _state == BeforeArgument )
                        {
                            Pt::SerializationInfo si;
                            si.setValue( chars.content() );
                            _args.push_back(si);
                        }
                    }

                    _state = Default;
                }
            }

            return n;
        };

        void finish(std::ostream& out)
        {
            // throw if not end of document

            if(_proc)
                _proc->exec( _retval, &_args[0], _args.size() );

            out << "<value>"<< _retval.toString().narrow() << "</value>";
        }

    private:
        TextIStream* _ts;
        Xml::XmlReader* _reader;
        State _state;
        RemoteProcedure* _proc;
        std::vector<Pt::SerializationInfo> _args;
        Pt::SerializationInfo _retval;
        Service* _service;
};

}

}

#endif
