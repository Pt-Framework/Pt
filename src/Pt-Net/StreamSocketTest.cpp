/***************************************************************************
 *   Copyright (C) 2006 by Marc Boris Duerner, Tommi Maekitalo             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include <string>

#include "Pt/System/Thread.h"

#include "Pt/Net/StreamSocket.h"
#include "Pt/Net/StreamServerSocket.h"

#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestMain.h"
#include "Pt/Unit/TestCase.h"
#include "Pt/Unit/RegisterTest.h"


class StreamServer : public Pt::System::Thread
{
	public:
		StreamServer(const std::string& ipaddr, short port)
		{
			_server.bind(ipaddr, port);
		}
		
	protected:
		void run()
		{
			_server.listen();
			Pt::Net::StreamSocket socket(_server);
			char buffer[80];
			socket.read(buffer, 80);
			std::cout << "READ: " << buffer << std::endl;
		}
		
	private:
		Pt::Net::StreamServerSocket _server;
};


class StreamSocketTest : public Pt::Unit::TestCase
{
	public:
		StreamSocketTest()
		: TestCase("StreamSocketTest")
		, _server(0)
		{ }

		void setUp()
		{
			_server= new StreamServer("127.0.0.1", 8080);
		}
		
		void test()
		{
			_server->start();
			Thread::sleep(250);
			Pt::Net::StreamSocket c("127.0.0.1", 8080);
			c.write("Hi", 3);
		}
		
		void tearDown()
		{
			delete _server;
			_server = 0;
		}
		
	private:
		StreamServer* _server;
};

Pt::Unit::RegisterTest<StreamSocketTest> register_StreamSocketTest;
