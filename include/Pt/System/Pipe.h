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

/** @brief A Pair of IODevices that form a half-duplex pipe

    A pipe consists of a pair of IODevices: A writable output device and a
    readable input device. If bytes are written to the output device they
    can be read from the input device in exactly the order in which they
    were written. Whether or not the writer to a pipe will block until the
    reader reads the data, or some previously-written bytes, from the pipe is
    system-dependent and therefore unspecified. Many pipe implementations will
    buffer up to a certain number of bytes between input and output, but such
    buffering should not be assumed.
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

        /** @brief Destructor

            Destroys the pipe and closes the internal IODevices.
        */
        ~Pipe();

        /** @brief Endpoint of the pipe to read from

            @return An IODevice used to read from the pipe
        */
        IODevice& out();

        const IODevice& out() const;

        /** @brief Endpoint of the pipe to write to

            @return An IODevice used to write to the pipe
        */
        IODevice& in();

        const IODevice& in() const;

        PipeImpl* impl()
        { return _impl; }
};

} // namespace System

} // namespace Pt

#endif // Pt_System_Pipe_h
