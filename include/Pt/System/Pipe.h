/*
 * Copyright (C) 2006-2013 Marc Boris Duerner
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

#ifndef Pt_System_Pipe_h
#define Pt_System_Pipe_h

#include <Pt/NonCopyable.h>
#include <Pt/System/Api.h>
#include <Pt/System/IODevice.h>

namespace Pt {

namespace System {

/** @brief Pair of %IODevice endpoints that form a half-duplex pipe.

    A pipe is two %IODevice objects created together. Bytes written to
    in() are read from out() in the same order. The constructor opens
    both ends. The destructor closes them.

    Whether a write blocks until a read consumes data is
    system-dependent. Some platforms buffer a limited number of bytes
    between the ends. That buffer must not be assumed. Treat a write
    as complete only when write() or endWrite() returns.

    Each end is a %Selectable. Attach out() to an %EventLoop to read
    without blocking the thread. Attach in() to write the same way.

    @ingroup Pt-System-IO
*/
class PT_SYSTEM_API Pipe : public NonCopyable
{
    private:
        class PipeImpl* _impl;

    public:
        /** @brief Creates the pipe with two IODevices.

            The default constructor will create the pipe and the appropriate
            IODevices to read and write to the pipe.
        */
        explicit Pipe();

        /** @brief Destructor.

            Destroys the pipe and closes the internal IODevices.
        */
        ~Pipe();

        /** @brief Endpoint of the pipe to read from.
        */
        IODevice& out();

        /** @brief Endpoint of the pipe to read from.
        */
        const IODevice& out() const;

        /** @brief Endpoint of the pipe to write to.
        */
        IODevice& in();

        /** @brief Endpoint of the pipe to write to.
        */
        const IODevice& in() const;

        //! @internal
        PipeImpl* impl()
        { return _impl; }
};

} // namespace System

} // namespace Pt

#endif // Pt_System_Pipe_h
