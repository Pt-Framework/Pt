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
#include "Pt/System/MainLoop.h"
#include <string>
#include <iostream>

//Configuration
std::string g_portReceiver("COM2:");
std::string g_portSender("COM3:");

class SerialDeviceManualTest : public Pt::Unit::TestSuite
{
    public:
        SerialDeviceManualTest()
        : Pt::Unit::TestSuite("SerialDeviceManualTest")
        {
            Pt::Unit::TestSuite::registerMethod( "SerSendReceiveTest", *this, &SerialDeviceManualTest::SendReceiveTest);
        }

        void setUp()
        {
			_readBuffer.resize(1024);
			_mainLoopThread = new Pt::System::AttachedThread(Pt::callable(*this, &SerialDeviceManualTest::loopRun));
        }

		void SendReceiveTest();

        void tearDown()
        {
			delete _mainLoopThread;
        }

		void loopRun()
		{
			
			Pt::System::SerialDevice receiverDevice( g_portReceiver,  std::ios_base::out |std::ios_base::in );

			receiverDevice.setBaudRate(Pt::System::SerialDevice::BaudRate115200);
			receiverDevice.setCharSize(8);
			receiverDevice.setStopBits(Pt::System::SerialDevice::OneStopBit);
			receiverDevice.setParity(Pt::System::SerialDevice::ParityNone);
			receiverDevice.setTimeout(1000);
			receiverDevice.inputReady() += Pt::slot(*this, &SerialDeviceManualTest::onInput);	
			receiverDevice.setActive(_loop);

			receiverDevice.beginRead((char*) & _readBuffer[0],  _readBuffer.size());
			_loop.run();
			receiverDevice.close();
		}

		void onInput(Pt::System::IODevice& device);

    protected:
		Pt::System::MainLoop _loop;
		Pt::System::AttachedThread* _mainLoopThread;
		std::vector<Pt::uint8_t> _readBuffer; 
		size_t _sendedBytes;
		size_t _receivedBytes;


};

Pt::Unit::RegisterTest<SerialDeviceManualTest> register_SerialDeviceManualTest;


void SerialDeviceManualTest::onInput(Pt::System::IODevice& device)
{
	size_t count = device.endRead();

	if(device.eof())
		return;

	PT_UNIT_ASSERT(count != 0);
	_receivedBytes += count;
	device.beginRead((char*) & _readBuffer[0],  _readBuffer.size());
}

void SerialDeviceManualTest::SendReceiveTest()
{
	_sendedBytes = 0;
	_receivedBytes = 0;

	//Set up sender
    Pt::System::SerialDevice senderDevice(g_portSender,  std::ios_base::out|std::ios_base::in );

    senderDevice.setBaudRate(Pt::System::SerialDevice::BaudRate115200);
    senderDevice.setCharSize(8);
    senderDevice.setStopBits(Pt::System::SerialDevice::OneStopBit);
    senderDevice.setParity(Pt::System::SerialDevice::ParityNone);
	senderDevice.setTimeout(1000);

	//Send some data
	std::vector<Pt::uint8_t> data(33);
	
	for( size_t i = 0; i < data.size(); ++i)
		data[i] = i;
			
	for( size_t i = 0; i < 1024; i+= 33)
	{
		size_t pos = 0;
		
		while( pos < 33)
			pos += senderDevice.write((char*) &data[pos], 33 - pos);

		_sendedBytes += 33;
	}

		//Start receiver
	_mainLoopThread->start();				
	Pt::System::Thread::sleep(2000);

	//Send again
	for( size_t i = 0; i < 1024*100; i+= 33)
	{
		size_t pos = 0;
		
		while( pos < 33)
			pos += senderDevice.write((char*) &data[pos], 33 - pos);

		_sendedBytes += 33;

		Pt::System::Thread::sleep(10);
	}

	Pt::System::Thread::sleep(2000);
	senderDevice.close();		
	_loop.exit();
	_mainLoopThread->join();
	PT_UNIT_ASSERT(_sendedBytes == _receivedBytes);

}
