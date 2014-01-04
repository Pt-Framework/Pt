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

#include "UdpSocketImpl.h"
#include "EndpointImpl.h"
#include "Pt/Net/AddressInUse.h"
#include <Pt/Net/UdpSocket.h>
#include <Pt/System/SystemError.h>
#include <Pt/System/IOError.h>
#include <Pt/System/Logger.h>
#include <cassert>

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Storage;
using namespace Windows::Storage::Streams;
using namespace Windows::Networking;
using namespace Windows::Networking::Sockets;

log_define("Pt.System.UdpSocket");

namespace Pt {

namespace Net {

UdpSocketImpl::UdpSocketImpl(UdpSocket& socket)
: _device(socket)
, _loop(0)
, _timeout(Pt::System::EventLoop::WaitInfinite)
, _broadcast(false)
, _isConnected(false)
, _isBound(false)
, _storeCount(0)
{
    _socket = ref new DatagramSocket();
    
    typedef TypedEventHandler<DatagramSocket^, 
                              DatagramSocketMessageReceivedEventArgs^> MessageReceivedHandler;

    _socket->MessageReceived += ref new MessageReceivedHandler
    (
        [&](DatagramSocket^ socket, DatagramSocketMessageReceivedEventArgs^ args)
        {
            this->onMessageReceived(socket, args);
        }
    );
}


UdpSocketImpl::~UdpSocketImpl()
{
    delete _socket;
}


void UdpSocketImpl::cancel(System::EventLoop& loop)
{
    System::MutexLock lock(_mtx);
    _loop = 0;
    lock.unlock();

    if(_bindOp)
    {
        _bindOp->Cancel();
        _bindOp = nullptr;
    }

    if(_connectOp)
    {
        _connectOp->Cancel();
        _connectOp = nullptr;
    }

    if(_getOutputOp)
    {
        _getOutputOp->Cancel();
        _getOutputOp = nullptr;
    }

    if(_storeOp)
    {
        _storeOp->Cancel();
        _storeOp = nullptr;
    }
}


void UdpSocketImpl::close()
{
    _messages.clear();

    delete _writer;
    _writer = nullptr;

    delete _socket;
    _socket = ref new DatagramSocket();

    typedef TypedEventHandler<DatagramSocket^, 
                              DatagramSocketMessageReceivedEventArgs^> MessageReceivedHandler;

    _socket->MessageReceived += ref new MessageReceivedHandler
    (
        [&](DatagramSocket^ socket, DatagramSocketMessageReceivedEventArgs^ args)
        {
            this->onMessageReceived(socket, args);
        }
    );

    _isConnected = false;
    _isBound = false;
}


bool UdpSocketImpl::beginBind(System::EventLoop& loop, 
                              const Endpoint& ep, 
                              const UdpSocketOptions& o)
{
    log_debug( "begin binding socket to " << ep.toString() );

    const std::string& host = ep.impl()->host();
    std::wstring whost(host.begin(), host.end());
    String^ shost = ref new String(whost.c_str());

    const std::string& service = ep.impl()->service();
    std::wstring wservice(service.begin(), service.end());
    String^ serviceName = ref new String(wservice.c_str());

    if( shost->IsEmpty() )
        _bindOp = _socket->BindServiceNameAsync(serviceName);
    else
        _bindOp = _socket->BindEndpointAsync(ref new HostName(shost), serviceName);
    
    _bindOp->Completed = ref new AsyncActionCompletedHandler
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


bool UdpSocketImpl::runBind(System::EventLoop& loop)
{
    // this method is called by the event loop, when it wakes up and checks
    // the devices in the ready state. When true is returned, the connected
    // signal will be emitted

    return _bindOp && ( _bindOp->Status == AsyncStatus::Completed ||
                        _bindOp->Status == AsyncStatus::Error );
}


void UdpSocketImpl::endBind(System::EventLoop& loop)
{
    // the application reacts to the connect signal by calling endConnect

    log_debug("ending bind");
    
    if( ! _bindOp )
        return;

    // TODO: handle connect exception
    try
    {
        _bindOp->GetResults();
    }
    catch(Platform::COMException^ ex)
    {
        std::wcerr << ex->Message->Data() << std::endl;
    }
    
    _bindOp = nullptr;

    _isBound = true;
}


void UdpSocketImpl::bind(const Endpoint& ep, const UdpSocketOptions&)
{
    throw System::IOError("blocking I/O not supported");
    _isBound = true;
}


//<Package>
//  <Capabilities>
//    <Capability Name="internetClientServer" />

bool UdpSocketImpl::beginConnect(System::EventLoop& loop, const Endpoint& ep, const UdpSocketOptions&)
{
    log_debug( "begin connecting socket to " << ep.toString() );

    const std::string& host = ep.impl()->host();
    std::wstring whost(host.begin(), host.end());
    String^ shost = ref new String(whost.c_str());

    const std::string& service = ep.impl()->service();
    std::wstring wservice(service.begin(), service.end());
    String^ serviceName = ref new String(wservice.c_str());

    _connectOp = _socket->ConnectAsync(ref new HostName(shost), serviceName);
    
    _connectOp->Completed = ref new AsyncActionCompletedHandler
    (
        [&](IAsyncAction^ action, AsyncStatus asyncStatus)
        {
            // set device ready state and wake our loop from the thread
            // context of the completion handler
            loop.setReady(_device);
            loop.wake();
        }
    );
    
    return false;
}


bool UdpSocketImpl::runConnect(System::EventLoop& loop)
{
    // this method is called by the event loop, when it wakes up and checks
    // the devices in the ready state. When true is returned, the connected
    // signal will be emitted

    return _connectOp && ( _connectOp->Status == AsyncStatus::Completed ||
                           _connectOp->Status == AsyncStatus::Error );
}


void UdpSocketImpl::endConnect(System::EventLoop& loop)
{
    // the application reacts to the connect signal by calling endConnect

    log_debug("ending connect");
    
    if( ! _connectOp )
        return;

    try
    {
        _connectOp->GetResults();
        _writer = ref new DataWriter(_socket->OutputStream);
    }
    catch(Platform::COMException^ ex)
    {
        std::wcerr << ex->Message->Data() << std::endl;
    }

    // TODO: handle connect exception

    _connectOp = nullptr;

    _isConnected = true;
}


void UdpSocketImpl::connect(const Endpoint& ep, const UdpSocketOptions& o)
{
    throw System::IOError("blocking I/O not supported");
    _isConnected = true;
}


void UdpSocketImpl::setTarget(const Endpoint& ep, const UdpSocketOptions& o)
{
    const std::string& host = ep.impl()->host();
    std::wstring whost(host.begin(), host.end());
    String^ shost = ref new String(whost.c_str());

    const std::string& service = ep.impl()->service();
    std::wstring wservice(service.begin(), service.end());
    String^ serviceName = ref new String(wservice.c_str());

    _sendAddress = ref new HostName(shost);
    _sendPort = serviceName;

    // next beginWrite must create new output stream
    _writer = nullptr;
}


bool UdpSocketImpl::isConnected() const
{
    return _isConnected;
}


bool UdpSocketImpl::isBound() const
{
    return _isBound;
}


void UdpSocketImpl::setBroadcast()
{
    _broadcast = true;
}


void UdpSocketImpl::setHopLimit(unsigned int n)
{
    _socket->Control->OutboundUnicastHopLimit = n;
}


void UdpSocketImpl::joinMulticastGroup(const std::string& ipaddr)
{
    std::wstring waddr( ipaddr.begin(), ipaddr.end() );
    String^ saddr = ref new String( waddr.c_str() );

    _socket->JoinMulticastGroup(ref new HostName(saddr) );
}


void UdpSocketImpl::dropMulticastGroup(const std::string& ipaddr)
{
    // TODO: is this supported?

    //throw System::IOError("multicast group drop failed");
}


void UdpSocketImpl::localEndpoint(Endpoint& ep) const
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


const Endpoint& UdpSocketImpl::remoteEndpoint() const
{
    return _peerAddr;
}


void UdpSocketImpl::onMessageReceived(DatagramSocket^ socket, 
                                      DatagramSocketMessageReceivedEventArgs^ args)
{
    System::MutexLock lock(_mtx);

    Message m;
    m.reader = args->GetDataReader();
    m.remoteAddress = args->RemoteAddress;
    m.remotePort = args->RemotePort;

    _messages.insert( _messages.begin(), m );

    if(_loop)
    {
        _loop->setReady(_device);
        _loop->wake();
    }
}


std::size_t UdpSocketImpl::beginRead(System::EventLoop& loop, char* buffer, std::size_t n, bool& eof)
{
    System::MutexLock lock(_mtx);
    
    _loop = &loop;
    return 0;
}


bool UdpSocketImpl::runRead(System::EventLoop& loop)
{
    System::MutexLock lock(_mtx);
    return ! _messages.empty();
}


std::size_t UdpSocketImpl::endRead(System::EventLoop& loop, char* buffer, std::size_t bufSize, bool& eof)
{
    System::MutexLock lock(_mtx);
    _loop = 0;

    assert( ! _messages.empty() );

    if( _messages.empty() )
        throw System::IOError("accept backlog empty");

    Message m = _messages.back();
    _messages.pop_back();

    DataReader^ reader = m.reader;

    const std::size_t avail = reader->UnconsumedBufferLength;

    unsigned n = 0;
    for( ; n < avail && n < bufSize; ++n)
    {
        buffer[n] = static_cast<char>( reader->ReadByte() );
    }

    _peerAddr.impl()->init(m.remoteAddress->DisplayName, m.remotePort);

    return n;
}


std::size_t UdpSocketImpl::read(char* buffer, std::size_t count, bool& eof)
{
    throw System::IOError("blocking I/O not supported");
    return 0;
}


std::size_t UdpSocketImpl::beginWrite(System::EventLoop& loop, 
                                 const char* buffer, std::size_t bufSize)
{
    log_debug("beginWrite " << bufSize);

    // no writer means neither connect nor setTarget was called
    if( ! _writer )
    {
        _getOutputOp = _socket->GetOutputStreamAsync(_sendAddress, _sendPort);

        _getOutputOp->Completed = ref new AsyncOperationCompletedHandler<IOutputStream^>
        (
            [&, buffer, bufSize](IAsyncOperation<IOutputStream^>^ output, AsyncStatus asyncStatus)
            {
                // access to _writer reference itself is atomic, so we do not
                // have to lock when setting the reference.
                _writer = ref new DataWriter(output->GetResults());
                _getOutputOp = nullptr;

                this->beginWrite(loop, buffer, bufSize);
            }
        );

        return 0;
    }

    // UnstoredBufferLength
    const unsigned char* ubuffer = reinterpret_cast<const unsigned char*>(buffer);

    unsigned n = 0;
    for( ; n < bufSize; ++n)
    {
        _writer->WriteByte( ubuffer[n] );
    }

    _storeCount = n;
    _storeOp = _writer->StoreAsync();
                                  
    _storeOp->Completed = ref new AsyncOperationCompletedHandler<unsigned int>
    (
        [&] (IAsyncOperation<unsigned int>^ asyncOp, AsyncStatus asyncStatus) 
        {
            loop.setReady(_device);
            loop.wake(); 
        }
    );

    return 0;
}


bool UdpSocketImpl::runWrite(System::EventLoop& loop)
{
    return _storeOp && ( _storeOp->Status == AsyncStatus::Completed ||
                         _storeOp->Status == AsyncStatus::Error );
}


std::size_t UdpSocketImpl::endWrite(System::EventLoop& loop, const char* buffer, std::size_t n)
{
    log_debug("endWrite");

    const std::size_t written = _storeOp->GetResults();

    _storeOp = nullptr;
    return _storeCount;
}


std::size_t UdpSocketImpl::write(const char* buffer, std::size_t n)
{
    throw System::IOError("blocking I/O not supported");
    return 0;
}

} // namespace Net

} // namespace Pt
