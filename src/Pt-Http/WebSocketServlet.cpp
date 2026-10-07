/* Copyright (C) 2026 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#include <Pt/Http/WebSocketServlet.h>
#include <Pt/Http/WebSocketService.h>

namespace Pt {

namespace Http {

WebSocketServlet::WebSocketServlet(WebSocketService& service)
: _service(&service)
{
    _service->activate(*this);
}


WebSocketServlet::~WebSocketServlet()
{
    if(_service)
        _service->deactivate();
}

} // namespace Http

} // namespace Pt
