/***************************************************************************
 *   Copyright (C) 2008 Marc Boris Duerner                                 *
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
#ifndef Pt_System_IOBuffer_h
#define Pt_System_IOBuffer_h

#include <Pt/System/Api.h>
#include <Pt/System/IODevice.h>
#include <Pt/System/StreamBuffer.h>


namespace Pt {

namespace System {

//! @brief a stream buffer for IODevices with linear buffer area.
class PT_SYSTEM_API IOBuffer : public BasicStreamBuffer<char>
                             , public Connectable
{
    public:
        //! @brief Contructs an IOBuffer for an IODevice.
        IOBuffer(IODevice& ioDevice, size_t bufferSize = 1024);

        //! @brief Default constructor.
        IOBuffer(size_t bufferSize = 1024);

        ~IOBuffer();

        void setDevice(IODevice& ioDevice);

        IODevice* device();

        void beginSync();

        size_t out_avail() const;

        void beginFlush();

        Signal<IOBuffer&> inputReady;

        Signal<IOBuffer&> outputReady;

    protected:
        virtual int sync();

        virtual std::streamsize _peek(char* buffer, std::streamsize size);

        virtual int_type underflow();

        virtual int_type overflow(int_type ch);

        virtual pos_type seekoff(off_type offset, std::ios::seekdir sd, std::ios::openmode mode);

    private:
        void onRead(IODevice& dev);

        void onWrite(IODevice& dev);

    private:
        SelectorBase* _selector;
        IODevice* _ioDevice;
        char* _buffer;
        const size_t _bufferSize;
        const size_t _putbackMax;
};

} //namespace System

} //namespace Pt

#endif


