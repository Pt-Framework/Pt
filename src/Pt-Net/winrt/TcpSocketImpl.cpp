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

#include "TcpSocketImpl.h"
#include "TcpServerImpl.h"
#include "EndpointImpl.h"
#include "Pt/Net/AddressInUse.h"
#include <Pt/Net/TcpSocket.h>
#include <Pt/System/SystemError.h>
#include <Pt/System/IOError.h>
#include <Pt/System/Logger.h>
#include <algorithm>
#include <cassert>

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Storage::Streams;
using namespace Windows::Networking;
using namespace Windows::Networking::Sockets;

log_define("Pt.Net.TcpSocket");

namespace Pt {

namespace Net {

TcpSocketImpl::TcpSocketImpl(TcpSocket& socket)
: _device(socket)
, _timeout(System::EventLoop::WaitInfinite)
, _isConnected(false)
, _socket(nullptr)
, _storeCount(0)
{
}


TcpSocketImpl::~TcpSocketImpl()
{
}


void TcpSocketImpl::cancel(System::EventLoop& loop)
{
    if(_connectOp)
    {
        _connectOp->Cancel();
        _connectOp = nullptr;
    }

    if(_loadOp)
    {
        _loadOp->Cancel();
        _loadOp = nullptr;
    }

    if(_storeOp)
    {
        _storeOp->Cancel();
        _storeOp = nullptr;
    }
}


void TcpSocketImpl::close()
{
    delete _reader;
    _reader = nullptr;

    delete _writer;
    _writer = nullptr;

    if(_socket)
    {
        delete _socket;
        _socket = nullptr;
    }
    
    _isConnected = false;
}


void TcpSocketImpl::accept(TcpServer& server, const TcpSocketOptions&)
{
    assert( ! _isConnected );

    _socket = server.impl().accept();
    _isConnected = true;
    log_debug("accepted socket");
}


void TcpSocketImpl::connect(const Endpoint& ep, const TcpSocketOptions&)
{
    log_debug( "connecting socket to " << ep.toString() );
    assert( ! _isConnected );

    throw System::IOError("blocking I/O not supported");
}


bool TcpSocketImpl::beginConnect(System::EventLoop& loop, const Endpoint& ep, const TcpSocketOptions&)
{
    assert( ! _isConnected );
    log_debug( "begin connecting socket to " << ep.toString() );

    _ep = ep;

    if( ! _socket )
    {
        _socket = ref new StreamSocket();
    }

    const std::string& host = _ep.impl()->host();
    std::wstring whost(host.begin(), host.end());
    String^ shost = ref new String(whost.c_str());

    const std::string& service = _ep.impl()->service();
    std::wstring wservice(service.begin(), service.end());
    String^ serviceName = ref new String( wservice.c_str() );

    _connectOp = _socket->ConnectAsync(ref new HostName(shost), serviceName);

    _connectOp->Completed = ref new AsyncActionCompletedHandler
    (
        [&](IAsyncAction^ asyncInfo, AsyncStatus status)
        {
            // set device ready state and wake our loop from the thread
            // context of the completion handler
            loop.setReady(_device);
            loop.wake();
        }
    );

    return false;
}


bool TcpSocketImpl::runConnect(System::EventLoop& loop, bool& isConnected)
{
    // this method is called by the event loop, when it wakes up and checks
    // the devices in the ready state. When true is returned, the connected
    // signal will be emitted

    return _connectOp && (_connectOp->Status == AsyncStatus::Completed || 
                          _connectOp->Status == AsyncStatus::Error);
}


void TcpSocketImpl::endConnect(System::EventLoop& loop)
{
    // the application reacts to the connect signal by calling endConnect

    log_debug( "ending connect to "  << _ep.toString() );
    
    if( ! _connectOp )
        return;
        
    if(_connectOp->Status == AsyncStatus::Error)
    {
        throw System::AccessFailed( _ep.toString() );
    }

    // TODO: handle connect exception
    _connectOp->GetResults();
    _connectOp = nullptr;

    _isConnected = true;
}


void TcpSocketImpl::localEndpoint(Endpoint& ep) const
{
    if( _socket )
    {
        String^ host = _socket->Information->LocalAddress->DisplayName;
        String^ service = _socket->Information->LocalPort;
        ep.impl()->init(host, service);
    }
    else
    {
        ep.clear();
    }
}


void TcpSocketImpl::remoteEndpoint(Endpoint& ep) const
{
    if( _socket )
    {
        String^ host = _socket->Information->RemoteAddress->DisplayName;
        String^ service = _socket->Information->RemotePort;
        ep.impl()->init(host, service);
    }
    else
    {
        ep.clear();
    }
}


std::size_t TcpSocketImpl::beginRead(System::EventLoop& loop, 
                                char* buffer, std::size_t bufSize, 
                                bool& eof)
{
    log_debug("beginRead " << bufSize);

    if( ! _reader )
    {
        _reader = ref new DataReader(_socket->InputStream);
        _reader->InputStreamOptions = InputStreamOptions::Partial;
    }

    std::size_t dataToRead = _reader->UnconsumedBufferLength;
    dataToRead = std::min(dataToRead, bufSize);
    
    if(dataToRead > 0)
    {
        _reader->ReadBytes(::Platform::ArrayReference<unsigned char>((unsigned char*) buffer,dataToRead)); 
        return dataToRead;
    }

    // TODO: does this report EOF? Or do we have to count the bytes until
    // we reach IRandomAccessStream.Size?

    _loadOp = _reader->LoadAsync(bufSize);

    _loadOp->Completed = ref new AsyncOperationCompletedHandler<unsigned int>
    (
        [&] (IAsyncOperation<unsigned int>^ asyncInfo, AsyncStatus asyncStatus) 
        {
            loop.setReady(_device);
            loop.wake(); 
        }
    );

    return 0;
}


bool TcpSocketImpl::runRead(System::EventLoop& loop)
{
    return _loadOp && ( _loadOp->Status == AsyncStatus::Completed || 
                        _loadOp->Status == AsyncStatus::Error );
}


std::size_t TcpSocketImpl::endRead(System::EventLoop& loop, 
                              char* buffer, std::size_t bufSize, 
                              bool& eof)
{
    log_debug("endRead");

    if( ! _loadOp )
        return 0;

    if(_loadOp->Status == AsyncStatus::Error)
    {
        throw System::IOError("read failed");
    }

    std::size_t dataToRead = _loadOp->GetResults();
    _loadOp = nullptr;

    if(dataToRead == 0)
    {
        eof = true;
        return 0;
    }

    dataToRead = std::min(dataToRead, bufSize);

    _reader->ReadBytes(::Platform::ArrayReference<unsigned char>((unsigned char*) buffer,dataToRead));

    //TODO: use ReadBytes 
    // http://stackoverflow.com/questions/10520335/how-to-wrap-a-char-buffer-in-a-winrt-ibuffer-in-c
    
    return dataToRead;
}


std::size_t TcpSocketImpl::read(char* buffer, std::size_t count, bool& eof)
{
    throw System::IOError("blocking I/O not supported");
    return 0;
}


std::size_t TcpSocketImpl::beginWrite(System::EventLoop& loop, 
                                 const char* buffer, std::size_t bufSize)
{
    log_debug("beginWrite " << bufSize);

    if( ! _writer )
        _writer = ref new DataWriter(_socket->OutputStream);

    _writer->WriteBytes(::Platform::ArrayReference<unsigned char>((unsigned char*) buffer,bufSize));

    _storeCount = bufSize;
    _storeOp = _writer->StoreAsync(); // FlushAsync    

    _storeOp->Completed = ref new AsyncOperationCompletedHandler<unsigned int>
    (
        [&] (IAsyncOperation<unsigned int>^ asyncInfo, AsyncStatus asyncStatus) 
        {
            loop.setReady(_device);
            loop.wake(); 
        }
    );

    return 0;
}


bool TcpSocketImpl::runWrite(System::EventLoop& loop)
{
    return _storeOp && _storeOp->Status == AsyncStatus::Completed || 
                       _storeOp->Status == AsyncStatus::Error;
}


std::size_t TcpSocketImpl::endWrite(System::EventLoop& loop, const char* buffer, std::size_t n)
{
    log_debug("endWrite");

    if( ! _storeOp )
        return 0;

    if(_storeOp->Status == AsyncStatus::Error)
    {
        throw System::IOError("read failed");
    }

    const std::size_t written = _storeOp->GetResults();
    assert(_storeCount == written);

    _storeOp = nullptr;
    return _storeCount;
}


std::size_t TcpSocketImpl::write(const char* buffer, std::size_t count)
{
    throw System::IOError("blocking I/O not supported");
    return 0;
}

} // namespace Net

} // namespace Pt
