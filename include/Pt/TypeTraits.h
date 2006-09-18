/***************************************************************************
 *   Copyright (C) 2005 Marc Boris Drner                                   *
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
#ifndef Pt_TypeTraits_h
#define Pt_TypeTraits_h

#include <Pt/Api.h>
#include <string>
#include <typeinfo>


namespace Pt {

	template <typename T>
	struct TypeTraits {
		static bool isSpecialized()
		{ return false; }

		static const char* typeName()
		{ return typeid(T).name(); }
	};


	template <>
	struct PT_EXPORT TypeTraits<void> {
		static bool isSpecialized()
		{ return true; }

		static const char* typeName()
		{ return "void"; }
	};


	template <>
	struct PT_EXPORT TypeTraits<bool> {
		static bool isSpecialized()
		{ return true; }

		static const char* typeName()
		{ return "bool"; }
	};


	template <>
	struct PT_EXPORT TypeTraits<int> {
		static bool isSpecialized()
		{ return true; }

		static const char* typeName()
		{ return "int"; }
	};


	template <>
	struct PT_EXPORT TypeTraits<float> {
		static bool isSpecialized()
		{ return true; }

		static const char* typeName()
		{ return "float"; }
	};


	template <>
	struct PT_EXPORT TypeTraits<double> {
		static bool isSpecialized()
		{ return true; }

		static const char* typeName()
		{ return "double"; }
	};

} // !namespace Pt


#endif
