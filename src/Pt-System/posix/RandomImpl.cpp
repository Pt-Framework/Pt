/*
 * Copyright (C) 2026 Marc Boris Duerner
 * SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
 */

#include "RandomImpl.h"

#include <Pt/System/SystemError.h>

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#if defined(WITH_GETENTROPY_SYS_RANDOM) || defined(WITH_GETRANDOM)
#include <sys/random.h>
#endif

namespace Pt {

namespace System {

RandomImpl::RandomImpl()
{
}


RandomImpl::~RandomImpl()
{
}


#ifdef WITH_GETENTROPY

void RandomImpl::get(Pt::uint8_t* buffer, std::size_t n)
{
    while(n != 0)
    {
        std::size_t chunk = n > 256 ? 256 : n;
        if(getentropy(buffer, chunk) != 0)
            throw SystemError("getentropy");

        buffer += chunk;
        n -= chunk;
    }
}

#elif defined(WITH_GETRANDOM)

void RandomImpl::get(Pt::uint8_t* buffer, std::size_t n)
{
    while(n != 0)
    {
        ssize_t r = getrandom(buffer, n, 0);
        if(r < 0)
        {
            if(errno == EINTR)
                continue;

            throw SystemError("getrandom");
        }

        buffer += static_cast<std::size_t>(r);
        n -= static_cast<std::size_t>(r);
    }
}

#else

namespace {

class RandomDevice
{
    public:
        static RandomDevice& instance()
        {
            static RandomDevice self;
            return self;
        }

        void get(Pt::uint8_t* buffer, std::size_t n)
        {
            while(n != 0)
            {
                ssize_t r = ::read(_fd, buffer, n);
                if(r < 0)
                {
                    if(errno == EINTR)
                        continue;

                    throw SystemError("read /dev/urandom");
                }

                if(r == 0)
                    throw SystemError("read /dev/urandom");

                buffer += static_cast<std::size_t>(r);
                n -= static_cast<std::size_t>(r);
            }
        }

    private:
        RandomDevice()
        : _fd(::open("/dev/urandom", O_RDONLY))
        {
            if(_fd < 0)
                throw SystemError("open /dev/urandom");
        }

        RandomDevice(const RandomDevice&) = delete;

        RandomDevice& operator=(const RandomDevice&) = delete;

    private:
        int _fd;
};

}

void RandomImpl::get(Pt::uint8_t* buffer, std::size_t n)
{
    RandomDevice::instance().get(buffer, n);
}

#endif

} // namespace System

} // namespace Pt
