/***************************************************************************
 *   Copyright (C) 2005-2006 by Marc Boris Duerner                         *
 *   Copyright (C)      2006 by Aloysius Indrayanto                        *
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

	/** @brief Makes derived classes become singleton
	 *  @ingroup Pt
	 *
	 *  @param T type of the singleton
	 *  @param A optional allocator for type T
	 *
	 *  This class is meant to be used as a mix-in class as shown in the code
	 *  example below.
	 *
	 *  @code
	 *		class SingletonClassName : public Singleton<SingletonClassName> {
	 *			friend class Singleton<SingletonClassName>;
	 *
	 *			// ...
	 *			// The class' implementation
	 *			// ...
	 *		};
	 *  @endcode
	 *
	 */
	template <typename T, typename A = std::allocator<T> >
	class Singleton : public NonCopyable {
		public:
			// For convenience
			typedef A Allocator;

		public:
			/** @brief Returns an instance of the class
			 *
			 *  This function will always returns the same class instance.
			 */
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
			/**  @brief Declared as protected to prevent direct instantiation of this class
			 */
			Singleton()
			{ }

			/**  @brief Declared as protected to prevent direct deletion of this class
			 */
			~Singleton()
			{ }

		private:
			/**  @brief On exit it will destroy the one and the only instance of the class
			 */
			static void atExit()
			{
				_allocator.destroy(_instance);
				_allocator.deallocate(_instance, 1);
				_instance = 0;
			}

		private:
			static A  _allocator;
			static T* _instance;
	};

	// Definition of the variable above
	template <typename T, typename A>
	A Singleton<T, A>::_allocator;

	// Definition of the variable above
	template <typename T, typename A>
	T* Singleton<T, A>::_instance = 0;

} // namespace Pt

#endif
