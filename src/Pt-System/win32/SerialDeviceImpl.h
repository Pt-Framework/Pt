/***************************************************************************
 *   Copyright (C) 2007 Marc Boris Dürner                                  *
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
#ifndef PT_SYSTEM_SERIALDEVICEIMPL_H
#define PT_SYSTEM_SERIALDEVICEIMPL_H

#include <string>
#include <windows.h>

#include "Pt/IO/IODevice.h"
#include "Pt/IO/IOError.h"

namespace Pt{
namespace System{

class SerialDeviceImpl
{
    public:
        SerialDeviceImpl(const std::string& file, std::ios_base::openmode mode ) throw(IO::IOError);
        ~SerialDeviceImpl();
        
        //! @brief Closes the I/O device
        void close();        

        //! @brief Waits until data is available
        bool wait( IO::IODevice::WaitMode mode, unsigned int msec );

        //! @brief Read bytes from device
        size_t read( char* buffer, size_t count );

        //! @brief Write bytes to device
        size_t write(const char* buffer, size_t count);

        void setBaudRate( BaudRate rate );
        BaudRate baudRate() const;
        
        void setCharSize( int size );
        int charSize() const;
              
        void setStopBits( StopBits bits );
        StopBits stopBits();    
                
        void setParity( Parity Parity );        
        Parity parity() const;
                
        void setFlowControl( FlowControl FlowControl );              
        FlowControl flowControl() const;
        
        void setInputMode( InputMode mode );
        InputMode inputMode() const;
              
        void setControlMode( ControlMode mode );
        ControlMode controlMode() const;
                
        LocalMode localMode() const;
        void setLocalMode( LocalMode mode );
      
        OutputMode outputMode() const;
        void setOutputMode( OutputMode mode );
        
        int vMin()const ;         
        void setVMin( const int chars );
      
        int vTime() const;
        void setVTime(const int secs);
                  
    
    private:
        void updateCommState();
        HANDLE _handle;
        DCB    _commState;
            
};

}//namespace System
}//namespaec Pt

#endif