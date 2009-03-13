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

#ifndef Pt_Net_HttpParser_h
#define Pt_Net_HttpParser_h

#include <Pt/Net/Api.h>
#include <Pt/Net/TcpServer.h>
#include <Pt/Net/TcpSocket.h>
#include <Pt/System/IOStream.h>
#include <Pt/System/Timer.h>
#include <Pt/Connectable.h>
#include <string>
#include <cstddef>
#include <map>

namespace Pt {

namespace Net {

class HttpServerRequest
{
};

class HttpRequestParser
{
    public:
        class Event
        {
            public:
                virtual void onUrl(const std::string& url) = 0;
                virtual void onUrlParam(const std::string& url) = 0;
                virtual void onHttpVersion(unsigned major, unsigned minor) = 0;
                virtual void onKey(const std::string& key) = 0;
                virtual void onValue(const std::string& value) = 0;
                virtual void onHeader(const std::string& key, const std::string& value) = 0;
                virtual void onEnd() = 0;
        };

        std::size_t advance(std::istream& is);
        bool parse(char ch);
};

} // namespace Net

} // namespace Pt

#endif
