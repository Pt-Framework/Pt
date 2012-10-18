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
#include <iostream>

namespace Pt {

namespace Http {

class Connection;

class PT_HTTP_API Reply
{
    friend class Connection;

    public:
        Reply(Http::Connection& conn)
        : _conn(&conn)
        , _isReceiving(false)
        , _isSending(false)
        , _finished(false)
        , _statusCode(200)
        , _statusText("OK")
        { }
        
        Connection& connection()
        { return *_conn; }
        
        void setStatus(unsigned c, const std::string& t)
        {
            _statusCode = c;
            _statusText = t;
        }

        unsigned statusCode() const
        { return _statusCode; }

        const std::string& statusText() const
        { return _statusText; }

        void receive();

        void beginReceive();

        MessageProgress endReceive();

        bool isReceiving() const
        { return _isReceiving; }

        void beginSend(bool finish = true);

        MessageProgress endSend();

        bool isSending() const
        { return _isSending; }

        bool isFinished() const
        { return _finished; }

        Signal<Reply&>& inputReceived()
        { return _inputReceived; }

        Signal<Reply&>& outputSent()
        { return _outputSent; }

        MessageHeader& header()
        { return _header; }

        const MessageHeader& header() const
        { return _header; }

        MessageBody& body()
        { return _body; }

        void clear();

    protected:
        void onInput()
        { _inputReceived.send(*this); }

        void onOutput()
        { _outputSent.send(*this); }

    private:
        Http::Connection* _conn;
        bool _isReceiving; // TODO: move to Connection
        bool _isSending; // TODO: move to Connection
        bool _finished;
        unsigned _statusCode;
        std::string _statusText;
        MessageHeader _header;
        MessageBody _body;
        Signal<Reply&> _inputReceived;
        Signal<Reply&> _outputSent;
};

} // namespace Http

} // namespace Pt

#endif
