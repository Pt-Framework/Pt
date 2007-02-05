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
#include "DrawEllipse.h"

namespace Pt{
namespace Gfx{

DrawEllipse::DrawEllipse()
{ }

DrawEllipse::~DrawEllipse()
{ }

void DrawEllipse::draw( ARgbImage& image, const Pen& pen, const Pt::Math::Point& topLeft, const Pt::Math::Size& size )
{ 
	int a  = size.width() /2;
	int b  = size.height() /2;

	int xc = topLeft.x() + a;
	int yc = topLeft.y() + b;
	
	int x = 0, y = b;
	long a2 = (long)a*a, b2 = (long)b*b;
	long crit1 = -(a2/4 + a%2 + b2);
	long crit2 = -(b2/4 + b%2 + a2);
	long crit3 = -(b2/4 + b%2);
	long t		= -a2*y; /* e(x+1/2,y-1/2) - (a^2+b^2)/4 */
	long dxt	= 2*b2*x, dyt = -2*a2*y;
	long d2xt	= 2*b2, d2yt = 2*a2;

	while (y>=0 && x<=a) {
		
		image.pixel(xc+x, yc+y) = pen.color();
		
		if (x!=0 || y!=0)
			image.pixel(xc-x, yc-y) = pen.color();
			
		if (x!=0 && y!=0) 
		{
			image.pixel(xc+x, yc-y) = pen.color();
			image.pixel(xc-x, yc+y) = pen.color();
		}		
		
		if (t + b2*x <= crit1 ||   /* e(x+1,y-1/2) <= 0 */
		    t + a2*y <= crit3)     /* e(x+1/2,y) <= 0 */
		{	
			//inc x		
			x++;
			dxt += d2xt;
			t += dxt;		
		}
		else if (t - a2*y > crit2) /* e(x+1/2,y-1) > 0 */
		{
			//inc y
			y--; 
			dyt += d2yt;
			t += dyt;
		}
		else 
		{
			//inc x
			x++;
			dxt += d2xt;
			t += dxt;		

			//inc y
			y--; 
			dyt += d2yt;
			t += dyt;
		}
	}

}

} //namespace Gfx
} //namespace Pt
