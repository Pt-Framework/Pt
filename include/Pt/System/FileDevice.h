/***************************************************************************
 *   Copyright (C) 2005 Marc Boris Dürner                                  *
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

#ifndef Pt_System_FileDevice_h
#define Pt_System_FileDevice_h

#include <Pt/NonCopyable.h>
#include <Pt/IO/IODevice.h>
#include <Pt/System/Api.h>

namespace Pt {

namespace System {

    class PT_SYSTEM_API FileDevice : public IO::IODevice {
        private:
            class FileDeviceImpl* _impl;

        public:
            FileDevice();

            FileDevice(const char* path, OpenMode mode) throw(IO::IOError);

            ~FileDevice() throw();

            void open(const char* path, OpenMode mode) throw(IO::IOError);

            const char* path() const 
            { return _path.c_str(); }

            OpenMode openMode() const
            { return _mode; }

            size_t size() const throw(Pt::IO::IOError);

        protected:
            void _close() throw(IO::IOError);

            bool _remote() const throw()
            { return false; }

            bool _seekable() const throw()
            { return true; }

            pos_type _seek(off_type offset, SeekMode mode) throw(IO::IOError);

            size_t _read(char* buffer, size_t count, bool& eof) throw(IO::IOError);
            
            size_t _write(const char* buffer, size_t count) throw(IO::IOError);

            size_t _peek(char* buffer, size_t count) throw(IO::IOError);
            
            void _sync() const throw(IO::IOError);

            bool _wait(WaitMode mode, unsigned int msec) throw(IO::IOError);

        private:
            std::string _path;
            OpenMode _mode;
    };

} // namespace System

} // namespace Pt

#endif
