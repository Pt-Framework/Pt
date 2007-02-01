/***************************************************************************
 *   Copyright (C) 2006 Marc Boris Duerner                                 *
 *   Copyright (C) 2006 by PTV AG                                          *
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
#if !defined(PT_Environment_H)
#define PT_Environment_H

#include "Pt/System/Api.h"
#include <string>

namespace Pt {

namespace System {

	//! @brief Environment class.
	/**
	*/

	class PT_SYSTEM_API Environment
	{
        friend class EnvironmentImpl;

		private:
			class EnvironmentImpl* _impl;

		protected:
			Environment();
			~Environment();

		public:
			//! @brief Returns the extension for a shared library on the current system.
			///
			static const std::string& sharedLibraryExtension();

			static const std::string& sharedLibraryPrefix();
	};

} // !namespace system

} // !namespace Pt

#endif // PT_Environment_H



