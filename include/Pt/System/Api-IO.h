/* Copyright (C) 2005-2013 by Dr. Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
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
