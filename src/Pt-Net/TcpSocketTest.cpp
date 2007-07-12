/***************************************************************************
 *   Copyright (C) 2006 - 2007 by Marc Boris Duerner                       *
 *   Copyright (C) 2006 - 2007 by Tommi Maekitalo                          *
 *   Copyright (C) 2006 - 2007 by Sebastian Pieck                          *
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

#include "Pt/System/Thread.h"
#include "Pt/System/Mutex.h"
#include "Pt/System/MutexLock.h"
#include "Pt/System/Condition.h"

#include "Pt/Net/TcpSocket.h"
#include "Pt/Net/TcpServerSocket.h"

#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestMain.h"
#include "Pt/Unit/TestCase.h"
#include "Pt/Unit/RegisterTest.h"

#include <string>

//TODO: put condition in TcpSocketTest
class ServerThread : public Pt::System::Thread
{
    public:
        ServerThread(const std::string& ipaddr, short port)
        : _server(ipaddr, port)
        , _mutex(Pt::System::Mutex::NonRecursive)
        { 
            _mutex.lock(); 
        }

        ~ServerThread()
        { 
            _mutex.unlock(); 
        }
            
        const std::string& receivedData() const
        { return _receivedData; }

        void waitReady()
        {
            _ready.wait(_mutex);
        }

    protected:
        void run()
        {
            this->signalReady();
            
            Pt::Net::TcpSocket socket(_server);
            char buffer[80];
            socket.read(buffer, 80);
            _receivedData.assign(buffer, 2);

            socket.write("Bye", 4);
            this->signalReady();
        }

    protected:
        void signalReady()
        {
            Pt::System::MutexLock lock(_mutex);                    
            _ready.signal();
        }
            
    private:
        Pt::Net::TcpServerSocket _server;
        Pt::System::Condition _ready;
        Pt::System::Mutex _mutex;
        std::string _receivedData;
};


class TcpSocketTest : public Pt::Unit::TestCase
{
    public:
        TcpSocketTest()
        : TestCase("TcpSocketTest")
        , _server(0)
        { }

        void setUp()
        {
            _server = new ServerThread("127.0.0.1", 6789);
            _server->start();
            _server->waitReady();
        }
        
        void test()
        {
            Pt::Net::TcpSocket socket("127.0.0.1", 6789);
            socket.write("Hi", 3);
            _server->waitReady();
            PT_UNIT_ASSERT(_server->receivedData() == "Hi");

            //char buffer[80];
            //socket.read(buffer, 80);
            //PT_UNIT_ASSERT( std::string(buffer, 3) == "Bye" );
        }
        
        void tearDown()
        {
            delete _server;
            _server = 0;
        }
        
    private:
        ServerThread* _server;
};

Pt::Unit::RegisterTest<TcpSocketTest> register_TcpSocketTest;
