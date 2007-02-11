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
#include "DrawThinPolyline.h"
#include <cassert>

namespace Pt{
namespace Gfx{

DrawThinPolyline::DrawThinPolyline()
{ }


void DrawThinPolyline::draw( ARgbImage& image,  const Pen& pen, const std::vector<Math::Point>& points )
{
    if( points.size() == 0 )
        return;
/*        
    int        xstart;
    int        ystart;
    int        x1;
    int        x2;
    int        y1; 
    int        y2;
    size_t    npt = points.size();
    
    std::vector<Math::Point>::iterator ppt = points.begin();
            
    xstart = ppt->x();
    ystart = ppt->y();
    
    x2 = xstart;
    y2 = ystart;
    
    while( --npt )
    {
        x1 = x2;
        y1 = y2;
        ++ppt;
        
        if( x1 == x2 ) // Vertical case
        {
            // Vertical lines are always drawn top to bottom (y-increasing).  
            // This requires adding one to the y-coordinate of each endpoint 
            // after swapping.
            
            if( y1 > y2 )
            {
                const int tmp = y2;
                y2 = y1 + 1;
                y1 = tmp + 1;
            }
            
            if( y1 != y2 )            
                image.pixel( x1, y1 ) = pen.color();
        
            //Restore final point    
            y2 = ppt->y();
        }
        else if( y1 == y2 ) // Horizontal case
        {
            // Horizontal lines are always drawn left to right; we have to move the
            // endpoints right by one after they're swapped.

            if( x1 > x2 )
            {
                const int tmp = x2;
                x2 = x1 + 1;
                x1 = tmp + 1;
            }
            
            if( x1 == x2 )
                image.pixel( x1, y1 ) = pen.color();
            
            //restore final point
            x2 = ppt->x();                                
        }
        else //Slope
        {
            int adx            = x2 - x1;
            int ady            = y2 - y1;
            int signdx        = 1;
            int signdy        = 1;
            int e, e1, e2;
            bool xaxis = true;
            int len;
            
            if( adx < 0)     
            { 
                adx       = -adx; 
                signdx = -1; 
            }
            
            if( ady < 0 )
            {
                ady    = - ady;
                signdy = -1;
            }
            
            if( adx > ady )
            {
                xaxis    = true;
                e1        = ady << 1;
                e2        = e1 - ( adx << 1 );
                e        = e1 - adx;                
                e        -= (_signdx < 0 ) ;                
                len        = adx;
            }
            else
            {
                xaxis    = false;
                e1        = adx << 1;
                e2        = e1 - ( ady << 1 );
                e        = e1 - ady;                
                e        -= (_signdy < 0 ) ;                            
                len        = ady;
            }
            
            
        }

    */
    
}

}
}
