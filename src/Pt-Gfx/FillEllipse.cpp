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
#include "FillEllipse.h"
#include "Fill.h"


namespace Pt {

namespace Gfx {

FillEllipse::FillEllipse()
: _fill(0)
{ }


FillEllipse::~FillEllipse()
{ }


void FillEllipse::outputSpan( ARgbImage& image, const Brush& brush, const Pt::Math::Point& topLeft, int x, int y, int width )
{
    const int imageWidth = static_cast<int>( image.width() );
    const int imageHeight = static_cast<int>( image.height() );

    if( y >= imageHeight )
        return;

    if( y < 0 )
        return;

    const ssize_t xend = std::min( x + width, imageWidth );
    ssize_t       xpos = std::max( 0, x );

    if( xend > xpos )
        _fill->fill(image, brush, topLeft, xpos, y, xend-xpos );
}


void FillEllipse::draw( ARgbImage& image, const Brush& brush, const Pt::Math::Point& topLeft, const Pt::Math::Size& size )
{    
    
    if( size.width() == 0 || size.height() == 0 )
        return;
    
    if( size.width() ==  1 && size.height() == 1 )
        return;

    /* e(x,y) = b^2*x^2 + a^2*y^2 - a^2*b^2 */      
    int errorx = 1;
    int errory = 1;

    if( size.width()%2 != 0 )
        errorx  =  0;

    if( size.height()%2 != 0)
        errory  = 0;

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
            outputSpan(image, brush, topLeft, xc-x, yc-y, width  - errorx);
            
            if( y!=0 )
                outputSpan(image, brush, topLeft, xc-x, yc+y - errory, width  - errorx);
                
             //Increment Y
            y--; 
            dyt += d2yt; 
            t   += dyt;
        }
        else 
        {
            outputSpan( image, brush, topLeft, xc-x, yc-y, width -errorx );
            
            if( y != 0 )
                outputSpan( image, brush, topLeft, xc-x, yc+y -errory, width - errorx );
                
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
        outputSpan( image, brush, topLeft, xc-a, yc, 2*a );
}

}//namespace Gfx

}//namespace Pt
