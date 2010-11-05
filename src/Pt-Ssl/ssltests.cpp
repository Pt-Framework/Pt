/*
 * Copyright (C) 2010-2010 by Marc Boris Duerner
 * Copyright (C) 2010-2010 by Aloysius Indrayanto
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

// Build using: ./jam.sh -q --with-openssl

#include <Pt/Connectable.h>
#include <Pt/System/MainLoop.h>
#include <Pt/Net/TcpServer.h>
#include <Pt/Net/TcpSocket.h>
#include <iostream>
#include <stdexcept>
#include <openssl/ssl.h>

class Client : public Pt::Connectable
{
    public:
        Client(Pt::System::EventLoop& loop, const std::string& addr, unsigned short port)
        : _loop(loop)
        {
            _loop.add(_socket);

            _socket.connected += Pt::slot(*this, &Client::onConnect);
            _socket.inputReady += Pt::slot(*this, &Client::onInput);
            _socket.outputReady += Pt::slot(*this, &Client::onOutput);

            _socket.beginConnect(addr, port);
        }

    protected:
        void onConnect(Pt::Net::TcpSocket& socket)
        {
            std::cout << "connected to server" << std::endl;
            _socket.endConnect();

            _socket.beginWrite("Hello", 5);
        }

        void onOutput(Pt::System::IODevice& socket)
        {
            std::size_t n = _socket.endWrite();
            std::cout << "client wrote " << n << " bytes" << std::endl;

            _socket.beginRead(inbuf, 200);
        }

        void onInput(Pt::System::IODevice& socket)
        {
            std::size_t n = _socket.endRead();
            std::cout << "client read " << n << " bytes: ";
            std::cout.write(inbuf, n) << std::endl;

            _socket.close();
            _loop.exit();
        }

    private:
        Pt::System::EventLoop& _loop;
        Pt::Net::TcpSocket _socket;
        char inbuf[200];
};


class Server : public Pt::Connectable
{
    public:
        Server(Pt::System::EventLoop& loop, const std::string& addr, unsigned short port)
        : _loop(loop)
        {
            _server.listen(addr, port);
            _server.connectionPending += Pt::slot(*this, &Server::onAccept);
            _loop.add(_server);

            _conn.inputReady += Pt::slot(*this, &Server::onInput);
            _conn.outputReady += Pt::slot(*this, &Server::onOutput);
            _loop.add(_conn);
        }

    protected:
        void onAccept(Pt::Net::TcpServer& server)
        {
            std::cout << "accepting client connection" << std::endl;
            _conn.accept(server);

            _conn.beginRead(inbuf, 200);
        }

        void onOutput(Pt::System::IODevice& socket)
        {
            std::size_t n = _conn.endWrite();
            std::cout << "server wrote " << n << " bytes" << std::endl;

            _conn.close();
        }

        void onInput(Pt::System::IODevice& socket)
        {
            std::size_t n = _conn.endRead();
            std::cout << "server read " << n << " bytes: ";
            std::cout.write(inbuf, n) << std::endl;

            _conn.beginWrite("Bye!", 4);
        }

    private:
        Pt::System::EventLoop& _loop;
        Pt::Net::TcpServer _server;
        Pt::Net::TcpSocket _conn;
        char inbuf[200];
};


int main(int argc, char** argv)
{
    try
    {
        std::cout << "OpenSSL test progam started..." << std::endl;

        Pt::System::MainLoop loop;

        std::string addr("127.0.0.1");
        unsigned short port = 8000;
        Server server(loop, addr, port);
        Client client(loop, addr, port);

        loop.run();

        std::cout << "OpenSSL test progam finished..." << std::endl;
        return 0;
    }
    catch(const std::exception& ex)
    {
        std::cerr << "error: " << ex.what() << std::endl;
    }

    return 1;
}
