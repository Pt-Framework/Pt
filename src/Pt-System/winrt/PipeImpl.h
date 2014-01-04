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
#ifndef Pt_System_win32_PipeImpl_h
#define Pt_System_win32_PipeImpl_h

#include "Pt/System/Api.h"
#include <Pt/System/IODevice.h>
#include <Pt/System/EventLoop.h>
#include <Pt/System/Mutex.h>
#include <vector>

namespace Pt {

namespace System {

class PipeImpl;

class PipeIODevice : public IODevice
{
    public:
        enum Mode
        {
          Read, 
          Write
        };

    public:
        PipeIODevice(Mode m);

        virtual ~PipeIODevice();

        void init(PipeImpl* pipe);

    protected:
        bool onRun();

        void onSetTimeout(size_t timeout);

        size_t onBeginRead(EventLoop&, char* buffer, size_t n, bool& eof);

        size_t onEndRead(EventLoop&, char* buffer, size_t n, bool& eof);

        size_t onBeginWrite(EventLoop&, const char* buffer, size_t n);

        size_t onEndWrite(EventLoop&, const char* buffer, size_t n);

        virtual void onClose();

        virtual size_t onRead(char* buffer, size_t count, bool& eof);

        virtual size_t onWrite(const char* buffer, size_t count);

        virtual void onSync() const;

        virtual void onCancel();

     private:
        PipeImpl* _pipe;
        Mode _mode;
        std::size_t _timeout;
};

class PipeImpl
{
    public:
        PipeImpl();

        ~PipeImpl();

		void close();

        size_t beginRead(EventLoop& loop, char* buf, size_t n, bool& eof);
        
        bool readAvail();

		size_t endRead(char* buf, size_t n, bool& eof);

        size_t beginWrite(EventLoop& loop, const char* buf, size_t n);

        bool writeAvail();

        size_t endWrite(const char* buf, size_t n);

        void cancelRead();

        void cancelWrite();

        PipeIODevice& in();

        PipeIODevice& out();

    private:
        Mutex _mtx;
		bool _eof;
        std::vector<char> _buffer;
        EventLoop* _readLoop;
        EventLoop* _writeLoop;
        PipeIODevice _in;
        PipeIODevice _out;
};

} // namespace System

} // namespace Pt

#endif
