/***************************************************************************
 *   Copyright (C) 2006 by Marc Boris Dürner                               *
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

#ifndef PT_REFCOUNTED_H
#define PT_REFCOUNTED_H

#include <Pt/Types.h>


namespace Pt
{
	class PT_EXPORT RefCounted
	{
		public:
			RefCounted()
			: _refs(0)
			{ }

			virtual ~RefCounted()
			{ }

			virtual void addRef()
			{ ++_refs; }

			virtual void release()
			{
				if (--_refs == 0)
				{
					delete this;
				}
			}

			size_t refs() const
			{ return _refs; }

		private:
			size_t _refs;
	};
}

#endif // PT_REFCOUNTED_H

