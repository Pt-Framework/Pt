/*
 * Copyright (C) 2005 by Marc Boris Duerner
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

#include<Pt/Utf16Codec.h>

const Pt::Char MaxUtf16 = 0x0010FFFF;

namespace Pt {

  Utf16BeCodec::Utf16BeCodec(size_t ref)
    : Pt::TextCodec<Char, char>(ref)
    {}

  Utf16BeCodec::result Utf16BeCodec::do_in(MBState& s, 
                             const char* from, const char* fromEnd, const char*& fromNext,
                             Char* to, Char* toEnd, Char*& toNext) const
  {
    fromNext = from;
    toNext = to;

    for(; 2 <= fromEnd - fromNext && toNext != toEnd; )
    {	// convert a multibyte sequence
      unsigned char *Ptr = (unsigned char *)fromNext;
      Pt::Char Ch;
      Pt::uint16_t Ch0, Ch1;

      Ch0 = (unsigned short)(Ptr[0] << 8 | Ptr[1]);

      if (Ch0 < 0xd800 || 0xdc00 <= Ch0)
      {	// one word, consume bytes
        fromNext += 2;
        Ch = Ch0;
      }
      else if (fromEnd - fromNext < 2 * 2)
        break;
      else
      {	// get second word
        Ch1 = (Pt::uint16_t)(Ptr[2] << 8 | Ptr[3]);

        if (Ch1 < 0xdc00 || 0xe000 <= Ch1)
          return (std::codecvt_base::error);

        fromNext += 2 * 2;

        Ch = (Pt::uint32_t)(Ch0 - 0xd800 + 0x0040) << 10 | (Ch1 - 0xdc00);
      }

      if (MaxUtf16 < Ch)
        return (std::codecvt_base::error);	// code too large

      *toNext++ = Ch;
    }

    return (fromNext != fromEnd ? std::codecvt_base::partial : std::codecvt_base::ok);
  }

Utf16BeCodec::result Utf16BeCodec::do_out(MBState& s, const Char* fromBegin,
                                            const Char* fromEnd, const Char*& fromNext,
                                            char* toBegin, char* toEnd, char*& toNext) const
{
  // convert [fromBegin, fromEnd) to bytes [toBegin, _Last)
  fromNext = fromBegin;
  toNext = toBegin;

  for(; fromNext != fromEnd && 2 <= toEnd - toNext; )
  {	// convert and put a wide char
    bool Extra = false;
    Pt::uint32_t Ch = *fromNext++;

    if(0x0010FFFF  < Ch)
      return (std::codecvt_base::error);	// value too large

    if(Ch <= 0xffff)
    {	// one word, can't be code for first of two
      if (0xd800 <= Ch && Ch < 0xdc00)
        return (std::codecvt_base::error);
    }
    else if (toEnd - toNext < 2 * 2)
    {	// not enough room for two-word output, back up
      --fromNext;
      return (std::codecvt_base::partial);
    }
    else
      Extra = true;

    if (!Extra)
    {	// put a single word MS byte first
      *toNext++ = (char)(Ch >> 8);
      *toNext++ = (char)Ch;
    }
    else
    {	// put a pair of words MS byte first
      unsigned short Ch0 = (unsigned short)(0xd800 | (unsigned short)(Ch >> 10) - 0x0040);
      *toNext++ = (char)(Ch0 >> 8);
      *toNext++ = (char)Ch0;

      Ch0 = (unsigned short)(0xdc00 | (unsigned short)Ch & 0x03ff);
      *toNext++ = (char)(Ch0 >> 8);
      *toNext++ = (char)Ch0;
    }
  }
  return (fromBegin == fromNext ? std::codecvt_base::partial : std::codecvt_base::ok);
}

int Utf16BeCodec::do_length(MBState& s, const char* fromBegin, const char* fromEnd, size_t max) const
{
  // return min(max, converted length of bytes [fromBegin, fromEnd))
  size_t Wchars = 0;
  MBState Mystate = s;

  for(; Wchars < max && fromBegin != fromEnd; )
  {	// convert another wide char
    const char *Mid1;
    Pt::Char *Mid2;
    Pt::Char Ch;

    switch(do_in(Mystate, fromBegin, fromEnd, Mid1, &Ch, &Ch + 1, Mid2))
    {	// test result of single wide-char conversion
    case std::codecvt_base::noconv:
      return ((int)(Wchars + (fromEnd - fromBegin)));

    case  std::codecvt_base::ok:
      if (Mid2 == &Ch + 1)
        ++Wchars;	// replacement do_in might not convert one
      fromBegin = Mid1;
      break;

    default:
      return ((int)Wchars);	// error or partial
    }
  }
  return ((int)Wchars);
}


// Little Endian
Utf16LeCodec::Utf16LeCodec(size_t ref)
   : Pt::TextCodec<Char, char>(ref)
   {}

Utf16LeCodec::result Utf16LeCodec::do_in(MBState& s, 
                     const char* from, const char* fromEnd, const char*& fromNext,
                     Char* to, Char* toEnd, Char*& toNext) const
 {
   fromNext = from;
   toNext = to;

   for (; 2 <= fromEnd - fromNext && toNext != toEnd; )
   {	// convert a multibyte sequence
     unsigned char *Ptr = (unsigned char *)fromNext;
     Pt::Char Ch;
     Pt::uint16_t Ch0, Ch1;

     Ch0 = (unsigned short)(Ptr[1] << 8 | Ptr[0]);

     if (Ch0 < 0xd800 || 0xdc00 <= Ch0)
     {	// one word, consume bytes
       fromNext += 2;
       Ch = Ch0;
     }
     else if (fromEnd - fromNext < 2 * 2)
       break;
     else
     {	// get second word
       Ch1 = (Pt::uint16_t)(Ptr[3] << 8 | Ptr[2]);

       if (Ch1 < 0xdc00 || 0xe000 <= Ch1)
         return (std::codecvt_base::error);

       fromNext += 2 * 2;

       Ch = (Pt::uint32_t)(Ch0 - 0xd800 + 0x0040) << 10 | (Ch1 - 0xdc00);
     }

     if (MaxUtf16 < Ch)
       return (std::codecvt_base::error);	// code too large

     *toNext++ = Ch;
   }
   return (fromNext != fromEnd ? std::codecvt_base::partial : std::codecvt_base::ok);
 }

Utf16LeCodec::result Utf16LeCodec::do_out(MBState& s, const Char* fromBegin,
                                            const Char* fromEnd, const Char*& fromNext,
                                            char* toBegin, char* toEnd, char*& toNext) const
{
  // convert [fromBegin, fromEnd) to bytes [toBegin, _Last)
  fromNext = fromBegin;
  toNext = toBegin;

  for (; fromNext != fromEnd && 2 <= toEnd - toNext; )
  {	// convert and put a wide char
    bool Extra = false;
    Pt::uint32_t Ch = *fromNext++;

    if (0x0010FFFF  < Ch)
      return (std::codecvt_base::error);	// value too large

    if (Ch <= 0xffff)
    {	// one word, can't be code for first of two
      if (0xd800 <= Ch && Ch < 0xdc00)
        return (std::codecvt_base::error);
    }
    else if (toEnd - toNext < 2 * 2)
    {	// not enough room for two-word output, back up
      --fromNext;
      return (std::codecvt_base::partial);
    }
    else
      Extra = true;

    if (!Extra)
    {	// put a single word LS byte first
      *toNext++ = (char)Ch;
      *toNext++ = (char)(Ch >> 8);
    }
    else
    {	// put a pair of words LS byte first
      unsigned short Ch0 = (unsigned short)(0xd800 | (unsigned short)(Ch >> 10) - 0x0040);
      *toNext++ = (char)Ch0;
      *toNext++ = (char)(Ch0 >> 8);

      Ch0 = (unsigned short)(0xdc00 | (unsigned short)Ch & 0x03ff);
      *toNext++ = (char)Ch0;
      *toNext++ = (char)(Ch0 >> 8);
    }
  }
		return (fromBegin == fromNext ? std::codecvt_base::partial : std::codecvt_base::ok);
}

int Utf16LeCodec::do_length(MBState& s, const char* fromBegin, const char* fromEnd, size_t max) const
{
  // return min(max, converted length of bytes [fromBegin, fromEnd))
  size_t Wchars = 0;
  MBState Mystate = s;

  for (; Wchars < max && fromBegin != fromEnd; )
  {	// convert another wide char
    const char *Mid1;
    Pt::Char *Mid2;
    Pt::Char Ch;

    switch (do_in(Mystate, fromBegin, fromEnd, Mid1, &Ch, &Ch + 1, Mid2))
    {	// test result of single wide-char conversion
    case std::codecvt_base::noconv:
      return ((int)(Wchars + (fromEnd - fromBegin)));

    case  std::codecvt_base::ok:
      if (Mid2 == &Ch + 1)
        ++Wchars;	// replacement do_in might not convert one
      fromBegin = Mid1;
      break;

    default:
      return ((int)Wchars);	// error or partial
    }
  }

  return ((int)Wchars);
}

}
