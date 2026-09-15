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

#ifndef PT_API_TEXT_H
#define PT_API_TEXT_H

/** @addtogroup Pt-Text

    @brief Unicode characters, strings, encodings and regular expressions.

    This set of classes and functions extends the string and localization
    support of the C++ standard library to work with unicode characters and
    strings. A unicode character type and string class (a specialization of
    std::basic_string) can be used to hold unicode text. A set of functions
    allows to transform and classify individual characters. Text can be
    converted e.g. between different encodings using i/o streams and text
    codecs. A regular expression class allows to search and match patterns
    in unicode strings. Localization facets are available for the systems
    which support standard C++ locales.

    One of the most common standards for character encoding is the
    ASCII standard. Each character is encoded using 7 bits of a byte, so 128
    different characters can be addressed. Reading and writing ASCII
    characters is straightforward, because each character is stored in
    exactly one byte. The builtin C++ type @c char can be used to represent
    ASCII characters. The draw-back of ASCII, of course, is the small
    character set of only 128 characters. There are a lot more characters
    than that in the languages all around the world.

    This problem was addressed by the Unicode standard, which was created to
    make every known character of the world available in a single character
    table. Each character has a defined position in the table, a so-called
    code point. The unicode table contains 0x10FFFF entries at the moment, so
    a 32 bit type is required to represent a raw unicode character.

    The UTF-8 encoding was introduced to store unicode characters in byte
    sequences, which are compatible to classic null-terminated C strings. One
    unicode character is encoded into a byte sequence of 1 or more bytes.
    Further, the characters are encoded such that a character in 7-bit ASCII
    has the exact same value as in UTF-8, so any valid ASCII text is valid
    UTF-8 encoded text. This demonstrates the difference between encodings
    and character types. ASCII and UTF-8 can both be represented by
    sequences of the character type @c char, but their values are
    interpreted according to the encoding. Besides UTF-8 encoding, many more
    encodings have been developed, for example Latin-1, UTF-16 or, in the
    broadest sense, Base64.
*/

#endif
