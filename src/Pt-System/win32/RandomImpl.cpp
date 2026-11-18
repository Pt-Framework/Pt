/*
 * Copyright (C) 2026 Marc Boris Duerner
 * SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
 */

#include "RandomImpl.h"
#include <Pt/System/SystemError.h>
#include <climits>

namespace Pt {

namespace System {

RandomImpl::RandomImpl()
: _alg(0)
{
    NTSTATUS st = BCryptOpenAlgorithmProvider(&_alg, BCRYPT_RNG_ALGORITHM, NULL, 0);
    if( ! BCRYPT_SUCCESS(st) )
        throw SystemError("BCryptOpenAlgorithmProvider");
}


RandomImpl::~RandomImpl()
{
    if(_alg)
        BCryptCloseAlgorithmProvider(_alg, 0);
}


void RandomImpl::get(Pt::uint8_t* buffer, std::size_t n)
{
    while(n != 0)
    {
        ULONG chunk = n > ULONG_MAX ? ULONG_MAX
                                    : static_cast<ULONG>(n);

        NTSTATUS st = BCryptGenRandom(_alg, buffer, chunk, 0);
        if( ! BCRYPT_SUCCESS(st) )
            throw SystemError("BCryptGenRandom");

        buffer += chunk;
        n -= chunk;
    }
}

} // namespace System

} // namespace Pt
