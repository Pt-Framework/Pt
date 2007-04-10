/***************************************************************************
 *   Copyright (C) 2006-2007 Laurentiu-Gheorghe Crisan                     *
 *   Copyright (C) 2006-2007 Marc Boris Duerner                            *
 *   Copyright (C) 2006-2007 PTV AG                                        *
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
    readerreads the data, or some previously-written bytes, from the pipe is
    system-dependent and therefore unspecified. Many pipe implementations will
    buffer up to a certain number of bytes between input and output, but such
    buffering should not be assumed.
    The following code writes data to a pipe and reads it afterwards:
    @code
    int main( )
    {
        Pt::System::Pipe pipe;
        pipe.output().write("Hello World!", 12);

        Pt::System::Selector selector;
        selector.addDevice( pipe.input(), Pt::System::Selector::WaitInput );
        selector.wait();

        char buffer[20];
        size_t sz = pipe.input().read(buffer, 20);

        return 0;
    }
    @endcode
*/
class PT_SYSTEM_API Pipe : public NonCopyable
{
    private:
        class PipeImpl* _impl;

    public:
        /** @brief Creates the pipe with two IODevices

            The default constructor will create the pipe and the appropriate
            IODevices to read and write to the pipe.
        */
        Pipe();

        /** @brief Destructor

            Destroys the pipe and closes the internal IODevices.
        */
        ~Pipe();

        /** @brief Endpoint of the pipe to read from

            @return An IODevice used to read from the pipe
        */
        IODevice& input();

        /** @brief Endpoint of the pipe to write to

            @return An IODevice used to write to the pipe
        */
        IODevice& output();
};

} // namespace System

} // namespace Pt

#endif
