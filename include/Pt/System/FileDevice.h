/*
 * Copyright (C) 2006-2007 Marc Boris Duerner
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

#ifndef Pt_System_FileDevice_h
#define Pt_System_FileDevice_h

#include <Pt/System/Api.h>
#include <Pt/System/IODevice.h>
#include <string>
#include <ios>

namespace Pt {

namespace System {

/** @brief Read and write files.
*/
class PT_SYSTEM_API FileDevice : public IODevice 
{
    public:
        FileDevice();

        FileDevice(const std::string& path, std::ios::openmode mode);

        FileDevice(const char* path, std::ios::openmode mode);

        ~FileDevice();

        void open(const std::string& path, std::ios::openmode mode);

        void open(const char* path, std::ios::openmode mode);

        void beginOpen(const std::string& path, std::ios::openmode mode);

        void beginOpen(const char* path, std::ios::openmode mode);

        void endOpen();

        const char* path() const
        { return _path.c_str(); }

        Signal<FileDevice&>& opened()
        { return _opened; }

        bool isOpen() const
        { return _isOpen; }

    protected:
        std::size_t onBeginRead(EventLoop& loop, char* buffer, std::size_t n, bool& eof);

        std::size_t onEndRead(EventLoop& loop, char* buffer, std::size_t n, bool& eof);

        std::size_t onBeginWrite(EventLoop& loop, const char* buffer, std::size_t n);

        std::size_t onEndWrite(EventLoop& loop, const char* buffer, std::size_t n);

        void onClose();

        void onCancel();

        void onSetTimeout(std::size_t timeout);

        bool onSeekable() const;

        pos_type onSeek(off_type offset, std::ios::seekdir sd);

        std::size_t onRead(char* buffer, std::size_t count, bool& eof);

        std::size_t onWrite(const char* buffer, std::size_t count);

        std::size_t onPeek(char* buffer, std::size_t count);

        void onSync() const;

        virtual bool onRun();

    private:
        class FileDeviceImpl* _impl;
        std::string _path;
        Signal<FileDevice&> _opened;
        bool _opening;
        bool _isOpen;
};

} // namespace System

} // namespace Pt

#endif // Pt_System_FileDevice_h
