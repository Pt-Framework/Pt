/* Copyright (C) 2015-2026 by Laurentiu-Gheorghe Crisan
   Copyright (C) 2026 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestSuite.h"
#include "Pt/Unit/RegisterTest.h"
#include "Pt/Http/Server.h"
#include "Pt/Http/Servlet.h"
#include "Pt/Http/Service.h"
#include "Pt/Http/Responder.h"
#include "Pt/Http/WebSocket.h"
#include "Pt/Http/WebSocketMessage.h"
#include "Pt/Http/Message.h"
#include "Pt/Http/WebSocketService.h"
#include "Pt/Http/WebSocketServlet.h"
#include "Pt/Http/WebSocketSession.h"
#include "Pt/Http/Client.h"
#include "Pt/Http/Request.h"
#include "Pt/Http/Reply.h"
#include "Pt/Net/Endpoint.h"
#include "Pt/System/MainLoop.h"
#include "Pt/System/Timer.h"

#include <iterator>
#include <stdexcept>
#include <string>

class DeclineResponder : public Pt::Http::Responder
{
    public:
        explicit DeclineResponder(Pt::Http::Service& s)
        : Pt::Http::Responder(s)
        {}

    protected:
        virtual void onBeginRequest(Pt::Http::Request&, Pt::Http::Reply&, Pt::System::EventLoop&)
        {
            setReady(false);
        }

        virtual void onReadRequest(Pt::Http::Request&, Pt::Http::Reply&, Pt::System::EventLoop&)
        {
            setReady(false);
        }

        virtual void onBeginReply(const Pt::Http::Request& request, Pt::Http::Reply& reply, Pt::System::EventLoop& loop)
        {
            onWriteReply(request, reply, loop);
        }

        virtual void onWriteReply(const Pt::Http::Request&, Pt::Http::Reply& reply, Pt::System::EventLoop&)
        {
            reply.setStatus(101, "Switching Protocols");
            reply.header().setUpgrade();
            reply.header().set("Upgrade", "websocket");
            reply.header().set("Connection", "Upgrade");
            setReady(true);
        }
};

typedef Pt::Http::BasicService<DeclineResponder> DeclineService;

class RecordSession : public Pt::Http::WebSocketSession
{
    public:
        RecordSession(Pt::Http::WebSocketService& service,
                      Pt::System::EventLoop& loop,
                      Pt::Http::Stream& stream,
                      const Pt::Http::Reply& reply,
                      std::string& message,
                      Pt::Http::WebSocketMessage::Type& type,
                      bool& received,
                      bool& closed,
                      Pt::System::EventLoop& exitLoop)
        : Pt::Http::WebSocketSession(service, loop, stream, reply)
        , _message(&message)
        , _type(&type)
        , _received(&received)
        , _closed(&closed)
        , _exitLoop(&exitLoop)
        {
            beginReceive();
        }

    protected:
        virtual void onInput()
        {
            endReceive();

            _message->assign( std::istreambuf_iterator<char>( incoming().body() ),
                              std::istreambuf_iterator<char>() );
            *_type = incoming().type();
            *_received = true;
            _exitLoop->exit();
        }

        virtual void onOutput()
        {
            endSend();
        }

        virtual void onClose()
        {
            *_closed = true;
            _exitLoop->exit();
        }

    private:
        std::string* _message;
        Pt::Http::WebSocketMessage::Type* _type;
        bool* _received;
        bool* _closed;
        Pt::System::EventLoop* _exitLoop;
};

class RecordService : public Pt::Http::WebSocketService
{
    public:
        RecordService(std::string& message,
                      Pt::Http::WebSocketMessage::Type& type,
                      bool& received,
                      bool& closed,
                      Pt::System::EventLoop& loop)
        : _message(&message)
        , _type(&type)
        , _received(&received)
        , _closed(&closed)
        , _loop(&loop)
        , _opened(0)
        , _released(0)
        {}

        std::size_t opened() const
        { return _opened; }

        std::size_t released() const
        { return _released; }

        std::size_t live() const
        { return _opened - _released; }

    protected:
        virtual Pt::Http::WebSocketSession* onGetSession(Pt::System::EventLoop& loop,
                                                         Pt::Http::Stream& stream,
                                                         const Pt::Http::Request&,
                                                         const Pt::Http::Reply& reply)
        {
            ++_opened;
            return new RecordSession(*this, loop, stream, reply,
                                     *_message, *_type, *_received, *_closed, *_loop);
        }

        virtual void onReleaseSession(Pt::Http::WebSocketSession* session)
        {
            ++_released;
            delete session;
        }

    private:
        std::string* _message;
        Pt::Http::WebSocketMessage::Type* _type;
        bool* _received;
        bool* _closed;
        Pt::System::EventLoop* _loop;
        std::size_t _opened;
        std::size_t _released;
};

class CollectSession : public Pt::Http::WebSocketSession
{
    public:
        CollectSession(Pt::Http::WebSocketService& service,
                       Pt::System::EventLoop& loop,
                       Pt::Http::Stream& stream,
                       const Pt::Http::Reply& reply,
                       std::string& message,
                       Pt::Http::WebSocketMessage::Type& type,
                       bool& received,
                       Pt::System::EventLoop& exitLoop)
        : Pt::Http::WebSocketSession(service, loop, stream, reply)
        , _message(&message)
        , _type(&type)
        , _received(&received)
        , _exitLoop(&exitLoop)
        {
            beginReceive();
        }

    protected:
        virtual void onInput()
        {
            Pt::Http::MessageProgress progress = endReceive();
            std::string chunk;
            chunk.assign( std::istreambuf_iterator<char>( incoming().body() ),
                          std::istreambuf_iterator<char>() );
            *_message += chunk;
            *_type = incoming().type();

            if( ! progress.finished() )
            {
                incoming().discard();
                beginReceive();
                return;
            }

            *_received = true;
            _exitLoop->exit();
        }

        virtual void onOutput()
        {
            endSend();
        }

        virtual void onClose()
        {}

    private:
        std::string* _message;
        Pt::Http::WebSocketMessage::Type* _type;
        bool* _received;
        Pt::System::EventLoop* _exitLoop;
};

class CollectService : public Pt::Http::WebSocketService
{
    public:
        CollectService(std::string& message,
                       Pt::Http::WebSocketMessage::Type& type,
                       bool& received,
                       Pt::System::EventLoop& loop)
        : _message(&message)
        , _type(&type)
        , _received(&received)
        , _loop(&loop)
        {}

    protected:
        virtual Pt::Http::WebSocketSession* onGetSession(Pt::System::EventLoop& loop,
                                                         Pt::Http::Stream& stream,
                                                         const Pt::Http::Request&,
                                                         const Pt::Http::Reply& reply)
        {
            return new CollectSession(*this, loop, stream, reply,
                                      *_message, *_type, *_received, *_loop);
        }

        virtual void onReleaseSession(Pt::Http::WebSocketSession* session)
        {
            delete session;
        }

    private:
        std::string* _message;
        Pt::Http::WebSocketMessage::Type* _type;
        bool* _received;
        Pt::System::EventLoop* _loop;
};

class IdleSession : public Pt::Http::WebSocketSession
{
    public:
        IdleSession(Pt::Http::WebSocketService& service,
                    Pt::System::EventLoop& loop,
                    Pt::Http::Stream& stream,
                    const Pt::Http::Reply& reply,
                    Pt::System::EventLoop& exitLoop,
                    bool exitOnAccept)
        : Pt::Http::WebSocketSession(service, loop, stream, reply)
        , _exitLoop(&exitLoop)
        {
            if(exitOnAccept)
                _exitLoop->exit();
        }

    protected:
        virtual void onInput()
        {}

        virtual void onOutput()
        {}

        virtual void onClose()
        {}

    private:
        Pt::System::EventLoop* _exitLoop;
};

class IdleService : public Pt::Http::WebSocketService
{
    public:
        IdleService(Pt::System::EventLoop& loop, bool exitOnAccept = true)
        : _loop(&loop)
        , _exitOnAccept(exitOnAccept)
        , _opened(0)
        , _released(0)
        {}

        std::size_t live() const
        { return _opened - _released; }

        std::size_t opened() const
        { return _opened; }

    protected:
        virtual Pt::Http::WebSocketSession* onGetSession(Pt::System::EventLoop& loop,
                                                         Pt::Http::Stream& stream,
                                                         const Pt::Http::Request&,
                                                         const Pt::Http::Reply& reply)
        {
            ++_opened;
            return new IdleSession(*this, loop, stream, reply, *_loop, _exitOnAccept);
        }

        virtual void onReleaseSession(Pt::Http::WebSocketSession* session)
        {
            ++_released;
            delete session;
        }

    private:
        Pt::System::EventLoop* _loop;
        bool _exitOnAccept;
        std::size_t _opened;
        std::size_t _released;
};

class NullService : public Pt::Http::WebSocketService
{
    protected:
        virtual Pt::Http::WebSocketSession* onGetSession(Pt::System::EventLoop&,
                                                         Pt::Http::Stream&,
                                                         const Pt::Http::Request&,
                                                         const Pt::Http::Reply&)
        {
            return 0;
        }

        virtual void onReleaseSession(Pt::Http::WebSocketSession*)
        {}
};

class ThrowSession : public Pt::Http::WebSocketSession
{
    public:
        ThrowSession(Pt::Http::WebSocketService& service,
                     Pt::System::EventLoop& loop,
                     Pt::Http::Stream& stream,
                     const Pt::Http::Reply& reply)
        : Pt::Http::WebSocketSession(service, loop, stream, reply)
        {
            throw std::runtime_error("session construction failed");
        }

    protected:
        virtual void onInput()
        {}

        virtual void onOutput()
        {}

        virtual void onClose()
        {}
};

class ThrowService : public Pt::Http::WebSocketService
{
    protected:
        virtual Pt::Http::WebSocketSession* onGetSession(Pt::System::EventLoop& loop,
                                                         Pt::Http::Stream& stream,
                                                         const Pt::Http::Request&,
                                                         const Pt::Http::Reply& reply)
        {
            return new ThrowSession(*this, loop, stream, reply);
        }

        virtual void onReleaseSession(Pt::Http::WebSocketSession* session)
        {
            delete session;
        }
};

class PushSession : public Pt::Http::WebSocketSession
{
    public:
        PushSession(Pt::Http::WebSocketService& service,
                    Pt::System::EventLoop& loop,
                    Pt::Http::Stream& stream,
                    const Pt::Http::Reply& reply)
        : Pt::Http::WebSocketSession(service, loop, stream, reply)
        {
            outgoing().setType(Pt::Http::WebSocketMessage::Text);
            outgoing().body() << "feed";
            beginSend();
        }

    protected:
        virtual void onInput()
        {}

        virtual void onOutput()
        {
            endSend();
        }

        virtual void onClose()
        {}
};

typedef Pt::Http::BasicWebSocketService<PushSession> PushService;

class ProtocolSession : public Pt::Http::WebSocketSession
{
    public:
        ProtocolSession(Pt::Http::WebSocketService& service,
                        Pt::System::EventLoop& loop,
                        Pt::Http::Stream& stream,
                        const Pt::Http::Reply& reply,
                        std::string& selected,
                        std::string& offered,
                        std::string& copied,
                        Pt::System::EventLoop& exitLoop)
        : Pt::Http::WebSocketSession(service, loop, stream, reply)
        , _selected(&selected)
        , _exitLoop(&exitLoop)
        {
            *_selected = protocol();
            _exitLoop->exit();
        }

        const std::string& copiedHeader() const
        { return _copied; }

    protected:
        virtual void onInput()
        {}

        virtual void onOutput()
        {}

        virtual void onClose()
        {}

    private:
        std::string* _selected;
        Pt::System::EventLoop* _exitLoop;
        std::string _copied;
};

class ProtocolService : public Pt::Http::WebSocketService
{
    public:
        ProtocolService(std::string& selected,
                        std::string& offered,
                        std::string& copied,
                        Pt::System::EventLoop& loop)
        : _selected(&selected)
        , _offered(&offered)
        , _copied(&copied)
        , _loop(&loop)
        , _opened(0)
        {}

        std::size_t opened() const
        { return _opened; }

    protected:
        virtual Pt::Http::WebSocketSession* onGetSession(Pt::System::EventLoop& loop,
                                                         Pt::Http::Stream& stream,
                                                         const Pt::Http::Request& request,
                                                         const Pt::Http::Reply& reply)
        {
            ++_opened;
            const char* field = request.header().get("Sec-WebSocket-Protocol");
            *_offered = field ? field : "";
            const char* origin = request.header().get("Origin");
            *_copied = origin ? origin : "";
            ProtocolSession* session = new ProtocolSession(*this, loop, stream, reply,
                                                           *_selected, *_offered, *_copied,
                                                           *_loop);
            return session;
        }

        virtual void onReleaseSession(Pt::Http::WebSocketSession* session)
        {
            delete session;
        }

    private:
        std::string* _selected;
        std::string* _offered;
        std::string* _copied;
        Pt::System::EventLoop* _loop;
        std::size_t _opened;
};

class WebSocketTest : public Pt::Unit::TestSuite
                    , public Pt::Connectable
{
    public:
        WebSocketTest()
        : Pt::Unit::TestSuite("Pt::Http::WebSocketTest")
        , _loop(0)
        , _received(false)
        , _declined(false)
        , _closed(false)
        , _status(0)
        , _type(Pt::Http::WebSocketMessage::Unknown)
        , _limitSocket(0)
        , _limitClient(0)
        {
            registerMethod("Text", *this, &WebSocketTest::Text);
            registerMethod("Accepted", *this, &WebSocketTest::Accepted);
            registerMethod("NoScope", *this, &WebSocketTest::NoScope);
            registerMethod("Scope", *this, &WebSocketTest::Scope);
            registerMethod("SecondScope", *this, &WebSocketTest::SecondScope);
            registerMethod("PeerClose", *this, &WebSocketTest::PeerClose);
            registerMethod("Decline", *this, &WebSocketTest::Decline);
            registerMethod("NullSession", *this, &WebSocketTest::NullSession);
            registerMethod("ConstructFails", *this, &WebSocketTest::ConstructFails);
            registerMethod("Push", *this, &WebSocketTest::Push);
            registerMethod("Limit", *this, &WebSocketTest::Limit);
            registerMethod("Binary", *this, &WebSocketTest::Binary);
            registerMethod("Empty", *this, &WebSocketTest::Empty);
            registerMethod("Large", *this, &WebSocketTest::Large);
            registerMethod("CloseHandshake", *this, &WebSocketTest::CloseHandshake);
            registerMethod("BeginTwice", *this, &WebSocketTest::BeginTwice);
            registerMethod("CloseCodes", *this, &WebSocketTest::CloseCodes);
            registerMethod("HandshakeControl", *this, &WebSocketTest::HandshakeControl);
            registerMethod("Protocol", *this, &WebSocketTest::Protocol);
            registerMethod("ProtocolNone", *this, &WebSocketTest::ProtocolNone);
            registerMethod("ProtocolRequired", *this, &WebSocketTest::ProtocolRequired);
            registerMethod("ProtocolRejected", *this, &WebSocketTest::ProtocolRejected);
            registerMethod("ProtocolHeader", *this, &WebSocketTest::ProtocolHeader);
        }

        void setUp()
        {
            _loop = new Pt::System::MainLoop();
            _received = false;
            _declined = false;
            _closed = false;
            _status = 0;
            _limitSocket = 0;
            _limitClient = 0;
            _message.clear();
            _offered.clear();
            _copied.clear();
            _type = Pt::Http::WebSocketMessage::Unknown;

            _exitTimer.setActive(*_loop);
            _exitTimer.timeout() += Pt::slot(*_loop, &Pt::System::EventLoop::exit);
            _exitTimer.start(5000);
        }

        void tearDown()
        {
            _exitTimer.detach();
            delete _loop;
            _loop = 0;
        }

    protected:
        void Text()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8011);

            Pt::Http::Server server(*_loop, ep);
            RecordService service(_message, _type, _received, _closed, *_loop);
            Pt::Http::WebSocketServlet sockets(service);

            Pt::Http::MapUrl mapUrl("/ws", service);
            server.addServlet(mapUrl);

            Pt::Http::Client http(*_loop, ep);
            Pt::Http::WebSocket socket(http);
            socket.connected() += Pt::slot(*this, &WebSocketTest::onConnected);
            socket.beginConnect("/ws");

            _loop->run();

            PT_UNIT_ASSERT(_received);
            PT_UNIT_ASSERT_EQUALS(_message, "hello");
            PT_UNIT_ASSERT_EQUALS(_type, Pt::Http::WebSocketMessage::Text);
        }

        void onConnected(Pt::Http::WebSocket& socket)
        {
            socket.endConnect();
            socket.outgoing().setType(Pt::Http::WebSocketMessage::Text);
            socket.outgoing().body() << "hello";
            socket.beginSend();
        }

    protected:
        void Accepted()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8012);

            Pt::Http::Server server(*_loop, ep);
            IdleService service(*_loop);
            Pt::Http::WebSocketServlet sockets(service);

            Pt::Http::MapUrl mapUrl("/ws", service);
            server.addServlet(mapUrl);

            Pt::Http::Client http(*_loop, ep);
            Pt::Http::WebSocket socket(http);
            socket.connected() += Pt::slot(*this, &WebSocketTest::onConnectedIdle);
            socket.beginConnect("/ws");

            _loop->run();

            PT_UNIT_ASSERT_EQUALS(service.size(), static_cast<std::size_t>(1));
        }

        void onConnectedIdle(Pt::Http::WebSocket& socket)
        {
            socket.endConnect();
        }

    protected:
        void NoScope()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8031);

            Pt::Http::Server server(*_loop, ep);
            IdleService service(*_loop, false);

            Pt::Http::MapUrl mapUrl("/ws", service);
            server.addServlet(mapUrl);

            Pt::Http::Client http(*_loop, ep);
            Pt::Http::WebSocket socket(http);
            socket.connected() += Pt::slot(*this, &WebSocketTest::onDeclineConnected);
            socket.beginConnect("/ws");

            _loop->run();

            PT_UNIT_ASSERT(_declined);
            PT_UNIT_ASSERT_EQUALS(service.opened(), static_cast<std::size_t>(0));
        }

    protected:
        void Scope()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8032);

            Pt::Http::Server server(*_loop, ep);
            IdleService service(*_loop);
            Pt::Http::Client http(*_loop, ep);
            Pt::Http::WebSocket socket(http);

            {
                Pt::Http::WebSocketServlet sockets(service);
                Pt::Http::MapUrl mapUrl("/ws", service);
                server.addServlet(mapUrl);

                socket.connected() += Pt::slot(*this, &WebSocketTest::onConnectedIdle);
                socket.beginConnect("/ws");

                _loop->run();

                PT_UNIT_ASSERT_EQUALS(service.size(), static_cast<std::size_t>(1));
                PT_UNIT_ASSERT_EQUALS(service.live(), static_cast<std::size_t>(1));
            }

            PT_UNIT_ASSERT_EQUALS(service.size(), static_cast<std::size_t>(0));
            PT_UNIT_ASSERT_EQUALS(service.live(), static_cast<std::size_t>(0));
        }

    protected:
        void SecondScope()
        {
            IdleService service(*_loop, false);
            Pt::Http::WebSocketServlet sockets(service);

            PT_UNIT_ASSERT_THROW(createScope(service), std::logic_error);
        }

        void createScope(Pt::Http::WebSocketService& service)
        {
            Pt::Http::WebSocketServlet scope(service);
        }

    protected:
        void PeerClose()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8013);

            Pt::Http::Server server(*_loop, ep);
            RecordService service(_message, _type, _received, _closed, *_loop);
            Pt::Http::WebSocketServlet sockets(service);

            Pt::Http::MapUrl mapUrl("/ws", service);
            server.addServlet(mapUrl);

            Pt::Http::Client http(*_loop, ep);
            Pt::Http::WebSocket socket(http);
            socket.connected() += Pt::slot(*this, &WebSocketTest::onConnectedClose);
            socket.beginConnect("/ws");

            _loop->run();

            PT_UNIT_ASSERT(_closed);
            PT_UNIT_ASSERT_EQUALS(service.size(), static_cast<std::size_t>(0));
        }

        void onConnectedClose(Pt::Http::WebSocket& socket)
        {
            socket.endConnect();
            socket.close();
        }

    protected:
        void Decline()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8014);

            Pt::Http::Server server(*_loop, ep);
            DeclineService service;

            Pt::Http::MapUrl mapUrl("/ws", service);
            server.addServlet(mapUrl);

            Pt::Http::Client client(*_loop, ep);
            Pt::Http::WebSocket socket(client);
            socket.connected() += Pt::slot(*this, &WebSocketTest::onDeclineConnected);
            socket.beginConnect("/ws");

            _loop->run();

            PT_UNIT_ASSERT(_declined);
        }

        void onDeclineConnected(Pt::Http::WebSocket& socket)
        {
            socket.endConnect();
            socket.closed() += Pt::slot(*this, &WebSocketTest::onDeclineClosed);
            socket.beginReceive();
        }

        void onDeclineClosed(Pt::Http::WebSocket&)
        {
            _declined = true;
            _loop->exit();
        }

    protected:
        void NullSession()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8015);

            Pt::Http::Server server(*_loop, ep);
            NullService service;
            Pt::Http::WebSocketServlet sockets(service);

            Pt::Http::MapUrl mapUrl("/ws", service);
            server.addServlet(mapUrl);

            Pt::Http::Client client(*_loop, ep);
            Pt::Http::WebSocket socket(client);
            socket.connected() += Pt::slot(*this, &WebSocketTest::onDeclineConnected);
            socket.beginConnect("/ws");

            _loop->run();

            PT_UNIT_ASSERT(_declined);
        }

    protected:
        void ConstructFails()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8016);

            Pt::Http::Server server(*_loop, ep);
            ThrowService service;
            Pt::Http::WebSocketServlet sockets(service);

            Pt::Http::MapUrl mapUrl("/ws", service);
            server.addServlet(mapUrl);

            Pt::Http::Client client(*_loop, ep);
            Pt::Http::WebSocket socket(client);
            socket.connected() += Pt::slot(*this, &WebSocketTest::onDeclineConnected);
            socket.beginConnect("/ws");

            _loop->run();

            PT_UNIT_ASSERT(_declined);
        }

    protected:
        void Push()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8017);

            Pt::Http::Server server(*_loop, ep);
            PushService service;
            Pt::Http::WebSocketServlet sockets(service);

            Pt::Http::MapUrl mapUrl("/ws", service);
            server.addServlet(mapUrl);

            Pt::Http::Client client(*_loop, ep);
            Pt::Http::WebSocket socket(client);
            socket.connected() += Pt::slot(*this, &WebSocketTest::onPushConnected);
            socket.inputReady() += Pt::slot(*this, &WebSocketTest::onPushInput);
            socket.beginConnect("/ws");

            _loop->run();

            PT_UNIT_ASSERT(_received);
            PT_UNIT_ASSERT_EQUALS(_message, "feed");
            PT_UNIT_ASSERT_EQUALS(_type, Pt::Http::WebSocketMessage::Text);
        }

        void onPushConnected(Pt::Http::WebSocket& socket)
        {
            socket.endConnect();
            socket.beginReceive();
        }

        void onPushInput(Pt::Http::WebSocket& socket)
        {
            socket.endReceive();
            _message.assign( std::istreambuf_iterator<char>( socket.incoming().body() ),
                             std::istreambuf_iterator<char>() );
            _type = socket.incoming().type();
            _received = true;
            _loop->exit();
        }

    protected:
        void Limit()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8018);

            Pt::Http::Server server(*_loop, ep);
            IdleService service(*_loop, false);
            Pt::Http::WebSocketServlet sockets(service);
            service.setMaxSockets(1);

            Pt::Http::MapUrl mapUrl("/ws", service);
            server.addServlet(mapUrl);

            Pt::Http::Client first(*_loop, ep);
            Pt::Http::WebSocket socket(first);

            Pt::Http::Client second(*_loop, ep);
            Pt::Http::WebSocket rejected(second);
            _limitSocket = &rejected;
            _limitClient = &second;

            socket.connected() += Pt::slot(*this, &WebSocketTest::onLimitConnected);
            rejected.connected() += Pt::slot(*this, &WebSocketTest::onLimitRejected);
            socket.beginConnect("/ws");

            _loop->run();

            PT_UNIT_ASSERT_EQUALS(_status, 503u);
            PT_UNIT_ASSERT_EQUALS(service.size(), static_cast<std::size_t>(1));
            _limitSocket = 0;
            _limitClient = 0;
        }

        void onLimitConnected(Pt::Http::WebSocket& socket)
        {
            socket.endConnect();
            _limitSocket->beginConnect("/ws");
        }

        void onLimitRejected(Pt::Http::WebSocket& socket)
        {
            PT_UNIT_ASSERT_THROW(socket.endConnect(), std::exception);

            if(_limitClient)
                _status = _limitClient->reply().statusCode();

            _loop->exit();
        }

    protected:
        void Binary()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8019);

            Pt::Http::Server server(*_loop, ep);
            RecordService service(_message, _type, _received, _closed, *_loop);
            Pt::Http::WebSocketServlet sockets(service);

            Pt::Http::MapUrl mapUrl("/ws", service);
            server.addServlet(mapUrl);

            Pt::Http::Client http(*_loop, ep);
            Pt::Http::WebSocket socket(http);
            socket.connected() += Pt::slot(*this, &WebSocketTest::onConnectedBinary);
            socket.beginConnect("/ws");

            _loop->run();

            PT_UNIT_ASSERT(_received);
            PT_UNIT_ASSERT_EQUALS(_message, std::string("\x00\x01\x02", 3));
            PT_UNIT_ASSERT_EQUALS(_type, Pt::Http::WebSocketMessage::Binary);
        }

        void onConnectedBinary(Pt::Http::WebSocket& socket)
        {
            socket.endConnect();
            socket.outgoing().setType(Pt::Http::WebSocketMessage::Binary);
            socket.outgoing().body().write("\x00\x01\x02", 3);
            socket.beginSend();
        }

    protected:
        void Empty()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8020);

            Pt::Http::Server server(*_loop, ep);
            RecordService service(_message, _type, _received, _closed, *_loop);
            Pt::Http::WebSocketServlet sockets(service);

            Pt::Http::MapUrl mapUrl("/ws", service);
            server.addServlet(mapUrl);

            Pt::Http::Client http(*_loop, ep);
            Pt::Http::WebSocket socket(http);
            socket.connected() += Pt::slot(*this, &WebSocketTest::onConnectedEmpty);
            socket.beginConnect("/ws");

            _loop->run();

            PT_UNIT_ASSERT(_received);
            PT_UNIT_ASSERT(_message.empty());
            PT_UNIT_ASSERT_EQUALS(_type, Pt::Http::WebSocketMessage::Text);
        }

        void onConnectedEmpty(Pt::Http::WebSocket& socket)
        {
            socket.endConnect();
            socket.outgoing().setType(Pt::Http::WebSocketMessage::Text);
            socket.beginSend();
        }

    protected:
        void Large()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8021);

            Pt::Http::Server server(*_loop, ep);
            CollectService service(_message, _type, _received, *_loop);
            Pt::Http::WebSocketServlet sockets(service);

            Pt::Http::MapUrl mapUrl("/ws", service);
            server.addServlet(mapUrl);

            Pt::Http::Client http(*_loop, ep);
            Pt::Http::WebSocket socket(http);
            _limitSocket = &socket;
            socket.connected() += Pt::slot(*this, &WebSocketTest::onConnectedLarge);
            socket.outputReady() += Pt::slot(*this, &WebSocketTest::onLargeOutput);
            socket.beginConnect("/ws");

            _loop->run();

            PT_UNIT_ASSERT(_received);
            PT_UNIT_ASSERT_EQUALS(_message.size(), static_cast<std::size_t>(5000));
            PT_UNIT_ASSERT_EQUALS(_type, Pt::Http::WebSocketMessage::Text);
            _limitSocket = 0;
        }

        void onConnectedLarge(Pt::Http::WebSocket& socket)
        {
            socket.endConnect();
            socket.outgoing().setType(Pt::Http::WebSocketMessage::Text);
            socket.outgoing().body() << std::string(5000, 'a');
            socket.beginSend();
        }

        void onLargeOutput(Pt::Http::WebSocket& socket)
        {
            Pt::Http::MessageProgress progress = socket.endSend();
            if( ! progress.finished() )
                socket.beginSend();
        }

    protected:
        void CloseHandshake()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8022);

            Pt::Http::Server server(*_loop, ep);
            RecordService service(_message, _type, _received, _closed, *_loop);
            Pt::Http::WebSocketServlet sockets(service);

            Pt::Http::MapUrl mapUrl("/ws", service);
            server.addServlet(mapUrl);

            Pt::Http::Client http(*_loop, ep);
            Pt::Http::WebSocket socket(http);
            socket.connected() += Pt::slot(*this, &WebSocketTest::onConnectedCloseCode);
            socket.beginConnect("/ws");

            _loop->run();

            PT_UNIT_ASSERT(_closed);
            PT_UNIT_ASSERT_EQUALS(service.size(), static_cast<std::size_t>(0));
        }

        void onConnectedCloseCode(Pt::Http::WebSocket& socket)
        {
            socket.endConnect();
            socket.close(1000, "bye");
        }

    protected:
        void BeginTwice()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8023);

            Pt::Http::Server server(*_loop, ep);
            IdleService service(*_loop, false);
            Pt::Http::WebSocketServlet sockets(service);

            Pt::Http::MapUrl mapUrl("/ws", service);
            server.addServlet(mapUrl);

            Pt::Http::Client http(*_loop, ep);
            Pt::Http::WebSocket socket(http);
            socket.connected() += Pt::slot(*this, &WebSocketTest::onConnectedBeginTwice);
            socket.beginConnect("/ws");

            _loop->run();
        }

        void onConnectedBeginTwice(Pt::Http::WebSocket& socket)
        {
            socket.endConnect();
            socket.beginReceive();
            PT_UNIT_ASSERT_THROW(socket.beginReceive(), std::logic_error);
            _loop->exit();
        }

    protected:
        void CloseCodes()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8024);

            Pt::Http::Server server(*_loop, ep);
            IdleService service(*_loop, false);
            Pt::Http::WebSocketServlet sockets(service);

            Pt::Http::MapUrl mapUrl("/ws", service);
            server.addServlet(mapUrl);

            Pt::Http::Client http(*_loop, ep);
            Pt::Http::WebSocket socket(http);
            socket.connected() += Pt::slot(*this, &WebSocketTest::onConnectedCloseCodes);
            socket.beginConnect("/ws");

            _loop->run();
        }

        void onConnectedCloseCodes(Pt::Http::WebSocket& socket)
        {
            socket.endConnect();
            socket.outgoing().setType(Pt::Http::WebSocketMessage::Text);
            socket.outgoing().body() << "x";
            socket.beginSend();
            PT_UNIT_ASSERT_THROW(socket.beginSend(), std::logic_error);
            PT_UNIT_ASSERT_THROW(socket.close(1005), std::invalid_argument);
            PT_UNIT_ASSERT_THROW(socket.close(1006), std::invalid_argument);
            PT_UNIT_ASSERT_THROW(socket.close(1015), std::invalid_argument);
            socket.close();
            PT_UNIT_ASSERT_THROW(socket.close(), std::logic_error);
            _loop->exit();
        }

    protected:
        void Protocol()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8026);

            Pt::Http::Server server(*_loop, ep);
            ProtocolService service(_message, _offered, _copied, *_loop);
            service.addProtocol("superchat");
            service.addProtocol("chat");
            Pt::Http::WebSocketServlet sockets(service);

            Pt::Http::MapUrl mapUrl("/ws", service);
            server.addServlet(mapUrl);

            Pt::Http::Client http(*_loop, ep);
            Pt::Http::WebSocket socket(http);
            socket.addProtocol("chat");
            socket.addProtocol("superchat");
            socket.connected() += Pt::slot(*this, &WebSocketTest::onConnectedProtocol);
            socket.beginConnect("/ws");

            _loop->run();

            PT_UNIT_ASSERT_EQUALS(socket.protocol(), "chat");
            PT_UNIT_ASSERT_EQUALS(_message, "chat");
            PT_UNIT_ASSERT_EQUALS(_offered, "chat, superchat");
            PT_UNIT_ASSERT_EQUALS(service.opened(), static_cast<std::size_t>(1));
        }

        void onConnectedProtocol(Pt::Http::WebSocket& socket)
        {
            socket.endConnect();
        }

    protected:
        void ProtocolNone()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8027);

            Pt::Http::Server server(*_loop, ep);
            ProtocolService service(_message, _offered, _copied, *_loop);
            Pt::Http::WebSocketServlet sockets(service);

            Pt::Http::MapUrl mapUrl("/ws", service);
            server.addServlet(mapUrl);

            Pt::Http::Client http(*_loop, ep);
            Pt::Http::WebSocket socket(http);
            socket.addProtocol("chat");
            socket.connected() += Pt::slot(*this, &WebSocketTest::onConnectedProtocol);
            socket.beginConnect("/ws");

            _loop->run();

            PT_UNIT_ASSERT(socket.protocol().empty());
            PT_UNIT_ASSERT(_message.empty());
            PT_UNIT_ASSERT_EQUALS(service.opened(), static_cast<std::size_t>(1));
        }

    protected:
        void ProtocolRequired()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8028);

            Pt::Http::Server server(*_loop, ep);
            ProtocolService service(_message, _offered, _copied, *_loop);
            service.addProtocol("chat");
            Pt::Http::WebSocketServlet sockets(service);

            Pt::Http::MapUrl mapUrl("/ws", service);
            server.addServlet(mapUrl);

            Pt::Http::Client http(*_loop, ep);
            Pt::Http::WebSocket socket(http);
            socket.connected() += Pt::slot(*this, &WebSocketTest::onConnectedProtocolFailed);
            socket.beginConnect("/ws");

            _loop->run();

            PT_UNIT_ASSERT(_declined);
            PT_UNIT_ASSERT_EQUALS(service.opened(), static_cast<std::size_t>(0));
        }

    protected:
        void ProtocolRejected()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8029);

            Pt::Http::Server server(*_loop, ep);
            ProtocolService service(_message, _offered, _copied, *_loop);
            service.addProtocol("chat");
            Pt::Http::WebSocketServlet sockets(service);

            Pt::Http::MapUrl mapUrl("/ws", service);
            server.addServlet(mapUrl);

            Pt::Http::Client http(*_loop, ep);
            Pt::Http::WebSocket socket(http);
            socket.addProtocol("other");
            socket.connected() += Pt::slot(*this, &WebSocketTest::onConnectedProtocolFailed);
            socket.beginConnect("/ws");

            _loop->run();

            PT_UNIT_ASSERT(_declined);
            PT_UNIT_ASSERT_EQUALS(service.opened(), static_cast<std::size_t>(0));
        }

        void onConnectedProtocolFailed(Pt::Http::WebSocket& socket)
        {
            PT_UNIT_ASSERT_THROW(socket.endConnect(), std::exception);
            _declined = true;
            _loop->exit();
        }

    protected:
        void ProtocolHeader()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8030);

            Pt::Http::Server server(*_loop, ep);
            ProtocolService service(_message, _offered, _copied, *_loop);
            Pt::Http::WebSocketServlet sockets(service);

            Pt::Http::MapUrl mapUrl("/ws", service);
            server.addServlet(mapUrl);

            Pt::Http::Client http(*_loop, ep);
            Pt::Http::WebSocket socket(http);
            socket.connected() += Pt::slot(*this, &WebSocketTest::onConnectedProtocol);
            socket.beginConnect("/ws", "https://example.test");

            _loop->run();

            PT_UNIT_ASSERT_EQUALS(_copied, "https://example.test");
            PT_UNIT_ASSERT_EQUALS(service.opened(), static_cast<std::size_t>(1));
        }

    protected:
        void HandshakeControl()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8025);
            Pt::Http::Client http(*_loop, ep);
            Pt::Http::WebSocket socket(http);
            PT_UNIT_ASSERT_THROW(socket.ping(), std::logic_error);
            PT_UNIT_ASSERT_THROW(socket.close(), std::logic_error);
        }

    private:
        Pt::System::MainLoop* _loop;
        Pt::System::Timer _exitTimer;
        std::string _message;
        std::string _offered;
        std::string _copied;
        bool _received;
        bool _declined;
        bool _closed;
        unsigned _status;
        Pt::Http::WebSocketMessage::Type _type;
        Pt::Http::WebSocket* _limitSocket;
        Pt::Http::Client* _limitClient;
};

Pt::Unit::RegisterTest<WebSocketTest> register_HttpWebSocketTest;
