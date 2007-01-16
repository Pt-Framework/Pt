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

#include <Pt/Api.h>
#include "Pt/Text/Char_codecvt.h"


namespace std {

std::locale::id codecvt<Pt::Char, char, mbstate_t>::id;


#ifdef _MSC_VER

codecvt<Pt::Char, char, mbstate_t>::codecvt(size_t ref)
: codecvt_base(ref)
{}

#else

codecvt<Pt::Char, char, mbstate_t>::codecvt(size_t ref)
: locale::facet(ref)
{}

#endif


codecvt<Pt::Char, char, mbstate_t>::~codecvt()
{}

}
