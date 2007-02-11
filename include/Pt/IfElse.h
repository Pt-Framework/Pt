/***************************************************************************
 *   Copyright (C) 2005 Marc Boris Dürner                                  *
 *                                                                         *
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
#ifndef _Pt_IfElse_h_
#define _Pt_IfElse_h_


namespace Pt {

    template <bool, typename A, typename B>
    struct IfElse {
    };


    template <typename A, typename B>
    struct IfElse<true, A, B> {
        public:
            typedef A ResultT;
            typedef B NotResultT;
    };


    template <typename A, typename B>
    struct IfElse<false, A, B> {
        public:
            typedef A NotResultT;
            typedef B ResultT;
    };

} // !namespace Pt


#endif
