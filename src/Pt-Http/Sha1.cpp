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
#include "Sha1.h"
#include <cstring>

namespace Pt {
namespace Http {

const uint32_t Sha1::K1 = 0x5A827999L;     // Rounds  0-19
const uint32_t Sha1::K2 = 0x6ED9EBA1L;     // Rounds 20-39
const uint32_t Sha1::K3 = 0x8F1BBCDCL;     // Rounds 40-59
const uint32_t Sha1::K4 = 0xCA62C1D6L;     // Rounds 60-79

Sha1::Sha1()
{
    _result.reserve(16);
    clear();
}


Sha1::Sha1(const void* data, std::size_t length)
{
    _result.reserve(16);
    reset(data, length);
}

Sha1::~Sha1()
{
}

void Sha1::reset(const void* buffer_, std::size_t count)
{
    clear();

    const uint8_t* buffer = (const uint8_t*)buffer_;
    uint8_t* db = (uint8_t*)&_data[0];

    if ((_countLo + ((uint32_t)count << 3)) < _countLo)
        _countHi++;

    _countLo += ((uint32_t)count << 3);
    _countHi += ((uint32_t)count >> 29);


    while (count-- > 0)
    {
        db[_slop++] = *(buffer++);

        if (_slop == BLOCK_SIZE)
        {
            // transform this one block
            hostToBe(_data, BLOCK_SIZE/4);
            transform();
            _slop = 0;	/* no slop left */
        }
    }
}

void Sha1::clear()
{
    _digest[0] = 0x67452301L;
    _digest[1] = 0xEFCDAB89L;
    _digest[2] = 0x98BADCFEL;
    _digest[3] = 0x10325476L;
    _digest[4] = 0xC3D2E1F0L;
    _countLo = 0;
    _countHi = 0;
    _slop = 0;
    std::memset(_data, 0, sizeof(_data));
}

const std::vector<uint8_t>& Sha1::calc()
{
    int count;
    uint32_t lowBitcount = _countLo;
    uint32_t highBitcount = _countHi;

    count = (int)((_countLo >> 3) & 0x3F);

    ((uint8_t*)_data)[count++] = 0x80;

    if (count > 56)
    {
        std::memset((uint8_t*)&_data + count, 0, 64 - count);
        hostToBe(_data, BLOCK_SIZE/4);
        transform();

        std::memset(&_data, 0, 56);
    }
    else
    {
        std::memset((uint8_t*)&_data + count, 0, 56 - count);
    }

    hostToBe(_data, BLOCK_SIZE/4);

    _data[14] = highBitcount;
    _data[15] = lowBitcount;

    transform();
    hostToBe(_data, DIGEST_SIZE/4);

    unsigned char hash[DIGEST_SIZE];
    for (count = 0; count < DIGEST_SIZE; count++)
        hash[count] = (uint8_t)((_digest[count >> 2]) >> (8 * (3 - (count & 0x3)))) & 0xff;

    _result.clear();
    _result.insert(_result.begin(), hash, hash + DIGEST_SIZE);
    clear();
    return _result;
}

void Sha1::transform()
{
    for (int i = 0; i < 16; i++)
        _W[i] = _data[i];

    // Step B.  Expand the 16 words into 64 temporary data words
    expand(16); expand(17); expand(18); expand(19); expand(20);
    expand(21); expand(22); expand(23); expand(24); expand(25);
    expand(26); expand(27); expand(28); expand(29); expand(30);
    expand(31); expand(32); expand(33); expand(34); expand(35);
    expand(36); expand(37); expand(38); expand(39); expand(40);
    expand(41); expand(42); expand(43); expand(44); expand(45);
    expand(46); expand(47); expand(48); expand(49); expand(50);
    expand(51); expand(52); expand(53); expand(54); expand(55);
    expand(56); expand(57); expand(58); expand(59); expand(60);
    expand(61); expand(62); expand(63); expand(64); expand(65);
    expand(66); expand(67); expand(68); expand(69); expand(70);
    expand(71); expand(72); expand(73); expand(74); expand(75);
    expand(76); expand(77); expand(78); expand(79);

    // Step C.  Set up first buffer
    _A = _digest[0];
    _B = _digest[1];
    _C = _digest[2];
    _D = _digest[3];
    _E = _digest[4];

    // Step D.  Serious mangling, divided into four sub-rounds
    subRound1(0); subRound1(1); subRound1(2); subRound1(3);
    subRound1(4); subRound1(5); subRound1(6); subRound1(7);
    subRound1(8); subRound1(9); subRound1(10); subRound1(11);
    subRound1(12); subRound1(13); subRound1(14); subRound1(15);
    subRound1(16); subRound1(17); subRound1(18); subRound1(19);
    subRound2(20); subRound2(21); subRound2(22); subRound2(23);
    subRound2(24); subRound2(25); subRound2(26); subRound2(27);
    subRound2(28); subRound2(29); subRound2(30); subRound2(31);
    subRound2(32); subRound2(33); subRound2(34); subRound2(35);
    subRound2(36); subRound2(37); subRound2(38); subRound2(39);
    subRound3(40); subRound3(41); subRound3(42); subRound3(43);
    subRound3(44); subRound3(45); subRound3(46); subRound3(47);
    subRound3(48); subRound3(49); subRound3(50); subRound3(51);
    subRound3(52); subRound3(53); subRound3(54); subRound3(55);
    subRound3(56); subRound3(57); subRound3(58); subRound3(59);
    subRound4(60); subRound4(61); subRound4(62); subRound4(63);
    subRound4(64); subRound4(65); subRound4(66); subRound4(67);
    subRound4(68); subRound4(69); subRound4(70); subRound4(71);
    subRound4(72); subRound4(73); subRound4(74); subRound4(75);
    subRound4(76); subRound4(77); subRound4(78); subRound4(79);

    // Step E.  Build message digest
    _digest[0] += _A;
    _digest[1] += _B;
    _digest[2] += _C;
    _digest[3] += _D;
    _digest[4] += _E;
}


}}
