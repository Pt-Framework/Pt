/*
 * Copyright (C) 2026 Marc Boris Duerner
 * SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
 */

#ifndef PT_SYSTEM_RANDOM_IMPL_H
#define PT_SYSTEM_RANDOM_IMPL_H

#include <Pt/WinVer.h>
#include <Pt/Types.h>

#include <windows.h>
#include <bcrypt.h>
#include <cstddef>

namespace Pt {

namespace System {

class RandomImpl
{
    public:
        RandomImpl();

        ~RandomImpl();

        void get(Pt::uint8_t* buffer, std::size_t n);

    private:
        BCRYPT_ALG_HANDLE _alg;
};

} // namespace System

} // namespace Pt

#endif
