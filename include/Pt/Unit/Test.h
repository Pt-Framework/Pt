/***************************************************************************
 *   Copyright (C) 2005-2006 by Dr. Marc Boris Dürner                      *
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
#ifndef PT_UNIT_TEST_H
#define PT_UNIT_TEST_H

#include <Pt/NonCopyable.h>
#include <Pt/Signal.h>
#include <Pt/Reflectable.h>

#include <string>


namespace Pt {

namespace Unit {

    class Test : public Reflectable, public NonCopyable
    {
        public:
            Test(const std::string& name)
            : Reflectable(name)
            { }

            virtual ~Test()
            { }

            virtual void run()
            { }

            Signal<const Test&> success;

            Signal<const std::string&, const Assertion&> assertion;

            Signal<const std::string&, const std::exception&> exception;

            Signal<const std::string&> error;
    };

} // namespace Unit

} // namespace Pt

#endif
