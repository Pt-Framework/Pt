/*
 * Copyright (C) 2013 Marc Boris Duerner
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

#ifndef PT_SYSTEM_FILEDEVICEIMPL_H
#define PT_SYSTEM_FILEDEVICEIMPL_H

#include "Pt/System/Api.h"
#include "Pt/System/IOError.h"
#include "Pt/System/FileDevice.h"
#include <windows.h>
#include <ios>

namespace Pt {

namespace System {

class FileDeviceImpl
{
    public:
        typedef FileDevice::pos_type pos_type;
        typedef FileDevice::off_type off_type;

    public:
        FileDeviceImpl(FileDevice& dev);

        ~FileDeviceImpl();

        void open( const char* path, std::ios::openmode mode);

        bool beginOpen(EventLoop& loop, const char* path, std::ios::openmode mode);

        bool runOpen(EventLoop& loop);

        void endOpen(EventLoop& loop);

        pos_type seek( off_type offset, std::ios::seekdir sd );
		
		void sync();

        size_t size();

        size_t peek( char* buffer, size_t count );

        void setTimeout(size_t timeout);

        bool runRead(EventLoop&);

        bool runWrite(EventLoop&);

        virtual size_t beginRead(EventLoop& loop, char* buffer, size_t n, bool& eof);

        virtual size_t endRead(EventLoop& loop, char* buffer, size_t n, bool& eof);

        virtual size_t beginWrite(EventLoop& loop, const char* buffer, size_t n);

        virtual size_t endWrite(EventLoop& loop, const char* buffer, size_t n);

        virtual void close();

        virtual size_t read(char* buffer, size_t count, bool& eof);

        virtual size_t write(const char* buffer, size_t count);

        virtual void cancel(EventLoop& loop) ;

    private:
        IODevice& _device;
        Windows::Foundation::IAsyncOperation<Windows::Storage::StorageFile^>^ _getFileOp;
        Windows::Foundation::IAsyncOperation<Windows::Storage::Streams::IRandomAccessStream^>^ _openOp;
        Windows::Storage::Streams::IRandomAccessStream^ _stream;
        Windows::Storage::Streams::DataReader^ _reader;
        Windows::Storage::Streams::DataReaderLoadOperation^ _loadOp;
        Windows::Storage::Streams::DataWriter^ _writer;
        Windows::Storage::Streams::DataWriterStoreOperation ^ _storeOp;
        size_t _storeCount;
};

}//namespace System

}//namespace Pt

#endif
