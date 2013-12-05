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
#include <Pt/Signal.h>
#include <Pt/System/Selectable.h>
#include <string>

namespace Pt {

namespace Net {

class TcpServerImpl;
class Endpoint;

/** @brief TCP server socket
 */
class PT_NET_API TcpServer : public System::Selectable
{
    public:
        class Options
        {
            public:
                explicit Options(int backlog = 5)
                : _flags(0)
                , _backlog(backlog)
                {}

                bool deferAccept() const
                { return (_flags & DeferAccept) != 0; }
                
                void setDeferAccept()
                { _flags |= DeferAccept; }

                int backlog() const
                { return _backlog; }

                void setBacklog(int backlog)
                { _backlog = backlog; }

            private:
                enum SocketFlags 
                { 
                    DeferAccept = 1,
                };

            private:
                unsigned long _flags;
                int _backlog;
        };

    public:
        TcpServer();
        
        /** @brief Creates a server socket and listens on an address
        */
        TcpServer(const std::string& ipaddr, unsigned short int port, const Options& options = Options());

        TcpServer(const Endpoint& ipaddr, const Options& options = Options());
        
        ~TcpServer();
        
        void listen(const std::string& ipaddr, unsigned short int port, const Options& options = Options());

        void listen(const Endpoint& ipaddr, const Options& options = Options());
        
        void beginAccept();
        
        void close();

        Signal<TcpServer&>& connectionPending();

        //! @brief Returns the parent event loop in which operations are running
        System::EventLoop* loop() const
         { return _loop; }

        //! @internal
        TcpServerImpl& impl() const;

    protected:
        virtual void onAttach(System::EventLoop& loop)
        { _loop = &loop;}

        virtual void onDetach(System::EventLoop& loop)
        { _loop = 0; }

        virtual void onCancel();
        
        virtual bool onRun();

    private:
        System::EventLoop* _loop;
        TcpServerImpl* _impl;
        Signal<TcpServer&> _connectionPending;
};

} // namespace Net

} // namespace Pt

#endif
