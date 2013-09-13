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

#include "FileDeviceImpl.h"
#include "MainLoopImpl.h"
#include "Pt/System/IODevice.h"
#include "Pt/System/SystemError.h"
#include "Pt/System/IOError.h"
#include <cassert>

namespace Pt {

namespace System {

FileDeviceImpl::FileDeviceImpl(FileDevice& dev)
: _device(dev)
{
}


FileDeviceImpl::~FileDeviceImpl()
{ 
}


void FileDeviceImpl::open( const char* path, std::ios::openmode mode)
{   
    String^ sPath = path;

    IAsyncOperation<StorageFile>^ getFileOp = Storagefile::GetFileFromPathAsync(sPath);

    getFileOp->Completed = ref new AsyncOperationCompletedHandler<StorageFile^>
    (
        [&] (IAsyncOperation<StorageFile^>^ operation) 
        {
            StorageFile^ file = operation->GetResults();

            IAsyncOperation<IRandomAccessStream>^ openOp = file.OpenAsync();
            openOp->Completed = ref new AsyncOperationCompletedHandler<IRandomAccessStream^>
            (
                [](IAsyncOperation<IRandomAccessStream^>^ op)
                {
                    IRandomAccessStream^ accessStream = op->GetResults();
                }
            );
        }
    );

}


FileDeviceImpl::pos_type FileDeviceImpl::seek(off_type offset, std::ios::seekdir sd)
{
    return 0;
}


size_t FileDeviceImpl::size()
{
    return 0;
}


size_t FileDeviceImpl::peek(char* buffer, size_t count)
{
    return 0;
}


void FileDeviceImpl::close()
{

}


void FileDeviceImpl::cancel(EventLoop& loop)
{
}


void FileDeviceImpl::setTimeout(size_t)
{

}


bool FileDeviceImpl::runRead(EventLoop& loop)
{
    return true;
}


bool FileDeviceImpl::runWrite(EventLoop& loop)
{
    return true;
}


size_t FileDeviceImpl::beginRead(EventLoop& loop, char* buffer, size_t n, bool& eof)
{
    String^ path = ...;
    
    IAsyncOperation<StorageFile>^ Storagefile::GetFileFromPathAsync(path);

    StorageFile^ file = ...;

    IAsyncOperation<IRandomAccessStream>^ = file.OpenAsync();

    IRandomAccessStream^ stream = ...

    IInputStream* input = 0;
    HRESULT result = GetInputStreamAt( 0, &input);

    IAsyncOperationWithProgress<IBuffer*, UINT32> operation = 0;
    result = input->ReadAsync(IBuffer buffer, UINT32 count, InputStreamOptions options, &operation);

    operation->Completed = ref new AsyncOperationWithProgressCompletedHandler<IBuffer*, UINT32>(
      [&] () 
      {
          loop.setReady(_device);
          loop.wake(); 
      }
    );

    return 0;
}


size_t FileDeviceImpl::endRead(EventLoop& loop, char* buffer, size_t n, bool& eof)
{
    return 0;
}


size_t FileDeviceImpl::read(char* buffer, size_t count, bool& eof)
{

}


size_t FileDeviceImpl::beginWrite(EventLoop& loop, const char* buffer, size_t n)
{
    return 0;
}


size_t FileDeviceImpl::endWrite(EventLoop& loop, const char* buffer, size_t n)
{
    return 0;
}


size_t FileDeviceImpl::write(const char* buffer, size_t count)
{
    return 0;
}

} //namespace System

} //namespace Pt
