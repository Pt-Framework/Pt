/***************************************************************************
 *   Copyright (C) 2004 Marc Boris Duerner                                 *
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
 ***************************************************************************/

#include "Pt/Text/TextStream.h"
#include "Pt/Text/Char_numpunct.h"
#include "Pt/Text/Char_ctype.h"

#include <iostream>
using namespace std;


namespace Pt {

namespace Text {


struct InitLocale
{
	InitLocale()
	{
	    #ifndef PT_WITHOUT_STD_LOCALE
		std::locale::global( std::locale(std::locale(), new std::ctype<Pt::Char>) );
		std::locale::global( std::locale(std::locale(), new std::numpunct<Pt::Char>) );
		std::locale::global( std::locale(std::locale(), new std::num_get<Pt::Char>) );
		std::locale::global( std::locale(std::locale(), new std::num_put<Pt::Char>) );
		#endif
	}
} _initLocale;


TextBuffer::TextBuffer(std::streambuf* buffer, CodecT* codec)
: BasicTextBuffer<Pt::Char, char>(buffer, codec)
{
	// When building a DLL under Visual studio, we need to imbue here
	#ifndef PT_WITHOUT_STD_LOCALE
		if( false == std::has_facet< std::ctype<Pt::Char> >( std::locale() ) )
		{
			std::locale::global( std::locale(std::locale(), new std::ctype<Pt::Char>) );
			std::locale::global( std::locale(std::locale(), new std::numpunct<Pt::Char>) );
			std::locale::global( std::locale(std::locale(), new std::num_get<Pt::Char>) );
			std::locale::global( std::locale(std::locale(), new std::num_put<Pt::Char>) );
		}
	#endif
}


TextIStream::TextIStream(std::istream& is, CodecT* codec)
: BasicTextIStream<Char, char>( new TextBuffer(is.rdbuf(), codec) )
, _buffer(0)
{
	_buffer = (TextBuffer*)( this->rdbuf() );
}


TextIStream::~TextIStream()
{
	delete _buffer;
}


TextIStream::TextIStream(TextBuffer* buffer)
: BasicTextIStream<Pt::Char, char>( buffer )
, _buffer(0)
{
	/*#ifndef PTV_WITHOUT_STD_LOCALE
		if( false == std::has_facet< std::ctype<Pt::Char> >( std::locale() ) ) {
			std::locale::global( std::locale(std::locale(), new std::ctype<Pt::Char>) );
			std::locale::global( std::locale(std::locale(), new std::numpunct<Pt::Char>) );
			std::locale::global( std::locale(std::locale(), new std::num_get<Pt::Char>) );
			std::locale::global( std::locale(std::locale(), new std::num_put<Pt::Char>) );
		}
	#endif*/
}




TextOStream::TextOStream(std::ostream& os, CodecT* codec)
: BasicTextOStream<Char, char>( new TextBuffer(os.rdbuf(), codec) )
{ }


TextOStream::~TextOStream()
{
	delete this->rdbuf();
}


TextStream::TextStream(std::iostream& ios, CodecT* codec)
: BasicTextStream<Char, char>( new TextBuffer(ios.rdbuf(), codec) )
{
}


TextStream::~TextStream()
{
	delete this->rdbuf();
}


} // namespace Text

} // namespace Pt
