/***************************************************************************
 *   Copyright (C) 2007 Marc Boris Drner                                  *
 *   Copyright (C) 2007 Laurentiu-Gheorghe Crisan                          *
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
#ifndef PT_SYSTEM_SERIALDEVICE_H
#define PT_SYSTEM_SERIALDEVICE_H
#include <Pt/System/Api.h>

#include <Pt/IO/IODevice.h>

namespace Pt {
namespace System {

class PT_SYSTEM_API SerialDevice : public  IO::IODevice
{
   private:
        //! @brief A pointer to the platform-specific SerialDevice implementation.
        class SerialDeviceImpl* _impl;

    public:
        enum BaudRate 
        {
            BaudRate0       = 0,
            BaudRate50      = 50,
            BaudRate75      = 75,
            BaudRate110     = 110,
            BaudRate134     = 134,
            BaudRate150     = 150,
            BaudRate200     = 200,
            BaudRate300     = 300,
            BaudRate600     = 600,
            BaudRate1200    = 1200,
            BaudRate1800    = 1800,
            BaudRate2400    = 2400,
            BaudRate4800    = 4800,
            BaudRate9600    = 9600,
            BaudRate19200   = 19200,
            BaudRate38400   = 38400,
            BaudRate57600   = 57600,
            BaudRate115200  = 115200,
            BaudRate230400  = 230400
        };
 
        enum Parity 
        {
            ParityEven, 
            ParityOdd,
            ParityNone
        };

        enum FlowControl 
        {
            FlowControlHard,            
            FlowControlSoft,
            FlowControlBoth
        }; 

        enum StopBits
        {
            OneStopBit,
            One5StopBits,
            TwoStopBits
        };

        SerialDevice( const std::string& file, std::ios_base::openmode mode )  throw(IO::IOError);
        virtual ~SerialDevice();
        
        void setBaudRate( BaudRate rate );
        BaudRate baudRate() const;
        
        void setCharSize( int size );
        int charSize() const;
              
        void setStopBits( StopBits bits );
        StopBits stopBits() const;    
                
        void setParity( Parity Parity );        
        Parity parity() const;
                
        void setFlowControl( FlowControl FlowControl );              
        FlowControl flowControl() const;       
        
        void flush();      
        
    protected:
        //! @brief Closes the I/O device
        virtual void _close();        

        //! @brief Read bytes from device
        virtual size_t _read(char* buffer, size_t count, bool& eof);

        //! @brief Write bytes to device
        virtual size_t _write(const char* buffer, size_t count);
        
        virtual bool _wait( WaitMode mode, unsigned int  msec);

};

} //namespace System
} //namespace Pt

#endif
