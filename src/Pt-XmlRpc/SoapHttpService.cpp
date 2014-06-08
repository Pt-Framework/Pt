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

#include <Pt/XmlRpc/SoapHttpService.h>
#include <Pt/XmlRpc/Fault.h>
#include <Pt/XmlRpc/ServiceDefinition.h>
#include <Pt/Xml/XmlError.h>
#include <Pt/Xml/StartElement.h>
#include <Pt/Xml/Characters.h>
#include <Pt/Xml/EndElement.h>
#include <Pt/Http/Request.h>
#include <Pt/Http/Reply.h>
#include <Pt/System/Logger.h>
#include <Pt/Utf8Codec.h>
#include <Pt/Convert.h>
#include <cassert>

namespace {

static const Pt::Char XMLRPC_XMLDECL[] = { '<', '?', 'x', 'm', 'l', ' ', 
    'v', 'e', 'r', 's', 'i', 'o', 'n', '=', '"', '1', '.', '0' , '"', ' ', 
    'e', 'n', 'c', 'o', 'd', 'i', 'n', 'g', '=', '"', 'U', 'T', 'F', '-', '8', '"', 
    '?', '>' };

static const Pt::Char SOAP_REPLY_BEGIN[]  = { '<', 's', 'o', 'a', 'p', ':', 'E', 'n', 'v', 'e', 'l', 'o', 'p', 'e', ' ',
                                               'x', 'm', 'l', 'n', 's', ':', 's', 'o', 'a', 'p', '=',
                                               '"', 'h', 't', 't', 'p', ':', '/', '/', 's', 'c', 'h', 'e', 'm', 'a', 's', '.', 
                                               'x', 'm', 'l', 's', 'o', 'a', 'p', '.', 'o', 'r', 'g', '/', 's', 'o', 'a', 'p', 
                                               '/', 'e', 'n', 'v', 'e', 'l', 'o', 'p', 'e', '/', '"', '>',
                                              '<', 's', 'o', 'a', 'p', ':', 'B', 'o', 'd', 'y', '>' };

static const Pt::Char SOAP_REPLY_END[]  = { '<', '/', 's', 'o', 'a', 'p', ':', 'B', 'o', 'd', 'y', '>',
                                            '<', '/', 's', 'o', 'a', 'p', ':', 'E', 'n', 'v', 'e', 'l', 'o', 'p', 'e', '>' }; 

static const Pt::Char SOAP_FAULT[]  = { '<', 's', 'o', 'a', 'p', ':', 'F', 'a', 'u', 'l', 't',  '>' };
static const Pt::Char SOAP_FAULT_END[]  = { '<', '/', 's', 'o', 'a', 'p', ':', 'F', 'a', 'u', 'l', 't',  '>' };

static const Pt::Char SOAP_CODE[]  = { '<', 's', 'o', 'a', 'p', ':', 'C', 'o', 'd', 'e', '>' };
static const Pt::Char SOAP_CODE_END[]  = { '<', '/', 's', 'o', 'a', 'p', ':', 'C', 'o', 'd', 'e', '>' };

static const Pt::Char SOAP_CODE_RECEIVER[]  = { 's', 'o', 'a', 'p', ':', 'R', 'e', 'c', 'e', 'i', 'v', 'e', 'r' };

static const Pt::Char SOAP_VALUE[]  = { '<', 's', 'o', 'a', 'p', ':', 'V', 'a', 'l', 'u', 'e',  '>' };
static const Pt::Char SOAP_VALUE_END[]  = { '<', '/', 's', 'o', 'a', 'p', ':', 'V', 'a', 'l', 'u', 'e',  '>' };

static const Pt::Char SOAP_REASON[]  = { '<', 's', 'o', 'a', 'p', ':', 'R', 'e', 'a', 's', 'o', 'n', '>' };
static const Pt::Char SOAP_REASON_END[]  = { '<', '/', 's', 'o', 'a', 'p', ':', 'R', 'e', 'a', 's', 'o', 'n', '>' };

} // namespace

namespace Pt {

namespace XmlRpc {

///////////////////////////////////////////////////////////////////////////////
// SoapResponderBase
///////////////////////////////////////////////////////////////////////////////

SoapResponderBase::SoapResponderBase(ServiceDefinition& serviceDef)
: _serviceDef(&serviceDef)
, _proc(0)
, _args(0)
{
}


SoapResponderBase::~SoapResponderBase()
{
    if(_proc)
        _serviceDef->releaseProcedure(_proc);
}


Pt::Composer** SoapResponderBase::setProcedure(const std::string& name)
{
    if(_proc)
        _serviceDef->releaseProcedure(_proc);

    _proc = _serviceDef->getProcedure( name, *this );

    _args = _proc ? _proc->beginArgs() : 0;
   
    return _args;
}


void SoapResponderBase::beginCall(System::EventLoop& loop)
{
    if( ! _proc )
    {
        throw Fault("invalid XML-RPC", 4);
    }

    if( _args && *_args )
    {
        throw Fault("expected more arguments", 5);
    }

    _proc->beginCall(loop); // throws Fault
}


Pt::Decomposer* SoapResponderBase::endCall()
{
    if( ! _proc )
    {
        throw Fault("invalid XML-RPC", 4);
    }

    return _proc->endCall(); // throws Fault
}


void SoapResponderBase::cancel()
{
    this->onCancel();
    
    if(_proc)
        _serviceDef->releaseProcedure(_proc);
    
    _proc = 0;
    _args = 0;
}

///////////////////////////////////////////////////////////////////////////////
// SoapResponder
///////////////////////////////////////////////////////////////////////////////

SoapResponder::SoapResponder(const SoapServiceDeclaration& decl, ServiceDefinition& def)
: SoapResponderBase(def)
, _serviceDecl( &decl )
, _op(0)
, _reader(_bin)
, _arg(0)
, _state(OnBegin)
, _utf8(1)
, _ts(&_utf8)
, _result(0)
, _formatter(_ts)
, _isFault(false)
{
}


SoapResponder::~SoapResponder()
{
    _ts.detach();
}


bool SoapResponder::isFault() const
{
    return  _isFault;
}


void SoapResponder::onCancel()
{
    _state = OnBegin;
    _ts.detach();
    _ts.discard();

    _arg = 0;
    _result = 0;
    _isFault = false;
}


void SoapResponder::beginMessage(std::istream& is)
{
    cancel();
    
    _bin.reset(is);
}


bool SoapResponder::parseMessage()
{
    try
    {
        if( this->isFault() )
            return true;
        
        for(;;)
        {
            const Xml::Node* node = _reader.advance();
            if( ! node )
            {
                break;
            }
            
            bool done = this->advance(*node);
            if(done)
            {
                return true;
            }
        }

        return false;
    }
    catch(const Xml::XmlError& error)
    {
        setFault(1, error.what() );
    }
    catch(const SerializationError& error)
    {
        setFault(2, error.what() );
    }
    catch(const ConversionError& error)
    {
        setFault(3, error.what() );
    }
    catch(const Fault& fault)
    {
        setFault(fault.rc(), fault.text().c_str() );
    }

    return true;
}


void SoapResponder::finishMessage(System::EventLoop& loop)
{
    if( this->isFault() )
    {
        onFault(_fault);
        return;
    }

    try
    {
        beginCall(loop);
    }
    catch(const Fault& fault)
    {
        setFault(fault.rc(), fault.text().c_str() );
        onFault(_fault);
    }
}


void SoapResponder::onReady()
{    
    try
    {
        // we get here only after setReady was called and we call setReady
        // only if no fault is pending.
        //if( _isFault )
        //{
        //    onError();
        //    return;
        //}
        
        _result = endCall(); // throws Fault
        onResult();
    }
    catch(const Fault& fault)
    {
        setFault( fault.rc(), fault.text().c_str() );
        onFault(_fault);
    } 
}


void SoapResponder::beginResult(std::ostream& os)
{
    _ts.clear();
    _ts.discard();
    _ts.attach(os);

    _ts.write( XMLRPC_XMLDECL, sizeof(XMLRPC_XMLDECL)/sizeof(Char) );

    assert(_result);
    _ts.write(SOAP_REPLY_BEGIN, sizeof(SOAP_REPLY_BEGIN)/sizeof(Char));

    const Pt::String& outName = _op->outputName();
    Pt::String targetNamespace = _serviceDecl->targetNamespace().c_str();
    _ts << '<' << outName << Pt::String(" xmlns=\"") << targetNamespace << '"' << '>';
    
    const Parameter* param = _op->getOutput();
    assert(param);
    if( ! param)
        throw SerializationError("no output defined"); // check if catched

    _formatter.setParameter(*param);
    _result->beginFormat(_formatter);
}


void SoapResponder::beginFault(std::ostream& os, const Fault& fault)
{
    int rc = fault.rc();
    const char* msg = fault.text().c_str();

    // text stream might still have bytes in text buffer
    _ts.flush();

    _ts.clear();
    _ts.discard();
    _ts.attach(os);
    
    _ts.write( XMLRPC_XMLDECL, sizeof(XMLRPC_XMLDECL)/sizeof(Char) );
    _ts.write( SOAP_REPLY_BEGIN, sizeof(SOAP_REPLY_BEGIN)/sizeof(Char) );

    _ts.write( SOAP_FAULT, sizeof(SOAP_FAULT)/sizeof(Char) );
    _ts.write( SOAP_CODE, sizeof(SOAP_CODE)/sizeof(Char) );
    _ts.write( SOAP_VALUE, sizeof(SOAP_VALUE)/sizeof(Char) );

    // TODO: insert fault code
    _ts.write( SOAP_CODE_RECEIVER, sizeof(SOAP_CODE_RECEIVER)/sizeof(Char) );

    _ts.write( SOAP_VALUE_END, sizeof(SOAP_VALUE_END)/sizeof(Char) );
    _ts.write( SOAP_CODE_END, sizeof(SOAP_CODE_END)/sizeof(Char) );

    _ts.write( SOAP_REASON, sizeof(SOAP_REASON)/sizeof(Char) );

     for(const char* str = msg; *str != '\0'; ++str)
        _ts << Char(*str);

    _ts.write( SOAP_REASON_END, sizeof(SOAP_REASON_END)/sizeof(Char) );
    _ts.write( SOAP_FAULT_END, sizeof(SOAP_FAULT_END)/sizeof(Char) );
    _ts.write( SOAP_REPLY_END, sizeof(SOAP_REPLY_END)/sizeof(Char) );

    _ts.flush();
}


bool SoapResponder::advanceResult()
{
    for(unsigned n = 0; _result && n < 10; ++n)
    {
        _result = _result->advanceFormat(_formatter);
    }

    return _result == 0;
}


void SoapResponder::finishResult()
{
    if( ! this->isFault() )
    {
        const Pt::String& outName = _op->outputName();
        _ts << '<' << '/' << outName << '>';
        _ts.write(SOAP_REPLY_END, sizeof(SOAP_REPLY_END)/sizeof(Char));
        _ts.flush();
    }
}


void SoapResponder::setFault(int rc, const char* msg)
{
    _fault.setRc(rc);
    _fault.setText(msg);
    _isFault = true;
}


bool SoapResponder::advance(const Pt::Xml::Node& node)
{
    switch(_state)
    {
        case OnBegin:
        {
            if(node.type() == Xml::Node::StartElement)
            {
                const Xml::StartElement& se = static_cast<const Xml::StartElement&>(node);
                if( se.name().name() != L"Envelope" )
                    throw SerializationError("invalid SOAP Envelope");

                _state = OnEnvelope;
            }

            break;
        }

        case OnEnvelope:
        {
            if(node.type() == Xml::Node::StartElement)
            {
                const Xml::StartElement& se = static_cast<const Xml::StartElement&>(node);
                if( se.name().name() == L"Body" )
                    _state = OnBody;
            }
            
            break;
        }

        case OnBody:
        {
            if(node.type() == Xml::Node::StartElement)
            {
                const Xml::StartElement& se = static_cast<const Xml::StartElement&>(node);

                _op = _serviceDecl->getOperation( se.name().local() );
                if( ! _op )
                    throw Fault("no such procedure", Pt::XmlRpc::Fault::MethodNotFound);

                if( ! setProcedure( se.name().local().narrow() ) )
                    throw Fault("no such procedure", Pt::XmlRpc::Fault::MethodNotFound);

                _state = OnMethod;
            }
            break;
        }

        case OnMethod:
        {
            if(node.type() == Xml::Node::EndElement) // end of method tag
            {
                _state = OnMethodEnd;
                break;
            }

            if(node.type() == Xml::Node::StartElement)
            {
                const Xml::StartElement& se = static_cast<const Xml::StartElement&>(node);

                _arg = arg();

                if( ! _arg )
                    throw SerializationError("too many arguments");

                const Parameter* param = _op->getInput( se.name().local().narrow() );
                if( ! param )
                    throw Fault("no such parameter", Pt::XmlRpc::Fault::MethodNotFound);

                _formatter.setParameter(*param);
                _formatter.beginParse(*_arg);
                
                _state = OnParam;
                break;
            }

            break;
        }

        case OnParam:
        {
            bool finished = _formatter.advance(node);
            if(finished)
            {
                nextArg();
                _state = OnMethod;
            }

            break;
        }

        case OnMethodEnd:
        {
            if(node.type() == Xml::Node::EndElement) // </Body>
            {
                assert( Xml::toEndElement(node).name().local() == L"Body" );
                _state = OnBodyEnd;
            }
            
            break;
        }

        case OnBodyEnd:
        {
            if(node.type() == Xml::Node::EndElement) // </Envelope>
            {
                assert( Xml::toEndElement(node).name().local() == L"Envelope" );
                _state = OnEnvelopeEnd;
            }
            
            break;
        }

        case OnEnvelopeEnd:
        {
            break;
        }
    }

    return _state == OnEnvelopeEnd;
}

///////////////////////////////////////////////////////////////////////////////
// SoapHttpResponder
///////////////////////////////////////////////////////////////////////////////

SoapHttpResponder::SoapHttpResponder(SoapHttpService& httpService, const SoapServiceDeclaration& decl, ServiceDefinition& def)
: Http::Responder(httpService)
, XmlRpc::SoapResponder(decl, def)
, _reply(0)
{
}

SoapHttpResponder::~SoapHttpResponder()
{
}


// pass only ReplyHeader and body stream
void SoapHttpResponder::onBeginRequest(Http::Request& request, Pt::Http::Reply& reply, System::EventLoop& loop)
{
    _reply = 0;
    
    beginMessage( request.body() );
}


// pass only ReplyHeader and body stream
void SoapHttpResponder::onReadRequest(Http::Request& request, Pt::Http::Reply& reply, System::EventLoop& loop)
{
    parseMessage();
}


void SoapHttpResponder::onBeginReply(const Http::Request& request, Http::Reply& reply, System::EventLoop& loop)
{
    _reply = &reply;
    finishMessage(loop);
}


void SoapHttpResponder::onWriteReply(const Http::Request& request, Http::Reply& reply, System::EventLoop& loop)
{
    advanceSoapReply(reply);
}


void SoapHttpResponder::advanceSoapReply(Http::Reply& reply)
{
    while( ! advanceResult() )
    {
        if(reply.buffer().size() > 8192)
        {
            reply.beginSend(false);
            return;
        }
    }

    finishResult();
    reply.beginSend(true);
}


void SoapHttpResponder::onResult()
{
    assert(_reply);

    if( _reply )
    {
        _reply->header().set("Content-Type", "text/xml");
        beginResult(_reply->body() );
        advanceSoapReply(*_reply);
    }
}


void SoapHttpResponder::onFault(const Fault& fault)
{
    assert(_reply);

    if( _reply )
    {
        _reply->header().set("Content-Type", "text/xml");
        _reply->header().set("Connection", "close");
        beginFault(_reply->body(), fault );
        advanceSoapReply(*_reply);
    }
}


void SoapHttpResponder::onCancel()
{
    SoapResponder::onCancel();

    // not really possible, since only the HTTP server uses this class
}


///////////////////////////////////////////////////////////////////////////////
// WsdlResponder
///////////////////////////////////////////////////////////////////////////////

class WsdlResponder : public Pt::Http::Responder
{
    public:
        WsdlResponder(SoapHttpService& httpService, const SoapServiceDeclaration& serviceDecl)
        : Pt::Http::Responder(httpService)
        , _serviceDecl( serviceDecl )
        {}

        ~WsdlResponder()
        {}

    protected:
        // inheritdoc
        void onBeginRequest(Http::Request& request, Pt::Http::Reply& reply, System::EventLoop& loop)
        {}

        // inheritdoc
        void onReadRequest(Http::Request& request, Pt::Http::Reply& reply, System::EventLoop& loop)
        {}

        // inheritdoc
        void onBeginReply(const Http::Request& request, Http::Reply& reply, System::EventLoop& loop)
        {
            _serviceDecl.toWsdl( reply.body() );
            reply.beginSend(true);
        }

        // inheritdoc
        void onWriteReply(const Http::Request& request, Http::Reply& reply, System::EventLoop& loop)
        {}

    private:
        const SoapServiceDeclaration& _serviceDecl;
};

///////////////////////////////////////////////////////////////////////////////
// SoapHttpService
///////////////////////////////////////////////////////////////////////////////

SoapHttpService::SoapHttpService(SoapServiceDefinition& serviceDef)
: _serviceDecl( &serviceDef.declaration() )
, _serviceDef( &serviceDef )
{ 
}


SoapHttpService::SoapHttpService(const SoapServiceDeclaration& decl, ServiceDefinition& def)
: _serviceDecl(&decl)
, _serviceDef(&def)
{
}


SoapHttpService::~SoapHttpService()
{
}


Http::Responder* SoapHttpService::onGetResponder(const Http::Request& req)
{
    //if (req.isHeaderValue("Content-Type", "text/xml"))
    //    return new XmlRpcResponder(*this);

    //if (req.isHeaderValue("Content-Type", "text/xml; charset=UTF-8")) //! ### Temporary fix ###
    //    return new XmlRpcResponder(*this);

    if(req.qparams() == "wsdl")
    {
        return new WsdlResponder(*this, *_serviceDecl);
    }

    return new SoapHttpResponder(*this, *_serviceDecl, *_serviceDef);
}


void SoapHttpService::onReleaseResponder(Http::Responder* resp)
{
    delete resp;
}

} // namespace XmlRpc

} // namespace Pt
