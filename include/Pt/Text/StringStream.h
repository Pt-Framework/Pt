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

#ifndef PTV_TEXT_STRINGSTREAM_H
#define PTV_TEXT_STRINGSTREAM_H

#include <Pt/Text/Api.h>
#include <Pt/Text/Char_ctype.h>
#include <Pt/Text/Char_numpunct.h>

#include <Pt/Char.h>
#include <sstream>

namespace Pt {
    namespace Text {
        typedef std::basic_stringstream<Pt::Char> StringStream;
    }
}

#endif
