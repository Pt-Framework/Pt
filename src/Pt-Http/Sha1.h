/*
 * Copyright (C) 2015 by Laurentiu-Gheorghe Crisan
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */
#ifndef PT_HTTP_SHA1_H
#define PT_HTTP_SHA1_H

#include <Pt/Http/Api.h>
#include <Pt/Byteorder.h>
#include <inttypes.h>
#include <vector>

namespace Pt {
namespace Http {

class PT_HTTP_API Sha1
{
    public:
        enum
        {
            BLOCK_SIZE = 64,
            DIGEST_SIZE = 20
        };

        Sha1();

        Sha1(const void* data, std::size_t length);

        ~Sha1();

        void reset(const void* data, std::size_t length);

        std::size_t digestLength() const
        {
            return DIGEST_SIZE;
        }

        void clear();

        const std::vector<uint8_t>& calc();

    private:
        void transform();

        // The SHA f()-functions
        inline static uint32_t f1(uint32_t x, uint32_t y, uint32_t z)
        {
            return ((x & y) | (~x & z)); //Rounds  0-19
        }

        inline static uint32_t f2(uint32_t x, uint32_t y, uint32_t z)
        {
            return (x ^ y ^ z); // Rounds 20-39
        }

        inline static uint32_t f3(uint32_t x, uint32_t y, uint32_t z)
        {
            return ((x & y) | (x & z) | (y & z));   // Rounds 40-59
        }

        inline static uint32_t f4(uint32_t x, uint32_t y, uint32_t z)
        {
            return (x ^ y ^ z); // Rounds 60-79
        }

        // 32-bit rotate - kludged with shifts
        inline static uint32_t S(uint32_t n, uint32_t X)
        {
            return ((((uint32_t)X) << n) | (((uint32_t)X) >> (32 - n)));
        }

        inline uint32_t expand(uint32_t count)
        {
            return _W[count] = S(1, (_W[count - 3] ^ _W[count - 8] ^ _W[count - 14] ^ _W[count - 16]));// to make this SHA-1
        }

        // The four SHA sub-rounds
        inline void subRound1(uint32_t count)
        { 
            _temp = S( 5, _A ) + f1( _B, _C, _D ) + _E + _W[ count ] + K1;
            _E = _D; 
            _D = _C;
            _C = S( 30, _B );
            _B = _A;
            _A = _temp;
        }

        inline void subRound2(uint32_t count)
        {
            _temp = S( 5, _A ) + f2( _B, _C, _D ) + _E + _W[ count ] + K2;
            _E = _D; 
            _D = _C; 
            _C = S( 30, _B );
            _B = _A;
            _A = _temp;
        }

        inline void subRound3(uint32_t count)
        { 
            _temp = S( 5, _A ) + f3( _B, _C, _D ) + _E + _W[ count ] + K3;
            _E = _D;
            _D = _C;
            _C = S( 30, _B );
            _B = _A;
            _A = _temp;
        }

        inline void subRound4(uint32_t count)
        {
            _temp = S( 5, _A ) + f4( _B, _C, _D ) + _E + _W[ count ] + K4;
            _E = _D;
            _D = _C;
            _C = S( 30, _B );
            _B = _A;
            _A = _temp;
        }

        inline void hostToBe(uint32_t* buffer, size_t size)
        {
    #ifdef PT_LE
            for (size_t i = 0; i < size; i++)
                buffer[i] = Pt::hostToBe(buffer[i]);
    #endif
        }

    private:
        // The SHA Mysterious Constants
        static const uint32_t K1;     // Rounds 0-19
        static const uint32_t K2;     // Rounds 20-39
        static const uint32_t K3;     // Rounds 40-59
        static const uint32_t K4;     // Rounds 60-79

        private:
        uint32_t _digest[5]; // Message digest
        uint32_t _countLo;   // 64-bit bit count
        uint32_t _countHi;
        uint32_t _data[16];  // SHA data buffer
        uint32_t _slop;      // # of bytes saved in data[]
        std::vector<uint8_t> _result;
        uint32_t _W[80];
        uint32_t _temp;
        uint32_t _A, _B, _C, _D, _E;
};

}}

#endif
