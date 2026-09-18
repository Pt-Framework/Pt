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

#ifndef PT_HTTP_API_MESSAGES_H
#define PT_HTTP_API_MESSAGES_H

/** @addtogroup Pt-Http-Messages

    @brief Header, body and progress of HTTP messages.

    An HTTP message is a header and an optional body. The header is a
    list of name and value fields together with the HTTP version, and
    the body is a byte stream. %Message holds both. %Request and %Reply
    are messages: a request adds the method, the URL and the query
    string, while a reply adds the status code and the status text.

    Callers do not normally construct these types. A %Client owns the
    request it will send and the reply it receives, and a %Responder is
    given the server-side request and reply for one exchange, so the
    same header and body operations apply on both sides.

    %MessageHeader stores fields in a fixed buffer. %set() replaces a
    field, %add() appends another value for the same name, and %get()
    and %has() look up a name. %begin() and %end() walk the fields.
    Keep-alive, chunked transfer coding, content length and Upgrade are
    derived from those fields rather than being a second header model.

    %body() is a bidirectional stream on the message. Write to it to
    fill a request or a reply that will be sent, and read from it when
    a receive step has delivered body bytes. %discard() drops buffered
    body data. %available() and %pending() report how much can be read
    or still has to be written.

    Asynchronous send and receive move a message in steps, and each
    completed step returns a %MessageProgress value. %header() is true
    when the header of that message is available, %body() is true when
    body bytes were processed, and %finished() is true when the
    message is complete. A short message often sets all three on one
    step, while a long body, or a send that only partly left the
    socket, needs another begin and end pair. Progress can also be
    empty, meaning the step made I/O progress without exposing a
    header or a body yet, in which case the next begin call continues
    the same message.

    %HttpError is the HTTP-specific I/O error. %Credential is a user
    name and password that client authentication and server
    authorization both use, though they do not use the same
    authenticator type.
*/

#endif
