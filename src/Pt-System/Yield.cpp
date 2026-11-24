/* Copyright (C) 2026 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#include <Pt/System/Yield.h>
#include <Pt/System/EventLoop.h>

#include <stdexcept>

namespace Pt {

namespace System {

Yield::Yield()
{
}


Yield::~Yield()
{
    // ~Selectable does not remove a posted request from the loop
    detach();
}


void Yield::setActive(EventLoop& loop)
{
    Selectable::setActive(loop);
}


void Yield::detach()
{
    Selectable::detach();
}


void Yield::cancel()
{
    Selectable::cancel();
}


EventLoop* Yield::loop() const
{
    return parent();
}


void Yield::run()
{
    if( ! parent() )
        throw std::logic_error("yield not active");

    post();
}


Signal<>& Yield::ready()
{
    return _ready;
}


void Yield::onCancel()
{
}


bool Yield::onRun()
{
    _ready.send();
    return true;
}

} // namespace System

} // namespace Pt
