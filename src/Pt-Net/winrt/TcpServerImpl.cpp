/*
 * Copyright (C) 2009 Marc Boris Duerner, Tommi Maekitalo
 *                    Laurentiu-Gheorghe Crisan
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

#include "TcpServerImpl.h"
#include "AddrInfoImpl.h"
#include <Pt/Net/AddrInfo.h>
#include <Pt/Net/AddressInUse.h>
#include <Pt/Net/TcpServer.h>
#include <Pt/System/Logger.h>
#include <Pt/System/EventLoop.h>
#include <Pt/System/SystemError.h>

#include <cassert>
#include <cstring>
#include <limits>

log_define("Pt.System.TcpServer");

namespace Pt {

namespace Net {

TcpServerImpl::TcpServerImpl(TcpServer& server)
: _server(server)
, _loop(0)
, _timeout(Pt::System::EventLoop::WaitInfinite)
, _bound(false)
{
    _listener = ref new StreamSocketListener();

    _listener->ConnectionReceived += ref new TypedEventHandler<StreamSocketListener^, 
                                                               StreamSocketListenerConnectionReceivedEventArgs^>
    (
        [&](StreamSocketListenerConnectionReceivedEventArgs^)
        {
            //this->_loop->setReady();
            //this->_loop->wake();
        }
    );
}


TcpServerImpl::~TcpServerImpl()
{
    delete _listener;
}


void TcpServerImpl::close()
{
    _listener.Close();
    _bound = false;
}


void TcpServerImpl::cancel(System::EventLoop& loop)
{
    _loop = 0;

    delete _bindOp;
    _bindOp = 0;
}


void TcpServerImpl::listen(const std::string& ipaddr, unsigned short int port,
                           const TcpServer::Options& options)
{
    AddrInfo ai(ipaddr, port, true);
    listen(ai, options);
}


void TcpServerImpl::listen(const AddrInfo& ai, const TcpServer::Options& options)
{
    log_debug("listen on " << _ai.host() << ":" << _ai.port());

    _ai = ai;
    _options = options;
}


void TcpServerImpl::beginAccept(System::EventLoop& loop)
{
    log_debug("beginAccept");
    
    _loop = &loop;

    if(_bound)
        return;

    const std::string& host = _ai.host();
	  std::wstring whost(host.begin(), host.end());
	  String^ shost = ref new String(whost.c_str());

    HostName^ hostName = ref new HostName(shost);

    std::wostringstream wss;
    wss << _ai.port();
	  std::wstring wport = wss.str();
	  String^ serviceName = ref new String( wport.c_str() );

    _bindOp = BindEndpointAsync(hostName, serviceName);
    _bindOp->Completed = ref new AsyncActionCompletedHandler
    (
        [&] (IAsyncAction^ asyncInfo) 
        {
            //loop.setReady(_server);
            //loop.wake();
        }
    );

    _bound = true;
}


StreamSocket^ TcpServerImpl::accept()
{
    _loop = 0;
}


bool TcpServerImpl::run()
{
    log_debug("TcpServerImpl::avail");
    
    if( _bindOp && _bindOp->Status == AsyncStatus::Completed )
        return false;

    return false;
}

} // namespace Net

} // namespace Pt
