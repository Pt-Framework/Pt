/***************************************************************************
 *   Copyright (C) 2007 by Laurentiu-Gheorghe Crisan                       *
 *   Copyright (C) 2007 by Marc Boris Dürner                               *
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
#include "Pt/System/SerialDevice.h"
#include "Pt/System/Thread.h"
#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestSuite.h"
#include "Pt/Unit/RegisterTest.h"
#include <string>
#include <iostream>


class SerialDeviceTest : public Pt::Unit::TestSuite
{
    public:
        SerialDeviceTest()
        : Pt::Unit::TestSuite("SerialDeviceTest")
        {
            //Pt::Unit::TestSuite::registerMethod( "ReadPnp", *this, &SerialDeviceTest::ReadPnp );
        }

    protected:
        void ReadPnp();
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
    catch(const Pt::System::OpenFailed& f)
    {
        reportMessage("No device found");
        // do not fail in case no device is connected.
    }

}
