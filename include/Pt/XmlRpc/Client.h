/*
 * Copyright (C) 2009 by Dr. Marc Boris Duerner
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
#ifndef Pt_XmlRpc_Client_h
#define Pt_XmlRpc_Client_h

#include <Pt/XmlRpc/Api.h>
#include <cstddef>

namespace Pt {

namespace XmlRpc {

class RequestFormatter : public Formatter
{
    public:
        RequestFormatter(std::ostream& out, const std::string& method)
        : _out(&out)
        {
            *_out << "<?xml version=\"1.0\"?>\n";
            *_out << "<methodCall>\n";
            *_out << "<methodName>" << method << "</methodName>\n";
            *_out << "<params>\n";
        }

        void addValue(const std::string& type, const Pt::String& value)
        {
            *_out << "<param>\n";
            *_out << "<value><" << type << ">" << value.narrow();
            *_out << "</" << type << "></value>\n";
            *_out << "</param>\n";
        }

        void finish()
        {
            *_out << "</params>\n";
            *_out << "</methodCall>\n";
        }

    private:
        std::ostream* _out;
};


class ResultReader
{
    enum State
    {
        OnParams,
        OnParam
    };

    public:
        ResultReader()
        : _state(OnParams)
        {}

        virtual ~ResultReader()
        {}

        bool advance(const Xml::Node& node)
        {
            switch(_state)
            {
                case OnParams:
                {
                    if(node.type() == Xml::Node::StartElement)
                    {
                        const Xml::StartElement& se = static_cast<const Xml::StartElement&>(node);

                        if(se.name() == L"param")
                        {
                            _state = OnParam;
                        }
                    }
                    else if(node.type() == Xml::Node::EndElement)
                    {
                        const Xml::EndElement& ee = static_cast<const Xml::EndElement&>(node);
                        if(ee.name() == L"params")
                        {
                            return true;
                        }
                    }

                    break;
                }

                case OnParam:
                {
                    bool finished = advanceResult(node);
                    if(finished)
                        _state = OnParams;
                }
            }

            return false;
        }

    protected:
        virtual bool advanceResult(const Xml::Node& node) = 0;

    private:
        State _state;
};


template <typename R>
class BasicResultReader : public ResultReader
{
    public:
        bool advanceResult(const Xml::Node& node)
        {
            return _r.advance(node);
        }

        const R& get() const
        { return _r.get(); }

    private:
        ParameterReader<R> _r;
};

template <typename R>
class ResponseReader
{
    enum State
    {
        OnBegin,
        OnMethodResponseBegin,
        OnParams,
        OnParamsEnd,
        OnMethodResponseEnd
    };

    public:
        ResponseReader(std::istream& is)
        : _state(OnBegin)
        , _ts(is, new Pt::Utf8Codec)
        , _reader(_ts)
        { }

        ~ResponseReader()
        { }

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
                        if(node.type() == Xml::Node::StartElement) // <methodResponse>
                        {
                            _state = OnMethodResponseBegin;
                        }
                        break;
                    }

                    case OnMethodResponseBegin:
                    {
                        if(node.type() == Xml::Node::StartElement) // <params>
                        {
                            _state = OnParams;
                        }
                        break;
                    }

                    case OnParams:
                    {
                        bool finished = _result.advance(node);
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
                            _state = OnMethodResponseEnd;
                        }
                        break;
                    }

                    case OnMethodResponseEnd:
                    {
                        if(node.type() == Xml::Node::EndDocument)
                        {
                            _state = OnMethodResponseEnd;
                        }
                        break;
                    }
                }
            }

            return n;
        }

        const R& result() const
        { return _result.get(); }

    private:
       State _state;
       Pt::TextIStream _ts;
       Pt::Xml::XmlReader _reader;
       BasicResultReader<R> _result;
};


template <typename R,
          typename A1,
          typename A2 >
class PT_XMLRPC_API RemoteMethod
{
    public:
        RemoteMethod(const std::string& name)
        {}

        virtual ~RemoteMethod()
        {}

        void begin(const A1& a1, const A2& a2)
        {

        }

        R result()
        {
        }

    private:

};


class PT_XMLRPC_API Client
{
    public:
        Client();

        virtual ~Client();
};

}

}

#endif
