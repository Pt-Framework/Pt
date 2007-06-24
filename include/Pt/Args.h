/***************************************************************************
 *   Copyright (C) 2004-2006 by Marc Boris Dürner                          *
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

#ifndef Pt_Args_h
#define Pt_Args_h

#include <Pt/Any.h>
#include <Pt/SourceInfo.h>

#include <stdexcept>
#include <vector>


//! \addtogroup Pt
namespace Pt {

/** @brief Method invokation arguments
    @ingroup Reflection
*/
class Args
{
    public:
        Args()
        {}

        template <typename A>
        explicit Args(A a)
        {
            _args.push_back( Any(a) );
        }

        template <typename A, typename B>
        Args(A a, B b)
        {
            _args.push_back( Any(a) );
            _args.push_back( Any(b) );
        }

        template <typename A, typename B, typename C>
        Args(A a, B b, C c)
        {
            _args.push_back( Any(a) );
            _args.push_back( Any(b) );
            _args.push_back( Any(c) );
        }

        virtual ~Args()
        {
            //_args.push_back( Any(a) );
        }

        void clear()
        { _args.clear(); }

        const Any& get(size_t n) const
        {
            //TODO: different exception type?
            if( n >= _args.size() )
                throw std::out_of_range("No such argument" + PT_SOURCEINFO);

            return _args[n];
        }

        template <typename T>
        void push_back(T value)
        {
            _args.push_back( Any(value) );
        }

    private:
        std::vector<Any> _args;
};


} // namespace Pt

#endif
