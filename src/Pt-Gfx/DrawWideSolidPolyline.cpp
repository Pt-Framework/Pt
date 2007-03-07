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
#include "DrawWideSolidPolyline.h"
#include <Pt/Math/Point.h>
#include <Pt/Math/MathUtils.h>
#include <algorithm>
#include <cmath>

using namespace Pt::Math;

namespace Pt {
namespace Gfx {



DrawWideSolidPolyline::DrawWideSolidPolyline()
{ }

DrawWideSolidPolyline::~DrawWideSolidPolyline()
{ }

void DrawWideSolidPolyline::draw( ARgbImage& image, const Pen& pen,const Pt::Math::Point* pPts, size_t npt)
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
                default:
                    throw std::runtime_error("Style is not implemented yet!" + PT_SOURCEINFO);
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


void DrawWideSolidPolyline::drawDashSegment( ARgbImage& image, const Pen& pen,
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
void DrawWideSolidPolyline::drawDashLine( ARgbImage& image, const Pen& pen, const  Math::Point* points, size_t pointCount )
{
}


void DrawWideSolidPolyline::drawSegment(ARgbImage& image, const Pen& pen,
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












} // namespace Gfx
} // namespace Pt
