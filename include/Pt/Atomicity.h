/***************************************************************************
 *   Copyright (C) 2006-2007 by Marc Boris Dürner                          *
 *   Copyright (C) 2006-2007 by Aloysius Indrayanto                        *
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
#ifndef PT_ATOMICITY_H
#define PT_ATOMICITY_H


#ifdef _MSC_VER

    #include "Atomicity.windows.h"

#elif __GNUC__

    #if defined( _i386_     ) || defined( __i386__ ) || \
        defined( __x86_64__ ) || defined( _M_IX86  )

        #include "Atomicity.gcc.x86.h"

    #elif defined( __arm__ )

        #include "Atomicity.gcc.arm.h"

    #elif defined( _M_PPC  ) || defined( PPC         ) || \
          defined( ppc     ) || defined( __powerpc__ ) || \
          defined( __ppc__ )

        #include "Atomicity.gcc.ppc.h"

    #elif defined( __mips__ )

        #include "Atomicity.gcc.mips.h"

    #else

       #include "Atomicity.generic.h"

    #endif

#endif

#endif
