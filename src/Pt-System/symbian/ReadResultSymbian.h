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
#ifndef PT_SYSTEM_READRESULTSYMBIAN_H
#define PT_SYSTEM_READRESULTSYMBIAN_H

#include "ReadResult.h"

// symbian APIs
#include <e32base.h>
#include "SymbianTools.h"

namespace Pt {

namespace System {

    // This class represents an IOResult for the SerialDeviceImpl class
    // on Symbian.
    // The goal is to provide a posix-style interface so 
    // Pt::System::Selector can be used to monitor the device 
    // with the underlying posix call select().
    // This is achieved by using an ordinary pipe which will signal
    // a successful read when the Symbian asynchronous request has 
    // been finished. 
    class ReadResultSymbian : 
        public ReadResult, 
        public SymbianTools::StatusRequestWatcher::NotificationListener
    {
        public:
            ReadResultSymbian()
            : _buff(0), _tempBuffer(0,0,0), _statusRequestWatcher(_status), _wrotePipe(false)
            {
                memset(&_status, 0, sizeof(_status));                
                _statusRequestWatcher.setNotificationListener(this);
           
                if (::pipe(_notificationPipe))
                {
                    throw std::runtime_error("Could not open pipe." + PT_SOURCEINFO);
                }
                
                setFdPrivate(_notificationPipe[0]);
                
                ::pthread_mutex_init(&_mutex, NULL);                
            }

            ~ReadResultSymbian()
            {
                end();
                if (_notificationPipe[0] != -1 && _notificationPipe[1] != -1)
                {
                    ::close(_notificationPipe[0]);
                    ::close(_notificationPipe[1]);
                }
                freeSymbianBuffer();
            
                ::pthread_mutex_destroy(&_mutex);
            }

            virtual void setFd(int fd)
            { 
                throw std::logic_error("File descriptor is read-only." + PT_SOURCEINFO);                  
            }

        private:            
            void setFdPrivate(int fd)
            { ReadResult::setFd(fd); }

            bool isReadPending() const
            {
                return (_status.Int() == KRequestPending);               
            }
            
            void allocSymbianBuffer(size_t size)
            {
                freeSymbianBuffer();

                _buff = new char[size];
                _tempBuffer.Set((TUint8*)_buff, 0, size);
                _tempBuffer.Zero();
            }
            
            void freeSymbianBuffer()
            {
                if (_buff)
                {
                    delete[] _buff;
                    _buff = 0;
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
            
            bool start()
            {
                return _statusRequestWatcher.start();
                //return true;
            }
            
            bool end()
            {
                // we wrote in the pipe to simulate file descriptor
                // modification... Read the pipe now to flush it
                // so select will block on it the next time
                ::pthread_mutex_lock(&_mutex);
                if (_wrotePipe)
                {
                    char msgbuf[10];
                    ::read(_notificationPipe[0], msgbuf, 10); 
                    _wrotePipe = false;
                }
                ::pthread_mutex_unlock(&_mutex);
                return _statusRequestWatcher.stop();            
                //return true;
            }
            
            // this will be called asynchronously through a pthread
            // by the StatusRequestWatcher
            virtual void OnStatusRequestComplete(SymbianTools::StatusRequestWatcher& src)
            {
                ::pthread_mutex_lock(&_mutex);
                ::write(_notificationPipe[1],"X", 1);
                ::fsync(_notificationPipe[1]);                
                _wrotePipe = true;
                ::pthread_mutex_unlock(&_mutex);
            }
            
            // symbian buffer
            char* _buff;
            TPtr8 _tempBuffer;  
            TRequestStatus _status;
            
            SymbianTools::StatusRequestWatcher _statusRequestWatcher;
            int _notificationPipe[2];
            
            bool _wrotePipe;
            // mutex to protect _wrotePipe
            pthread_mutex_t _mutex;
            
            // tight coupling is ok for our purposes
            friend class SerialDeviceImpl;
    };    
    
}//namespace System

}//namespace Pt

#endif
