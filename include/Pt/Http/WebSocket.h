/*
 * Copyright (C) 2015 by Laurentiu-Gheorghe Crisan
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */
#ifndef PT_HTTP_WEBSOCKET_H
#define PT_HTTP_WEBSOCKET_H

#include <Pt/Http/Api.h>
#include <Pt/Connectable.h>
#include <Pt/Signal.h>
#include <Pt/System/Timer.h>
#include <Pt/System/IODevice.h>
#include <Pt/System/MainLoop.h>
#include <Pt/Net/TcpSocket.h>
#include <Pt/Http/IOStream.h>


#include <vector>

namespace Pt {
namespace Http {

class WebSocketParser;
class WebSocketStream;

class PT_HTTP_API WebSocket : public Pt::System::IODevice, public Pt::Connectable
{
    public:
        /**@brief Define the frame kind.*/
        enum Frame
        {
            Unknow, ///< Unknow frame 
            Text,   ///< Text data frame
            Binary, ///< Binary data frame
            Ping,   ///< Ping frame
            Pong    ///< Pong frame
        };

        /**@brief Contructor. */
        WebSocket();

        WebSocket(Pt::Http::IOStream* stream);

        /**@brief Destructor */
        virtual ~WebSocket();

        void accept(Pt::Http::IOStream* stream);

        /**@brief Start the connection request.
        *
        * @param url The target Url. Ex.: ws://localhost/ws
        * @param origin The origin Url. Ee.: localhost:5000
        * @param keepAlive If true the keep alive request ist send to the server.*/
        void beginConnect(const std::string& url, const std::string& origin = std::string(), bool keepAlive = true);

        /**@brief The connected signal.
        *
        *  Usage:
        *  @code
        *   webSocket.connected() += Pt::slot(obj, & MyObject::onConnected);
        *  @endcode
        *
        *  @return Gets the connected signal. */
        Pt::Signal<WebSocket&>& connected()
        {
            return _connected;
        }

        /**@brief Call this to get the connect result.
        *
        * Throw std::exception derivate if the connection failed.*/
        void endConnect();

        /**@brief Sets the send frame mode.
        *
        * @param m The send frame mode. */
        void setSendFrame(Frame m)
        {
            _sendFrameMode = m;
        }

        /**@brief Gets for the current received frame the mode.
        *
        * @return The frame mode.*/
        Frame receiveFrame() const
        {
            return _receiveframeMode;
        }

        /**@brief Sends the pong frame.
        *
        * Shoult be called after receiving a ping frame*/
        void sendPongFrame();

        /**@brief Sends the ping frame.*/
        void sendPingFrame();

    private:
        void parseUrl(const std::string& url, const std::string& origin);

    private://Clinet
        void onConnected();

        void onInput();

        void onOutput();

    private:
        static std::string createKey();

    private:
        Pt::uint32_t createMask();

    protected://Pt::System::IODevice
        virtual void onClose();

        virtual void onSetTimeout(size_t ms);

        virtual std::size_t onBeginRead(Pt::System::EventLoop& loop, char* buffer, std::size_t n, bool& eof);

        virtual std::size_t onRead(char* buffer, std::size_t count, bool& eof);

        virtual std::size_t onEndRead(Pt::System::EventLoop& loop, char* buffer, std::size_t n, bool& eof);

        virtual std::size_t onBeginWrite(Pt::System::EventLoop& loop, const char* buffer, std::size_t n);

        virtual std::size_t onWrite(const char* buffer, std::size_t count);

        virtual std::size_t onEndWrite(Pt::System::EventLoop& loop, const char* buffer, std::size_t n);

        virtual void onCancel();

        virtual bool onRun()
        {
            return true;
        }

        virtual void onAttach(Pt::System::EventLoop& loop);

        virtual void onDetach(Pt::System::EventLoop& loop);

    protected:
        void handleInput(size_t size);

    protected:
            enum State
            {
                Unknown,
                Connecting,
                ReceiveHeader,
                ReceivePayloadLenght,
                ReceiveMask,
                ReceivePayloadChunk,
                Send,
            };

    private:
        void handleReceiveHeader(const char* buffer);

        void handleReceiveMask(const char* buffer);

        void handleReceivePayloadLenght(const char* buffer);

    private://Client
        std::string _host;
        std::string _path;
        std::string _protocol;
        std::string _connectHeader;
        unsigned short _port;
        WebSocketStream* _ioStream;
        Pt::Signal<WebSocket&> _connected;
        WebSocketParser* _httpParser;
        size_t _timeout;
        bool _keepAlive;

    private:
        Pt::System::MainLoop _dummyLoop;
        Pt::System::EventLoop* _loop;

    private://Read/Write
        size_t _payloadSize;
        size_t _currentOffset;
        char* _userReceiveBuffer;
        size_t _userReceiveBufferLenght;
        size_t _userSendBufferLenght;
        std::vector<char> _buffer;
        Pt::uint32_t _sendMask;
        Frame _sendFrameMode;
        Frame _receiveframeMode;
        bool _hasMask;
        Pt::uint32_t _mask;
        std::vector<char> _currentBuffer;
        size_t _payloadLenghtType;
        bool    _error;
        State   _state;
};

}}

#endif
