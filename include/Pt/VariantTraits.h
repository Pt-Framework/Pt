/***************************************************************************
 *   Copyright (C) 2004-2006 by Marc Boris Duerner                         *
 *   Copyright (C) 2004-2006 by Stepan Beal                                *
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
#ifndef Pt_VariantTraits_h
#define Pt_VariantTraits_h

#include <string>
#include <sstream>


namespace Pt {

	template <typename T>
	struct VariantTraits {
		static void toData(std::string& data, const T& value)
		{
			std::ostringstream os;
			os << value;
			data = os.str();
		}

		static void fromData(T& value, const std::string& data)
		{
			std::istringstream is(data);
			is >> value;
		}
	};

	template <>
	struct VariantTraits<std::string> {
		static void toData(std::string& data, const std::string& value)
		{ data = value; }

		static void fromData(std::string& value, const std::string& data)
		{ value = data; }
	};

} // namespace Pt


#endif 

