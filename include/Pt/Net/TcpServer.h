/*
 * Copyright (C) 2006-2013 by Marc Boris Duerner
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

#ifndef Pt_Net_TcpServer_h
#define Pt_Net_TcpServer_h

#include <Pt/Net/Api.h>
#include <Pt/System/Selectable.h>
#include <Pt/Signal.h>
#include <Pt/Types.h>

namespace Pt {

namespace Net {

/** @brief TCP server listen options.

    @ingroup Pt-Net-Tcp
*/
class PT_NET_API TcpServerOptions
{
    public:
        /** @brief Creates options with accept backlog @a backlog.
        */
        explicit TcpServerOptions(int backlog = 5);

        /** @brief Copies the options.
        */
        TcpServerOptions(const TcpServerOptions& opts);

        /** @brief Destroys the options.
        */
        ~TcpServerOptions();

        /** @brief Assigns the options.
        */
        TcpServerOptions& operator=(const TcpServerOptions& opts);

        /** @brief Returns the deferred-accept timeout in seconds, or -1 if unset.
        */
        int acceptDeferred() const
        { return _deferAccept; }
        
        /** @brief Sets deferred accept to wait at most @a n seconds for data.
        */  
        void setDeferAccept(int n)
        { _deferAccept = n; }

        /** @brief Returns the accept backlog size.
        */
        int backlog() const
        { return _backlog; }

        /** @brief Sets the accept backlog size.
        */
        void setBacklog(int backlog)
        { _backlog = backlog; }

    private:
        Pt::uint32_t _flags;
        int _backlog;
        int _deferAccept;
        varint_t _r0;
        varint_t _r1;
        varint_t _r2;
};

class TcpServerImpl;

/** @brief Listens for TCP connections.

    %TcpServer is the listening side of the TCP model above.

    %listen() binds a local endpoint and waits for peers. If that
    address is already occupied, the call throws %AddressInUse.

    %beginAccept() waits for the next pending connection. The server
    must be attached to an event loop.

    connectionPending is emitted when a peer is ready. There is no
    matching end-accept.

    A %TcpSocket takes the connection with accept().

    %close() stops listening and accepting.

    The caller owns the server. Attaching it to a loop does not
    transfer ownership.

    @code
    class Acceptor : public Pt::Connectable
    {
        public:
            explicit Acceptor(Pt::Net::TcpServer& server)
            : _server(&server)
            {
                _server->connectionPending() += Pt::slot(*this, &Acceptor::onPending);
            }

            void onPending(Pt::Net::TcpServer& server)
            {
                _peer.accept(server);
            }

        private:
            Pt::Net::TcpServer* _server;
            Pt::Net::TcpSocket _peer;
    };
    @endcode

    @ingroup Pt-Net-Tcp
*/
class PT_NET_API TcpServer : public System::Selectable
{
    public:
        /** @brief Creates a closed server.
        */
        TcpServer();

        /** @brief Creates a server and attaches it to @a loop.
        */
        explicit TcpServer(System::EventLoop& loop);
        
        /** @brief Creates a server and listens on @a ep.
        */
        explicit TcpServer(const Endpoint& ep);

        /** @brief Destroys the server.
        */
        ~TcpServer();

        /** @brief Listens on local endpoint @a ep.
        */
        void listen(const Endpoint& ep);
        
        /** @brief Listens on @a ep with @a options.
        */
        void listen(const Endpoint& ep, const TcpServerOptions& options);

        /** @brief Begins accepting a connection.

            The server must be attached to an event loop.
        */
        void beginAccept();
        
        /** @brief Closes the server and stops listening.
        */
        void close();

        /** @brief Returns the signal that a connection is pending.
        */
        Signal<TcpServer&>& connectionPending()
        { return _connectionPending; }

        /** @brief Returns the parent event loop.
        */
        System::EventLoop* loop() const
        { return _loop; }

        //! @internal
        const TcpServerImpl& impl() const
        { return *_impl; }

        //! @internal
        TcpServerImpl& impl()
        { return *_impl; }

    protected:
        // inherit doc
        virtual void onAttach(System::EventLoop& loop);

        // inherit doc
        virtual void onDetach(System::EventLoop& loop);

        // inherit doc
        virtual void onCancel();
        
        // inherit doc
        virtual bool onRun();

    private:
        System::EventLoop* _loop;
        TcpServerImpl* _impl;
        Signal<TcpServer&> _connectionPending;
};

} // namespace Net

} // namespace Pt

#endif // Pt_Net_TcpServer_h
