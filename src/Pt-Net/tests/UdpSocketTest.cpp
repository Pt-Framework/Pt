/*
 * Copyright (C) 2006 - 2009 by Marc Boris Duerner
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

#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestSuite.h"
#include "Pt/Unit/RegisterTest.h"
#include "Pt/Net/UdpSocket.h"
#include "Pt/System/MainLoop.h"
#include <string>

class UdpSocketTest : public Pt::Unit::TestSuite
{
    public:
        UdpSocketTest()
        : Pt::Unit::TestSuite("UdpSocketTest")
        , _loop(0)
        {
          this->registerMethod( "Unicast", *this,
                                &UdpSocketTest::Unicast);

          this->registerMethod( "Broadcast", *this,
                                &UdpSocketTest::Broadcast);

          this->registerMethod( "Multicast", *this,
                                &UdpSocketTest::Multicast);
        }

        void setUp()
        {
            std::memset(inbuf, 0, 200);
            std::memset(inbuf2, 0, 200);

            _sender = new Pt::Net::UdpSocket();
            _receiver = new Pt::Net::UdpSocket();
            _receiver2 = new Pt::Net::UdpSocket();

            _loop = new Pt::System::MainLoop();
            _loop->timeout() += Pt::slot(*_loop, &Pt::System::MainLoop::exit);
            _loop->setIdleTimeout(2000);
        }

        void tearDown()
        {
            _sender->close();
            delete _sender;
            _sender = 0;

            _receiver->close();
            delete _receiver;
            _receiver = 0;

            _receiver2->close();
            delete _receiver2;
            _receiver2 = 0;

            delete _loop;
            _loop = 0;
        }

        void Unicast()
        {
            _sender->connect("127.0.0.1", 8000);
            PT_UNIT_ASSERT( _sender->isConnected() );

            _receiver->bind("127.0.0.1", 8000, 0);
            PT_UNIT_ASSERT( _receiver->isBound() );

            _loop->add(*_receiver);
            _loop->add(*_sender);

            _receiver->inputReady += Pt::slot(*this, &UdpSocketTest::onUnicastInput);
            _receiver->beginRead(inbuf, 200);

            _sender->outputReady += Pt::slot(*this, &UdpSocketTest::onUnicastOutput);
            _sender->beginWrite("Hello UNICAST!", 14);

            _loop->run();

            PT_UNIT_ASSERT( 0 == std::strncmp(inbuf, "Hello UNICAST!", 14) );
        }

        void onUnicastOutput(Pt::System::IODevice& device)
        {
            std::size_t n = device.endWrite();

            //this->reportMessage("OUTPUT SENT");
            PT_UNIT_ASSERT(n > 10);
        }

        void onUnicastInput(Pt::System::IODevice& device)
        {
            //char* buffer = device.rbuf();
            std::size_t n = device.endRead();

            //std::string msg(buffer, n);
            //this->reportMessage("INPUT RECVD: " + msg);
            PT_UNIT_ASSERT(n > 10);

            _loop->exit();
        }

        void Broadcast()
        {
            _sender->setBroadcast();
            _sender->connect("255.255.255.255", 8000);

            PT_UNIT_ASSERT( _sender->isConnected() );

            _receiver->bind("0.0.0.0", 8000, 0);
            PT_UNIT_ASSERT( _receiver->isBound() );

            _receiver2->bind("0.0.0.0", 8000, 0);
            PT_UNIT_ASSERT( _receiver2->isBound() );

            _loop->add(*_receiver);
            _loop->add(*_receiver2);
            _loop->add(*_sender);

            _sender->outputReady += Pt::slot(*this, &UdpSocketTest::onBroadcastOutput);
            _sender->beginWrite("Hello BROADCAST!", 16);

            _receiver->inputReady += Pt::slot(*this, &UdpSocketTest::onBroadcastInput);
            _receiver->beginRead(inbuf, 200);

            _receiver2->inputReady += Pt::slot(*this, &UdpSocketTest::onBroadcastInput);
            _receiver2->beginRead(inbuf2, 200);

            _loop->run();

            PT_UNIT_ASSERT( 0 == std::strncmp(inbuf, "Hello BROADCAST!", 16) );
            PT_UNIT_ASSERT( 0 == std::strncmp(inbuf2, "Hello BROADCAST!", 16) );
        }

        void onBroadcastOutput(Pt::System::IODevice& device)
        {
            std::size_t n = device.endWrite();
            //this->reportMessage("OUTPUT SENT");
        }

        void onBroadcastInput(Pt::System::IODevice& device)
        {
            char* buffer = device.rbuf();
            std::size_t n = device.endRead();

            std::string msg(buffer, n);
            //this->reportMessage("INPUT RECVD: " + msg);

            _loop->exit();
        }

        void Multicast()
        {
            _sender->connect("224.0.1.1", 8000);
            PT_UNIT_ASSERT( _sender->isConnected() );

            _receiver->bind("0.0.0.0", 8000, 0);
            _receiver->joinMulticastGroup("224.0.1.1");
            PT_UNIT_ASSERT( _receiver->isBound() );

            _receiver2->bind("0.0.0.0", 8000, 0);
            _receiver2->joinMulticastGroup("224.0.1.1");
            PT_UNIT_ASSERT( _receiver2->isBound() );

            _loop->add(*_receiver);
            _loop->add(*_receiver2);
            _loop->add(*_sender);

            _sender->outputReady += Pt::slot(*this, &UdpSocketTest::onMulticastOutput);
            _sender->beginWrite("Hello MULTICAST!", 16);

            _receiver->inputReady += Pt::slot(*this, &UdpSocketTest::onMulticastInput);
            _receiver->beginRead(inbuf, 200);

            _receiver2->inputReady += Pt::slot(*this, &UdpSocketTest::onMulticastInput);
            _receiver2->beginRead(inbuf2, 200);

            _loop->run();

            _receiver->dropMulticastGroup("224.0.1.1");
            _receiver2->dropMulticastGroup("224.0.1.1");

            PT_UNIT_ASSERT( 0 == std::strncmp(inbuf, "Hello MULTICAST!", 16) );
            PT_UNIT_ASSERT( 0 == std::strncmp(inbuf2, "Hello MULTICAST!", 16) );
        }

        void onMulticastOutput(Pt::System::IODevice& device)
        {
            std::size_t n = device.endWrite();
            PT_UNIT_ASSERT(n > 10);
            //this->reportMessage("OUTPUT SENT");
        }

        void onMulticastInput(Pt::System::IODevice& device)
        {
            //char* buffer = device.rbuf();
            std::size_t n = device.endRead();

            //std::string msg(buffer, n);
            //this->reportMessage("INPUT RECVD: " + msg);

            _loop->exit();
        }

    private:
        Pt::Net::UdpSocket* _sender;
        Pt::Net::UdpSocket* _receiver;
        Pt::Net::UdpSocket* _receiver2;

        Pt::System::MainLoop* _loop;
        char inbuf[200];
        char inbuf2[200];
};

Pt::Unit::RegisterTest<UdpSocketTest> register_UdpSocketTest;
