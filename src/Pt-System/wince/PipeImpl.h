/*************************************************************************** 
 *   Copyright (C) 2006-2007 Marc Boris Duerner                            *
 *   Copyright (C) 2006-2007 Bjoern Oliver Streule                         *
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
#include <cassert>
#include "..\win32\IOResultImpl.h"

namespace Pt {

namespace System {


class ReadResultPipe : public IOResultImpl
{
public:
    ReadResultPipe()    
    : _bufferSize(0)
    , _buffer(0)
    {}

    virtual void onComplete()
    {
        this->device()->inputReady(*this);        
    }

    void attach(char* buffer, size_t size)
    {
        assert(buffer);
        if(buffer && size > 0)
        {
            _buffer = buffer;
            _bufferSize = size;
        }
    }    

    char* buffer() const
    { return _buffer; }

    DWORD bufferSize() const
    { return _bufferSize; }    

private:
    char*   _buffer;
    DWORD   _bufferSize;   
};

class WriteResultPipe : public IOResultImpl
{
public:
    WriteResultPipe()    
    : _bufferSize(0)
    , _buffer(0)
    {}

    virtual void onComplete()
    {
        this->device()->outputReady(*this);        
    }

    void attach(const char* buffer, size_t size)
    {
        assert(buffer);
        if(buffer && size > 0)
        {
            _buffer = buffer;
            _bufferSize = size;
        }
    }    

    const char* buffer() const
    { return _buffer; }

    DWORD bufferSize() const
    { return _bufferSize; }    

private:
    const char*   _buffer;
    DWORD         _bufferSize; 
};


class PipeIODevice : public IODevice, private IODeviceImpl
{
    public:
        enum Mode {Read, Write};

        PipeIODevice(Mode mode);

        virtual ~PipeIODevice();

        virtual void open(HANDLE handle, bool isAsync);

        virtual HANDLE deviceHandle() const;
        
        //virtual void eventHandles( std::vector<HANDLE>& handles, size_t waitMode );

        //virtual WaitResult waitResult( HANDLE handle );

        //virtual void beginWait( size_t waitMode ) {}        

        virtual IODeviceImpl* impl()
        { return this; }

    protected:
        //! @brief Closes the I/O device
        virtual void onClose();

        virtual IOResult& onBeginRead(char* buffer, size_t n, bool& eof);

        virtual size_t onEndRead(IOResult& result, bool& eof);

        IOResult& onBeginWrite(const char* buffer, size_t n);

        size_t onEndWrite(IOResult& result);

        //! @brief Read bytes from device
        virtual size_t onRead(char* buffer, size_t count, bool& eof);

        //! @brief Write bytes to device
        virtual size_t onWrite(const char* buffer, size_t count);
        
        virtual bool _waitable() const
        { return _isWaitable; }

        virtual void onSync() const;

        void writeMessage(const char* buffer, size_t count);

     private:
        HANDLE                      _handle;  
        Mode                        _mode;
        DWORD                       _msgSize;
        size_t                      _bufferSize;
        std::vector<char>           _buffer;               
        bool                        _isWaitable;
        ReadResultPipe              _readResult;
        WriteResultPipe             _writeResult;
        HANDLE                      _internalBufferWaitHandle;
};

class PipeImpl
{
    public:
        PipeImpl(bool isAsync);

        ~PipeImpl();

        IODevice& input();

        IODevice& output();

    private:
        PipeIODevice        _inputDevice;
        PipeIODevice        _outputDevice;         
};

} // namespace System

} // namespace Pt

#endif
