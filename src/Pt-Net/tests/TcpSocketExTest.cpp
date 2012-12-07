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
#include "Pt/Net/TcpServer.h"
#include "Pt/Net/TcpSocket.h"
#include "Pt/System/MainLoop.h"
#include "Pt/System/Logger.h"
#include "Pt/System/Thread.h"
#include <string>
#include <vector>

class TcpSocketExTest : public Pt::Unit::TestSuite
{ 
    public:
        TcpSocketExTest()
        : Pt::Unit::TestSuite("TcpSocketExTest")
        {
          Pt::System::Logger::setLogLevel("", Pt::System::Warn);

          this->registerMethod("SendReceiveTest", *this, &TcpSocketExTest::SendReceiveTest);
        }

        void setUp()
        {
			_readBuffer.resize(100);
			_mainLoopThread = new Pt::System::AttachedThread(Pt::callable(*this, &TcpSocketExTest::loopRun));
        }

        void tearDown()
        {
			delete _mainLoopThread;
			delete _serverSocket;
        }

		void SendReceiveTest()
		{			
			_mainLoopThread->start();	
			
			Pt::System::Thread::sleep(5000);

			Pt::Net::TcpSocket socket("127.0.0.1", 5050);
			std::vector<Pt::uint8_t> data(1024);
			std::memset(&data[0],234,data.size());
			
			for( size_t i = 0; i < data.size(); i+= 100)
			{
				socket.write((char*) &data[i], 100);
				Pt::System::Thread::sleep(10);
			}

			Pt::System::Thread::sleep(100);
			socket.close();			

			Pt::System::Thread::sleep(2000);	
			PT_UNIT_ASSERT(_serverSocket != 0);
			_serverSocket->close();
			_loop.exit();
			_mainLoopThread->join();
		}
		
		void loopRun()
		{
			Pt::Net::TcpServer server("127.0.0.1",5050);
			server.connectionPending() += Pt::slot(*this, &TcpSocketExTest::onAccept);
			server.setActive(_loop);					
			server.beginAccept();

			_loop.run();
		}

        void onAccept(Pt::Net::TcpServer& server)
        {
			_serverSocket = new Pt::Net::TcpSocket();
			_serverSocket->accept(server);
			_serverSocket->inputReady() += Pt::slot(*this, &TcpSocketExTest::onInput);	
			_serverSocket->setActive(_loop);
			_serverSocket->beginRead((char*) & _readBuffer[0],  _readBuffer.size());
        }

        void onInput(Pt::System::IODevice& device)
        {
			size_t count = device.endRead();
			if(!device.eof())
			{
				PT_UNIT_ASSERT(count != 0);
				device.beginRead((char*) & _readBuffer[0],  _readBuffer.size());
			}
        }


    private:
        Pt::System::MainLoop _loop;
		Pt::System::AttachedThread* _mainLoopThread;
		Pt::Net::TcpSocket* _serverSocket;
		std::vector<Pt::uint8_t> _readBuffer; 		
};

Pt::Unit::RegisterTest<TcpSocketExTest> register_TcpSocketExTest;
