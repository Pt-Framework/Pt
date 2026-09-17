/*
 * Copyright (C) 2005-2013 by Dr. Marc Boris Duerner
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

#ifndef PT_SYSTEM_API_IO_H
#define PT_SYSTEM_API_IO_H

/** @addtogroup Pt-System-IO

    @brief Synchronous and asynchronous I/O devices and streams.

    %IODevice is the endpoint for I/O. It is a %Selectable. read() and
    write() block. beginRead() and beginWrite() run with an %EventLoop.
    endRead() and endWrite() complete those operations. inputReady and
    outputReady fire when an asynchronous operation finishes.
    %FileDevice is the file endpoint.

    %Pipe is a pair of %IODevice objects. Bytes written to in() are
    read from out() in the same order. %SerialDevice is a serial port
    with baud rate, parity, stop bits, and flow control.

    %IOBuffer is the stream buffer for an %IODevice. %IOStream,
    %IStream, and %OStream attach a device through that buffer.

    %IONotifier waits on a native handle or file descriptor in an
    %EventLoop.
*/

#endif
