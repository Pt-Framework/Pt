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
        { }

        virtual void addValue(const std::string& type, const Pt::String& value)
        { }

        virtual void beginArray()
        { }

        virtual void finishArray()
        { }

        virtual void beginObject()
        { }

        virtual void beginMember(const std::string& name)
        { }

        virtual void finishMember()
        { }

        virtual void finishObject()
        { }
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
            *_out << "<param>\n";
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
            *_out << "<value><array><data>\n";
        }

        void finishArray()
        {
            *_out << "</data></array></value>\n";
        }

        void beginObject()
        {
            *_out << "<value><struct>\n";
        }

        void beginMember(const std::string& name)
        {
            *_out << "<member>\n";
            *_out << "<name>" << name << "</name>\n";
        }

        void finishMember()
        {
            *_out << "</member>\n";
        }

        void finishObject()
        {
            *_out << "</struct></value>\n";
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

        virtual void setValue(const Pt::String& value) = 0;

        virtual ISerializationHandler* beginMember(const std::string& name) = 0;

        virtual ISerializationHandler* leaveMember() = 0;

        virtual void finish() = 0;

        virtual void decompose(Formatter& f) = 0;

        void setParent(ISerializationHandler* parent)
        { _parent = parent; }

        ISerializationHandler* parent()
        { return _parent; }

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

        void begin(const T& type)
        { _type = const_cast<T*>(&type); }

        virtual void setValue(const Pt::String& value)
        { //std::cerr << "-S SET VALUE " << value.narrow() << std::endl;
            _current->setValue(value);
        }

        virtual ISerializationHandler* beginMember(const std::string& name)
        { //std::cerr << "-S BEGIN MEMBER" << std::endl;
            SerializationInfo& child = _current->addMember(name);
            _current = &child;
            return this;
        }

        virtual ISerializationHandler* leaveMember()
        { //std::cerr << "-S LEAVE MEMBER" << std::endl;
            if( ! _current->parent() )
            {
                this->finish();

                if( ! this->parent() )
                    throw std::runtime_error("invalid member");

                return this->parent();
            }

            _current = _current->parent();
            return this;
        }

        virtual void finish()
        { //std::cerr << "-S FINISH" << std::endl;
            *_current >>= *_type;
        }

        virtual void decompose(Formatter& formatter)
        {
            _si <<= *_type;
            this->formatEach(_si, formatter);
        }

        static void formatEach(const Pt::SerializationInfo& si, Formatter& formatter)
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
                    formatEach(*it, formatter);
                    formatter.finishMember();
                }

                formatter.finishObject();
            }

            //TODO arrays should use SerializationInfo Array
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

        void begin(const std::vector<T>& type)
        { _type = const_cast<std::vector<T>*>(&type); }

        void setValue(const Pt::String& value)
        { throw std::runtime_error("type mismatch"); }

        ISerializationHandler* beginMember(const std::string& name)
        { //std::cerr << "V begin member" << std::endl;
            _type->push_back( T() );
            T& elem = _type->back();
            _elemBuilder.begin(elem);
            _elemBuilder.setParent(this);
            return &_elemBuilder;
        }

        virtual ISerializationHandler* leaveMember()
        { //std::cerr << "V begin member" << std::endl;
            ISerializationHandler* parent = this->parent();
            if( ! parent )
                throw std::runtime_error("invalid member");

            _elemBuilder.finish();
            return parent;
        }

        virtual void finish()
        { }

        void decompose(Formatter& formatter)
        {
            formatter.beginArray();

            typename std::vector<T>::iterator it;
            for(it = _type->begin(); it != _type->end(); ++it)
            {
                _elemBuilder.begin(*it);
                _elemBuilder.decompose(formatter);
            }

            formatter.finishArray();
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
        OnStructEnd,

        OnArrayBegin,
        OnDataBegin,
        OnDataEnd,
        OnArrayEnd
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

        bool compose(const Xml::Node& node)
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
                    bool finished = composeParam(_argNo, node);
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
        virtual bool composeParam(unsigned n, const Xml::Node& node) = 0;

    private:
        State _state;
        unsigned _argNo;
};


template <typename A1, typename A2>
class BasicArgs : public Args
{
    public:
        bool composeParam(unsigned n, const Xml::Node& node)
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
