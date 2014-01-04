/*
 * Copyright (C) 2005-2013 Marc Boris Duerner
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <Pt/Utf8Codec.h>
#include <Pt/ConversionError.h>

#define halfShift uint32_t(10)
#define halfBase Pt::Char(0x0010000)
#define halfMask Pt::Char(0x3FF)
#define byteMask uint32_t(0xBF)
#define byteMark uint32_t(0x80)

namespace
{
    inline bool invalid_leading_octet(unsigned char octet_1)
    {
        return (0x7f < octet_1 && octet_1 < 0xc0) ||
          (octet_1 > 0xfd);
    }

    inline bool invalid_continuing_octet(unsigned char octet_1)  
    {
        return (octet_1 < 0x80|| 0xbf< octet_1);
    }

    inline unsigned int get_octet_count( unsigned char lead_octet)
    {
        // if the 0-bit (MSB) is 0, then 1 character
        if (lead_octet <= 0x7f) return 1;

        // Otherwise the count number of consecutive 1 bits starting at MSB
        //    assert(0xc0 <= lead_octet && lead_octet <= 0xfd);

        if      (0xc0 <= lead_octet && lead_octet <= 0xdf) return 2;
        else if (0xe0 <= lead_octet && lead_octet <= 0xef) return 3;
        else if (0xf0 <= lead_octet && lead_octet <= 0xf7) return 4;
        else if (0xf8 <= lead_octet && lead_octet <= 0xfb) return 5;
        else return 6;
    }

    inline unsigned int get_cont_octet_count(unsigned char lead_octet) 
    {
        return get_octet_count(lead_octet) - 1;
    }

} // namespace

namespace Pt {

const Pt::Char ReplacementChar = 0x0000FFFD;
const Pt::Char MaxBmp = 0x0000FFFF;
const Pt::Char MaxUtf32 = 0x7FFFFFFF;
const Pt::Char MaxLegalUtf32 = 0x0010FFFF;
const Pt::Char SurHighStart = 0xD800;
const Pt::Char SurHighEnd = 0xDBFF;
const Pt::Char SurLowStart = 0xDC00;
const Pt::Char SurLowEnd = 0xDFFF;
const Pt::Char ByteOrderMark = 0xFEFF;
const Pt::Char ByteOrderSwapped = 0xFFFE;

/*
 * Once the bits are split out into bytes of UTF-8, this is a mask OR-ed
 * into the first byte, depending on how many bytes follow.  There are
 * as many entries in this table as there are UTF-8 sequence types.
 * (I.e., one byte sequence, two byte... etc.). Remember that sequencs
 * for *legal* UTF-8 will be 4 or fewer bytes total.
 */
const uint8_t firstByteMark[7] = {
    0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC
};

Utf8Codec::Utf8Codec(std::size_t ref)
: Pt::TextCodec<Char, char>(ref)
{}

Utf8Codec::result Utf8Codec::do_in(MBState& s, const char* fromBegin, const char* fromEnd, const char*& fromNext,
                                   Pt::Char* toBegin, Pt::Char* toEnd, Pt::Char*& toNext) const
{
  while (fromBegin != fromEnd && toBegin != toEnd)
  {
      if( invalid_leading_octet(*fromBegin) )
      {
          fromNext = fromBegin;
          toNext = toBegin;
          return std::codecvt_base::error;
      }

      // The first octet is adjusted by a value dependent upon 
      // the number   of "continuing octets" encoding the character
      const int cont_octet_count = get_cont_octet_count(*fromBegin);
      
      const unsigned char octet1_modifier_table[] =   
      {
          0x00, 0xc0, 0xe0, 0xf0, 0xf8, 0xfc
      };

      // The unsigned char conversion is necessary in case char is
      // signed   (I learned this the hard way)
    
      unsigned ucs_result = 
          (unsigned char)(*fromBegin++) - octet1_modifier_table[cont_octet_count];

      // Invariants   : 
      //   1) At the start of the loop,   'i' continuing characters have been
      //    processed 
      //   2) *from   points to the next continuing character to be processed.
      int i = 0;
      while(i != cont_octet_count && fromBegin != fromEnd)
      {
          // Error checking on continuing characters
          if( invalid_continuing_octet(*fromBegin) )
          {
              fromNext = fromBegin;
              toNext = toBegin;
              return std::codecvt_base::error;
          }

          ucs_result *= (1 << 6);

          // each continuing character has an extra (10xxxxxx)b attached to 
          // it that must be removed.
          ucs_result += (unsigned char)(*fromBegin++) - 0x80;
          ++i;
      }

      // If the buffer ends with an incomplete unicode character...
      if (fromBegin == fromEnd && i != cont_octet_count)
      {
          // rewind "from" to before the current character translation
          fromNext = fromBegin - (i+1); 
          toNext = toBegin;
          return std::codecvt_base::partial;
      }
    
      *toBegin++   = ucs_result;
  }
  
  fromNext = fromBegin;
  toNext = toBegin;

  // Were we done converting or did we run out of destination space?
  if(fromBegin == fromEnd) 
      return std::codecvt_base::ok;
  else 
      return std::codecvt_base::partial;
}


Utf8Codec::result Utf8Codec::do_out(MBState& s, const Pt::Char* fromBegin, const Pt::Char* fromEnd, const Pt::Char*& fromNext,
                                                char* toBegin, char* toEnd, char*& toNext) const
{
    result retstat = ok;
    fromNext  = fromBegin;
    toNext = toBegin;
    
    std::size_t bytesToWrite;

    while(fromNext < fromEnd) 
    {
        Pt::uint32_t ch = fromNext->value();

        if (ch >= SurHighStart && ch <= SurLowEnd) 
        {
            retstat = error;
            break;
        }

        // Figure out how many bytes the result will require. Turn any
        // illegally large UTF32 things (> Plane 17) into replacement chars.
        if (ch < 0x80) 
        {
            bytesToWrite = 1;
        }
        else if (ch < 0x800) 
        {
            bytesToWrite = 2;
        }
        else if (ch < 0x10000) 
        {
            bytesToWrite = 3;
        }
        else if (ch <= MaxLegalUtf32) 
        {
            bytesToWrite = 4;
        }
        else 
        {
            bytesToWrite = 3;
            ch = ReplacementChar.value();
        }

        uint8_t* current = (uint8_t*)(toNext + bytesToWrite);
        if( current >= (uint8_t*)(toEnd) ) {
            retstat = partial;
            break;
        }

        switch(bytesToWrite) 
        { // note: everything falls through...
            case 4: *--current = (uint8_t)((ch | byteMark) & byteMask); ch >>= 6;
            case 3: *--current = (uint8_t)((ch | byteMark) & byteMask); ch >>= 6;
            case 2: *--current = (uint8_t)((ch | byteMark) & byteMask); ch >>= 6;
            case 1: *--current = (uint8_t) (ch | firstByteMark[bytesToWrite]);
        }

        toNext += bytesToWrite;
        ++fromNext;
    }

    return retstat;
}

int Utf8Codec::do_length(MBState& s, const char* fromBegin, const char* fromEnd, std::size_t max) const
{
    int last_octet_count = 0;
    std::size_t char_count = 0;
    const char* from_next = fromBegin;
    
    while(from_next + last_octet_count <= fromEnd && char_count <= max) 
    {
        from_next += last_octet_count;
        last_octet_count = (get_octet_count(*from_next));
        ++char_count;
    }
    
    return static_cast<int>(from_next-fromBegin);
}


int Utf8Codec::do_max_length() const throw()
{
    return 6;
}


bool Utf8Codec::do_always_noconv() const throw()
{
    return false;
}


std::codecvt_base::result Utf8Codec::do_unshift(Pt::MBState&, char*, char*, char*&) const
{ 
    return std::codecvt_base::noconv; 
}


int Utf8Codec::do_encoding() const throw()
{ 
    return 0; 
}

String Utf8Codec::decode(const char* data, std::size_t size)
{
    Utf8Codec codec;

    Char to[64];
    MBState state;
    String ret;
    const char* from = data;

    result r;
    do
    {
        Char* to_next = to;

        const char* from_next = from;
        r = codec.in(state, from, from + size, from_next, to, &to[sizeof(to) / sizeof(Char)], to_next);

        if (r == error)
            throw ConversionError("character encoding");

        if (r == partial && from_next == from)
            throw ConversionError("character encoding");

        ret.append(to, to_next);

        size -= (from_next - from);
        from = from_next;

    }
    while (r == partial);

    return ret;
}

std::string Utf8Codec::encode(const Char* data, std::size_t size)
{
    Utf8Codec codec;
    char to[64];
    MBState state;
    
    result r;
    const Char* from = data;
    std::string ret;

    do
    {
        const Char* from_next;

        char* to_next = to;
        r = codec.out(state, from, from + size, from_next, to, to + sizeof(to), to_next);

        if (r == error)
            throw ConversionError("character encoding");

        ret.append(to, to_next);

        size -= (from_next - from);
        from = from_next;

    } 
    while (r == partial);

    return ret;
}

} // namespace Pt
