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

#ifndef _Pt_Void_h_
#define _Pt_Void_h_

#include <Pt/TypeTraits.h>


namespace Pt {

	struct PT_API Void
	{};


	template <>
	struct PT_API TypeTraits<Void> {
		static const char* typeName()
		{ return "Pt::Void"; }

		template <typename T>
		static bool isVoid(const T&)
		{ return false; }

		static bool isVoid(const Void&)
		{ return true; }
	};

}

#endif
