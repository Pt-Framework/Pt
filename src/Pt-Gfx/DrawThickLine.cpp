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

#include "Pt/Math/MathUtils.h"

#include <algorithm>
#include <cmath>


namespace Pt {

namespace Gfx {

static int buildLineEdge (double x0, double y0, double k, int dx, int dy, int xi, int yi, bool left, LineEdge *edge)
{
    int x, y, e;
    int xady;

    // make dy positive, since edge will be traversed downward
    if (dy < 0)
    {
        dy = -dy;
        dx = -dx;
        k = -k;
    }

    // integer starting value for y: round up the floating-point value
    y = (int) ceil(y0);

    // work out integer starting value for x
    xady = ( (int) ceil(k) ) + y * dx;
    if (xady <= 0)
        x = - (-xady / dy) - 1;
    else
        x = (xady - 1) / dy;

    // start working out initial value of decision variable
    e = xady - x * dy;

    // work out optional and non-optional x increment for algorithm
    if (dx >= 0)
    {
        // optional step
        edge->setSignDX(1);

        // non-optional step, 0 if dx<dy in mag.
        edge->setStepX( dx / dy );

        edge->setDX( dx % dy );
    }
    else
    {
        // optional step
        edge->setSignDX(-1);

        // non-optional step, 0 if dx<dy in mag.
        edge->setStepX( -(-dx/dy) );

        edge->setDX(-dx % dy);
        e = dy - e + 1;
    }
    edge->setDY(dy);

    // starting value for x
    edge->setX( x + (left == true ? 1 : 0) + xi );

    // bias: initial value for e
    edge->setE( e - dy );

    // return integer starting value for y, i.e. top of edge
    return y + yi;
}


static void fillRect(ARgbImage& image,const Pen& pen, int x, int y, unsigned int w, unsigned int h)
{

    Pt::ssize_t xbegin = std::max( 0, x );

    Pt::ssize_t xend = 0;

    if( (x + (int)w)  >= 0 )
        xend = std::min( image.width(), x + w ) ;

    Pt::ssize_t ypos = std::max( 0, y );

    Pt::ssize_t yend = 0;

    if( (y + (int) h) > 0 )
        yend = std::min( image.height(), y + h ) ;

    for( ; ypos < yend; ypos++ )
        for( ssize_t xpos = xbegin; xpos < xend; xpos++ )
            image.pixel( xpos, ypos) = pen.color();
}

static void fillLine(ARgbImage& image, const Pen& pen, int y, unsigned int overall_height, LineEdge *left, LineEdge *right, int left_count, int right_count)

{
    int left_x = 0, left_e = 0;
    int left_stepx = 0;
    int left_signdx = 0;
    int left_dy = 0, left_dx = 0;

    int right_x = 0, right_e = 0;
    int right_stepx = 0;
    int right_signdx = 0;
    int right_dy = 0, right_dx = 0;

    unsigned int left_height = 0, right_height = 0;

    while ((left_count || left_height) && (right_count || right_height))
    {
        unsigned int height;

        // load fields from next left edge, right edge
        if (!left_height && left_count)
        {
            left_height = left->height();
            left_x = left->x();
            left_stepx = left->stepx();
            left_signdx = left->signdx();
            left_e = left->e();
            left_dy = left->dy();
            left_dx = left->dx();
            --left_count;
            ++left;
        }

        // load fields from next right edge
        if (!right_height && right_count)
        {
            right_height = right->height();
            right_x = right->x();
            right_stepx = right->stepx();
            right_signdx = right->signdx();
            right_e = right->e();
            right_dy = right->dy();
            right_dx = right->dx();
            --right_count;
            ++right;
        }

        height = std::min(left_height, right_height);
        left_height -= height;
        right_height -= height;

        // walk down to end of left or right edge, whichever comes first
        while (height--)
        {

                    if( y >= 0 &&  y < (int)image.height())
            {
                // generate a span (omitting point on right end, see above)
                if (right_x >= left_x)
                {
                    int xpos = std::max( left_x, 0 );
                    const int endx = std::min<int>( right_x, image.width() -1);

                    for(; xpos <= endx; ++xpos)
                    {
                        image.pixel(xpos, y) = pen.color();
                    }
                }
            }

            y++;

            // update left_x, right_x by stepping along left and right edges,
            // using midpoint line algorithm
            left_x += left_stepx;
            left_e += left_dx;
            if (left_e > 0)
            {
                left_x += left_signdx;
                left_e -= left_dy;
            }

            right_x += right_stepx;
            right_e += right_dx;
            if (right_e > 0)
            {
                right_x += right_signdx;
                right_e -= right_dy;
            }
        }
    }
}


DrawThickLine::DrawThickLine()
{
    _dashPaterrn.push_back( true );
    _dashPaterrn.push_back( true );
    _dashPaterrn.push_back( true );
    _dashPaterrn.push_back( false);
}


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


void DrawThickLine::rasterize( ARgbImage& image, const Pen& pen,
                               const Math::Point& from, const Math::Point to,
                               RasterBuffer& rasterBuffer )
{
    const int   dx = to.x() - from.x() ;
    const int   dy = to.y() - from.y() ;
    const double halfPen = ( 0.5 * (double) ( pen.size()) );
    const double L = Math::hypot( (double)dx,  (double) dy );
    const double r1 = halfPen / L;
    Pt::ssize_t xa = Pt::ssize_t(r1 * dy) ;
    Pt::ssize_t ya = Pt::ssize_t(-r1 * dx) ;

    std::vector<Math::Point> polygon;
    polygon.push_back( Math::Point(from.x() - xa, from.y() - ya) ); // leftTop
    polygon.push_back( Math::Point(from.x() + xa, from.y() + ya) ); // rightTop
    polygon.push_back( Math::Point(to.x()   + xa, to.y()   + ya) ); // rightBottom
    polygon.push_back( Math::Point(to.x()   - xa, to.y()   - ya) ); // leftBottom

    Math::Rect clippingRect( Math::Point( 0, 0 ), Math::Size( image.width() - 1, image.height() - 1 ) );

    _fillConvexPolygon.clip( clippingRect, polygon );

    rasterBuffer.clear();
    rasterBuffer.spans().resize( image.height() );

    if( polygon.size() == 0 )
        return;

    _fillConvexPolygon.rasterize(  polygon, rasterBuffer );
}


void DrawThickLine::draw( ARgbImage& image, const Pen& pen,
                          const Math::Point& from, const Math::Point& to )
{
    if( image.height() == 0 || image.width() == 0 )
        return;

    LineFace leftFace;
    LineFace rightFace;
    
    switch( pen.style() )
    {
        case Pen::SolidStyle:
            this->drawSegment(image, pen, from, to, false, false, &leftFace, &rightFace);
        break;
        case Pen::DashStyle:
            this->drawPattern( image, pen, from, to, false, false, &leftFace, &rightFace, _dashPaterrn );
        break;
    }
}

void DrawThickLine::drawPattern( ARgbImage& image, const Pen& pen,
                                 Pt::Math::Point from, Pt::Math::Point to,
                                 bool projectLeft, bool projectRight,
                                LineFace* leftFace, LineFace* rightFace, 
                                const std::vector<bool>& pattern )
{

   

}


void DrawThickLine::drawSegment(ARgbImage& image, const Pen& pen,
                                Pt::Math::Point from, Pt::Math::Point to,
                                bool projectLeft, bool projectRight,
                                LineFace* leftFace, LineFace* rightFace)
{
    double      projectXoff = 0.0, projectYoff = 0.0;
    double      maxy;
    int          x, y;
    int          finaly;
    LineEdge* left = 0;
    LineEdge* right = 0;
    LineEdge* top = 0;
    LineEdge* bottom = 0;
    int          lefty, righty, topy, bottomy;
    LineEdge  lefts[2], rights[2];
    int          lw = pen.size();

    //
    // draw from top to bottom or from left to right
    //
    if( to.y() <  from.y() ||
    ( to.y() == from.y() && to.x() < from.x() ) )
    {
        std::swap(from, to);
        std::swap(projectLeft, projectRight);
        std::swap(leftFace, rightFace);
    }

    int dy = to.y() - from.y();
    int signdx = 1;
    int dx = to.x() - from.x();

    if (dx < 0)
        signdx = -1;

    leftFace->setX( from.x() );
    leftFace->setY( from.y() );
    leftFace->setDX( dx );
    leftFace->setDY( dy );

    // for faces, (dx, dy) points into the line
    rightFace->setX( to.x() );
    rightFace->setY( to.y() );
    rightFace->setDX( -dx );
    rightFace->setDY( -dy );

    //
    // neither horizontal
    //
    if (dy == 0)
    {
        rightFace->setXA( 0 );
        rightFace->setYA( 0.5 * (double)lw );
        rightFace->setK( -0.5 * (double)(lw * dx));
        leftFace->setXA( 0 );
        leftFace->setYA( -rightFace->ya() );
        leftFace->setK( rightFace->k() );
        x = from.x();

        if( projectLeft )
            x -= (lw >> 1);

        y = from.y() - (lw >> 1);
        dx = to.x() - x;

        if( projectRight )
            dx += ((lw + 1) >> 1);

        dy = lw;
        fillRect( image, pen,  x, y, (unsigned int)dx, (unsigned int)dy );
    }
    //
    // vertical
    //
    else if (dx == 0)
    {
        leftFace->setXA( 0.5 * (double)lw );
        leftFace->setYA( 0 );
        leftFace->setK( 0.5 * (double)(lw * dy) );
        rightFace->setXA( -leftFace->xa() );
        rightFace->setYA( 0 );
        rightFace->setK( leftFace->k() );
        y = from.y();

        if( projectLeft )
            y -= lw >> 1;

        x = from.x() - (lw >> 1);
        dy = to.y() - y;

        if( projectRight )
        dy += ((lw + 1) >> 1);

        dx = lw;
        fillRect( image, pen, x, y, (unsigned int)dx, (unsigned int)dy);
    }
    //
    // neither horizontal nor vertical
    //
    else
    {
        double l = 0.5 * ((double) lw);
        double L = Math::hypot ((double) dx, (double) dy);

        if (dx < 0)
        {
            right = &rights[1];
            left = &lefts[0];
            top = &rights[0];
            bottom = &lefts[1];
        }
            else
        {
            right = &rights[0];
            left = &lefts[1];
            top = &lefts[0];
            bottom = &rights[1];
        }

        double r = l / L;
        double ya = -r * dx;
        double xa = r * dy;

        if( projectLeft | projectRight )
        {
            projectXoff = -ya;
            projectYoff = xa;
        }

        double k = l * L;

        leftFace->setXA(xa);
        leftFace->setYA(ya);
        leftFace->setK(k);

        rightFace->setXA(-xa);
        rightFace->setYA(-ya);
        rightFace->setK(k);


        if(projectLeft)
            righty = buildLineEdge(xa - projectXoff, ya - projectYoff, k, dx, dy, from.x(), from.y(), false, right);
        else
            righty = buildLineEdge(xa, ya, k, dx, dy, from.x(), from.y(), false, right);

        //
        // first long edge
        //
        if(projectRight)
        {
            double xap = xa + projectXoff;
            double yap = ya + projectYoff;
            bottomy = buildLineEdge(xap, yap, xap * dx + yap * dy, -dy, dx,  to.x(), to.y(), (dx < 0 ? true : false), bottom);
            maxy = -ya + projectYoff;
        }
        else
        {
            bottomy = buildLineEdge(xa, ya, 0.0, -dy, dx,to.x(), to.y(), (dx < 0 ? true : false), bottom);
            maxy = -ya;
        }

        //
        // second long edge
        //
        ya = -ya;
        xa = -xa;
        k = -k;

        if (projectLeft)
            lefty = buildLineEdge (xa - projectXoff, ya - projectYoff, k, dx, dy, from.x(), from.y(), true, left);
        else
            lefty = buildLineEdge (xa, ya, k, dx, dy, from.x(), from.y(), true, left);

        //
        // first short edge on left end
        //
        if (signdx > 0)
        {
            ya = -ya;
            xa = -xa;
        }

        if (projectLeft)
        {
            double xap = xa - projectXoff;
            double yap = ya - projectYoff;
            topy = buildLineEdge (xap, yap, xap * dx + yap * dy, -dy, dx, from.x(), from.y(), (dx > 0 ? true : false), top);
        }
        else
            topy = buildLineEdge (xa, ya, 0.0, -dy, dx, from.x(), from.y(), (dx > 0 ? true : false), top);

        //
        // first short edge on right end
        //
        if (projectRight)
        {
            double xap = xa + projectXoff;
            double yap = ya + projectYoff;
            bottomy = buildLineEdge (xap, yap, xap * dx + yap * dy, -dy, dx,  to.x(), to.y(), (dx < 0 ? true : false), bottom);
            maxy = -ya + projectYoff;
        }
        else
        {
            bottomy = buildLineEdge (xa, ya, 0.0, -dy, dx, to.x(), to.y(), (dx < 0 ? true : false), bottom);
            maxy = -ya;
        }

        finaly = ( (int) ceil(maxy) ) + to.y();

        if (dx < 0)
        {
            left->setHeight( (unsigned int)(bottomy - lefty) );
            right->setHeight( (unsigned int)(finaly - righty) );
            top->setHeight( (unsigned int)(righty - topy) );
        }
        else
        {
            right->setHeight( (unsigned int)(bottomy - righty) );
            left->setHeight( (unsigned int)(finaly - lefty) );
            top->setHeight( (unsigned int)(lefty - topy) );
        }
        bottom->setHeight( (unsigned int)(finaly - bottomy) );

        fillLine(image, pen, topy, (unsigned int)(bottom->height() + bottomy - topy), lefts, rights, 2, 2);
    }
}

} // namespace Gfx

} // namespace Pt
