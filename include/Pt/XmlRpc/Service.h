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

class Formatter
{
    public:
        virtual ~Formatter()
        {}

        virtual void addValue(const std::string& type, const Pt::String& value) = 0;
};


class ResponseFormatter : public Formatter
{
    public:
        ResponseFormatter(std::ostream& out)
        : _out(&out)
        {
            *_out << "<?xml version=\"1.0\"?>\n";
            *_out << "<methodResponse>\n";
            *_out << "<params>\n";
        }

        void begin(std::ostream& out)
        {
            _out = &out;
            *_out << "<?xml version=\"1.0\"?>\n";
            *_out << "<methodResponse>\n";
            *_out << "<params>\n";
            *_out << "<param>\n";
        }

        void addValue(const std::string& type, const Pt::String& value)
        {
            *_out << "<value><" << type << ">" << value.narrow();
            *_out << "</" << type << "></value>\n";
        }

        void beginArray()
        {
            *_out << "<array><data>";
        }

        void finishArray()
        {
            *_out << "</value></data>\n";
        }

        void finish()
        {
            *_out << "</param>\n";
            *_out << "</params>\n";
            *_out << "</methodResponse>\n";
        }

    private:
        std::ostream* _out;
};


class ISerializationHandler
{
    public:
        ISerializationHandler()
        : _parent(0)
        {}

        virtual ~ISerializationHandler()
        {}

        void setParent(ISerializationHandler* parent)
        { _parent = parent; }

        ISerializationHandler* parent()
        { return _parent; }

        virtual void setValue(const Pt::String& value) = 0;

        virtual ISerializationHandler* beginMember(const std::string& name) = 0;

        virtual void finishMember() = 0;

        virtual void finish() = 0;

        virtual void decompose(Formatter& f) = 0;

    private:
        ISerializationHandler* _parent;
};


template <typename T>
class SerializationHandler : public ISerializationHandler
{
    public:
        SerializationHandler()
        : _type(0)
        , _current(&_si)
        {}

        void begin(T& type)
        { _type = & type; }

        virtual void setValue(const Pt::String& value)
        {
          _si.setValue(value);
        }

        virtual ISerializationHandler* beginMember(const std::string& name)
        {
            SerializationInfo& child = _current->addMember(name);
            _current = &child;
            return this;
        }

        virtual void finishMember()
        {
            _current = _current->parent();
        }

        virtual void finish()
        {
            _si >>= *_type;
        }

        virtual void decompose(Formatter& formatter)
        {
            _si <<= *_type;

            if(_si.category() == SerializationInfo::Value)
            {
                formatter.addValue( _si.typeName(), _si.toString() );
            }
        }

    private:
        T* _type;
        Pt::SerializationInfo _si;
        Pt::SerializationInfo* _current;
};


template <typename T>
class SerializationHandler< std::vector<T> > : public ISerializationHandler
{
    public:
        SerializationHandler()
        : _type(0)
        {}

        void begin(std::vector<T>& type)
        { _type = & type; }

        void setValue(const Pt::String& value)
        { throw std::runtime_error("type mismatch"); }

        ISerializationHandler* beginMember(const std::string& name)
        {
            _type->push_back( T() );
            T& elem = _type->back();
            _elemBuilder.begin(elem);
            return &_elemBuilder;
        }

        void finishMember()
        { }

        void finish()
        { }

        void decompose(Formatter& formatter)
        {
            // formatter.beginArray();
            // formatter.finishArray();
        }

    private:
        SerializationHandler<T>  _elemBuilder;
        std::vector<T>* _type;
};


template <typename T>
class ParameterReader
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

        OnArrayBegin,
        OnDataBegin,
        OnDataEnd
    };

    public:
        ParameterReader()
        : _builder()
        , _current(&_builder)
        , _state(OnParamBegin)
        {
            _builder.begin(_value);
        }

        bool advance(const Xml::Node& node)
        {
            switch(_state)
            {
                case OnParamBegin:
                {
                    if(node.type() == Xml::Node::StartElement) // value
                    {
                        _state = OnValueBegin;
                    }
                    break;
                }

                case OnValueBegin:
                {
                    if(node.type() == Xml::Node::StartElement) // i4, struct, array...
                    {
                        const Xml::StartElement& se = static_cast<const Xml::StartElement&>(node);
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
                {
                    if(node.type() == Xml::Node::EndElement)
                    {
                        const Xml::EndElement& ee = static_cast<const Xml::EndElement&>(node);
                        if(ee.name() == L"member")
                        {
                            _current->finishMember();
                            _current = _current->parent();
                            if( ! _current )
                                throw std::runtime_error("invalid member");
                        }
                        else
                        {
                            _state = OnParamEnd;
                        }

                        return true;
                    }
                    else if(node.type() == Xml::Node::StartElement)
                    {
                        const Xml::StartElement& se = static_cast<const Xml::StartElement&>(node);
                        if(se.name() == L"value")
                        {
                            _current->finishMember();
                            _current = _current->parent();
                            if( ! _current )
                                throw std::runtime_error("invalid array");

                            ISerializationHandler* member = _current->beginMember("");
                            member->setParent(_current);
                            _current = member;
                            _state = OnValueBegin;
                        }

                        return true;
                    }
                    break;
                }

                case OnStructBegin:
                {
                    if(node.type() == Xml::Node::StartElement) //member
                    {
                        _state = OnMemberBegin;
                    }
                    break;
                }

                case OnMemberBegin:
                {
                    if(node.type() == Xml::Node::StartElement) // name
                    {
                        _state = OnNameBegin;
                    }
                    break;
                }

                case OnNameBegin:
                {
                    if(node.type() == Xml::Node::Characters) // member-name
                    {
                        const Xml::Characters& chars = static_cast<const Xml::Characters&>(node);
                        const std::string& name = chars.content().narrow();
                        ISerializationHandler* member = _current->beginMember(name);
                        member->setParent(_current);
                        _current = member;
                        _state = OnName;
                    }
                    break;
                }

                case OnName:
                {
                    if(node.type() == Xml::Node::EndElement)
                    {
                        _state = OnNameEnd;
                    }
                    break;
                }

                case OnNameEnd:
                {
                    if(node.type() == Xml::Node::StartElement)
                    {
                        _state = OnValueBegin;
                    }
                    break;
                }

                case OnScalarBegin:
                {
                    if(node.type() == Xml::Node::Characters)
                    {
                        const Xml::Characters& chars = static_cast<const Xml::Characters&>(node);
                        _state = OnScalar;
                        _current->setValue( chars.content() );
                    }
                    break;
                }

                case OnScalar:
                {
                    if(node.type() == Xml::Node::EndElement) // i4, boolean ...
                    {
                        _current->finish();
                        _state = OnScalarEnd;
                    }
                    break;
                }

                case OnScalarEnd:
                {
                    if(node.type() == Xml::Node::EndElement) // </value>
                    {
                        _state = OnValueEnd;
                    }
                    break;
                }

                case OnArrayBegin:
                {
                    if(node.type() == Xml::Node::StartElement) // data
                    {
                        _state = OnDataBegin;
                    }
                    break;
                }

                case OnDataBegin:
                {
                    if(node.type() == Xml::Node::StartElement) // value
                    {
                        ISerializationHandler* member = _current->beginMember("");
                        member->setParent(_current);
                        _current = member;
                        _state = OnValueBegin;
                    }
                    break;
                }

                case OnDataEnd:
                    break;

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
        SerializationHandler<T> _builder;
        ISerializationHandler* _current;
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
        , _argNo(0)
        {}

        virtual ~Args()
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
        ParameterReader<A1> _a1;
        ParameterReader<A2> _a2;
};


class ServiceProcedure
{
    public:
        ServiceProcedure()
        {}

        virtual ~ServiceProcedure()
        {}

        virtual Args* createArgs() const = 0;

        virtual void exec(std::ostream& ret, const Args& args) = 0;
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
class BasicServiceProcedure : public Method<R, C, A1, A2, A3, A4, A5, A6, A7, A8>
                            , public ServiceProcedure
{
    public:
        typedef R (C::*MemFuncT)(A1, A2, A3, A4, A5, A6, A7, A8);

    public:
        BasicServiceProcedure(C& object, MemFuncT ptr)
        : Method<R, C, A1, A2, A3, A4, A5, A6, A7, A8>(object, ptr)
        , ServiceProcedure()
        {}
};


template < typename R,
           class C,
           typename A1,
           typename A2>
class BasicServiceProcedure<R, C, A1, A2,
                            Pt::Void,
                            Pt::Void,
                            Pt::Void,
                            Pt::Void,
                            Pt::Void,
                            Pt::Void> : public Method<R, C, A1, A2>
                                      , public ServiceProcedure
{
    public:
        typedef R (C::*MemFuncT)(A1, A2);
        typedef typename TypeTraits<A1>::Value V1;
        typedef typename TypeTraits<A2>::Value V2;
        typedef typename TypeTraits<R>::Value RV;

    public:
        BasicServiceProcedure(C& object, MemFuncT ptr)
        : Method<R, C, A1, A2>(object, ptr)
        , ServiceProcedure()
        {}

        Args* createArgs() const
        {
            return new BasicArgs<V1, V2>();
        }

        void exec(std::ostream& ret, const Args& a)
        {
            const BasicArgs<V1, V2>& args = static_cast<const BasicArgs<V1, V2>& >(a);
            R result = Pt::Method<R, C, A1, A2>::call( args.first(), args.second() );

            SerializationHandler<R> builder;
            builder.begin(result);

            ResponseFormatter resp(ret);
            builder.decompose(resp);
            resp.finish();
        }
};


class PT_XMLRPC_API Service //: public ::Responder
{
    public:
        Service();

        virtual ~Service();

        ServiceProcedure* procedure(const std::string& name);

        template <typename R, class C, typename A1, typename A2>
        void registerMethod(const std::string& name, C& obj, R (C::*method)(A1, A2) )
        {
            ServiceProcedure* proc = new BasicServiceProcedure<R, C, A1, A2>(obj, method);
            this->registerProcedure(name, proc);
        }

    protected:
        void registerProcedure(const std::string& name, ServiceProcedure* proc);

    private:
        typedef std::map<std::string, ServiceProcedure*> ProcedureMap;
        ProcedureMap _procedures;
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
