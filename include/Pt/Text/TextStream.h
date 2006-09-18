/***************************************************************************
 *   Copyright (C) 2005 by Marc Boris Dürner                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 **************************************************************************/

#ifndef Pt_Text_TextStream_h
#define Pt_Text_TextStream_h

#include <Pt/Api.h>
#include <Pt/Text/Char.h>
#include <Pt/Text/BasicTextStream.h>


namespace Pt {

namespace Text {

	class PT_EXPORT TextBuffer : public BasicTextBuffer<Pt::Char, char> {
		public:
			typedef TextCodec<Pt::Char, char> Codec;

		public:
			TextBuffer(std::streambuf* buffer, Codec* codec);
	};


	class PT_EXPORT TextIStream : public BasicTextIStream<Char, char> {
		public:
			typedef TextCodec<Pt::Char, char> CodecT;

		public:
			TextIStream(std::istream& is, CodecT* codec);

			~TextIStream();

		protected:
			TextIStream(TextBuffer* buffer);

		private:
			TextBuffer* _buffer;
	};


	class PT_EXPORT TextOStream : public BasicTextOStream<Char, char> {
		public:
			typedef TextCodec<Pt::Char, char> CodecT;

		public:
			TextOStream(std::ostream& os, CodecT* codec);

			~TextOStream();
	};


	class PT_EXPORT TextStream : public BasicTextStream<Char, char> {
		public:
			typedef TextCodec<Pt::Char, char> CodecT;

		public:
			TextStream(std::iostream& ios, CodecT* codec);

			~TextStream();
	};

} // namespace Text

} // namespace Pt

#endif

