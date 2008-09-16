/***************************************************************************
 *   Copyright (C) 2005 Marc Boris Duerner                                 *
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

#ifndef Pt_System_FileStream_h
#define Pt_System_FileStream_h

#include <Pt/NonCopyable.h>
#include <Pt/System/Api.h>
#include <Pt/System/IOStream.h>
#include <Pt/System/StreamBuffer.h>
#include <Pt/System/FileDevice.h>

namespace Pt {

namespace System {
/*
    class SelectorBase;

    class PT_SYSTEM_API FileBuffer : public StreamBuffer
    {
        public:
            FileBuffer(const char* s, IODevice::OpenMode mode);

            ~FileBuffer();

            void setSelector(SelectorBase* sel);

            void open(const char* s, IODevice::OpenMode mode);

            void close();

            bool is_open();

        private:
            FileDevice _file;
    };


    class PT_SYSTEM_API FileStream : public BasicIOStream<char>
    {
        public:
            FileStream(const char* s, IODevice::OpenMode mode);

            ~FileStream();

            void open(const char* s, IODevice::OpenMode mode);

            void close();

            bool is_open();

            FileBuffer* rdbuf();

        private:
            FileBuffer _buffer;
    };
*/
} // namespace System

} // namespace Pt

#endif
