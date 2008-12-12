/*
 * Copyright (C) 2006-2008 Laurentiu-Gheorghe Crisan
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

void DrawWideSolidPolyline::draw( ARgbImage& image, const Pen& pen,const Point* pPts, size_t npt)
{
    int		   x1, y1, x2, y2;
    bool	   projectLeft, projectRight;
    LineFace   leftFace, rightFace, prevRightFace;
    LineFace   firstFace;
    int        first;
    bool	   somethingDrawn = false;
    bool	   selfJoin;

    // Ensure we have >=1 points.
    if (npt <= 0)
        return;

    x2 = pPts->x();
    y2 = pPts->y();
    first = true;

    // Determine whether polyline is closed.
    selfJoin = false;

    if( npt > 1 )
        if (x2 == pPts[npt-1].x() && y2 == pPts[npt-1].y())
            selfJoin = true;

    // Line segments (except for the last) will not project right; they'll
    // project left if the cap mode is "projecting".
    projectLeft = (pen.capStyle() == Pen::ProjectingCap && !selfJoin);
    projectRight = false;

    // Iterate through points, drawing all line segments of nonzero length.
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
            
            if (npt == 1 && pen.capStyle() == Pen::ProjectingCap && !selfJoin) // last point; and need a projecting cap here                
                projectRight = true;
            
            // Draw segment (pixel=1), returning faces.
            drawSegment( image, pen, Point(x1, y1), Point(x2, y2), projectLeft, projectRight, &leftFace, &rightFace );

            if (first)
            { //First line segment, draw round cap if needed.
                if (selfJoin)
                {
                    firstFace = leftFace;
                }
                else if (pen.capStyle() == Pen::RoundCap || pen.capStyle() == Pen::TriangularCap )
                {
                    // Invoke miLineArc, isInt = true, to draw a round cap on left face in paint type #1. 
                    lineArc( image, pen, &leftFace, 0,(double)0.0, (double)0.0, true );
                }
            }
            else
            {
                // General case: draw join at beginning of segment (pixel=1).
                lineJoin( image, pen, &leftFace, &prevRightFace );
            }

            prevRightFace = rightFace;
            first = false;
            projectLeft = false;
        }

        // Final point of polyline.
        if (npt == 1 && somethingDrawn)
        {
            if (selfJoin) // Add line join to close the polyline, pixel=1.
                lineJoin( image, pen, &firstFace, &rightFace);
            
            else if (pen.capStyle() == Pen::RoundCap || pen.capStyle() == Pen::TriangularCap )
                // Invoke miLineArc, isInt = true, to draw round cap on right face, pixel=1.
                lineArc( image, pen, 0, &rightFace, (double)0.0, (double)0.0, true );			
        }
    }

    // handle crock where all points are coincident
    if( !somethingDrawn )
    {
        projectLeft = (pen.capStyle() == Pen::ProjectingCap );

        drawSegment( image, pen, Point(x2, y2), Point(x2, y2), projectLeft, projectRight, &leftFace, &rightFace );

        if( pen.capStyle() == Pen::RoundCap || pen.capStyle() == Pen::TriangularCap)
        {
            // invoke miLineArc, isInt = true, to draw round cap in paint type #1
            lineArc( image, pen, &leftFace, (LineFace *)NULL, (double)0.0, (double)0.0, true );

            // invoke miLineArc, isInt = true, to draw other round cap in paint type #1
            rightFace.setDX( -1);	// sleazy hack to make it work
            lineArc( image, pen, (LineFace *) NULL, &rightFace, (double)0.0, (double)0.0, true );
        }
    }
}

void DrawWideSolidPolyline::drawSegment( ARgbImage& image, const Pen& pen, Point from, Point to, bool projectLeft, bool projectRight,
                                         LineFace* leftFace, LineFace* rightFace)
{
    double	 l, L, r;
    double	 xa, ya;
    double	 projectXoff = 0.0, projectYoff = 0.0;
    double	 k;
    double	 maxy;
    int		 dx, dy;
    int		 x, y;
    int		 finaly;
    LineEdge *left, *right;
    LineEdge *top, *bottom;
    int		 lefty, righty, topy, bottomy;
    int		 signdx;
    LineEdge lefts[2], rights[2];
    int		 lw = pen.size();

    if (to.y() < from.y() || (to.y() == from.y() && to.x() < from.x()))
    {	// Interchange, so as always to draw top-to-bottom, or left-to-right if horizontal.
        int tx, ty;
        bool tbool;
        LineFace *tface;

        tx = from.x();
        from.setX( to.x());
        to.setX(tx);
        
        ty = from.y();
        from.setY(to.y());
        to.setY(ty);

        tbool = projectLeft;
        projectLeft = projectRight;
        projectRight = tbool;

        tface = leftFace;
        leftFace = rightFace;
        rightFace = tface;
    }

    dy = to.y() - from.y();
    signdx = 1;
    dx = to.x() - from.x();
    
    if (dx < 0)
        signdx = -1;

    leftFace->setX(from.x());
    leftFace->setY(from.y());
    leftFace->setDX(dx);
    leftFace->setDY(dy);

    rightFace->setX(to.x());
    rightFace->setY(to.y());
    rightFace->setDX(-dx); //for faces, (dx,dy) points _into_ line
    rightFace->setDY(-dy);

    if (dy == 0) // Segment is horizontal.
    {
        rightFace->setXA(0);
        rightFace->setYA( 0.5 * (double)lw );
        rightFace->setK(-0.5 * (double)(lw * dx) ); // k = xa * dy - ya * dx
        leftFace->setXA(0);
        leftFace->setYA(-rightFace->ya());
        leftFace->setK(rightFace->k()); // k = xa * dy - ya * dx
      
        x = from.x();
      
        if (projectLeft)
            x -= (lw >> 1);

        y = from.y() - (lw >> 1);

        dx = to.x() - x;

        if (projectRight)
            dx += ((lw + 1) >> 1);

        dy = lw;

        fillRect( image, pen, x, y,(unsigned int)dx, (unsigned int)dy );
    }
    else if (dx == 0) // Segment is vertical.
    {
        leftFace->setXA(0.5 * (double)lw );
        leftFace->setYA(0);
        leftFace->setK( 0.5 * (double)(lw * dy) ); // k = xa * dy - ya * dx
        rightFace->setXA( -leftFace->xa());
        rightFace->setYA( 0 );
        rightFace->setK( leftFace->k()); // k = xa * dy - ya * dx
        y = from.y();

        if (projectLeft)
        y -= lw >> 1;

        x = from.x() - (lw >> 1);
        dy = to.y() - y;

        if (projectRight)
        dy += ((lw + 1) >> 1);

        dx = lw;

        fillRect( image, pen, x, y,(unsigned int)dx, (unsigned int)dy );
    }
    else 
    { // General case: segment is neither horizontal nor vertical.
        l = 0.5 * ((double) lw);
        L = Math::hypot((double) dx, (double) dy);

        if (dx < 0)
        {
            right  = &rights[1];
            left   = &lefts[0];
            top    = &rights[0];
            bottom = &lefts[1];
        }
        else
        {
            right  = &rights[0];
            left   = &lefts[1];
            top    = &lefts[0];
            bottom = &rights[1];
        }
      
        r = l / L; // this is ell / L, not 1 / L.

        ya = -r * dx;
        xa = r * dy;

        if (projectLeft | projectRight)
        {
            projectXoff = -ya;
            projectYoff = xa;
        }

        // Build first long edge.
        k = l * L; // xa * dy - ya * dx
        leftFace->setXA(xa);
        leftFace->setYA(ya);
        leftFace->setK(k);
        rightFace->setXA(-xa);
        rightFace->setYA(-ya);
        rightFace->setK(k);

        if (projectLeft)
            righty = buildLineEdge( xa - projectXoff, ya - projectYoff, k, dx, dy, from.x(), from.y(), false, right);
        else
            righty = buildLineEdge( xa, ya, k, dx, dy, from.x(), from.y(), false, right);

          // Build second long edge.
          ya = -ya;
          xa = -xa;
          k = -k; // xa * dy - ya * dx

        if (projectLeft)
            lefty = buildLineEdge(xa - projectXoff, ya - projectYoff, k, dx, dy, from.x(), from.y(), true, left);
        else
            lefty = buildLineEdge(xa, ya, k, dx, dy, from.x(), from.y(), true, left);

        // Build first short edge, on left end.
        if (signdx > 0)
        {
            ya = -ya;
            xa = -xa;
        }

        if (projectLeft)
        {
            double xap = xa - projectXoff;
            double yap = ya - projectYoff;
            topy = buildLineEdge( xap, yap, xap * dx + yap * dy, -dy, dx, from.x(), from.y(), dx > 0 , top );
        }
        else
        {
            topy = buildLineEdge( xa, ya, 0.0, -dy, dx, from.x(), from.y(), dx > 0, top );
        }

        // Build second short edge, on right end.
        if (projectRight)
        {
            double xap = xa + projectXoff;
            double yap = ya + projectYoff;
            bottomy = buildLineEdge( xap, yap, xap * dx + yap * dy, -dy, dx, to.x(), to.y(),dx < 0, bottom );
            maxy = -ya + projectYoff;
        }
        else
        {
            bottomy = buildLineEdge( xa, ya, 0.0, -dy, dx, to.x(), to.y(), (dx < 0 ? true : false), bottom );
            maxy = -ya;
        }
        
        finaly = ceil(maxy) + to.y();

        if (dx < 0)
        {
            left->setHeight( (unsigned int)(bottomy - lefty));
            right->setHeight( (unsigned int)(finaly - righty) );
            top->setHeight( (unsigned int)(righty - topy) );
        }
        else
        {
            right->setHeight( (unsigned int)(bottomy - righty));
            left->setHeight( (unsigned int)(finaly - lefty) );
            top->setHeight( (unsigned int)(lefty - topy) );
        }

        bottom->setHeight( (unsigned int)(finaly - bottomy) );

        // Fill the rectangle (2 left edges, 2 right edges).
        fillLine(image, pen, topy, (unsigned int)(bottom->height() + bottomy - topy), lefts, rights, 2, 2 );
    }
}

} // namespace Gfx
} // namespace Pt
