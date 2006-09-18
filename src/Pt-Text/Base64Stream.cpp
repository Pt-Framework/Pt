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
 ***************************************************************************/

#include "Pt/Text/Base64Stream.h"

#include <iostream>
using namespace std;


namespace Pt {

namespace Text {

Base64Buffer::Base64Buffer(std::streambuf* buffer, Base64Codec* codec)
: BasicTextBuffer<char, char>(buffer, codec)
{ }


Base64IStream::Base64IStream(std::istream& is)
: BasicTextIStream<char, char>( new Base64Buffer(is.rdbuf(), new Base64Codec) )
{}


Base64IStream::~Base64IStream()
{ delete this->rdbuf(); }


Base64OStream::Base64OStream(std::ostream& os)
: BasicTextOStream<char, char>( new Base64Buffer(os.rdbuf(), new Base64Codec) )
{ }


Base64OStream::~Base64OStream()
{ delete this->rdbuf(); }


Base64Stream::Base64Stream(std::iostream& ios)
: BasicTextStream<char, char>( new Base64Buffer(ios.rdbuf(), new Base64Codec) )
{ }


Base64Stream::~Base64Stream()
{ delete this->rdbuf(); }

} // namespace Text

} // namespace Pt
