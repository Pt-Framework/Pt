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
#include "Pt/System/MainLoop.h"
#include "Pt/System/Thread.h"
#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestSuite.h"
#include "Pt/Unit/RegisterTest.h"
#include <string>
#include <cstdlib>
#include <iostream>


class SerialDeviceTest : public Pt::Unit::TestSuite
                       , public Pt::Connectable
{
	Pt::uint16_t _rbuf[4096];

    public:
        SerialDeviceTest()
        : Pt::Unit::TestSuite("SerialDeviceTest")
		, _port("COM5")
        {
            //Pt::Unit::TestSuite::registerMethod( "ReadAsync", *this, &SerialDeviceTest::ReadAsync );
			Pt::Unit::TestSuite::registerMethod( "testRTS", *this, &SerialDeviceTest::testRTS );			
			//Pt::Unit::TestSuite::registerMethod( "testDTR", *this, &SerialDeviceTest::testDTR );			
			//Pt::Unit::TestSuite::registerMethod( "testBreak", *this, &SerialDeviceTest::testBreak );			
			//Pt::Unit::TestSuite::registerMethod( "testDSR", *this, &SerialDeviceTest::testDSR );			
			//Pt::Unit::TestSuite::registerMethod( "testCTS", *this, &SerialDeviceTest::testCTS );			

        }

		void ReadAsync()
		{
			try
			{
				Pt::System::MainLoop loop;

				Pt::System::SerialDevice serdev( _port, std::ios_base::in );
				serdev.setBaudRate(Pt::System::SerialDevice::BaudRate115200);
				serdev.setCharSize(8);
                serdev.setParity(Pt::System::SerialDevice::ParityNone);
				serdev.setStopBits(Pt::System::SerialDevice::OneStopBit);
				
                serdev.setActive(loop);
                serdev.beginRead((char*)_rbuf, sizeof(_rbuf));
				serdev.inputReady() += Pt::slot(*this, &SerialDeviceTest::onRead);

				loop.run();
			} 
			catch(const std::exception& ex)
			{
				std::string s = ex.what();
			}
		}

		void onRead(Pt::System::IODevice& iodev)
		{
			std::size_t n = iodev.endRead();
			bool eof = iodev.isEof();
			std::clog << "READ: " << n << " EOF: " << eof << std::endl;

			if(n == 0)
				std::clog << "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";

			iodev.beginRead((char*)_rbuf, sizeof(_rbuf));
		}

		void testRTS()
		{
			try {
				std::cout << "running" << std::endl;
				Pt::System::SerialDevice serdev( _port, std::ios_base::in| std::ios_base::out );				

				for(;;)
				{ 
					serdev.setRts(true);
					Pt::System::Thread::sleep( 500 );

					serdev.setRts(false);
					Pt::System::Thread::sleep( 500 );

					std::cout << '.' << std::flush;
				}
			}
			catch(const Pt::System::AccessFailed&)
			{
				std::cerr << "No device found" <<  std::endl;
				// do not fail in case no device is connected.
			}

			std::exit(0);
		}

		void testDTR()
		{
			try {
				std::cout << "running" << std::endl;
				Pt::System::SerialDevice serdev( _port, std::ios_base::in| std::ios_base::out );
				
				for(;;)
				{ 
					serdev.setDtr(true);
					Pt::System::Thread::sleep( 500 );

					serdev.setDtr(false);
					Pt::System::Thread::sleep( 500 );

					std::cout << '.' << std::flush;
				}
			}
			catch(const Pt::System::AccessFailed&)
			{
				std::cerr << "No device found" <<  std::endl;
				// do not fail in case no device is connected.
			}

			std::exit(0);
		}

		void testBreak()
		{
			try {
				std::cout << "running" << std::endl;
				Pt::System::SerialDevice serdev( _port, std::ios_base::in| std::ios_base::out );

				for(;;)
				{ 
					serdev.setBreak(true);
					Pt::System::Thread::sleep( 500 );

					serdev.setBreak(false);
					Pt::System::Thread::sleep( 500 );

					std::cout << '.' << std::flush;
				}
			}
			catch(const Pt::System::AccessFailed&)
			{
				std::cerr << "No device found" <<  std::endl;
				// do not fail in case no device is connected.
			}

			std::exit(0);
		}
	

	
		void testDSR()
		{
			try {
				std::cout << "running" << std::endl;
				Pt::System::SerialDevice serdev( _port, std::ios_base::in| std::ios_base::out );

				for(;;)
				{ 

					Pt::System::Thread::sleep(1);
					std::cout << serdev.isDsr() << '.' << std::flush;
				}
			}
			catch(const Pt::System::AccessFailed&)
			{
				std::cerr << "No device found" <<  std::endl;
				// do not fail in case no device is connected.
			}

			std::exit(0);
		}
	

		
		void testCTS()
		{
			try {
				std::cout << "running" << std::endl;				
				Pt::System::SerialDevice serdev( _port, std::ios_base::in| std::ios_base::out );

				for(;;)
				{ 

					Pt::System::Thread::sleep(1);
					std::cout << serdev.isCts() << '.' << std::flush;
				}
			}
			catch(const Pt::System::AccessFailed&)
			{
				std::cerr << "No device found" <<  std::endl;
				// do not fail in case no device is connected.
			}

			std::exit(0);
		}
	

    protected:
        void ReadPnp();
		std::string _port;
};

Pt::Unit::RegisterTest<SerialDeviceTest> register_SerialDeviceTest;


void SerialDeviceTest::ReadPnp()
{
#if defined(WIN32) || defined(_WIN32)
    std::string port("COM1:");
#else
    std::string port("/dev/ttyS0");
#endif

    try {
        Pt::System::SerialDevice serdev( port,  std::ios_base::in );

        serdev.setBaudRate(Pt::System::SerialDevice::BaudRate1200);
        serdev.setCharSize(7);
        serdev.setStopBits(Pt::System::SerialDevice::OneStopBit);
        serdev.setParity(Pt::System::SerialDevice::ParityNone);
        serdev.setFlowControl(Pt::System::SerialDevice::FlowControlHard);
        Pt::System::Thread::sleep( 300 );

        serdev.setFlowControl(Pt::System::SerialDevice::FlowControlSoft);
        Pt::System::Thread::sleep( 300 );

        char buffer[201];
        char byte;
        std::memset( buffer, 0, 201);

        std::size_t size = serdev.read( buffer, 1);

        if( (int)buffer[0] == 0 )
            return;

        size = serdev.read( buffer, 200);

        std::string pnpString;
        for( std::size_t i = 0; i < size; i++)
        {
            byte = buffer[i];
            if(byte == 0x08 || byte == 0x28)
            {
                int offset = 0x28 - byte;
                int stop = byte + 1;
                i++;
                for( ; i < size; i++ )
                {
                    byte = buffer[i];
                    if(byte == stop)
                        break;

                    byte += offset;
                    pnpString.append(1, byte);
                }
                break;
            }
        }

        reportMessage(pnpString);

        PT_UNIT_ASSERT(pnpString.empty() == false);
    }
    catch(const Pt::System::AccessFailed&)
    {
        reportMessage("No device found");
        // do not fail in case no device is connected.
    }

}
