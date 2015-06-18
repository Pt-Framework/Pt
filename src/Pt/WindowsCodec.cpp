/*
 * Copyright (C) 2005-2015 by Dr. Marc Boris Duerner
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

#include <Pt/WindowsCodec.h>

namespace Pt {

/////////////////////////////////////////////////////////////////////////////
// Windows1252Codec
/////////////////////////////////////////////////////////////////////////////

Windows1252Codec::Windows1252Codec(std::size_t ref)
: TextCodec<Char, char>(ref)
{}


Windows1252Codec::~Windows1252Codec()
{}


Pt::Char fromCP1252(uint8_t n)
{
  static const Pt::uint32_t cp1252ToUnicode[] = 
  {
      0x0000,0x0001,0x0002,0x0003,0x0004,0x0005,0x0006,0x0007,
      0x0008,0x0009,0x000A,0x000B,0x000C,0x000D,0x000E,0x000F,
      0x0010,0x0011,0x0012,0x0013,0x0014,0x0015,0x0016,0x0017,
      0x0018,0x0019,0x001A,0x001B,0x001C,0x001D,0x001E,0x001F,
      0x0020,0x0021,0x0022,0x0023,0x0024,0x0025,0x0026,0x0027,
      0x0028,0x0029,0x002A,0x002B,0x002C,0x002D,0x002E,0x002F,
      0x0030,0x0031,0x0032,0x0033,0x0034,0x0035,0x0036,0x0037,
      0x0038,0x0039,0x003A,0x003B,0x003C,0x003D,0x003E,0x003F,
      0x0040,0x0041,0x0042,0x0043,0x0044,0x0045,0x0046,0x0047,
      0x0048,0x0049,0x004A,0x004B,0x004C,0x004D,0x004E,0x004F,
      0x0050,0x0051,0x0052,0x0053,0x0054,0x0055,0x0056,0x0057,
      0x0058,0x0059,0x005A,0x005B,0x005C,0x005D,0x005E,0x005F,
      0x0060,0x0061,0x0062,0x0063,0x0064,0x0065,0x0066,0x0067,
      0x0068,0x0069,0x006A,0x006B,0x006C,0x006D,0x006E,0x006F,
      0x0070,0x0071,0x0072,0x0073,0x0074,0x0075,0x0076,0x0077,
      0x0078,0x0079,0x007A,0x007B,0x007C,0x007D,0x007E,0x007F, // ASCII
      0x20AC,0x81  ,0x201A,0x0192,0x201E,0x2026,0x2020,0x2021,
      0x02C6,0x2030,0x0160,0x2039,0x0152,0x8D  ,0x017D,0x8F  ,
      0x90  ,0x2018,0x2019,0x201C,0x201D,0x2022,0x2013,0x2014,
      0x02DC,0x2122,0x0161,0x203A,0x0153,0x9D  ,0x017E,0x0178,
      0x00A0,0x00A1,0x00A2,0x00A3,0x00A4,0x00A5,0x00A6,0x00A7, // Latin-1
      0x00A8,0x00A9,0x00AA,0x00AB,0x00AC,0x00AD,0x00AE,0x00AF,
      0x00B0,0x00B1,0x00B2,0x00B3,0x00B4,0x00B5,0x00B6,0x00B7,
      0x00B8,0x00B9,0x00BA,0x00BB,0x00BC,0x00BD,0x00BE,0x00BF,
      0x00C0,0x00C1,0x00C2,0x00C3,0x00C4,0x00C5,0x00C6,0x00C7,
      0x00C8,0x00C9,0x00CA,0x00CB,0x00CC,0x00CD,0x00CE,0x00CF,
      0x00D0,0x00D1,0x00D2,0x00D3,0x00D4,0x00D5,0x00D6,0x00D7,
      0x00D8,0x00D9,0x00DA,0x00DB,0x00DC,0x00DD,0x00DE,0x00DF,
      0x00E0,0x00E1,0x00E2,0x00E3,0x00E4,0x00E5,0x00E6,0x00E7,
      0x00E8,0x00E9,0x00EA,0x00EB,0x00EC,0x00ED,0x00EE,0x00EF,
      0x00F0,0x00F1,0x00F2,0x00F3,0x00F4,0x00F5,0x00F6,0x00F7,
      0x00F8,0x00F9,0x00FA,0x00FB,0x00FC,0x00FD,0x00FE,0x00FF 
  };

  return cp1252ToUnicode[n];
}


uint8_t toCP1252(Pt::Char ch)
{
  if( ch < 0x80 || (ch > 0xA0 && ch <= 0xFF) )
  {
    return static_cast<uint8_t>( ch.value() );
  }

  uint8_t r = 0x3F; // question mark
  
  switch( ch.value() )
  {
      case 0x20AC: r = 0x80; break;
      case 0x81  : r = 0x81; break;
      case 0x201A: r = 0x82; break;
      case 0x0192: r = 0x83; break;
      case 0x201E: r = 0x84; break;
      case 0x2026: r = 0x85; break;
      case 0x2020: r = 0x86; break;
      case 0x2021: r = 0x87; break;
      case 0x02C6: r = 0x88; break;
      case 0x2030: r = 0x89; break;
      case 0x0160: r = 0x8a; break;
      case 0x2039: r = 0x8b; break;
      case 0x0152: r = 0x8c; break;
      case 0x8D  : r = 0x8d; break;
      case 0x017D: r = 0x8e; break;
      case 0x8F  : r = 0x8f; break;
      case 0x90  : r = 0x90; break;
      case 0x2018: r = 0x91; break;
      case 0x2019: r = 0x92; break;
      case 0x201C: r = 0x93; break;
      case 0x201D: r = 0x94; break;
      case 0x2022: r = 0x95; break;
      case 0x2013: r = 0x96; break;
      case 0x2014: r = 0x97; break;
      case 0x02DC: r = 0x98; break;
      case 0x2122: r = 0x99; break;
      case 0x0161: r = 0x9a; break;
      case 0x203A: r = 0x9b; break;
      case 0x0153: r = 0x9c; break;
      case 0x9D  : r = 0x9d; break;
      case 0x017E: r = 0x9e; break;
      case 0x0178: r = 0x9f; break;
      default:     r = 0x3F; break;
  }

  return r;
}


Windows1252Codec::result Windows1252Codec::do_in(MBState& s, 
                                                 const char* fromBegin, 
                                                 const char* fromEnd, 
                                                 const char*& fromNext,
                                                 Char* toBegin, 
                                                 Char* toEnd, 
                                                 Char*& toNext) const
{
    fromNext = fromBegin;
    toNext = toBegin;

    while( fromNext != fromEnd && toNext != toEnd )
    {
        *toNext++ = fromCP1252(*fromNext++);
    }

    return fromNext != fromEnd ? std::codecvt_base::partial 
                               : std::codecvt_base::ok;
}


Windows1252Codec::result Windows1252Codec::do_out(MBState& s, 
                                                  const Char* fromBegin,
                                                  const Char* fromEnd, 
                                                  const Char*& fromNext,
                                                  char* toBegin, 
                                                  char* toEnd, 
                                                  char*& toNext) const
{
    fromNext = fromBegin;
    toNext = toBegin;

    while(fromNext != fromEnd && toNext != toEnd)
    {
        *toNext++ = toCP1252(*fromNext++);
    }

    return fromBegin == fromNext ? std::codecvt_base::partial 
                                 : std::codecvt_base::ok;
}

/////////////////////////////////////////////////////////////////////////////
// Windows936Codec
/////////////////////////////////////////////////////////////////////////////

Windows936Codec::Windows936Codec(std::size_t ref)
: TextCodec<Char, char>(ref)
{}


Windows936Codec::~Windows936Codec()
{}



Pt::Char fromCP936(Pt::uint16_t upper, Pt::uint8_t lower)
{ 
  // 0xFE4F -> 21790

  upper -= 0x81;
  // 7d00

  lower -= 40;
  // 0f
  
  Pt::uint16_t offset = upper << 8;
  offset += lower;
  // 7d05

  offset -= upper * 0x41; // 1fbd
  // 5d48

  return 0x3f;
  //return cp936ToUnicode[n];
}


Windows936Codec::result Windows936Codec::do_in(MBState& s, 
                                               const char* fromBegin, 
                                               const char* fromEnd, 
                                               const char*& fromNext,
                                               Char* toBegin, 
                                               Char* toEnd, 
                                               Char*& toNext) const
{
    fromNext = fromBegin;
    toNext = toBegin;

    while( fromNext != fromEnd && toNext != toEnd )
    {
      if(*fromNext < 0x81)
      {
        *toNext++ = *fromNext++;
        continue;
      }
      
      if(fromEnd - fromNext < 2)
        break;

      Pt::uint16_t upper = *fromNext++;
      Pt::uint8_t lower = *fromNext++;

      if(upper < 0x81 || upper == 0xff)
        return std::codecvt_base::error;
      
      *toNext++ = fromCP936(upper, lower);
    }

    return fromNext != fromEnd ? std::codecvt_base::partial 
                               : std::codecvt_base::ok;
}


Windows936Codec::result Windows936Codec::do_out(MBState& s, 
                                               const Char* fromBegin,
                                               const Char* fromEnd, 
                                               const Char*& fromNext,
                                               char* toBegin, 
                                               char* toEnd, 
                                               char*& toNext) const
{
    return std::codecvt_base::error;
}


int Windows936Codec::do_length(MBState& s, 
                               const char* from, 
                               const char* fromEnd, std::size_t max) const
{
    std::size_t n = 0;
    while(from != fromEnd && n < max)
    {
      if(*from++ < 0x81)
        ++n;

      if(from == fromEnd)
        break;

      ++from;
      ++n;
    }

    return n;
}

} // namespace Pt
