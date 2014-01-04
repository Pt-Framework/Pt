/*
 * Copyright (C) 2006-2007 Laurentiu-Gheorghe Crisan
 * Copyright (C) 2006-2007 Marc Boris Duerner
 * Copyright (C) 2006-2007 PTV AG
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */
#include "Dash.h"

namespace Pt{
namespace Gfx{

Dash::Dash()
{ }

Dash::~Dash()
{ }
        
void Dash::stepDash( int dist, int* pDashNum, int* pDashIndex, const unsigned int* pDash, int numInDashList, int *pDashOffset )
{
    int	dashNum, dashIndex, dashOffset;
    int totallen;
    int	i;

    dashNum     = *pDashNum;
    dashIndex   = *pDashIndex;
    dashOffset  = *pDashOffset;

    // Offset won't take us beyond end of present dash. 
    if( dashOffset + dist < (int)(pDash[dashIndex]) )
    {
        *pDashOffset = dashOffset + dist;
        return;
    }

    // Move to next dash.
    dist -= (int)(pDash[dashIndex]) - dashOffset;
    dashNum++;
    dashIndex++;
    
    // Wrap to beginning of dash list.
    if( dashIndex == numInDashList )
        dashIndex = 0;

    // Make it easy on ourselves: work modulo iteration interval.
    totallen = 0;
    
    for (i = 0; i < numInDashList; i++)
        totallen += (int)(pDash[i]);

    if (totallen <= dist)
        dist = dist % totallen;

    while (dist >= (int)(pDash[dashIndex]))
    {
        dist -= (int)(pDash[dashIndex]);
        dashNum++;
        dashIndex++;
         
         // Wrap to beginning of dash list.
        if( dashIndex == numInDashList )    	   
	        dashIndex = 0;
    }
    
    *pDashNum = dashNum;
    *pDashIndex = dashIndex;
    *pDashOffset = dist;
}

}//namespace Gfx
}//namespace Pt
