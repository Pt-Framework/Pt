/*
* Copyright (C) 2012 by Marc Boris Duerner
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
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
*/

#ifndef Pt_Http_IOStream_h
#define Pt_Http_IOStream_h

#include <Pt/Http/Api.h>
#include <Pt/System/IOStream.h>
#include <Pt/Signal.h>
#include <Pt/Connectable.h>

namespace Pt {

namespace Http {

class Connection;

/** @brief Stream of an upgraded HTTP connection.

    %IOStream is the connection after an HTTP upgrade, not the body of
    a request or reply. %Service::upgradeRequested() provides it, and
    %WebSocket::accept() takes it.

    @ingroup Pt-Http-WebSocket
*/
class PT_HTTP_API IOStream : public Pt::BasicIOStream<char>,
                             public Pt::Connectable
{
    public :
        /** @brief Constructs the stream for @a conn.
        */
        IOStream(Connection* conn);

        /** @brief Destructor.
        */
        virtual ~IOStream();

        /** @brief Begins an asynchronous read.
        */
        void beginInput();

        /** @brief Ends an asynchronous read.
        */
        size_t endInput();

        /** @brief Begins an asynchronous write.
        */
        void beginOutput();

        /** @brief Ends an asynchronous write.
        */
        size_t endOutput();

        /** @brief Returns the signal emitted when input is ready.
        */
        Pt::Signal<>& inputReady()
        {
            return _inputReady;
        }

        /** @brief Returns the signal emitted when output is ready.
        */
        Pt::Signal<>& outputReady()
        {
            return _outputReady;
        }

        /** @brief Cancels pending I/O.
        */
        void cancel();

    private:
        void onInput(Pt::System::IOBuffer& b)
        {
            _inputReady.send();
        }

        void onOutput(Pt::System::IOBuffer& b)
        {
            _outputReady.send();
        }
    private:
        Connection* _conn;
        std::streambuf* _buffer;
        Pt::Signal<> _inputReady;
        Pt::Signal<> _outputReady;
};

}

}

#endif
