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

#include <Pt/System/IODevice.h>

namespace Pt {
namespace System {

/** @brief Serial device
    @ingroup Pt::System

    This class implements access to a serial port as a %IODevice. A
    %SerialDevice can be opened by passing a system dependent path
    and an open mode. Then serial port attributes can be set before
    read or write operations are performed. The following example
    opens a COM port on windows, sets serial device attrubutes for
    a serial mouse and toggles the flow control to cause the device
    to send a PNP string which will be read subsequently:

    @code
        using Pt::System;

        Pt::System::SerialDevice serdev( "COM1",  std::ios_base::in );
        serdev.setBaudRate(Pt::System::SerialDevice::BaudRate1200);
        serdev.setCharSize(7);
        serdev.setStopBits(Pt::System::SerialDevice::OneStopBit);
        serdev.setParity(Pt::System::SerialDevice::ParityNone);

        serdev.setFlowControl(Pt::System::SerialDevice::FlowControlHard);
        Thread::sleep( 300 );

        serdev.setFlowControl(Pt::System::SerialDevice::FlowControlSoft);
        Thread::sleep( 300 );

        char pnp_id[200];
        size_t size = serdev.read( pnp_id, 200);
        std::cerr << "Mouse Id: ";
        std::cerr.write(pnp_id, size) << std::endl;
    @endcode
*/
class PT_SYSTEM_API SerialDevice : public IODevice
{
   private:
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
            BaudRate115200  = 115200
            #ifdef B230400
            , BaudRate230400  = 230400
            #endif
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

        //! Default constructor.
        SerialDevice();

        /** @brief Constructs a serial device and open the specified device file

            \param file     The serial device file
            \param mode     The open mode
            \param synchron  The Read/Write mode
        */
        SerialDevice( const std::string& file, std::ios_base::openmode mode );

        //! @brief Destructor
        virtual ~SerialDevice();

        /** @brief Open the specified device file

            \param file The serial device file
            \param mode The open mode
        */
        void open( const std::string& file, std::ios_base::openmode mode );

        //! @brief Sets the baud rate
        /*!
            \param rate The new baud rate
        */
        void setBaudRate( BaudRate rate );

        //! @brief Gets the baud rate
        /*!

            \return The current baud rate
        */
        BaudRate baudRate() const;

        //! @brief Sets the char size
        /*!

            \param size The char size to set
        */
        void setCharSize( int size );

        //! @brief Gets the current char size
        /*!
            \return The current char size
        */
        int charSize() const;

        //! @brief Sets the number of stop bits
        /*!

            \param bits The number of stop bits
        */
        void setStopBits( StopBits bits );

        //! @brief Gets the current number of stop bits
        /*!

            \return The current number of stop bits
        */
        StopBits stopBits() const;

        //! @brief Sets the parity
        /*!

            \param parity The new parity
        */
        void setParity( Parity parity );

        //! @brief Gets the current parity
        /*!

            \return The current parity
        */
        Parity parity() const;

        //! @brief Sets the flow control kind
        /*!

            \param flowControl The new flow control kind
        */
        void setFlowControl( FlowControl flowControl );

        //! @brief Gets the current flow control kind
        /*!

            \return The current flow control kind
        */
        FlowControl flowControl() const;

        void setTimeout( size_t msec );

        size_t timeout() const;

        //! @brief Transmit the current buffered characters.
        void flush();

        virtual IODeviceImpl* impl()
        { return (IODeviceImpl*) _impl; }

    protected:
        virtual void _close();

        IOResult& _beginRead(char* buffer, size_t n, bool& eof)
        { IOResult* res; return *res; }

        size_t _endRead(IOResult& result, bool& eof)
        { return 0;}

        virtual size_t _read(char* buffer, size_t count, bool& eof);

        virtual size_t _write(const char* buffer, size_t count);

        virtual bool _waitable() const
        { return true; }
};

} //namespace System
} //namespace Pt

#endif
