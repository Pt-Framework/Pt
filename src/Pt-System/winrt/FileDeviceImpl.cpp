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

#include "../win32/win32.h"
#include "FileDeviceImpl.h"
#include "MainLoopImpl.h"
#include "Pt/System/IODevice.h"
#include "Pt/System/SystemError.h"
#include "Pt/System/IOError.h"
#include <cassert>

using namespace Platform;
using namespace Windows::Storage;
using namespace Windows::Storage::Streams;
using namespace Windows::Foundation;

namespace Pt {

namespace System {

FileDeviceImpl::FileDeviceImpl(FileDevice& dev)
: _device(dev)
{
}


FileDeviceImpl::~FileDeviceImpl()
{ 
    // FileDevice destructor will call cancel and close
}


void FileDeviceImpl::close()
{
    delete _reader;
    _reader = nullptr;

    delete _writer;
    _writer = nullptr;

    delete _stream;
    _stream = nullptr;
}


void FileDeviceImpl::cancel(EventLoop& loop)
{
    if(_storeOp)
    {
        _storeOp->Cancel();
        _storeOp = nullptr;
    }

    if(_loadOp)
    {
        _loadOp->Cancel();
        _loadOp = nullptr;
    }

    if(_openOp)
    {
        _openOp->Cancel();
        _openOp = nullptr;
    }

    if(_getFileOp)
    {
        _getFileOp->Cancel();
        _getFileOp = nullptr;
    }
}


void FileDeviceImpl::open( const char* path, std::ios::openmode mode)
{   
    throw IOError("blocking I/O not supported");
}


bool FileDeviceImpl::beginOpen(EventLoop& loop, const char* path, std::ios::openmode mode)
{
    std::wstring wpath;
    win32::fromMultiByte(path, wpath);

    String^ sPath = ref new String(wpath.c_str());

    _getFileOp = StorageFile::GetFileFromPathAsync(sPath);

    _getFileOp->Completed = ref new AsyncOperationCompletedHandler<StorageFile^>
    (
        [&] (IAsyncOperation<StorageFile^>^ operation, AsyncStatus status) 
        {
            StorageFile^ file = operation->GetResults();

            // TODO: depending on openmode
            _openOp = file->OpenAsync(FileAccessMode::ReadWrite);
            _openOp->Completed = ref new AsyncOperationCompletedHandler<IRandomAccessStream^>
            (
                [&](IAsyncOperation<IRandomAccessStream^>^ op, AsyncStatus status)
                {
                    loop.setReady(_device);
                    loop.wake();
                }
            );
        }
    );

    return false;
}


bool FileDeviceImpl::runOpen(EventLoop& loop)
{
    return _openOp && _openOp->Status == AsyncStatus::Completed;
}


void FileDeviceImpl::endOpen(EventLoop& loop)
{
    _stream = _openOp->GetResults();
    _openOp = nullptr;
    _getFileOp = nullptr;
}


void FileDeviceImpl::sync()
{
    //TODO: implement flush
}


FileDeviceImpl::pos_type FileDeviceImpl::seek(off_type offset, std::ios::seekdir sd)
{
    if( ! _stream)
      throw IOError("seek failed");

    pos_type pos = 0;

    switch(sd)
    {
        default:
        case std::ios::beg:
            pos = 0;
            break;

        case std::ios::cur:
            pos = _stream->Position;
            
            // subtract unread data in reader
            if(_reader)
                pos -= _reader->UnconsumedBufferLength;
            break;

        case std::ios::end:
            pos = _stream->Size;
            break;
    }

    delete _reader;
    _reader = nullptr;

    delete _writer;
    _writer = nullptr;
    
    _stream->Seek(pos);
    return pos;
}


std::size_t FileDeviceImpl::size()
{
    if(_stream)
        _stream->Size;
    
    return 0;
}


std::size_t FileDeviceImpl::peek(char* buffer, std::size_t count)
{
    return 0;
}


void FileDeviceImpl::setTimeout(std::size_t)
{
    // blocking I/O is not supported
}


std::size_t FileDeviceImpl::beginRead(EventLoop& loop, char* buffer, std::size_t bufSize, bool& eof)
{
    if( ! _reader )
    {
        _reader = ref new DataReader(_stream);
    }

    const std::size_t avail = _reader->UnconsumedBufferLength;
    if(avail > 0)
    {
        //TODO: use ReadBytes
        unsigned n = 0;
        for(; n < avail && n < bufSize; ++n)
        {
            buffer[n] = static_cast<char>( _reader->ReadByte() );
        }

        return n;
    }

    // TODO: does this report EOF? Or do we have to count the bytes until
    // we reach IRandomAccessStream.Size?

    _loadOp = _reader->LoadAsync(bufSize);

    _loadOp->Completed = ref new AsyncOperationCompletedHandler<unsigned int>(
      [&] (IAsyncOperation<unsigned int>^ asyncInfo, AsyncStatus asyncStatus) 
      {
          loop.setReady(_device);
          loop.wake(); 
      }
    );

    return 0;
}


bool FileDeviceImpl::runRead(EventLoop& loop)
{
    return _loadOp && _loadOp->Status == AsyncStatus::Completed;
}


std::size_t FileDeviceImpl::endRead(EventLoop& loop, char* buffer, std::size_t bufSize, bool& eof)
{
    const std::size_t avail = _loadOp->GetResults();
    _loadOp = nullptr;

    //TODO: use ReadBytes 
    unsigned n = 0;
    for( ; n < avail && n < bufSize; ++n)
    {
        buffer[n] = static_cast<char>( _reader->ReadByte() );
    }

    return n;
}


std::size_t FileDeviceImpl::read(char* buffer, std::size_t count, bool& eof)
{
    throw IOError("blocking I/O not supported");
    return 0;
}


std::size_t FileDeviceImpl::beginWrite(EventLoop& loop, const char* buffer, std::size_t bufSize)
{
    if( ! _writer )
    {
        _writer = ref new DataWriter(_stream);
    }

    // UnstoredBufferLength
    const unsigned char* ubuffer = reinterpret_cast<const unsigned char*>(buffer);

    unsigned n = 0;
    for( ; n < bufSize; ++n)
    {
        _writer->WriteByte( ubuffer[n] );
    }

    _storeCount = n;
    _storeOp = _writer->StoreAsync(); // FlushAsync

    _storeOp->Completed = ref new AsyncOperationCompletedHandler<unsigned int>(
      [&] (IAsyncOperation<unsigned int>^ asyncInfo, AsyncStatus asyncStatus) 
      {
          loop.setReady(_device);
          loop.wake(); 
      }
    );

    return 0;
}


bool FileDeviceImpl::runWrite(EventLoop& loop)
{
    return _storeOp && _storeOp->Status == AsyncStatus::Completed;
}


std::size_t FileDeviceImpl::endWrite(EventLoop& loop, const char* buffer, std::size_t n)
{
    _storeOp->GetResults();

    _storeOp = nullptr;
    return _storeCount;
}


std::size_t FileDeviceImpl::write(const char* buffer, std::size_t count)
{
    throw IOError("blocking I/O not supported");
    return 0;
}

} //namespace System

} //namespace Pt
