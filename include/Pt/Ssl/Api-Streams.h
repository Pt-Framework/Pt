/* Copyright (C) 2013 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#ifndef PT_SSL_API_STREAMS_H
#define PT_SSL_API_STREAMS_H

/** @addtogroup Pt-Ssl-Streams

    @brief Handshake, encrypted I/O and shutdown on an iostream.

    A secure connection is an %IOStream or a %StreamBuffer that wraps
    another iostream. Bytes written to the SSL stream are encrypted
    and written to that underlying stream. Bytes read from the
    underlying stream are decrypted and made available on the SSL
    stream. The module does not own the transport: any iostream that
    can move bytes to the peer is enough, including a
    %Pt::System::IOStream used with an event loop.

    Opening needs a %Context, the underlying iostream, and an
    %OpenMode. %Connect is a client handshake. %Accept is a server
    handshake. The context is not copied into the stream as a
    snapshot the caller can then discard without care: the stream
    uses the context for the connection, so the context must remain
    while the stream is open. %IOStream is the iostream most callers
    use. %StreamBuffer is the streambuf that implements the
    connection, and it is the type that sets the expected peer name.

    @par Handshake

    Construction or %open() prepares the connection; it does not
    finish the handshake. %readHandshake() and %writeHandshake()
    run until %isConnected() is true. %readHandshake() returns true
    when more handshake data must be read and the underlying stream
    did not have enough bytes. It consumes only as many bytes as
    the underlying streambuf reports with %in_avail(), so it can be
    called again when more data arrives. When it returns false, no
    more input is needed for this step and %writeHandshake() should
    run. %writeHandshake() returns true when handshake data was
    written to the underlying stream and still has to be sent. In
    the non-blocking case, call it again after that data has been
    flushed. A handshake that cannot complete throws
    %HandshakeFailed.

    @par Encrypted I/O

    Once the stream is connected, the blocking API inherited from
    %std::iostream reads and writes decrypted application data.
    Non-blocking input uses %import(), which decrypts from the
    underlying stream without reading more than %in_avail()
    indicates. It may decrypt only part of what is available, so
    draining the underlying stream means calling %import() until it
    makes no more progress. After %import(), decrypted bytes can be
    read, and %isShutdown() reports whether a shutdown alert arrived
    instead of application data. Writes encrypt into the SSL output
    buffer; %flush() pushes the encrypted bytes to the underlying
    stream so the code that owns that stream can send them. I/O that
    fails for an SSL reason throws %SslError. Premature end of the
    transport is reported by the code that maintains the underlying
    stream.

    @par Shutdown

    Either peer can start a shutdown. %isShutdown() is true when a
    shutdown alert has been received and the acknowledge still has
    to be completed. If that alert arrived during a read,
    %shutdown() writes the acknowledge to the underlying stream and
    returns true; the acknowledge still has to be sent, but the SSL
    stream then treats the connection as finished and %isShutdown()
    is no longer true. If %shutdown() is called to start the
    shutdown, the alert is written to the underlying stream and must
    be sent. When the peer's acknowledge becomes available,
    %shutdown() must be called again to consume it, and it returns
    true when the acknowledge is complete. %isClosed() is a
    premature close of the connection, not a completed TLS shutdown.
    Shutdown that fails throws %SslError.

    The expected name in the peer certificate is %setPeerName() on
    %StreamBuffer. %currentCipher() is the cipher in use after the
    handshake, or a placeholder when no connection is open.

    The example opens a client stream and runs the handshake until
    the stream is connected. Reading and writing application data
    is the %IOStream chapter.

    @code
    std::iostream& ios = ...;
    Pt::Ssl::IOStream ssl(ctx, ios, Pt::Ssl::Connect);

    while( ! ssl.isConnected() )
    {
        bool wantRead = ssl.readHandshake();
        if(wantRead)
        {
            // wait until the underlying stream has more data
            continue;
        }

        bool wantWrite = ssl.writeHandshake();
        if(wantWrite)
        {
            // flush the underlying stream
            continue;
        }
    }
    @endcode
*/

#endif
