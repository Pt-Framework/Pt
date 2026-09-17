/*
 * Copyright (C) 2007 Marc Boris Drner
 * Copyright (C) 2007 Laurentiu-Gheorghe Crisan
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

#ifndef PT_SYSTEM_SERIALDEVICE_H
#define PT_SYSTEM_SERIALDEVICE_H

#include <Pt/System/Api.h>
#include <Pt/System/IODevice.h>
#include <Pt/Types.h>

namespace Pt {

namespace System {

/** @brief Serial port as an %IODevice.

    %SerialDevice opens a serial port as an %IODevice. The path is
    system-dependent, for example "COM1" on Windows or
    "/dev/ttyS0" on POSIX. Open the port, then set baud rate,
    character size, stop bits, parity, and flow control before
    read or write.

    The device supports the blocking and asynchronous transfers of
    %IODevice. Control lines are available as setRts(), setDtr(),
    isCts(), and isDsr(). setBreak() and sendBreak() generate a
    break condition.

    @code
    Pt::System::SerialDevice device("COM1", std::ios_base::in);
    device.setBaudRate(Pt::System::SerialDevice::BaudRate9600);
    device.setCharSize(8);
    device.setStopBits(Pt::System::SerialDevice::OneStopBit);
    device.setParity(Pt::System::SerialDevice::ParityNone);
    device.setFlowControl(Pt::System::SerialDevice::FlowControlNone);

    char buffer[64];
    std::size_t n = device.read(buffer, sizeof(buffer));
    @endcode

    @ingroup Pt-System-IO
*/
class PT_SYSTEM_API SerialDevice : public IODevice
{
   private:
        class SerialDeviceImpl* _impl;
        Pt::uint32_t _r0;

   public:
        /** @brief Baud rates.
        */
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

        /** @brief Parity values.
        */
        enum Parity
        {
            ParityEven,
            ParityOdd,
            ParityNone
        };

        /** @brief Flow control values.
        */
        enum FlowControl
        {
            FlowControlHard,
            FlowControlSoft,
			FlowControlNone
        };

        /** @brief Stop bits values.
        */
        enum StopBits
        {
            OneStopBit,
            One5StopBits,
            TwoStopBits
        };


        //! Default constructor.
        SerialDevice();

        /** @brief Constructs a serial device and open the specified device file
         */
        SerialDevice(const std::string& file, std::ios::openmode mode);

        /** @brief Constructs a serial device and open the specified device file
         */
        SerialDevice(const char* file, std::ios::openmode mode);

        //! @brief Destructor
        virtual ~SerialDevice();

        /** @brief Open the specified device file
         */
        void open(const std::string& file, std::ios::openmode mode);

        /** @brief Open the specified device file
         */
        void open(const char* file, std::ios::openmode mode);

        //! @brief Sets the baud rate
        void setBaudRate( unsigned rate );

        //! @brief Gets the baud rate
        unsigned baudRate() const;

        //! @brief Sets the char size
        void setCharSize( int size );

        //! @brief Gets the current char size
        int charSize() const;

        //! @brief Sets the number of stop bits
        void setStopBits( StopBits bits );

        //! @brief Gets the current number of stop bits
        StopBits stopBits() const;

        //! @brief Sets the parity
        void setParity( Parity parity );

        //! @brief Gets the current parity
        Parity parity() const;

        //! @brief Sets the flow control kind
        void setFlowControl( FlowControl flowControl );

        //! @brief Gets the current flow control kind
        FlowControl flowControl() const;

        /** @brief Sets the RTS control line.
        */
        void setRts(bool on);

        /** @brief Sets the DTR control line.
        */
        void setDtr(bool on);

        /** @brief Sets or clears a break condition.
        */
        void setBreak(bool on);

        /** @brief Sends a break condition.
        */
        void sendBreak(int duration = 0);

        /** @brief Returns true if CTS is asserted.
        */
        bool isCts() const;

        /** @brief Returns true if DSR is asserted.
        */
        bool isDsr() const;

        /** @brief Clears pending error flags.
        */
        void clear();

    protected:
        // inherit docs
        void onClose();

        // inherit docs
        void onSetTimeout(std::size_t timeout);

        // inherit docs
        std::size_t onBeginRead(EventLoop& loop, char* buffer, std::size_t n, bool& eof);

        // inherit docs
        std::size_t onEndRead(EventLoop& loop, char* buffer, std::size_t n, bool& eof);

        // inherit docs
        std::size_t onBeginWrite(EventLoop& loop, const char* buffer, std::size_t n);

        // inherit docs
        std::size_t onEndWrite(EventLoop& loop, const char* buffer, std::size_t n);

        // inherit docs
        std::size_t onRead(char* buffer, std::size_t count, bool& eof);

        // inherit docs
        std::size_t onWrite(const char* buffer, std::size_t count);

        // inherit docs
        void onSync() const;

        // inherit docs
        void onCancel();

        // inherit docs
        bool onRun();
};

} // namespace System

} // namespace Pt

#endif // PT_SYSTEM_SERIALDEVICE_H
