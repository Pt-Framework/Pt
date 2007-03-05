/***************************************************************************
 *   Copyright (C) 2006-2007 Laurentiu-Gheorghe Crisan                     *
 *   Copyright (C) 2006-2007 Marc Boris Duerner                            *
 *   Copyright (C) 2006-2007 PTV AG                                        *
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

#ifndef PT_GFX_DASH_H
#define PT_GFX_DASH_H

namespace Pt{
namespace Gfx

class Dash
{
    public:
        Dash()
        ~Dash();
        
         /** @brief Helper function called by dash drawing.
           
           Helper function, called by wide line  and also by thin poly arc
           and thin dahed line to perform initial offsetting into the dash array, 
           before dash #0 is drawn.  In all cases, dashNum=0, dashIndex=0 and dashOffset=0. 
          
           @param dist          Additional offset (assumed >= 0)      
           @param pDashNum      Dash number 
           @param pDashIndex    Current dash 
           @param pDash	        Dash list 
           @param numInDashList	Dashlist length 
           @param pDashOffset   Offset into current dash   
        */
        static void stepDash( int dist, int* pDashNum, int* pDashIndex, const unsigned int* pDash, int numInDashList, int *pDashOffset );
};

} //namespace Gfx
} //namespaec Pt

#endif
