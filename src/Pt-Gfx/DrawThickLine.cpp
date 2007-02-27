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

using namespace Pt::Math;

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
{ }

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
            this->drawDashSegment( image, pen, from, to, false, false, &leftFace, &rightFace);
        break;
    }
    
    lineArc( image, pen, &leftFace, &rightFace, from.x(), from.y(), false );
    lineArc( image, pen, &leftFace, &rightFace, to.x(), to.y(), false );
}

void DrawThickLine::drawDashSegment( ARgbImage& image, const Pen& pen,
                                    Pt::Math::Point from, Pt::Math::Point to,
                                    bool projectLeft, bool projectRight,
                                    LineFace* leftFace, LineFace* rightFace )
{
    const size_t  dashLength       = pen.size() * 3;
    const size_t  spaceLength      = pen.size();
    const size_t  segmentLenght    = dashLength + spaceLength;
    const ssize_t dx               = to.x() - from.x();
    const ssize_t dy               = to.y() - from.y();    
    const double  lineLength       = Math::hypot( dx , dy );
    const double  xincdashspace    = dx / (lineLength / segmentLenght);
    const double  yincdashspace    = dy / (lineLength / segmentLenght);
    const double  noOfDashes       = lineLength / dashLength;
    const double  xincdash         = dx / noOfDashes;
    const double  yincdash         = dy / noOfDashes;
        
    size_t  counter = 0;
    Point   segFrom;
    Point   segTo;

    for( size_t i = 0; i < lineLength - dashLength; i += segmentLenght )
    {    
        segFrom.setX( ssize_t( from.x() + xincdashspace * counter ) );
        segFrom.setY( ssize_t( from.y() + yincdashspace * counter ) );
        
        segTo.setX( ssize_t( from.x() + xincdashspace * counter + xincdash ) );
        segTo.setY( ssize_t( from.y() + yincdashspace * counter + yincdash ) );
        
        drawSegment( image, pen, segFrom,  segTo, false, false, leftFace, rightFace );
        counter++;
    }
    
    if( ( dashLength + spaceLength ) * counter <= lineLength )
    {    
        segFrom.setX( ssize_t( from.x() + xincdashspace * counter ) );
        segFrom.setY( ssize_t( from.y() + yincdashspace * counter ) );
        
        segTo.setX( to.x() );
        segTo.setY( to.y() );
                
        drawSegment( image, pen, segFrom, segTo, false, false, leftFace, rightFace );
    }
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

void DrawThickLine::lineArc( ARgbImage& image, const Pen& pen, LineFace *leftFace, LineFace *rightFace, double xorg, double yorg, bool isInt)
{
    std::vector<Pt::Math::Point>    points;
    std::vector<size_t>             widths;
    int             xorgi = 0, yorgi = 0;
    int 		    n;
    PolyEdge	    edge1, edge2;
    int		        edgey1, edgey2;
    bool	    	edgeleft1, edgeleft2;

    if (isInt)
    /* in integer case, take (xorgi,yorgi) from face; otherwise (0,0) */
    {
      xorgi = leftFace ? leftFace->x() : rightFace->x();
      yorgi = leftFace ? leftFace->y() : rightFace->y();
    }

    edgey1 = INT_MAX;
    edgey2 = INT_MAX;
    edge1.setX( 0 );			/* not used, keep memory checkers happy */
    edge1.setDY( -1 );
    edge2.setX( 0 );			/* not used, keep memory checkers happy */
    edge2.setDY( -1 );
    
    edgeleft1 = false;
    edgeleft2 = false;
    
    if( (pen.style() != Pen::SolidStyle || pen.size() > 2) &&
        ((pen.capStyle() == Pen::RoundCap && pen.joinStyle() != Pen::RoundJoin)  ||
        ( pen.joinStyle() == Pen::RoundJoin/* && pen.capStyle() == (int)MI_CAP_BUTT*/)))
        /* construct clipping edges from the passed line faces (otherwise,
        ignore them; will just draw a disk) */
    {
        if (isInt)
	    {
	        xorg = (double) xorgi;
	        yorg = (double) yorgi;
        }
        
        if (leftFace && rightFace)
	        /* have two faces, so construct clipping edges for pie wedge */
	        //miRoundJoinClip (leftFace, rightFace, &edge1, &edge2, &edgey1, &edgey2, &edgeleft1, &edgeleft2);
	        ;

        else if (leftFace)
	        /* will draw half-disk on left face, so construct clipping edge */
	        //edgey1 = miRoundCapClip (leftFace, isInt, &edge1, &edgeleft1);      
	        ;
        
        else if (rightFace)
	        /* will draw half-disk on right face, so construct clipping edge */
	        //edgey2 = miRoundCapClip (rightFace, isInt, &edge2, &edgeleft2);
	        ;

        /* due to clipping, switch to using floating-point coordinates */
        isInt = false;
    }

    points.resize( pen.size() );
    widths.resize( pen.size() );

  /* construct a Spans by calling integer or floating point routine */
  if (isInt)
    /* integer routine, no clipping: just draw a disk */
    n = lineArcI( pen, xorgi, yorgi, points, widths );
  else
    /* call floating point routine, supporting clipping by edge(s) */
    n = lineArcD( pen, xorg, yorg, points, widths, &edge1, edgey1, edgeleft1, &edge2, edgey2, edgeleft2);
  
    //MI_PAINT_SPANS(paintedSet, pixel, n, points, widths)
    
    for( size_t i = 0; i < n; i++)
    {
        for( size_t j = 0; j < widths[i]; j++)
            image.pixel(points[i].x() + j, points[i].y() ) = pen.color();

    }   
}


int DrawThickLine::lineArcI (const Pen& pen, int xorg, int yorg, std::vector<Pt::Math::Point>& points, std::vector<size_t>& widths)
{
    Pt::Math::Point *tpts, *bpts;
    unsigned int *twids, *bwids;
    int x, y, e, ex;
    int slw;

    tpts = &points[0];
    twids = &widths[0];
    slw = (int)(pen.size());

    if (slw == 1)
    /* `disk' is a single pixel */
    {
        tpts->setX( xorg );
        tpts->setY( yorg );
        *twids = 1;
        return 1;
    }

    /* otherwise, draw the disk scanline by scanline */
    bpts = tpts + slw;
    bwids = twids + slw;
    y = (slw >> 1) + 1;

    if (slw & 1)
        e = - ((y << 2) + 3);
    else
        e = - (y << 3);
    
    ex = -4;
    x = 0;
    
    while (y)
    {
        e += (y << 3) - 4;
        while (e >= 0)
        {
            x++;
            e += (ex = -((x << 3) + 4));
        }
        
        y--;
        slw = (x << 1) + 1;
        
        if ((e == ex) && (slw > 1))
            slw--;
            
        tpts->setX( xorg - x );
        tpts->setY( yorg - y );
        tpts++;
        *twids++ = slw;
        
        if ((y != 0) && ((slw > 1) || (e != ex)))
        {
            bpts--;
            bpts->setX( xorg - x );
            bpts->setY( yorg + y );
            *--bwids = slw;
        }
    }

    /* return linewidth (no. of spans in the Spans) */
    return (int)( pen.size() );
}

#define CLIPSTEPEDGE( edgey, edge, edgeleft ) \
if (ybase == edgey) \
{ \
    if (edgeleft) \
      { \
	if (edge->x() > xcl) \
	  xcl = edge->x(); \
} \
  else \
    { \
      if (edge->x() < xcr) \
	xcr = edge->x(); \
} \
  edgey++; \
    edge->setX( edge->x() + edge->stepx() ); \
      edge->setE( edge->e() + edge->dx()); \
	if (edge->e() > 0) \
	  { \
	    edge->setX( edge->x() + edge->signdx() ); \
	      edge->setE( edge->e() - edge->dy() ); \
} \
}


int DrawThickLine::lineArcD( const Pen & pen, double xorg, double yorg, std::vector<Pt::Math::Point>& points, std::vector<size_t>& widths, PolyEdge *edge1, int edgey1, bool edgeleft1, PolyEdge *edge2, int edgey2, bool edgeleft2)
{
    Pt::Math::Point *pts;
    size_t *wids;
    double radius, x0, y0, el, er, yk, xlk, xrk, k;
    int xbase, ybase, y, boty, xl, xr, xcl, xcr;
    int ymin, ymax;
    bool edge1IsMin, edge2IsMin;
    int ymin1, ymin2;

    pts = &points[0];
    wids = &widths[0];
    xbase = (int)(floor(xorg));
    x0 = xorg - xbase;
    ybase = ceil(yorg);
    y0 = yorg - ybase;
    xlk = x0 + x0 + 1.0;
    xrk = x0 + x0 - 1.0;
    yk = y0 + y0 - 1.0;
    radius = 0.5 * ((double)pen.size());
    y = (int)(floor(radius - y0 + 1.0));
    ybase -= y;
    ymin = ybase;
    ymax = std::numeric_limits<int>::max();
    edge1IsMin = false;
    ymin1 = edgey1;
    
    if( edge1->dy() >= 0 )
    {
        if( !edge1->dy() )
        {
            if (edgeleft1)
                edge1IsMin = true;
            else
                ymax = edgey1;
                
            edgey1 = std::numeric_limits<int>::max();
        }
        else
        {
            if( (edge1->signdx() < 0 ) == edgeleft1)
                edge1IsMin = true;
        }
    }
    edge2IsMin = false;
    ymin2 = edgey2;
    
    if( edge2->dy() >= 0 )
    {
        if( !edge2->dy() )
        {
            if( edgeleft2 )
                edge2IsMin = true;
            else
                ymax = edgey2;
                
            edgey2 = std::numeric_limits<int>::max();
        }
        else
        {
            if( (edge2->signdx() < 0) == edgeleft2 )
                edge2IsMin = true;
        }
    }
    
    if( edge1IsMin )
    {
        ymin = ymin1;
        
        if( edge2IsMin && ymin1 > ymin2 )
            ymin = ymin2;
    } 
    else if (edge2IsMin)
    {
        ymin = ymin2;
    }
    
    el = radius * radius - ((y + y0) * (y + y0)) - (x0 * x0);
    er = el + xrk;
    xl = 1;
    xr = 0;
    
    if (x0 < 0.5)
    {
        xl = 0;
        el -= xlk;
    }
    
    boty = (y0 < -0.5) ? 1 : 0;
    
    if (ybase + y - boty > ymax)
        boty = ymax - ybase - y;
    
    while (y > boty)
    {
        k = (y << 1) + yk;
        er += k;
        
        while (er > 0.0)
        {
            xr++;
            er += xrk - (xr << 1);
        }
        el += k;
        
        while (el >= 0.0)
        {
            xl--;
            el += (xl << 1) - xlk;
        }
        
        y--;
        ybase++;
        
        if (ybase < ymin)
            continue;
        
        xcl = xl + xbase;
        xcr = xr + xbase;

        CLIPSTEPEDGE(edgey1, edge1, edgeleft1);
        CLIPSTEPEDGE(edgey2, edge2, edgeleft2);
        
        if (xcr >= xcl)
        {
            pts->setX( xcl );
            pts->setY( ybase );
            pts++;
            *wids++ = (unsigned int)(xcr - xcl + 1);
        }
    }
    
    er = xrk - (xr << 1) - er;
    el = (xl << 1) - xlk - el;
    boty = (int)(floor(-y0 - radius + 1.0));

    if (ybase + y - boty > ymax)
        boty = ymax - ybase - y;
        
    while (y > boty)
    {
        k = (y << 1) + yk;
        er -= k;
        
        while ((er >= 0.0) && (xr >= 0))
        {
            xr--;
            er += xrk - (xr << 1);
        }
        
        el -= k;
        
        while ((el > 0.0) && (xl <= 0))
        {
            xl++;
            el += (xl << 1) - xlk;
        }
        
        y--;
        ybase++;
        
        if( ybase < ymin )
            continue;
            
        xcl = xl + xbase;
        xcr = xr + xbase;
        
        CLIPSTEPEDGE(edgey1, edge1, edgeleft1);
        CLIPSTEPEDGE(edgey2, edge2, edgeleft2);
        
        if (xcr >= xcl)
        {
            pts->setX( xcl );
            pts->setY( ybase );
            pts++;
            *wids++ = (unsigned int)(xcr - xcl + 1);
        }
    }

    /* return number of spans in the Spans */
    return (pts - &points[0]);
}

} // namespace Gfx
} // namespace Pt
