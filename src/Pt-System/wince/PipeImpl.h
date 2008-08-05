/*************************************************************************** 
 *   Copyright (C) 2006-2007 Marc Boris Duerner                            *
 *   Copyright (C) 2006-2007 Bjoern Oliver Streule                         *
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
#include <cassert>

namespace Pt {

namespace System {

class PipeIODevice : public IODevice, private IODeviceImpl
{
    public:
        enum Mode {Read, Write};

        PipeIODevice(Mode mode);

        ~PipeIODevice();

        void open(HANDLE handle, bool isAsync);
        
        bool setWaitHandle(HANDLE h);
		
        bool getWaitHandles(HandleMap& handles);
		
        bool checkEvent();

        virtual IODeviceImpl& ioimpl()
        { return *this; }

        virtual SelectableImpl& simpl()
        { return *this; }

    protected:
        void onBeginRead(char* buffer, size_t n, bool& eof);

        size_t onEndRead(bool& eof);

        void onBeginWrite(const char* buffer, size_t n);

        size_t onEndWrite();

        bool onWait(unsigned n);
        
        //! @brief Closes the I/O device
        virtual void onClose();

        //! @brief Read bytes from device
        virtual size_t onRead(char* buffer, size_t count, bool& eof);

        //! @brief Write bytes to device
        virtual size_t onWrite(const char* buffer, size_t count);

        virtual void onSync() const;

        void onAttach(SelectorBase& mon)
        { }

        void onDetach(SelectorBase& mon)
        { }
        
     protected:
        void writeMessage(const char* buffer, size_t count);

     private:
        Mode                        _mode;
        DWORD                       _msgSize;
        size_t                      _bufferSize;
        std::vector<char>           _buffer;               
        HANDLE                      _internalBufferWaitHandle;
        char* _rbuf;
        size_t _rbuflen;
        const char* _wbuf;
        size_t _wbuflen;
};

class PipeImpl
{
    public:
        PipeImpl(bool isAsync);

        ~PipeImpl();

        IODevice& input();

        IODevice& output();

    private:
        PipeIODevice  _inputDevice;
        PipeIODevice  _outputDevice;         
};

} // namespace System

} // namespace Pt

#endif
