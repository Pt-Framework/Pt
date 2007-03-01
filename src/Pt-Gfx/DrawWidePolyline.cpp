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

#include "DrawWidePolyline.h"
#include "Pt/Math/MathUtils.h"

#include <algorithm>
#include <cmath>

using namespace Pt::Math;

namespace Pt {
namespace Gfx {

/*
 double x0, y0;		starting point of edge (rel. to (xi,yi)) 
 double k;			 x0 * dy - y0 * dx 
 int dx, dy;		 edge has rational slope dy/dx 
 int xi, yi;		 integer offset for coordinate system 
 bool left;			 left edge, not right edge? 
 LineEdge *edge;	 integer edge data, to be filled in 
*/    

int DrawWidePolyline::buildLineEdge (double x0, double y0, double k, int dx, int dy, int xi, int yi, bool left, LineEdge *edge)
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


void DrawWidePolyline::fillRect(ARgbImage& image,const Pen& pen, int x, int y, unsigned int w, unsigned int h)
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


void DrawWidePolyline::fillLine(ARgbImage& image, const Pen& pen, int y, unsigned int overall_height, LineEdge *left, LineEdge *right, int left_count, int right_count)
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


DrawWidePolyline::DrawWidePolyline()
{ }

void DrawWidePolyline::draw( ARgbImage& image, const Pen& pen,const Pt::Math::Point* pPts, size_t npt)
{
    int		   x1, y1, x2, y2;
    bool	   projectLeft, projectRight;
    LineFace   leftFace, rightFace, prevRightFace;
    LineFace   firstFace;
    int        first;
    bool	   somethingDrawn = false;
    bool	   selfJoin;

    /* ensure we have >=1 points */
    if (npt <= 0)
        return;

    x2 = pPts->x();
    y2 = pPts->y();
    first = true;

    /* determine whether polyline is closed */
    selfJoin = false;
    
    if( npt > 1 )    
        if (x2 == pPts[npt-1].x() && y2 == pPts[npt-1].y())
        	selfJoin = true;
        

    /* line segments (except for the last) will not project right; they'll
    project left if the cap mode is "projecting" */
    projectLeft = false;//(pGC->capStyle == (int)MI_CAP_PROJECTING && !selfJoin) ? true : false;
    projectRight = false;

    /* iterate through points, drawing all line segments of nonzero length */
    while( --npt )
    {
        x1 = x2;
        y1 = y2;

        ++pPts;

        x2 = pPts->x();
        y2 = pPts->y();


        if (x1 != x2 || y1 != y2)
        {
            somethingDrawn = true;

            /*
            if (npt == 1 && pen.capStyle() == (int)MI_CAP_PROJECTING && !selfJoin)
                // last point; and need a projecting cap here 
                projectRight = true;
            */                

            /* draw segment (pixel=1), returning faces */
            
            switch( pen.style() )
            {
                case Pen::SolidStyle:
                    drawSegment( image, pen, Pt::Math::Point(x1, y1), Pt::Math::Point(x2, y2), projectLeft, projectRight, &leftFace, &rightFace );
                break;
                case Pen::DashStyle:
                    drawDashSegment( image, pen, Pt::Math::Point(x1, y1), Pt::Math::Point(x2, y2), projectLeft, projectRight, &leftFace, &rightFace );
                break;
            }                
                     
            if (first)
            /* first line segment, draw round cap if needed */
            {
                if (selfJoin)
                    firstFace = leftFace;

                else if (pen.capStyle() == Pen::RoundCap )
                    /* invoke miLineArc, isInt = true, to draw a round cap
                    on left face in paint type #1 */
                    lineArc( image, pen, &leftFace, 0,(double)0.0, (double)0.0, true );
            }
            else
                /* general case: draw join at beginning of segment (pixel=1) */
//               if( pen.style() == Pen::SolidStyle )
                    lineJoin( image, pen, &leftFace, &prevRightFace );
                
            prevRightFace = rightFace;
            first = false;
            projectLeft = false;
        }

        /* final point of polyline */
        if (npt == 1 && somethingDrawn)
        {
            if (selfJoin)
                /* add line join to close the polyline, pixel=1 */
                lineJoin( image, pen, &firstFace, &rightFace);
            else if (pen.capStyle() == Pen::RoundCap /*|| pGC->capStyle == (int)MI_CAP_TRIANGULAR */)
                /* invoke miLineArc, isInt = true, to draw round cap
                on right face, pixel=1 */
                 lineArc( image, pen, (LineFace *)NULL, &rightFace, (double)0.0, (double)0.0, true );
        }
    }
/*
    // handle crock where all points are coincident
    if( !somethingDrawn )
    {
        projectLeft = true; //(pGC->capStyle == (int)MI_CAP_PROJECTING) ? true : false;
        
        switch( pen.style() )
        {
            case Pen::SolidStyle:
                drawSegment( image, pen, Pt::Math::Point(x2, y2), Pt::Math::Point(x2, y2), projectLeft, projectRight, &leftFace, &rightFace );
            break;
            case Pen::DashStyle:
                drawDashSegment( image, pen, Pt::Math::Point(x1, y1), Pt::Math::Point(x2, y2), projectLeft, projectRight, &leftFace, &rightFace );
            break;
        }                


        drawSegment(image, pen, Pt::Math::Point(x2, y2), Pt::Math::Point(x2, y2), projectLeft, projectLeft, &leftFace, &rightFace);
        
        if( pen.capStyle() == Pen::RoundCap  ) //|| pGC->capStyle == (int)MI_CAP_TRIANGULAR
        {
            // invoke miLineArc, isInt = true, to draw round cap in paint type #1 
            lineArc( image, pen, &leftFace, (LineFace *)NULL, (double)0.0, (double)0.0, true );
            
            // invoke miLineArc, isInt = true, to draw other round cap in paint type #1 
            rightFace.setDX( -1);	// sleazy hack to make it work 
            lineArc( image, pen, (LineFace *) NULL, &rightFace, (double)0.0, (double)0.0, true );
        }
    }
    */
}


void DrawWidePolyline::drawDashSegment( ARgbImage& image, const Pen& pen,
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
    LineFace face;
        
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
        
        if( i == 0 )
            face = *leftFace;
            
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
    
    *leftFace = face;
}



/* Draw a wide, dashed polyline, by dashing each line segment and joining
   appropriately.  miWideDashSegment() is called to dash each line
   segment. */
void DrawWidePolyline::drawDashLine( ARgbImage& image, const Pen& pen, const  Math::Point* points, size_t pointCount )
{
}


void DrawWidePolyline::drawSegment(ARgbImage& image, const Pen& pen,
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
            righty = buildLineEdge( xa - projectXoff, ya - projectYoff, k, dx, dy, from.x(), from.y(), false, right );
        else
            righty = buildLineEdge( xa, ya, k, dx, dy, from.x(), from.y(), false, right );

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

void DrawWidePolyline::lineArc( ARgbImage& image, const Pen& pen, LineFace *leftFace, LineFace *rightFace, double xorg, double yorg, bool isInt)
{
    std::vector<Pt::Math::Point>    points;
    std::vector<size_t>             widths;
    int             xorgi = 0, yorgi = 0;
    int 		    n;
    LineEdge	    edge1, edge2;
    int		        edgey1, edgey2;
    bool	    	edgeleft1, edgeleft2;

    if (isInt)
    /* in integer case, take (xorgi,yorgi) from face; otherwise (0,0) */
    {
      xorgi = leftFace ? leftFace->x() : rightFace->x();
      yorgi = leftFace ? leftFace->y() : rightFace->y();
    }

    edgey1 = std::numeric_limits<int>::max();
    edgey2 = std::numeric_limits<int>::max();
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
	        roundJoinClip (leftFace, rightFace, &edge1, &edge2, &edgey1, &edgey2, &edgeleft1, &edgeleft2);

        else if (leftFace)
	        /* will draw half-disk on left face, so construct clipping edge */
	        edgey1 = roundCapClip( leftFace, isInt, &edge1, &edgeleft1 );

        
        else if (rightFace)
	        /* will draw half-disk on right face, so construct clipping edge */
	        edgey2 = roundCapClip (rightFace, isInt, &edge2, &edgeleft2);

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
    
    for( ssize_t i = 0; i < n; i++)
    {
        //A stupide clipping.
        if( points[i].y() < 0 )
            continue;

        if( points[i].y() >= (Pt::ssize_t)image.height() )
            continue;
            
        if( points[i].x() >= (Pt::ssize_t) image.width() )
            continue;
        
            
        if( points[i].x() < 0 )
        {
            if(  widths[i] > -points[i].x()  )
            {
                widths[i] += points[i].x();
                points[i].setX( 0 );
            }
            else
            {
                continue;
            }
        }

       
        if( points[i].x() +  widths[i] >  (ssize_t) image.width() )
            widths[i] =  image.width() - points[i].x();
                        
         _stroke->stroke( image, pen, points[i].x(), points[i].y(), widths[i] );
    }   
}

/* From a line face, construct a clipping edge that will be used by
   miLineArcD when drawing a half-disk.  */
int DrawWidePolyline::roundCapClip( const LineFace *face, bool isInt, LineEdge *edge, bool *leftEdge )
{
    int	    y;
    int 	dx, dy;
    double  xa, ya, k;
    bool	left;

    dx = -face->dy();
    dy = face->dx();
    xa = face->xa();
    ya = face->ya();
    k = 0.0;
    
    if( !isInt )
        k = face->k();
        
    left = true;
    
    if (dy < 0 || (dy == 0 && dx > 0))
    {
        dx = -dx;
        dy = -dy;
        xa = -xa;
        ya = -ya;
        left = (left ? false : true);
    }
    
    if( dx == 0 && dy == 0 )
        dy = 1;
        
    if( dy == 0 )
    {
        y = ceil( face->ya() ) + face->y();
        edge->setX( std::numeric_limits<int>::min() );
        edge->setStepX( 0 );
        edge->setSignDX( 0 );
        edge->setE( -1 );
        edge->setDY( 0 );
        edge->setDX( 0 );
        edge->setHeight( 0 );
    }
    else
    {
        y = buildLineEdge( xa, ya, k, dx, dy, face->x(), face->y(), (left ? false : true), edge);
        edge->setHeight( std::numeric_limits<unsigned int>::max() );	/* number of scanlines to process */
    }
    
    *leftEdge = (left ? false : true);

    return y;
}


/* From two line faces, construct clipping edges that will be used by
   miLineArcD when drawing a pie wedge.  The line faces may be modified. */
void DrawWidePolyline::roundJoinClip (LineFace *pLeft, LineFace *pRight, LineEdge *edge1, LineEdge *edge2, int *y1, int *y2, bool *left1, bool *left2)
{
    int	denom;

    denom = - pLeft->dx() * pRight->dy() + pRight->dx() * pLeft->dy();
    
    if (denom >= 0)
    {
        pLeft->setXA( -pLeft->xa() );
        pLeft->setYA( -pLeft->ya() );
    }
    else
    {
        pRight->setXA( -pRight->xa() );
        pRight->setYA( -pRight->ya() );
    }
    
    *y1 = roundJoinFace( pLeft, edge1, left1 );
    *y2 = roundJoinFace( pRight, edge2, left2 );
}

/* helper function called by the preceding */
int DrawWidePolyline::roundJoinFace( const LineFace *face, LineEdge *edge, bool *leftEdge )
{
    int	    y;
    int	    dx, dy;
    double  xa, ya;
    bool    left;

    dx = -face->dy();
    dy = face->dx();
    xa = face->xa();
    ya = face->ya();
    left = true;
    
    if (ya > 0)
    {
        ya = 0.0;
        xa = 0.0;
    }
    
    if (dy < 0 || (dy == 0 && dx > 0))
    {
        dx = -dx;
        dy = -dy;
        left = (left ? false : true);
    }
    
    if (dx == 0 && dy == 0)
        dy = 1;
        
    if (dy == 0)
    {
        y = ceil( face->ya() ) + face->y();
        edge->setX( std::numeric_limits<int>::min() );
        edge->setStepX( 0 );
        edge->setSignDX( 0 );
        edge->setE( -1 );
        edge->setDY( 0 );
        edge->setDX( 0 );
        edge->setHeight( 0 );
    }
    else
    {
        y = buildLineEdge( xa, ya,  0.0, dx, dy, face->x(), face->y(), (left ? false : true), edge );
        
        edge->setHeight( std::numeric_limits<unsigned int>::max() );	/* number of scanlines to process */
    }
    
    *leftEdge = (left ? false : true);

    return y;
}

int DrawWidePolyline::lineArcI (const Pen& pen, int xorg, int yorg, std::vector<Pt::Math::Point>& points, std::vector<size_t>& widths)
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

/* Draw as a Spans a filled disk of diameter equal to the linewidth, paying
   attention to one or two clipping edges.  This is used for round caps and
   round joins, respectively (it respectively yields a half-disk or a pie
   wedge).  Floating point coordinates are used.  Returns number of spans
   in the Spans.  The clipping edges may be modified. */
   
int DrawWidePolyline::lineArcD( const Pen & pen, double xorg, double yorg, std::vector<Pt::Math::Point>& points, std::vector<size_t>& widths, LineEdge *edge1, int edgey1, bool edgeleft1, LineEdge *edge2, int edgey2, bool edgeleft2)
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

/* Paint all types of line join: round/miter/bevel/triangular.  Called by
   both miWideLine() and miWideDash().  Left and right line faces are
   supplied, each with its own value of k.  They may be modified. */
void DrawWidePolyline::lineJoin( ARgbImage& image, const Pen& pen, LineFace *pLeft, LineFace *pRight )
{
    double	            mx = 0.0, my = 0.0;
    int		            denom = 0;	/* avoid compiler warnings */
    Pt::Math::PointF    vertices[4];
    PolySlope           slopes[4];
    int		            edgecount;
    LineEdge            left[4], right[4];
    int                 nleft, nright;
    int                 y;
    unsigned int        height;
    bool		        swapslopes;
    int		            lw = pen.size();

    if( pen.joinStyle() == Pen::RoundJoin )
    {
        /* invoke miLineArc to fill the round join, isInt = true */
        lineArc( image, pen,pLeft, pRight, (double)0.0, (double)0.0, true );
        return;
    }

    denom = - pLeft->dx() * pRight->dy() + pRight->dx() * pLeft->dy();
    
    if( denom == 0 )
        return;			/* no join to draw */

    /* Now must handle cases where line join is a small polygon to be filled;
    specify its vertices clockwise. */

    /* swap slopes if cross product of line faces has wrong sign */
    if (denom > 0)
    {
        swapslopes = false;
        pLeft->setXA( -pLeft->xa() );
        pLeft->setYA( -pLeft->ya() );
        pLeft->setDX( -pLeft->dx() );
        pLeft->setDY( -pLeft->dy() );
    }
    else
    {
        swapslopes = true;
        pRight->setXA( -pRight->xa() );
        pRight->setYA( -pRight->ya() );
        pRight->setDX( -pRight->dx() );
        pRight->setDY( -pRight->dy() );
    }

    /* vertex #0 is at the right end of the right face */
    vertices[0].setX( pRight->xa() );
    vertices[0].setY( pRight->ya() );
    slopes[0].dx = -pRight->dy();
    slopes[0].dy =  pRight->dx();
    slopes[0].k = 0;

    /* vertex #1 is the nominal join point (i.e. halfway across both the
    right face and the left face) */
    vertices[1].setX( 0 );
    vertices[1].setY( 0 );
    slopes[1].dx =  pLeft->dy();
    slopes[1].dy = -pLeft->dx();
    slopes[1].k = 0;

    /* vertex #2 is at the left end of the left face */
    vertices[2].setX( pLeft->xa() );
    vertices[2].setY( pLeft->ya() );

/*    if( joinStyle == (int)MI_JOIN_MITER )
    {
        double miterlimit = pGC->miterLimit;

        // compute vertex (mx,my) of miter quadrilateral 
        my = (pLeft->dy  * (pRight->xa * pRight->dy - pRight->ya * pRight->dx) -
            pRight->dy * (pLeft->xa  * pLeft->dy  - pLeft->ya  * pLeft->dx )) /
            (double) denom;
        if (pLeft->dy != 0)
            mx = pLeft->xa + (my - pLeft->ya) *
            (double) pLeft->dx / (double) pLeft->dy;
        else
            mx = pRight->xa + (my - pRight->ya) *
            (double) pRight->dx / (double) pRight->dy;
        // if miter limit violated, switch to bevelled join
        if ((mx * mx + my * my) * 4 > miterlimit * miterlimit * lw * lw)
            joinStyle = (int)MI_JOIN_BEVEL;
    }
*/    

    double scale, dx, dy, adx, ady;
    
    switch( pen.joinStyle() )
    {
//        case (int)MI_JOIN_MITER:
        default:
        /* join by adding a quadrilateral */
        edgecount = 4;

        slopes[2].dx = pLeft->dx();
        slopes[2].dy = pLeft->dy();
        slopes[2].k =  pLeft->k();
        
        if( swapslopes )
        {
            slopes[2].dx = -slopes[2].dx;
            slopes[2].dy =  -slopes[2].dy;
            slopes[2].k  = -slopes[2].k;
        }

        /* vertex #3 is miter vertex (mx,my) */
        vertices[3].setX( mx );
        vertices[3].setY( my );
        slopes[3].dx = pRight->dx();
        slopes[3].dy = pRight->dy();
        slopes[3].k  = pRight->k();
        
        if (swapslopes)
        {
            slopes[3].dx = -slopes[3].dx;
            slopes[3].dy = -slopes[3].dy;
            slopes[3].k  = -slopes[3].k;
        }
    break;

    case Pen::BevelJoin: //join by adding a triangle    
    {
        Pt::Math::PointF midpoint;

        edgecount = 3;

        /* third edge of triangle will pass through midpoint */
        midpoint.setX( 0.5 * (pLeft->xa() + pRight->xa() ) );
        midpoint.setY( 0.5 * (pLeft->ya() + pRight->ya() ) );

        /* vector along third edge of triangle */
        dx = pRight->xa() - pLeft->xa();
        dy = pRight->ya() - pLeft->ya();

        /* compute scale = max(|dx|,|dy|) */
        adx = dx;
        ady = dy;
        if (adx < 0)
            adx = -adx;
        if (ady < 0)
            ady = -ady;
        scale = ady;
        if (adx > ady)
            scale = adx;

        /* use integer dx, dy in range -65536..65536 */
        slopes[2].dx = (int)((dx * 65536) / scale);
        slopes[2].dy = (int)((dy * 65536) / scale);
        slopes[2].k = midpoint.x() * slopes[2].dy - midpoint.y() * slopes[2].dx;
    }
    break;
/*
    case (int)MI_JOIN_TRIANGULAR:
        // join by adding a stubby quadrilateral
        {
            PolyVertex midpoint, newpoint;
            double mid2, mid, dx2, dy2, dx3, dy3;

            edgecount = 4;

            // compute additional vertex, offset by linewidth/2
            midpoint.x = 0.5 * (pLeft->xa + pRight->xa);
            midpoint.y = 0.5 * (pLeft->ya + pRight->ya);
            mid2 = midpoint.x * midpoint.x + midpoint.y * midpoint.y;
            mid = sqrt (mid2);
            newpoint.x = 0.5 * lw * midpoint.x / mid;
            newpoint.y = 0.5 * lw * midpoint.y / mid;	
            vertices[3] = newpoint;

            // offset from vertices[2] to vertices[3]
            dx2 = vertices[3].x - vertices[2].x;
            dy2 = vertices[3].y - vertices[2].y;	

            // offset from vertices[3] back to vertices[0] 
            dx3 = vertices[0].x - vertices[3].x;
            dy3 = vertices[0].y - vertices[3].y;	

            // compute scale = max(|dx|,|dy|), where (dx,dy) is offset between
            // the two corners, i.e. vertices[0] and vertices[2] 
            dx = pRight->xa - pLeft->xa;
            dy = pRight->ya - pLeft->ya;
            adx = dx;
            ady = dy;
            if (adx < 0)
                adx = -adx;
            if (ady < 0)
                ady = -ady;
            scale = ady;
            if (adx > ady)
                scale = adx;

            // use integer dx, dy in range -65536..65536 
            slopes[2].dx = (int)((dx2 * 65536) / scale);
            slopes[2].dy = (int)((dy2 * 65536) / scale);
            slopes[2].k = newpoint.x * slopes[2].dy - newpoint.y * slopes[2].dx;

            // use integer dx, dy in range -65536..65536 
            slopes[3].dx = (int)((dx3 * 65536) / scale);
            slopes[3].dy = (int)((dy3 * 65536) / scale);
            slopes[3].k = newpoint.x * slopes[3].dy - newpoint.y * slopes[3].dx;
        }
        break;
        */
    }

    /* compute lists of left and right edges for the small polygon, using the
    just-computed slopes array */
    y = polyBuildPoly( vertices, slopes, edgecount, pLeft->x(), pLeft->y(), left, right, &nleft, &nright, &height);
    
    /* fill the small polygon */    
    fillLine( image, pen, y, height, left, right, nleft, nright);
}

#define StepAround(v, incr, max) (((v) + (incr) < 0) ? (max - 1) : ((v) + (incr) == max) ? 0 : ((v) + (incr)))

int DrawWidePolyline::polyBuildPoly( const Pt::Math::PointF *vertices, const PolySlope *slopes, int count, int xi, int yi, LineEdge *left, LineEdge *right, int *pnleft, int *pnright, unsigned int *h)
{
    int	    top, bottom;
    double  miny, maxy;
    int     i;
    int     j;
    int	    clockwise;
    int	    slopeoff;
    int     s;
    int     nright, nleft;
    int     y, lasty = 0, bottomy, topy = 0;

    /* compute min, max y values for polygon (floating-point); also location
    of corresponding vertices in vertex array */
    maxy = miny = vertices[0].y();
    bottom = top = 0;

    for (i = 1; i < count; i++)
    {
        if (vertices[i].y() < miny)
        {
            top = i;
            miny = vertices[i].y();
        }
        
        if (vertices[i].y() >= maxy)
        {
            bottom = i;
            maxy = vertices[i].y();
        }
    }

    /* compute integer y-value for bottom of polygon (round up) */
    bottomy = ceil( maxy ) + yi;

    /* determine whether should go `clockwise' or `counterclockwise'
    to move down the right side of the polygon */
    i = top;
    j = StepAround (top, -1, count);
    clockwise = 1;
    slopeoff = 0;
    if (slopes[j].dy * slopes[i].dx > slopes[i].dy * slopes[j].dx)
    {
        clockwise = -1;
        slopeoff = -1;
    }

    /* step around right side of polygon from top to bottom, building array
    of `right' edges (horizontal edges are ignored) */
    i = top;
    s = StepAround (top, slopeoff, count);
    nright = 0;
    while (i != bottom)
    {
        if (slopes[s].dy != 0)
        {
            y = buildLineEdge( vertices[i].x(), vertices[i].y(), slopes[s].k, slopes[s].dx, slopes[s].dy, xi, yi, false, &right[nright]);
            
            if( nright != 0 )
                right[nright-1].setHeight( y - lasty );
            else			/* y is top of first edge */
                topy = y;
                
            nright++;
            lasty = y;
        }

        i = StepAround (i, clockwise, count);
        s = StepAround (s, clockwise, count);
    }
    
    if( nright != 0 )
        right[nright-1].setHeight( bottomy - lasty );

    /* step around left side of polygon from top to bottom, building array of
    `left' edges (horizontal edges are ignored) */
    if (slopeoff == 0)
        slopeoff = -1;
    else
        slopeoff = 0;
        
    i = top;
    s = StepAround (top, slopeoff, count);
    nleft = 0;
    
    while (i != bottom)
    {
        if( slopes[s].dy != 0 )
        {
            y = buildLineEdge( vertices[i].x(), vertices[i].y(), slopes[s].k, slopes[s].dx,  slopes[s].dy, xi, yi, true, &left[nleft]);

            if( nleft != 0 )
                left[nleft-1].setHeight( y - lasty );
                
            nleft++;
            lasty = y;
        }

        i = StepAround( i, -clockwise, count );
        s = StepAround( s, -clockwise, count );
    }
    if (nleft != 0)
        left[nleft-1].setHeight( bottomy - lasty );

    /* return number of left-side and right-side edges; also height (vertical
    range, an unsigned int) and the vertical location of the top vertex
    (an integer) */
    *pnleft = nleft;
    *pnright = nright;
    *h = bottomy - topy;

    return topy;
}


} // namespace Gfx
} // namespace Pt
