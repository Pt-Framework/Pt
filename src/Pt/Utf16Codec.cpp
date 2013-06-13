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

  Utf16Codec::Utf16Codec(size_t ref)
    : Pt::TextCodec<Char, char>(ref)
    {}

 Utf16Codec::result Utf16Codec::do_in(MBState& s, 
                             const char* from, const char* fromEnd, const char*& fromNext,
                             Char* to, Char* toEnd, Char*& toNext) const
 {
		fromNext = from;
		toNext = to;

		for (; 2 <= fromEnd - fromNext && toNext != toEnd; )
			{	// convert a multibyte sequence
			unsigned char *_Ptr = (unsigned char *)fromNext;
      Pt::Char _Ch;
      Pt::uint16_t _Ch0, _Ch1;

			_Ch0 = (unsigned short)(_Ptr[0] << 8 | _Ptr[1]);

			if (_Ch0 < 0xd800 || 0xdc00 <= _Ch0)
				{	// one word, consume bytes
				fromNext += 2;
				_Ch = _Ch0;
				}
			else if (fromEnd - fromNext < 2 * 2)
				break;
			else
				{	// get second word

				  _Ch1 = (Pt::uint16_t)(_Ptr[2] << 8 | _Ptr[3]);

				if (_Ch1 < 0xdc00 || 0xe000 <= _Ch1)
					return (std::codecvt_base::error);

				fromNext += 2 * 2;

				_Ch = (Pt::uint32_t)(_Ch0 - 0xd800 + 0x0040) << 10 | (_Ch1 - 0xdc00);
				}

			if (MaxUtf16 < _Ch)
				return (std::codecvt_base::error);	// code too large

			*toNext++ = _Ch;
			}

		return (from == fromNext ? std::codecvt_base::partial : std::codecvt_base::ok);
 }

Utf16Codec::result Utf16Codec::do_out(MBState& s, const Char* fromBegin,
                                            const Char* fromEnd, const Char*& fromNext,
                                            char* toBegin, char* toEnd, char*& toNext) const
{
  // convert [fromBegin, fromEnd) to bytes [toBegin, _Last)
		char *_Pstate = (char *)&s;
		fromNext = fromBegin;
		toNext = toBegin;

		for (; fromNext != fromEnd && 2 <= toEnd - toNext; )
			{	// convert and put a wide char
			bool _Extra = false;
      Pt::uint32_t _Ch = *fromNext++;

			if (0x0010FFFF  < _Ch)
				return (std::codecvt_base::error);	// value too large

			if (_Ch <= 0xffff)
			{	// one word, can't be code for first of two
				if (0xd800 <= _Ch && _Ch < 0xdc00)
					return (std::codecvt_base::error);
			}
			else if (toEnd - toNext < 2 * 2)
				{	// not enough room for two-word output, back up
				--fromNext;
				return (std::codecvt_base::partial);
				}
			else
				_Extra = true;

			if (!_Extra)
				{	// put a single word MS byte first
				*toNext++ = (char)(_Ch >> 8);
				*toNext++ = (char)_Ch;
				}
			else
				{	// put a pair of words MS byte first
				unsigned short _Ch0 = (unsigned short)(0xd800
					| (unsigned short)(_Ch >> 10) - 0x0040);
				*toNext++ = (char)(_Ch0 >> 8);
				*toNext++ = (char)_Ch0;

				_Ch0 = (unsigned short)(0xdc00
					| (unsigned short)_Ch & 0x03ff);
				*toNext++ = (char)(_Ch0 >> 8);
				*toNext++ = (char)_Ch0;
				}
			}

		return (fromBegin == fromNext ? std::codecvt_base::partial : std::codecvt_base::ok);
}

int Utf16Codec::do_length(MBState& s, const char* fromBegin, const char* fromEnd, size_t max) const
{
  // return min(max, converted length of bytes [fromBegin, fromEnd))
  size_t _Wchars = 0;
  MBState _Mystate = s;

  for (; _Wchars < max && fromBegin != fromEnd; )
  {	// convert another wide char
    const char *_Mid1;
    Pt::Char *_Mid2;
    Pt::Char _Ch;

    switch (do_in(_Mystate, fromBegin, fromEnd, _Mid1, &_Ch, &_Ch + 1, _Mid2))
    {	// test result of single wide-char conversion
    case std::codecvt_base::noconv:
      return ((int)(_Wchars + (fromEnd - fromBegin)));

    case  std::codecvt_base::ok:
      if (_Mid2 == &_Ch + 1)
        ++_Wchars;	// replacement do_in might not convert one
      fromBegin = _Mid1;
      break;

    default:
      return ((int)_Wchars);	// error or partial
    }
  }

  return ((int)_Wchars);
}

}