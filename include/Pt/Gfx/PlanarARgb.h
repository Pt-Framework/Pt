/***************************************************************************
 *   Copyright (C) 2006-2007 by Aloysius Indrayanto                        *
 *   Copyright (C) 2006-2007 by Marc Boris Dürner                          *
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
#ifndef Pt_Gfx_PlanarARgb_h
#define Pt_Gfx_PlanarARgb_h

#include <Pt/Gfx/ARgbColor.h>


namespace Pt {

    namespace Gfx {

        struct PlanarARgb
        {
            static const size_t NumberOfChannels = 4;

            typedef uint16_t Component;

            typedef ARgbColor Color;

            typedef const ARgbColor ConstColor;

            typedef ARgbColorRef ColorRef;

            typedef ARgbConstColorRef ConstColorRef;

            typedef ARgbColorPtr ColorPtr;

            typedef ARgbConstColorPtr ConstColorPtr;
        };

    }

}

#endif
