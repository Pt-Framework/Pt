/*
 * Copyright (C) 2005 by Marc Boris Duerner
 * Copyright (C) 2005 by Aloysius Indrayanto
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
#ifndef Pt_Base64Codec_h
#define Pt_Base64Codec_h

#include <Pt/Api.h>
#include <Pt/Types.h>
#include <Pt/TextCodec.h>

namespace Pt {

	char toBase64(uint8_t n)
	{
		static char b64Table[65] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
		return b64Table[n];
	}

	uint8_t fromBase64(char b64)
	{
		switch(b64) {
			case 'A': return 0; case 'B': return 1; case 'C': return 2;
			case 'D': return 3; case 'E': return 4; case 'F': return 5;
			case 'G': return 6; case 'H': return 7; case 'I': return 8;
			case 'J': return 9; case 'K': return 10; case 'L': return 11;
			case 'M': return 12; case 'N': return 13; case 'O': return 14;
			case 'P': return 15; case 'Q': return 16; case 'R': return 17;
			case 'S': return 18; case 'T': return 19; case 'U': return 20;
			case 'V': return 21; case 'W': return 22; case 'X': return 23;
			case 'Y': return 24; case 'Z': return 25; case 'a': return 26;
			case 'b': return 27; case 'c': return 28; case 'd': return 29;
			case 'e': return 30; case 'f': return 31; case 'g': return 32;
			case 'h': return 33; case 'i': return 34; case 'j': return 35;
			case 'k': return 36; case 'l': return 37; case 'm': return 38;
			case 'n': return 39; case 'o': return 40; case 'p': return 41;
			case 'q': return 42; case 'r': return 43; case 's': return 44;
			case 't': return 45; case 'u': return 46; case 'v': return 47;
			case 'w': return 48; case 'x': return 49; case 'y': return 50;
			case 'z': return 51; case '0': return 52; case '1': return 53;
			case '2': return 54; case '3': return 55; case '4': return 56;
			case '5': return 57; case '6': return 58; case '7': return 59;
			case '8': return 60; case '9': return 61; case '+': return 62;
			case '/': return 63; case '=': return 64;

			default:
				return 0;
		}
	}

    class Base64Codec : public TextCodec<char, char>
    {
        public:
			explicit Base64Codec(size_t ref = 0)
			: TextCodec<char, char>(ref)
			{}

			virtual ~Base64Codec()
			{}

            virtual result do_in(MBState& s, const char* fromBegin,
                                 const char* fromEnd, const char*& fromNext,
                                 char* toBegin, char* toEnd, char*& toNext) const
			{
				fromNext = fromBegin;
				toNext = toBegin;

				if(fromBegin == fromEnd)
				{
					return ok;
				}

				if( (toEnd - toNext) < 3 ||(fromEnd - fromNext) < 4 )
				{
					return error;
				}

				while( (fromEnd - fromNext) >= 4 && (toEnd - toNext) >= 3 )
				{
					Pt::uint8_t first  = fromBase64( *fromNext );
					Pt::uint8_t second = fromBase64( *(++fromNext) );
					Pt::uint8_t third  = fromBase64( *(++fromNext) );
					Pt::uint8_t fourth = fromBase64( *(++fromNext) );

					*toNext = (first << 2) + (second >> 4);
					*(++toNext) = (second << 4) + (third >> 2);

					if(fourth != 64) {
						*(++toNext) = (third << 6) + (fourth);
					}

					++toNext;
					++fromNext;

					if( fromEnd == fromNext ) {
						return ok;
					}
				}

				return partial;
			}

            virtual result do_out(MBState& s, const char* fromBegin,
                                  const char* fromEnd, const char*& fromNext,
                                  char* toBegin, char* toEnd, char*& toNext) const
			{
				fromNext = fromBegin;
				toNext = toBegin;

				// process until two or less characters are left
				while( (fromNext + 2) < fromEnd )
				{
					if(toEnd - toNext < 4)
					{
						return partial;
					}

					Pt::uint8_t* first = (Pt::uint8_t*)(fromNext);
					Pt::uint8_t* second = (Pt::uint8_t*)(first+1);
					Pt::uint8_t* third = (Pt::uint8_t*)(first+2);

					*toNext     = base64( (*first >> 2) & 0x3f );
					*(++toNext) = base64( ((*first << 4) + ((*second) >> 4)) & 0x3f );
					*(++toNext) = base64( ((*second << 2) + ((*third) >> 6)) & 0x3f );
					*(++toNext) = base64( *third & 0x3f );

					++toNext;
					fromNext = fromNext + 3;
				}

				switch( fromEnd - fromNext )
				{
					case 2:
						state.value.mbytes[0]  = (Pt::uint8_t)( *fromNext );
						state.value.mbytes[1] = (Pt::uint8_t)( *(fromNext+1) );
						state.n = 2;
						fromNext += 2;
						return partial;

					case 1:
						state.value.mbytes[0] = (Pt::uint8_t)(*fromNext);
						state.n = 1;
						fromNext += 1;
						return partial;

					case 0:
						state = MBState();
						return ok;

					default:
						return error;
				}

				return error;
			}

            virtual result do_unshift(MBState& state,
                                      char* toBegin, char* toEnd, char*& toNext) const
			{
				toNext = toBegin;

				if(toEnd - toBegin < 4)
				{
					return partial;
				}

				switch(state.n)
				{
					case 2:
						*toNext     = base64( (_first >> 2) & 0x3f );
						*(++toNext) = base64( (_first << 4) & 0x3f );
						*(++toNext) = '=';
						*(++toNext) = '=';
						break;

					case 1:
						*toNext     = base64( (_first >> 2) & 0x3f );
						*(++toNext) = base64( ((_first << 4) + ((_second) >> 4)) & 0x3f );
						*(++toNext) = base64( (_second << 2) &  0x3f );
						*(++toNext) = '=';
						break;

					case 0:
						return noconv;

					default:
						return error;
				}

				state = MBState()
				++toNext;
				return ok;
			}

            virtual bool do_always_noconv() const throw()
            { return false; }

            virtual int do_length(mbstate_t& s, const char* fromBegin, const char* fromEnd, size_t max) const;

            virtual int do_encoding() const throw()
            { return 0; } // variable due to terminating pads

            virtual int do_max_length() const throw()
            { return 2; }
    };

} //namespace Pt

#endif

