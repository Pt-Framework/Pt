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

/* set the dash-related attributes in an miGC */

void
#ifdef _HAVE_PROTOS
miSetGCDashes (miGC *pGC, int ndashes, const unsigned int *dashes, int offset)
#else
miSetGCDashes (pGC, ndashes, dashes, offset)
     miGC *pGC;
     int ndashes;
     const unsigned int *dashes;
     int offset;
#endif
{
  int i;

  if (pGC == (miGC *)NULL || ndashes < 0)
    return;
  if (pGC->dash)
    free (pGC->dash);
  pGC->dashOffset = offset;
  pGC->numInDashList = ndashes;
  if (ndashes == 0)
    pGC->dash = (unsigned int *)NULL;
  else
    {
      pGC->dash = (unsigned int *)mi_xmalloc (ndashes * sizeof(unsigned int));
      for (i = 0; i < ndashes; i++)
	pGC->dash[i] = dashes[i];
    }
}
}//namespace Gfx
}//namespace Pt