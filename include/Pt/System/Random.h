/*
 * Copyright (C) 2026 Marc Boris Duerner
 * SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
 */

#ifndef PT_SYSTEM_RANDOM_H
#define PT_SYSTEM_RANDOM_H

#include <Pt/System/Api.h>
#include <Pt/NonCopyable.h>
#include <Pt/Types.h>
#include <cstddef>
#include <limits>
#include <stdexcept>

namespace Pt {

namespace System {

/** @brief Random bytes and integers from the operating system.

    %Random draws bytes from the operating system's cryptographically
    strong generator. Construct a %Random when the program needs nonces,
    masking keys, tokens, or unbiased integers. Each instance talks to
    the same kernel generator. The object is not a seeded sequence. It
    cannot be copied.

    get() fills a caller buffer with random bytes. get<T>() returns a
    value whose object representation is filled with random bytes.
    get(n) returns an unsigned integer in the half-open range [0, n)
    without modulo bias.

    A size of zero returns without writing. A null buffer with a
    nonzero size throws %std::invalid_argument. Failure of the
    operating system throws %SystemError from the constructor and from
    every method that draws bytes. Draws go to the kernel on every
    call, so a child process after fork does not inherit a userspace
    sequence.

    @ingroup Pt-System
*/
class PT_SYSTEM_API Random : private NonCopyable
{
    public:
        /** @brief Constructs a Random.
        */
        Random();

        /** @brief Destructor.
        */
        ~Random();

        /** @brief Fills @a buffer with @a n random bytes.

            @throw %std::invalid_argument if @a buffer is null and @a n
                   is nonzero.
        */
        void get(Pt::uint8_t* buffer, std::size_t n);

        /** @brief Returns a value of type @a T filled with random bytes.
        */
        template <typename T>
        T get()
        {
            T value = T();
            get(reinterpret_cast<Pt::uint8_t*>(&value), sizeof(value));
            return value;
        }

        /** @brief Returns an unsigned integer in the range [0, @a n).

            @throw %std::invalid_argument if @a n is 0.
        */
        template <typename T>
        T get(T n)
        {
            if(n == 0)
                throw std::invalid_argument("Random::get");

            if(n == 1)
                return 0;

            const T limit = (std::numeric_limits<T>::max() / n) * n;
            T x = 0;
            do
            {
                x = get<T>();
            }
            while(x >= limit);

            return x % n;
        }

    private:
        class RandomImpl* _impl;
};

} // namespace System

} // namespace Pt

#endif
