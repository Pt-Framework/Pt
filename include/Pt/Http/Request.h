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

#ifndef Pt_Http_Request_h
#define Pt_Http_Request_h

#include <Pt/Http/Api.h>
#include <Pt/Http/RequestHeader.h>
#include <Pt/Signal.h>
#include <string>
#include <iostream>
#include <cassert>

namespace Pt {

namespace Http {

class Connection;

class PT_HTTP_API Request
{
    friend class Connection;

    public:
        explicit Request( const std::string& url = std::string() )
        : _header(url)
        , _buf()
        , _body(&_buf)
        { }

        RequestHeader& header()
        { return _header; }

        const RequestHeader& header() const
        { return _header; }

        void clear()
        {
            _header.clear();
            _body.clear();
            _buf.reset();
        }

        void clearBody()
        {
            _buf.reset();
        }

        const std::string& url() const
        { return _header.url(); }

        void url(const std::string& u)
        { _header.url(u); }

        const std::string& method() const
        { return _header.method(); }

        void method(const std::string& m)
        { _header.method(m); }

        const std::string& qparams() const
        { return _header.qparams(); }

        void qparams(const std::string& q)
        { _header.qparams(q); }

        const char* data() const
        { return _buf.data(); }

        std::iostream& body()
        { return _body; }

        std::size_t size() const
        { return _buf.size(); }

        Signal<Request&>& inputReceived()
        {
            return _inputReceived;
        }

    protected:
        void onInput(std::streambuf& sb)
        {
            _body.rdbuf(&sb);
            _inputReceived.send(*this);
        }

    private:
        RequestHeader _header;
        MessageBuffer _buf;
        std::iostream _body;
        Signal<Request&> _inputReceived;
};

} // namespace Http

} // namespace Pt

#endif
