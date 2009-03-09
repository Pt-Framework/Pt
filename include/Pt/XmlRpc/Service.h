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
#ifndef Pt_XmlRpc_Service_h
#define Pt_XmlRpc_Service_h

#include <Pt/XmlRpc/Api.h>
#include <Pt/Void.h>
#include <Pt/SerializationInfo.h>
#include <Pt/Method.h>
#include "Pt/TextStream.h"
#include "Pt/Utf8Codec.h"
#include <Pt/Xml/XmlReader.h>
#include <Pt/Xml/StartElement.h>
#include <Pt/Xml/EndElement.h>
#include <Pt/Xml/Characters.h>
#include <string>
#include <vector>
#include <map>
#include <cstddef>

namespace Pt {

namespace XmlRpc {

template <typename T>
class TypeBuilder
{
    public:
        TypeBuilder(T& type)
        : _type(&type)
        {}

        void setValue(const Pt::String& value)
        { convert(*_type, value); }

        void addMember(const std::string& name, const Pt::String& value)
        {}

    private:
        T* _type;
};


template <typename T>
class Parameter
{
    enum State
    {
        OnParamBegin,
        OnValueBegin,
        OnTypeBegin,
        OnContent,
        OnTypeEnd,
        OnValueEnd,
        OnParamEnd
    };

    public:
        Parameter()
        : _builder(_value)
        , _state(OnParamBegin)
        {}

        bool advance(const Xml::Node& node)
        {
            switch(_state)
            {
                case OnParamBegin:
                {
                    if(node.type() == Xml::Node::StartElement)
                    {
                        _state = OnValueBegin;
                    }
                    break;
                }

                case OnValueBegin:
                {
                    if(node.type() == Xml::Node::StartElement)
                    {
                        _state = OnTypeBegin;
                    }
                    break;
                }

                case OnTypeBegin:
                {
                    if(node.type() == Xml::Node::Characters)
                    {
                        const Xml::Characters& chars = static_cast<const Xml::Characters&>(node);
                        _state = OnContent;
                        _builder.setValue( chars.content() );
                    }
                    break;
                }

                case OnContent:
                {
                    if(node.type() == Xml::Node::EndElement)
                    {
                        _state = OnTypeEnd;
                    }
                    break;
                }

                case OnTypeEnd:
                {
                    if(node.type() == Xml::Node::EndElement)
                    {
                        _state = OnValueEnd;
                    }
                    break;
                }

                case OnValueEnd:
                {
                    if(node.type() == Xml::Node::EndElement)
                    {
                        _state = OnParamEnd;
                        return true;
                    }
                    break;
                }
                default:
                {
                    throw std::runtime_error("OnParamEnd");
                }

            }

            return false;
        }

        const T& get() const
        { return _value; }

    private:
        T _value;
        TypeBuilder<T> _builder;
        State _state;
};


class Args
{
    enum State
    {
        OnParams,
        OnParam
    };

    public:
        Args()
        : _state(OnParams)
        {}

        void begin()
        {
            _state = OnParams;
            _argNo = 0;
        }

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
                    bool finished = advanceParam(_argNo, node);
                    if(finished)
                    {
                        ++_argNo;
                        _state = OnParams;
                    }
                }
            }

            return false;
        }

    protected:
        virtual bool advanceParam(unsigned n, const Xml::Node& node) = 0;

    private:
        State _state;
        unsigned _argNo;
};


template <typename A1, typename A2>
class BasicArgs : public Args
{
    public:
        bool advanceParam(unsigned n, const Xml::Node& node)
        {
            switch(n)
            {
                case 0:
                    return _a1.advance(node);
                    break;

                case 1:
                    return _a2.advance(node);
                    break;
            }

            return true;
        }

        const A1& first() const
        { return _a1.get(); }

        const A2& second() const
        { return _a2.get(); }

    private:
        Parameter<A1> _a1;
        Parameter<A2> _a2;
};


class RemoteProcedure
{
    public:
        RemoteProcedure()
        {}

        virtual ~RemoteProcedure()
        {}

        virtual Args* createArgs() const = 0;

        virtual void exec(SerializationInfo& ret, SerializationInfo* si, unsigned argCount) = 0;

        virtual void exec2(std::string& ret, const Args& args) = 0;
};


template < typename R,
           class C,
           typename A1 = Pt::Void,
           typename A2 = Pt::Void,
           typename A3 = Pt::Void,
           typename A4 = Pt::Void,
           typename A5 = Pt::Void,
           typename A6 = Pt::Void,
           typename A7 = Pt::Void,
           typename A8 = Pt::Void >
class BasicRemoteProcedure : public Method<R, C, A1, A2, A3, A4, A5, A6, A7, A8>
                           , public RemoteProcedure
{
    public:
        typedef C ClassT;
        typedef R (C::*MemFuncT)(A1, A2, A3, A4, A5, A6, A7, A8);

    public:
        BasicRemoteProcedure(C& object, MemFuncT ptr)
        : Method<R, C, A1, A2, A3, A4, A5, A6, A7, A8>(object, ptr)
        , RemoteProcedure()
        {}

        void exec(SerializationInfo& ret, SerializationInfo* si, unsigned argCount)
        { }
};


template < typename R,
           class C,
           typename A1,
           typename A2>
class BasicRemoteProcedure<R,
                           C,
                           A1,
                           A2,
                           Pt::Void,
                           Pt::Void,
                           Pt::Void,
                           Pt::Void,
                           Pt::Void,
                           Pt::Void> : public Method<R, C, A1, A2>
                                     , public RemoteProcedure
{
    public:
        typedef C ClassT;
        typedef R (C::*MemFuncT)(A1, A2);

    public:
        BasicRemoteProcedure(C& object, MemFuncT ptr)
        : Method<R, C, A1, A2>(object, ptr)
        , RemoteProcedure()
        {}

        void exec(SerializationInfo& ret, SerializationInfo* si, unsigned argCount)
        {
            if(argCount != 2)
                throw std::invalid_argument("invalid number of arguments");

            // convert to arguments A1 ... A3

            A1 a1;
            si[0] >>= a1;

            A2 a2;
            si[1] >>= a2;

            R r = Pt::Method<R, C, A1, A2>::call(a1, a2);

            ret <<= r;
            // convert return value r to rpc response
        }

        void exec2(std::string& r, const Args& a)
        {
            const BasicArgs<A1, A2>& args = static_cast<const BasicArgs<A1, A2>& >(a);
            R result = Pt::Method<R, C, A1, A2>::call( args.first(), args.second() );
            Pt::convert(r, result);
        }

        Args* createArgs() const
        { return new BasicArgs<A1, A2>(); }
};


class PT_XMLRPC_API Service //: public ::Responder
{
    public:
        Service();

        virtual ~Service();

        RemoteProcedure* procedure(const std::string& name);

        template <typename R, class C, typename A1, typename A2>
        void registerMethod(const std::string& name, C& obj, R (C::*method)(A1, A2) )
        {
            RemoteProcedure* proc = new BasicRemoteProcedure<R, C, A1, A2>(obj, method);
            this->registerProcedure(name, proc);
        }

    protected:
        void registerProcedure(const std::string& name, RemoteProcedure* proc);

    private:
        typedef std::map<std::string, RemoteProcedure*> ProcedureMap;
        ProcedureMap _procedures;
};


class RequestHandler2
{
    public:
        RequestHandler2(Service& service, std::istream& is)
        : _ts(is, new Pt::Utf8Codec)
        , _reader(_ts)
        , _service(&service)
        , _args(0)
        {
            _args = _service->procedure("multiply")->createArgs();
            _args->begin();
        }

        ~RequestHandler2()
        {
            delete _args;
        }

        std::size_t advance()
        {
            std::size_t n = _ts.buffer().import();
            if(n)
            {
                while( _reader.advance() )
                {
                    const Pt::Xml::Node& node = _reader.get();
                    bool finished = _args->advance(node);
                    if(finished)
                        break;
                }
            }
 
            return n;
        }

        void finish(std::ostream& out)
        {
            std::string res;
            _service->procedure("multiply")->exec2(res, *_args);
            out << res;
        }

    private:
       Pt::TextIStream _ts;
       Pt::Xml::XmlReader _reader;
       Service* _service;
       Pt::XmlRpc::Args* _args;
};

/*

class HttpSocket : public TcpSocket
{
    public:
        HttpSocket(Selector s, HttpServer server)
        {
            _selector.add(*client);
            selector.add(timer);
            connect(client.inputReady, *client, &HttpSocket::onInput);
        }

        void onInput(TcpSocket& server)
        {
            if( readHeader )
            {
                url = server.read();
                caller.set(url);
            }
            else
            {
                size_t n = caller.exec(*this);
                _timer->start(timeout);
                ...

                close();
                delete this;
            }
        }

        void onTimeout()
        {
             //timeut error
             close();
             delete this;
        }

    private:
        Timer _timer;
};


class HttpServer : public TcpServer
{
    public:
        HttpServer(Selector& selector)
        : _selector(selector)
        {
            _selector.add(*this);
            connect(connectionPending, *this, &HttpServer::onConnect)
        }

        void addResponder(const std::string& url, Responder& resp)
        { _responder.insert(url, &resp); }

        void onConnect(TcpServer& server)
        {
            HttpSocket* client = new HttpSocket(_selector, *this);

        }

    private:
        std::map<std::string, Resopnder*> _responder;
        Selector* _selector;
};


int multiply(int a, int b);


int main()
{
    Application app;

    Service service();
    service.registerMethod( multiply );

    HttpServer server( app.loop() );
    server.addResponder("Calc", service);

    app.run();
}

*/

}

}

#endif
