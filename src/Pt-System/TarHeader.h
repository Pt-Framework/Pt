/* Copyright (C) 2008 Marc Boris Duerner

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  As a special exception, you may use this file as part of a free
  software library without restriction. Specifically, if other files
  instantiate templates or use macros or inline functions from this
  file, or you compile this file and link it with other files to
  produce an executable, this file does not by itself cause the
  resulting executable to be covered by the GNU General Public
  License. This exception does not however invalidate any other
  reasons why the executable file might be covered by the GNU Library
  General Public License.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
  MA 02110-1301 USA
*/

#ifndef PT_SYSTEM_TAR_HEADER_H
#define PT_SYSTEM_TAR_HEADER_H

#include <Pt/DateTime.h>

#include <cassert>
#include <cstddef>
#include <ctime>

namespace Pt {

namespace System {

// UStar header block layout (512 bytes, POSIX.1-1988)
struct UStarHeader
{
    char name[100];      //   0
    char mode[8];        // 100
    char uid[8];         // 108
    char gid[8];         // 116
    char size[12];       // 124
    char mtime[12];      // 136
    char checksum[8];    // 148
    char typeflag;       // 156
    char linkname[100];  // 157
    char magic[6];       // 257
    char version[2];     // 263
    char uname[32];      // 265
    char gname[32];      // 297
    char devmajor[8];    // 329
    char devminor[8];    // 337
    char prefix[155];    // 345
    char pad[12];        // 500
};                       // 512

static_assert(sizeof(UStarHeader) == 512, "UStarHeader must be 512 bytes");


inline std::size_t tarParseOctal(const char* field, std::size_t len)
{
    std::size_t result = 0;
    const char* end = field + len;
    while(field < end && (*field == ' ' || *field == '\0'))
        ++field;
    while(field < end && *field >= '0' && *field <= '7')
    {
        result = result * 8u + static_cast<std::size_t>(*field - '0');
        ++field;
    }
    return result;
}


inline bool tarIsNullBlock(const char* block)
{
    for(int i = 0; i < 512; ++i)
    {
        if(block[i] != '\0')
            return false;
    }
    return true;
}


inline bool tarVerifyChecksum(const char* block)
{
    unsigned long stored =
        static_cast<unsigned long>(tarParseOctal(block + 148, 8));
    unsigned long sum = 0;
    for(int i = 0; i < 512; ++i)
    {
        if(i >= 148 && i < 156)
            sum += 32ul;
        else
            sum += static_cast<unsigned long>(
                       static_cast<unsigned char>(block[i]));
    }
    return sum == stored;
}


inline std::size_t tarFieldLen(const char* field, std::size_t maxLen)
{
    std::size_t n = 0;
    while(n < maxLen && field[n] != '\0')
        ++n;
    return n;
}


// Convert Unix epoch seconds to Pt::DateTime (UTC).
inline Pt::DateTime tarEpochToDatetime(time_t t)
{
    const std::tm* gm = std::gmtime(&t);
    if( ! gm )
        return Pt::DateTime(1970, 1, 1);
    return Pt::DateTime(gm->tm_year + 1900,
                        static_cast<unsigned>(gm->tm_mon + 1),
                        static_cast<unsigned>(gm->tm_mday),
                        static_cast<unsigned>(gm->tm_hour),
                        static_cast<unsigned>(gm->tm_min),
                        static_cast<unsigned>(gm->tm_sec));
}


// Convert Pt::DateTime (UTC) to Unix epoch seconds.
// Uses Howard Hinnant's days_from_civil algorithm.
inline time_t tarDatetimeToEpoch(const Pt::DateTime& dt)
{
    int y = dt.year();
    unsigned m = dt.month();
    unsigned d = dt.day();

    if(m <= 2u)
    {
        --y;
        m += 9u;
    }
    else
    {
        m -= 3u;
    }

    long long era = (y >= 0 ? y : y - 399) / 400;
    unsigned yoe = static_cast<unsigned>(y - era * 400);
    unsigned doy = (153u * m + 2u) / 5u + d - 1u;
    unsigned doe = yoe * 365u + yoe / 4u - yoe / 100u + doy;
    long long days = era * 146097LL + static_cast<long long>(doe) - 719468LL;

    return static_cast<time_t>(
          days * 86400LL
        + static_cast<long long>(dt.hour())   * 3600LL
        + static_cast<long long>(dt.minute()) * 60LL
        + static_cast<long long>(dt.second()));
}

} // namespace System

} // namespace Pt

#endif // include guard
