/* Copyright (C) 2015 by Laurentiu-Gheorghe Crisan
   Copyright (C) 2026 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#ifndef PT_HTTP_WEBSOCKETRESPONDER_H
#define PT_HTTP_WEBSOCKETRESPONDER_H

#include <Pt/Http/Responder.h>
#include <Pt/Http/Reply.h>

namespace Pt {
namespace Http {

class WebSocketService;

// Answers the WebSocket handshake. Every upgrade request takes a place from
// the session limit of the service first and holds it until the responder is
// destroyed.
class WebSocketResponder : public Pt::Http::Responder
{
    public:
        explicit WebSocketResponder(Pt::Http::WebSocketService& s);

        ~WebSocketResponder();

    protected:
        virtual void onBeginRequest(Pt::Http::Request& request, Pt::Http::Reply& reply, Pt::System::EventLoop& loop);

        virtual void onReadRequest(Pt::Http::Request& request, Pt::Http::Reply& reply, Pt::System::EventLoop& loop);

        virtual void onBeginReply(const Pt::Http::Request& request, Pt::Http::Reply& reply, Pt::System::EventLoop& loop);

        virtual void onWriteReply(const Pt::Http::Request& request, Pt::Http::Reply& reply, Pt::System::EventLoop& loop);

    private:
        bool _reserved;
};

}}

#endif
