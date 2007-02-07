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
#include "DrawThickEllipse.h"
#include "DrawThinEllipse.h"
 
namespace Pt{
namespace Gfx{
 
DrawThickEllipse::DrawThickEllipse()
{ }

DrawThickEllipse::~DrawThickEllipse()
{ }

void DrawThickEllipse::outputSpan( int x, int y, unsigned len )
{

}

void DrawThickEllipse::fillEllipse(  const Pt::Math::Point& topLeft, const Pt::Math::Size& size )
{
 	/* e(x,y) = b^2*x^2 + a^2*y^2 - a^2*b^2 */	
	const int       a      = size.width() /2;
	const int       b      = size.height() /2;
	const int       xc     = topLeft.x() + a;
	const int       yc     = topLeft.y() + b;		
    int             x      = 0;
    int             y      = b;
    unsigned int    width  = 1;
    long            a2     = (long)a*a;
    long            b2     = (long)b*b;
    long            crit1  = -(a2/4 + a%2 + b2);
    long            crit2  = -(b2/4 + b%2 + a2);
    long            crit3  = -(b2/4 + b%2);
    long            t      = -a2*y; /* e(x+1/2,y-1/2) - (a^2+b^2)/4 */
    long            dxt    = 2*b2*x;
    long            dyt    = -2*a2*y;
    long            d2xt   = 2*b2;
    long            d2yt   = 2*a2;

    while( y >= 0 && x <= a ) 
    {
	    if( t + b2*x <= crit1 /* e(x+1,y-1/2) <= 0 */ || t + a2*y <= crit3 /* e(x+1/2,y) <= 0 */ )
        {
		    //Increment x
		    x++; 
		    dxt += d2xt; 
		    t   += dxt;
		    
		    width += 2;
	    }
	    else if( t - a2*y > crit2 ) /* e(x+1/2,y-1) > 0 */
	    {
		    outputSpan(xc-x, yc-y, width  - 1);
		    
		    if( y!=0 )
			    outputSpan(xc-x, yc+y - 1, width  - 1);
			    
		     //Increment Y
		    y--; 
		    dyt += d2yt; 
		    t   += dyt;
	    }
	    else 
	    {
		    outputSpan(xc-x, yc-y, width -1 );
		    
		    if( y != 0 )
			    outputSpan(xc-x, yc+y -1, width - 1 );
			    
		     //Increment x
		    x++; 
		    dxt += d2xt; 
		    t   += dxt;
		    
		    //Increment Y
		    y--; 
		    dyt += d2yt; 
		    t   += dyt;
		    
		    width += 2;
	    }
    }

    if( b == 0 )
        outputSpan(xc-a, yc, 2*a );
}

void DrawThickEllipse::draw( ARgbImage& image, const Pen& pen, const Pt::Math::Point& topLeft, const Pt::Math::Size& size )
{
//    _outterEllispe.clear();
//    _innerEllispe.clear();  
    
}
 
}//namespace Gfx
}//namespace Pt