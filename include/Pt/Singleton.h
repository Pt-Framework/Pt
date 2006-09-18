/***************************************************************************
 *   Copyright (C) 2005 -2006 by Marc Boris Dürner                         *
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

#ifndef Pt_Singleton_h
#define Pt_Singleton_h

#include <Pt/NonCopyable.h>

#include <memory>
#include <cstdlib>


namespace Pt {

/**
 \param T type of the singleton
 \param A optional allocator for type T
*/
template <typename T, typename A = std::allocator<T> >

/** Singletons can be instantiated with this class.
 FileSystem is an example how to use the %Singleton class
 */
class Singleton : public NonCopyable {
	public:
		typedef A Allocator;

	public:
		static T& instance()
		{
			if(!_instance)
			{
				_instance = _allocator.allocate(1);
				new (_instance) T();
				std::atexit(&atExit);
			}

			return *_instance;
		}

	protected:
		Singleton()
		{}

		~Singleton()
		{}

	private:
		static void atExit()
		{
			_allocator.destroy(_instance);
			_allocator.deallocate(_instance, 1);
			_instance = 0;
		}

	private:
		static A _allocator;
		static T* _instance;
};

template <typename T, typename A>
A Singleton<T, A>::_allocator;

template <typename T, typename A>
T* Singleton<T, A>::_instance = 0;
}

#endif
