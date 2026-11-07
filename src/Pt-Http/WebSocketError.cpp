/* Copyright (C) 2026 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#include <Pt/Http/WebSocketError.h>

namespace Pt {

namespace Http {

WebSocketError::WebSocketError(unsigned code)
: System::IOError("WebSocket protocol error")
, _code(code)
{
}

} // namespace Http

} // namespace Pt
