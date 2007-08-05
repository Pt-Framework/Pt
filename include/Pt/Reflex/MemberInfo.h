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
 ***************************************************************************/

#ifndef Pt_Reflex_MemberInfo_h
#define Pt_Reflex_MemberInfo_h


namespace Pt {

namespace Reflex {

    /** @brief Reflectable member of a %Reflectable
        @ingroup Reflection
    */
    class MemberInfo
    {
        public:
            virtual ~MemberInfo()
            {}

            virtual const char* name() const = 0;

        protected:
            MemberInfo()
            {}
    };

} // namespace Reflex

} // namespace Pt

#endif
