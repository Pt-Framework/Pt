/*
 * Copyright (C) 2007 by Laurentiu-Gheorghe Crisan
 * Copyright (C) 2007 by Marc Boris Duerner
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
#include "Pt/System/SerialDevice.h"
#include "Pt/System/Thread.h"
#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestSuite.h"
#include "Pt/Unit/RegisterTest.h"
#include <string>
#include <iostream>


class SerialDeviceManualTest : public Pt::Unit::TestSuite
{
    public:
        SerialDeviceManualTest()
        : Pt::Unit::TestSuite("SerialDeviceManualTest")
        {
            Pt::Unit::TestSuite::registerMethod( "SendReceiveTest", *this, &SerialDeviceManualTest::SendReceive);
        }

        void setUp()
        {
			_readBuffer.resize(100);
			_mainLoopThread = new Pt::System::AttachedThread(Pt::callable(*this, &SerialDeviceManualTest::run));
        }

        void tearDown()
        {
			delete _mainLoopThread;
			delete _serverSocket;
        }


    protected:
		Pt::System::MainLoop _loop;
		Pt::System::AttachedThread* _mainLoopThread;
		Pt::Net::TcpSocket* _serverSocket;
		std::vector<Pt::uint8_t> _readBuffer; 


};

Pt::Unit::RegisterTest<SerialDeviceTest> register_SerialDeviceTest;


void SerialDeviceManualTest::onInput(Pt::System::IODevice& device)
{
	size_t count = device.endRead();

	PT_UNIT_ASSERT(count < 1014);

	device.beginRead((char*) & _readBuffer[0],  _readBuffer.size());
}

void SerialDeviceManualTest::SendReceiveTest()
{
#if defined(WIN32) || defined(_WIN32)
    std::string portSender("COM1:");
#else
    std::string portSender("/dev/ttyS0");
#endif

#if defined(WIN32) || defined(_WIN32)
    std::string portReceiver("COM3:");
#else
    std::string portReceiver("/dev/ttyS1");
#endif

	//Senup sender
    Pt::System::SerialDevice senderDevice( portSender,  std::ios_base::out );

    senderDevice.setBaudRate(Pt::System::SerialDevice::BaudRate115200);
    senderDevice.setCharSize(8);
    senderDevice.setStopBits(Pt::System::SerialDevice::OneStopBit);
    senderDevice.setParity(Pt::System::SerialDevice::ParityNone);

	//Sent up receiver
	_mainLoopThread->start();	
			
	Pt::System::Thread::sleep(1000);

	Pt::System::SerialDevice receiverDevice( portReceiver,  std::ios_base::in );

	receiverDevice.setBaudRate(Pt::System::SerialDevice::BaudRate115200);
    receiverDevice.setCharSize(8);
    receiverDevice.setStopBits(Pt::System::SerialDevice::OneStopBit);
    receiverDevice.setParity(Pt::System::SerialDevice::ParityNone);
	receiverDevice.inputReady() += Pt::slot(*this, &SerialDeviceTest::onInput);	
	receiverDevicesetActive(_loop);
	receiverDevice.beginRead((char*) & _readBuffer[0],  _readBuffer.size());


	std::vector<Pt::uint8_t> data(1024);
	memset(&data[0],234,data.size());
			
	for( size_t i = 0; i < data.size(); i+= 100)
	{
		senderDevice.write((char*) &data[i], 100);
		Pt::System::Thread::sleep(10);
	}

	Pt::System::Thread::sleep(100);
	senderDevice.close();			
	Pt::System::Thread::sleep(2000);	
	receiverDevice.close();
	_loop.exit();
	_mainLoopThread->join();

}
