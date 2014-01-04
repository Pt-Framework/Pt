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

#ifndef Pt_Http_Reply_h
#define Pt_Http_Reply_h

#include <Pt/Http/Api.h>
#include <Pt/Http/Message.h>
#include <Pt/Signal.h>
#include <string>

namespace Pt {

namespace Http {

class PT_HTTP_API Reply : public Message
{
    friend class Connection;

    public:
        enum StatusCode
        {
            Continue = 100,
            OK = 200,
            MultipleChoices = 300,
            BadRequest = 400,
            Unauthorized = 401,
            RequestEntityTooLarge = 413,
            InternalServerError = 500
        };

    public:
        explicit Reply(Http::Connection& conn)
        : Message(conn)
        , _statusCode(200)
        , _statusText("OK")
        { }
        
        void setStatus(unsigned code, const std::string& txt)
        {
            _statusCode = code;
            _statusText = txt;
        }

        void setStatus(unsigned code, const char* txt)
        {
            _statusCode = code;
            _statusText = txt;
        }

        unsigned statusCode() const
        { return _statusCode; }

        const std::string& statusText() const
        { return _statusText; }

        void receive();

        void beginReceive();

        MessageProgress endReceive();

        void beginSend(bool finish = true);

        MessageProgress endSend();

        Signal<Reply&>& inputReceived()
        { return _inputReceived; }

        Signal<Reply&>& outputSent()
        { return _outputSent; }

        void clear();

    protected:
        void onInput()
        { _inputReceived.send(*this); }

        void onOutput()
        { _outputSent.send(*this); }

    private:
        unsigned _statusCode;
        std::string _statusText;
        Signal<Reply&> _inputReceived;
        Signal<Reply&> _outputSent;
};

} // namespace Http

} // namespace Pt

#endif // Pt_Http_Reply_h
