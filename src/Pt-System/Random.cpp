/*
 * Copyright (C) 2026 Marc Boris Duerner
 * SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
 */

#include "RandomImpl.h"
#include <Pt/System/Random.h>
#include <stdexcept>

namespace Pt {

namespace System {

Random::Random()
: _impl(new RandomImpl())
{
}


Random::~Random()
{
    delete _impl;
}


void Random::get(Pt::uint8_t* buffer, std::size_t n)
{
    if(n == 0)
        return;

    if( ! buffer)
        throw std::invalid_argument("Random::get");

    _impl->get(buffer, n);
}

} // namespace System

} // namespace Pt
