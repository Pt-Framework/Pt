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
#include <Pt/Http/ReplyHeader.h>
#include <string>
#include <sstream>

namespace Pt {

namespace Http {

class Connection;

class PT_HTTP_API Reply
{
        ReplyHeader _header;
        MessageBuffer _buf;
        std::ostream _body;
        Http::Connection* _conn;
        bool _advanced;
        bool _finished;

    public:
        Reply()
        : _conn(0)
        , _body(&_buf)
        , _advanced(false)
        , _finished(false)
        { }

        void init(Http::Connection& conn)
        {
            _conn = &conn;
            _advanced = false;
            _finished = false;
        }

        bool finished() const
        { return _finished; }

        void advance();

        void finish();

        ReplyHeader& header()
        { return _header; }

        const ReplyHeader& header() const
        { return _header; }

        void clear()
        {
            _header.clear();
            _body.clear();
            _buf.reset();
            _advanced = false;
            _finished = false;
        }

        const MessageBuffer& buffer()
        { return _buf; }

        std::ostream& body()
        { return _body; }
};

} // namespace Http

} // namespace Pt

#endif
