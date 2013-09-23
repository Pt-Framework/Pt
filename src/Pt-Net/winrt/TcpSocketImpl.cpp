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
#include "Pt/Net/AddressInUse.h"
#include <Pt/Net/TcpSocket.h>
#include <Pt/System/SystemError.h>
#include <Pt/System/IOError.h>
#include <Pt/System/Logger.h>
#include <cassert>

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Networking;
using namespace Windows::Networking::Sockets;

log_define("Pt.System.TcpSocket");

namespace Pt {

namespace Net {

TcpSocketImpl::TcpSocketImpl(TcpSocket& socket)
: _device(socket)
, _timeout(System::EventLoop::WaitInfinite)
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
        _socket->Close();

        delete _socket;
        _socket = nullptr;
    }
    
    _isConnected = false;
}


void TcpSocketImpl::accept(const TcpServer& server, const TcpSocket::Options&)
{
    assert( ! _isConnected );

    _socket = server.impl().accept();
    _isConnected = true;
    log_debug("accepted socket");
}


void TcpSocketImpl::connect(const AddrInfo& ai)
{
    log_debug( "connecting socket to " << ai.host() << ":" << ai.port() );
    assert( ! _isConnected );

    _ai = addrinfo;

    throw IOError("blocking I/O not supported");
}


bool TcpSocketImpl::beginConnect(System::EventLoop& loop, const AddrInfo& ai)
{
    assert( ! _isConnected );
    log_debug( "begin connecting socket to " << addrinfo.host() << ":" << addrinfo.port() );

    if( ! _socket )
    {
        _socket = ref new StreamSocket();
    }

    const std::string& host = _ai.host();
	  std::wstring whost(host.begin(), host.end());
	  String^ shost = ref new String(whost.c_str());

    std::wostringstream wss;
    wss << _ai.port();
	  std::wstring wport = wss.str();
	  String^ serviceName = ref new String( wport.c_str() );

    _connectOp = _socket->ConnectAsync(ref new HostName(shost), serviceName);

    _connectOp->Completed = ref new AsyncActionCompletedHandler
    (
        [&](IAsyncAction asyncInfo^ asyncInfo, AsyncStatus status)
        {
            // set device ready state and wake our loop from the thread
            // context of the completion handler
            loop.setReady(_device);
            loop.wake();
        }
    );

    return _isConnected;
}


bool TcpSocketImpl::runConnect(System::EventLoop& loop)
{
    // this method is called by the event loop, when it wakes up and checks
    // the devices in the ready state. When true is returned, the connected
    // signal will be emitted

    return _connectOp && _connectOp->Status == AsyncStatus::Completed;
}


void TcpSocketImpl::endConnect(System::EventLoop& loop)
{
    // the application reacts to the connect signal by calling endConnect

    log_debug( "ending connect to "  << _ai.host() << ":" << _ai.port() );
    
    if( ! _connectOp )
        return;

    // TODO: handle connect exception
    _connectOp->GetResults();
    _connectOp = nullptr;

    _isConnected = true;
}


std::string TcpSocketImpl::socketAddress() const
{
    std::wstring waddr;

    if( _socket )
    {
        // TODO: use CanonicalName ?
        String^ addr = _socket->LocalAddress->DisplayName;
        if(addr)
            waddr = addr->Data();
    }

    return std::string( waddr.begin(), waddr.end() );
}


std::string TcpSocketImpl::peerAddress() const
{
    std::wstring waddr;

    if( _socket )
    {
        // TODO: use CanonicalName ?
        String^ addr = _socket->RemoteAddress->DisplayName;
        if(addr)
            waddr = addr->Data();
    }

    return std::string( waddr.begin(), waddr.end() );
}


size_t TcpSocketImpl::beginRead(System::EventLoop& loop, char* buffer, size_t n, bool& eof)
{
    log_debug("beginRead " << n);

    if( ! _reader )
    {
        _reader = ref new DataReader(_socket->InputStream);
    }

    const size_t avail = _reader->UnconsumedBufferLength;
    if(avail > 0)
    {
        //TODO: use ReadBytes
        unsigned n = 0;
        for( ; n < avail && n < bufSize; ++n)
        {
            buffer[n] = static_cast<char>( _reader->ReadByte() );
        }

        return n;
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
    return _loadOp && _loadOp->Status == AsyncStatus::Completed;
}


size_t TcpSocketImpl::endRead(System::EventLoop& loop, char* buffer, size_t n, bool& eof)
{
    log_debug("endRead");

    const size_t avail = _loadOp->GetResults();
    _loadOp = nullptr;

    //TODO: use ReadBytes 
    // http://stackoverflow.com/questions/10520335/how-to-wrap-a-char-buffer-in-a-winrt-ibuffer-in-c
    unsigned n = 0;
    for( ; n < avail && n < bufSize; ++n)
    {
        buffer[n] = static_cast<char>( _reader->ReadByte() );
    }

    return n;
}


size_t TcpSocketImpl::read(char* buffer, size_t count, bool& eof)
{
    throw IOError("blocking I/O not supported");
    return 0;
}


size_t TcpSocketImpl::beginWrite(System::EventLoop& loop, const char* buffer, size_t n)
{
    log_debug("beginWrite " << n);

    if( ! _writer )
    {
        _writer = ref new DataWriter(_socket->OutputStream);
    }

    // UnstoredBufferLength
    const unsigned char* ubuffer = reinterpret_cast<const unsigned char*>(buffer);

    unsigned n = 0;
    for( ; n < bufSize; ++n)
    {
        _writer->WriteByte( ubuffer[n] );
    }

    _storeCount = n;
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
    return _storeOp && _storeOp->Status == AsyncStatus::Completed;
}


size_t TcpSocketImpl::endWrite(System::EventLoop& loop, const char* buffer, size_t n)
{
    log_debug("endWrite");

    const size_t written = _storeOp->GetResults();

    _storeOp = nullptr;
    return _storeCount;
}


size_t TcpSocketImpl::write(const char* buffer, size_t count)
{
    throw IOError("blocking I/O not supported");
    return 0;
}

} // namespace Net

} // namespace Pt
