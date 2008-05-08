/***************************************************************************
 *   Copyright (C) 2006-2007 Laurentiu-Gheorghe Crisan                     *
 *   Copyright (C) 2006-2007 Marc Boris Duerner                            *
 *   Copyright (C) 2008 Peter Barth                                        *
 *   Copyright (C) 2006-2008 PTV AG                                        *
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
#ifndef PT_SYSTEM_READRESULT_H
#define PT_SYSTEM_READRESULT_H

#include "IOResultImpl.h"

#include <sys/select.h>
#include <sys/time.h>
#include <unistd.h>
#include <string.h>
#include <cerrno>

// symbian APIs
#include <e32base.h>

namespace Pt {

namespace System {

    class ReadResultPosix : public IOResultImpl
    {
        public:
            ReadResultPosix()
            {}

            virtual void add(fd_set& readFds, fd_set& writeFds)
            {  FD_SET( this->fd(), &readFds ); }

        protected:
            virtual bool _wait(unsigned int msecs)
            {
                fd_set wfds;
                FD_ZERO(&wfds);

                fd_set fds;
                FD_ZERO(&fds);
                FD_SET(  this->fd(), &fds );

                return select( this->fd(), fds, wfds, msecs);
            }
    };

    // This class is used in the serial device implementation for Symbian
    class ReadResultSymbian : public IOResultImpl
    {
        public:
            ReadResultSymbian()
            : _hBuf(0), _tempBuffer(0,0,0)
            {                
            }

            ~ReadResultSymbian()
            {
                freeSymbianBuffer();
            }

            virtual void add(fd_set& readFds, fd_set& writeFds)
            {  
                throw IOError("This method is not allowed on a Symbian IOResult instance.", PT_SOURCEINFO);                    
            }

        protected:
            virtual bool _wait(unsigned int msecs)
            {
                // TODO: Handle timeout
                User::WaitForRequest(_status);                
                return (_status.Int() == KErrNone || 
                        _status.Int() == KErrEof);
            }
        
        private:
            void allocSymbianBuffer(size_t size)
            {
                freeSymbianBuffer();

                TRAPD(allocError, _hBuf = HBufC8::NewL(size));
                if (allocError)
                {
                    throw IOError("Failed to allocate Symbian HBufC8.", PT_SOURCEINFO);                                        
                }
                
                _tempBuffer.Set(_hBuf->Des());        
                _tempBuffer.Zero();
            }
            
            void freeSymbianBuffer()
            {
                if (_hBuf)
                {
                    delete _hBuf;
                    _hBuf = 0;
                }
            }
            
            size_t transferData()
            {
                if ((unsigned)_tempBuffer.Size() > bufferSize())
                {
                    throw IOError("Read too much data.", PT_SOURCEINFO);                                        
                }
                
                char* dst = this->buffer();
                for (int j = 0; j < _tempBuffer.Size(); j++)
                    dst[j] = _tempBuffer[j];  
                
                return (size_t)_tempBuffer.Size();
            }
            
            // symbian buffer
            HBufC8* _hBuf;
            TPtr8 _tempBuffer;  
            TRequestStatus _status;
            
            // tight coupling is ok for our purposes
            friend class SerialDeviceImpl;
    };    
    
}//namespace System

}//namespace Pt

#endif
