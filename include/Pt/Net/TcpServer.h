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

/** @brief TCP server options.
 */
class PT_NET_API TcpServerOptions
{
    public:
        TcpServerOptions(int backlog = 5);

        TcpServerOptions(const TcpServerOptions& opts);

        ~TcpServerOptions();

        TcpServerOptions& operator=(const TcpServerOptions& opts);

        int acceptDeferred() const
        { return _deferAccept; }
                
        void setDeferAccept(int n)
        { _deferAccept = n; }

        int backlog() const
        { return _backlog; }

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

/** @brief TCP server socket.
 */
class PT_NET_API TcpServer : public System::Selectable
{
    public:
        TcpServer();

        explicit TcpServer(System::EventLoop& loop);
        
        /** @brief Creates a server socket and listens on an address
        */
        explicit TcpServer(const Endpoint& ep);

        ~TcpServer();

        void listen(const Endpoint& ep);
        
        void listen(const Endpoint& ep, const TcpServerOptions& options);

        void beginAccept();
        
        void close();

        Signal<TcpServer&>& connectionPending()
        { return _connectionPending; }

        //! @brief Returns the parent event loop in which operations are running
        System::EventLoop* loop() const
        { return _loop; }

        //! @internal
        const TcpServerImpl& impl() const
        { return *_impl; }

        //! @internal
        TcpServerImpl& impl()
        { return *_impl; }

    protected:
        virtual void onAttach(System::EventLoop& loop);

        virtual void onDetach(System::EventLoop& loop);

        virtual void onCancel();
        
        virtual bool onRun();

    private:
        System::EventLoop* _loop;
        TcpServerImpl* _impl;
        Signal<TcpServer&> _connectionPending;
};

} // namespace Net

} // namespace Pt

#endif // Pt_Net_TcpServer_h
