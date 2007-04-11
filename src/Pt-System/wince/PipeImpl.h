/***************************************************************************
 *   Copyright (C) 2006-2007 Laurentiu-Gheorghe Crisan                     *
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
#ifndef Pt_System_win32_PipeImpl_h
#define Pt_System_win32_PipeImpl_h

#include <Pt/System/Api.h>
#include <Pt/System/IODevice.h>
#include "IODeviceImpl.h"
#include <windows.h>
#include <vector>

namespace Pt {

namespace System {

class PipeIODevice : public IODevice, private IODeviceImpl
{
    public:
        enum Mode {Read, Write};

        PipeIODevice(Mode mode);

        virtual ~PipeIODevice();

        virtual void open(HANDLE handle);

        virtual HANDLE deviceHandle() const;
        
        virtual void eventHandles( std::vector<HANDLE>& handles, size_t waitMode );

        virtual WaitResult waitResult( HANDLE handle );

        virtual void beginWait( size_t waitMode ) {}        

        virtual IODeviceImpl* impl()
        { return this; }

    protected:
        //! @brief Closes the I/O device
        virtual void _close();

        //! @brief Read bytes from device
        virtual size_t _read(char* buffer, size_t count, bool& eof);

        //! @brief Write bytes to device
        virtual size_t _write(const char* buffer, size_t count);
        
        virtual bool _waitable() const
        { return _isWaitable; }

        virtual void _sync() const;

        void writeMessage(const char* buffer, size_t count);

     private:
        HANDLE                      _handle;  
        Mode                        _mode;
        DWORD                       _msgSize;
        std::vector<char>           _buffer;
        std::vector<char>           _tempBuffer;        
        bool                        _isWaitable;
};

class PipeImpl
{
    public:
        PipeImpl();

        ~PipeImpl();

        IODevice& input();

        IODevice& output();
    private:
        PipeIODevice        _inputDevice;
        PipeIODevice        _outputDevice; 
        static Pt::uint32_t _nameId;
};

} // namespace System

} // namespace Pt

#endif
