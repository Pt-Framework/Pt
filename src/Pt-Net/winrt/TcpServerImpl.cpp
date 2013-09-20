/*
 * Copyright (C) 2013 Marc Boris Duerner
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
#include <Pt/Net/AddressInUse.h>
#include <Pt/System/Logger.h>
#include <Pt/System/IOError.h>
#include <Pt/System/SystemError.h>
#include <cassert>

log_define("Pt.System.TcpServer");

namespace Pt {

namespace Net {

TcpServerImpl::TcpServerImpl(TcpServer& server)
: _server(server)
, _loop(0)
, _listener(0)
, _bindOp(0)
{
    _listener = ref new StreamSocketListener();

    typedef TypedEventHandler<StreamSocketListener^, 
                              StreamSocketListenerConnectionReceivedEventArgs^> ConnectionReceivedHandler;

    _listener->ConnectionReceived += ref new ConnectionReceivedHandler(*this, &TcpServerImpl::ConnectionReceived);
}


TcpServerImpl::~TcpServerImpl()
{
    delete _listener;
}


void TcpServerImpl::close()
{
    delete _bindOp;
    _bindOp = 0;

    _listener.Close();
}


void TcpServerImpl::cancel(System::EventLoop& loop)
{
    MutexLock lock(_mtx);
    _loop = 0;
    lock.unlock();
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

    const std::string& host = _ai.host();
	  std::wstring whost(host.begin(), host.end());
	  String^ shost = ref new String(whost.c_str());

    std::wostringstream wss;
    wss << _ai.port();
	  std::wstring wport = wss.str();
	  String^ serviceName = ref new String( wport.c_str() );

    if( shost->IsEmpty() )
        _bindOp = BindServiceNameAsync(serviceName);
    else
        _bindOp = BindEndpointAsync(ref new HostName(shost), serviceName);
    
    _bindOp->Completed = ref new AsyncActionCompletedHandler
    (
        [&] (IAsyncAction^ asyncAction) 
        {
            try
            {
                asyncAction->GetResults();
            }
            catch(...)
            {
                std::cerr << "BindEndpointAsync failed." << std::endl;
            }
        }
    );
}


void TcpServerImpl::onConnectionReceived(StreamSocketListener^ listener, 
                                         StreamSocketListenerConnectionReceivedEventArgs^ args)
{
    MutexLock lock(_mtx);

    // I really hope that the OS stops emitting ConnectionReceived events
    // at some point, so that apps do not get flooded in case of high load.

    _backlog.push_back( args->Socket() );

    if(_loop)
    {
        _loop->setReady(_server);
        _loop->wake();
    }
}


void TcpServerImpl::beginAccept(System::EventLoop& loop)
{
    log_debug("beginAccept");
    
    MutexLock lock(_mtx);

    if( ! _backlog.empty() )
    {
        loop.setReady(_server);
        loop.wake();
    }
    else
    {
        _loop = &loop;
    }
}


bool TcpServerImpl::run()
{
    log_debug("TcpServerImpl::avail");
    
    MutexLock lock(_mtx);
    return ! _backlog.empty();
}


StreamSocket^ TcpServerImpl::accept()
{
    MutexLock lock(_mtx);
    _loop = 0;

    assert( ! _backlog.empty() );

    if( ! _backlog.empty() )
        throw IOError("accept backlog empty");

    StreamSocket^ socket = _backlog.back();
    _backlog.pop_back();
    return socket;
}

} // namespace Net

} // namespace Pt
