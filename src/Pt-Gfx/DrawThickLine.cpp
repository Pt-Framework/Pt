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
#include <Pt/Math/Point.h>
#include <Pt/Math/MathUtils.h>

#include "DrawThickLine.h"
#include "DrawThinLine.h"
   
namespace Pt {
namespace Gfx {

DrawThickLine::DrawThickLine()
{}

Pt::ssize_t round(double x) 
{	
	double _x = x; 
	int _i;
    
    if( _x >= INT_MAX )  
		_i = INT_MAX; 
		
    else if( _x <= -(INT_MAX)) 
		_i = -(INT_MAX);
    else 
		_i = (_x > 0.0 ? (int)(_x + 0.5) : (int)(_x - 0.5));

    return _i;
}
void perp( ARgbImage& image, ARgbColor color,  int x1, int y1, int dx, int dy, int thick)
{
    int sx = (dx > 0)? 1: -1;
    int sy = (dy > 0)? 1: -1;
    int ax = 2*sx*dx;
    int ay = 2*sy*dy;
    int x = x1;
    int y = y1;
    int T = thick;

    if (ax > ay) {
        int d = ay - ax/2;
        while (1) {
            if (T-- <= 0) return;
            if (d >= 0) {
                y += sy;
                image.pixel(x,y) = color;
                d -= ax;
            }
            x += sx;
            d += ay;
            image.pixel(x,y) = color;
        }
    }
    else {
        int d = ax - ay/2;
        while (1) {
            if (T-- <= 0) return;
            if (d >= 0) {
                x += sx;
				image.pixel(x,y) = color;                
                d -= ay;
            }
            y += sy;
            d += ax;
            image.pixel(x,y) = color;            
        }
    }
}


void bresenham_line(ARgbImage& image, ARgbColor color,int x1, int y1, int x2, int y2, int thick)
{
    int dx = x2-x1;
    int dy = y2-y1;
    int sx = (dx > 0)? 1: -1;
    int sy = (dy > 0)? 1: -1;
    int ax = 2*sx*dx;
    int ay = 2*sy*dy;
    int x = x1;
    int y = y1;

    if (ax > ay) {
        int d = ay - ax/2;
        while (1) {
            image.pixel(x,y) = color;            
            perp(image,color,x,y,dy,-dx, thick/2);
            perp(image,color,x,y,-dy,dx, thick-thick/2-1);
            if (x == x2)  return;
            if (d >= 0) {
                y += sy;
                d -= ax;
            }
            x += sx;
            d += ay;
        }
    }
    else {
        int d = ax - ay/2;
        while (1) {
            image.pixel(x,y) = color;            
            perp(image,color,x,y,dy,-dx, thick/2);
            perp(image,color,x,y,-dy,dx, thick-thick/2-1);
            if (y == y2)  return;
            if (d >= 0) {
                x += sx;
                d -= ay;
            }
            y += sy;
            d += ax;
        }
    }
}

void DrawThickLine::rasterize( ARgbImage& image, const Pen& pen,
                               const Math::Point& from, const Math::Point to, RasterBuffer& rasterBuffer )
{
    //bresenham_line( image, pen.color() ,from.x(), from.y(), to.x(), to.y(), pen.size() );

    const int   dx = to.x() - from.x() ;
    const int   dy = to.y() - from.y() ;
    const double halfPen = ( 0.5 * (double) ( pen.size()) );
    const double L = Math::hypot( (double)dx,  (double) dy );
    const double r1 = halfPen / L;
    double xa =  round( r1 * (double)dy );
    double ya = round(-r1 * (double)dx );

    std::vector<Math::Point> polygon;
    polygon.push_back( Math::Point(from.x() + xa, from.y() + ya ) ); // rightTop
    polygon.push_back( Math::Point(to.x() + xa, to.y() + ya) );   // rightBottom
    polygon.push_back( Math::Point(from.x() - floor(xa), from.y() - floor(ya)) ); // leftTop
    polygon.push_back( Math::Point(to.x() - xa, to.y() - ya) );      // leftBottom

    Math::Rect clippingRect( Math::Point( 0, 0 ), Math::Size( image.width() - 1, image.height() - 1 ) );

    //std::cerr << xa << " : " <<  ya << "\n\n";
    //std::cerr << Pt::ssize_t(ceil((double)from.x() - xa)) << ":" <<  Pt::ssize_t(round((double)from.y() - ya)) << "\n\n";

    _fillConvexPolygon.clip( clippingRect, polygon );
    // Pt::Gfx::Brush brush( pen.color() );
    // _fillPolygon.draw( image, brush, polygon );

    rasterBuffer.clear();
    rasterBuffer.spans().resize( image.height() );

    if( polygon.size() == 0 )
        return;

    _fillConvexPolygon.rasterize(  polygon, rasterBuffer );  
}


void DrawThickLine::draw( ARgbImage& image, const Pen& pen, const Math::Point& from, const Math::Point& to )
{
    Brush brush( pen.color() );
	
/*	std::vector<Math::Point>   polygon;

    const int   dx      = to.x() - from.x();
    const int   dy      = to.y() - from.y() ;
    const float halfPen = pen.size() / 2.0;
    const float L       = float(Math::hypot( dx,  dy ));
    const float r       = halfPen / L;
    const float ya		= -r * dx;
    const float xa		=  r * dy;

    polygon.push_back( Math::Point( ssize_t( from.x() - xa), ssize_t( from.y() - ya )) );
    polygon.push_back( Math::Point( ssize_t( to.x() - xa), ssize_t( to.y() - ya )) );
    polygon.push_back( Math::Point( ssize_t( to.x() + xa), ssize_t( to.y() + ya)) );
    polygon.push_back( Math::Point( ssize_t( from.x() + xa), ssize_t( from.y() + ya)) );

    Math::Rect clippingRect( Math::Point( 0, 0 ), Math::Size( image.width() - 1, image.height()- 1 ) );

    _fillConvexPolygon.clip( clippingRect, polygon );

	if( polygon.empty() )
		return;
		
	_fillPolygon.draw( image,   brush,  polygon ); */
	
    rasterize( image, pen, from, to , _rasterBuffer );
    _fillConvexPolygon.output( image, brush, _rasterBuffer );	
}


} // namespace Gfx

} // namespace Pt
