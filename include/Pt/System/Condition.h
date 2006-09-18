/***************************************************************************
 *   Copyright (C) 2005 by Dr. Marc Boris Drner                           *
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

#ifndef Pt_System_Condition_h
#define Pt_System_Condition_h

#include <Pt/Api.h>
#include <Pt/NonCopyable.h>
#include <cstddef>


namespace Pt {

namespace System {
	class Mutex;

	class PT_EXPORT Condition : public NonCopyable {
		public:
			//! @brief Default Constructor.
			Condition();

			//! @brief Destructor.
			~Condition();

			//! @brief Wait until condition becomes signalled.
			void wait( Mutex& mtx);

			//! @brief Wait until condition becomes signalled. Returns true if successful,
			bool wait( Mutex& mtx, unsigned int ms);

			//! @brief Unblock a single blocked thread.
			void signal();

			//! @brief Unblock all blocked threads.
			void broadcast();

		private:
			class ConditionImpl* _impl;

		friend class ConditionImpl;
	};


} // !namespace System

} // !namespace Pt

#endif
