/*
 * Copyright (C) 2009 by Marc Boris Duerner, Tommi Maekitalo
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

#ifndef Pt_Http_Responder_h
#define Pt_Http_Responder_h

#include <Pt/Http/Api.h>
#include <Pt/Http/Service.h>
#include <Pt/Signal.h>
#include <Pt/System/EventLoop.h>
#include <iosfwd>
#include <exception>

namespace Pt {

namespace Http {

class Request;
class Reply;

class PT_HTTP_API Responder
{
    public:
        explicit Responder(Service& service)
            : _service(service)
        { }

        virtual ~Responder() { }

        virtual void beginRequest(std::istream& in, Request& request);
        virtual std::size_t readBody(std::istream&);
        virtual void reply(std::ostream&, Request& request, Reply& reply) = 0;
        virtual void replyError(std::ostream&, Request& request, Reply& reply, const std::exception& ex);

        void release()     
        { _service.doReleaseResponder(this); }

        // TODO: pass object that contains loop and notifier, so we do not need member variables
        virtual void beginReply(System::EventLoop& loop, std::ostream& os, Request& request, Reply& reply)
        {
            _replyFinished();
        }

        virtual void endReply(std::ostream& os, Http::Request& request, Http::Reply& reply) 
        { 
            this->reply(os, request, reply);
        }

        Signal<>& replyFinished()
        { return _replyFinished; }

    protected:
        Signal<> _replyFinished;

    private:
        Service& _service;
};

} // namespace Http

} // namespace Pt

#endif
