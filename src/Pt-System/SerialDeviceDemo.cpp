/*
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
#include <Pt/Main.h>
#include <Pt/System/SerialDevice.h>
#include <Pt/System/Thread.h>
#include <Pt/System/EventLoop.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>

void readMousePnp(const std::string& port)
{
    Pt::System::SerialDevice serdev( port, Pt::System::SerialDevice::Read );
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

    std::cerr << "Reading PNP data..." << std::endl;
    size_t size = serdev.read( buffer, 1);
    std::cerr << "RAW PNP ID: " << (int) buffer[0] << std::endl;

    if( (int)buffer[0] == 0 )
        return;

    size = serdev.read( buffer, 200);
    std::cerr << "RAW PNP DATA: " << buffer << " (" << size << " bytes)" << std::endl;

    std::cerr << "Parsing PnP data..." << std::endl;
    std::string pnpString;
    for( size_t i = 0; i < size; i++)
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

    std::cerr << "PNP COOKED DATA: " << pnpString << std::endl << std::endl;
}


const size_t size = 1024;
char buffer[size];


void onInput(Pt::System::IODevice& dev)
{
    size_t n = dev.endRead();
    std::cerr.write(buffer, n);
    dev.beginRead(buffer, size);
}


int main( int argc, char* argv[] )
{
    if(argc < 2)
    {
        std::cerr << "ERROR: Need a port name." << std::endl;
        return 1;
    }

    try
    {
        std::string port = argv[1]; // COM1: or /dev/ttyS0
        std::cerr << "'=> Opening " << port << std::endl;

        readMousePnp(port);

        Pt::System::SerialDevice serialDevice(port, Pt::System::SerialDevice::Read);
        serialDevice.setBaudRate(Pt::System::SerialDevice::BaudRate4800);
        serialDevice.setCharSize(8);
        serialDevice.setStopBits(Pt::System::SerialDevice::OneStopBit);
        serialDevice.setParity(Pt::System::SerialDevice::ParityNone);
        serialDevice.setFlowControl(Pt::System::SerialDevice::FlowControlHard);
        serialDevice.setTimeout(100);
        connect(serialDevice.inputReady, onInput);

        Pt::System::EventLoop loop;
        loop.add(serialDevice);

        serialDevice.beginRead(buffer, size);
        loop.run();
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
