/***************************************************************************
 *   Copyright (C) 2006-2007 Marc Boris Duerner                            *
 *   Copyright (C) 2006-2007 PTV AG                                        *
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

#include <termios.h>
#include "Pt/System/IODevice.h"
#include "Pt/System/IOError.h"
#include "Pt/System/SerialDevice.h"
#include "IODeviceImpl.h"
#include <string>


namespace Pt {

namespace System {

class SerialDeviceImpl : public IODeviceImpl
{
    public:
        SerialDeviceImpl(IODevice& device);

        ~SerialDeviceImpl();

        void open(const std::string& path, std::ios::openmode mode);

        void close();

        void setBaudRate( unsigned rate );

        unsigned baudRate() const;

        void setCharSize( int size );

        int charSize() const;

        void setStopBits( SerialDevice::StopBits bits );

        SerialDevice::StopBits stopBits() const;

        void setParity( SerialDevice::Parity parity );

        SerialDevice::Parity parity() const;

        void setFlowControl( SerialDevice::FlowControl flowControl );

        SerialDevice::FlowControl flowControl() const;
        
        bool setSignal(SerialDevice::Signal signal);

        //void setTimeout( size_t msec );

        //size_t timeout() const;

        void sync() const;

    private:
        termios                     _prevIos;
        SerialDevice::FlowControl   _flowControl;
};

} //namespace System

} //namespaec Pt

#endif
