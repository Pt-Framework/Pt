/* Copyright (C) 2026 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#ifndef PT_HTTP_WEBSOCKETERROR_H
#define PT_HTTP_WEBSOCKETERROR_H

#include <Pt/Http/Api.h>
#include <Pt/System/IOError.h>

namespace Pt {

namespace Http {

/** @brief WebSocket protocol error.

    A protocol violation of the framed stream. It is an I/O error.
    %code() is the close status that the engine sends or records.

    @ingroup Pt-Http-WebSocket
*/
class PT_HTTP_API WebSocketError : public System::IOError
{
    public:
        /** @brief Constructs the error with close status @a code.
        */
        explicit WebSocketError(unsigned code);

        /** @brief Returns the close status of this protocol error.
        */
        unsigned code() const
        { return _code; }

    private:
        unsigned _code;
};

} // namespace Http

} // namespace Pt

#endif
