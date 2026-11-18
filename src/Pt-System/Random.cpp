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


Pt::uint32_t Random::getUInt32(Pt::uint32_t n)
{
    if(n == 0)
        throw std::invalid_argument("Random::getUInt32");

    if(n == 1)
        return 0;

    const Pt::uint32_t limit = (0xffffffffu / n) * n;
    Pt::uint32_t x = 0;
    do
    {
        x = getUInt32();
    }
    while(x >= limit);

    return x % n;
}


Pt::uint64_t Random::getUInt64(Pt::uint64_t n)
{
    if(n == 0)
        throw std::invalid_argument("Random::getUInt64");

    if(n == 1)
        return 0;

    const Pt::uint64_t limit = (0xffffffffffffffffull / n) * n;
    Pt::uint64_t x = 0;
    do
    {
        x = getUInt64();
    }
    while(x >= limit);

    return x % n;
}

} // namespace System

} // namespace Pt
