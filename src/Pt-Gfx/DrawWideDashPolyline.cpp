/*
 * Copyright (C) 2006-2008 Laurentiu-Gheorghe Crisan
 * Copyright (C) 2006-2007 Marc Boris Duerner
 * Copyright (C) 2006-2007 PTV AG
 * Copyright (C) 2010 Aloysius Indrayanto
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
#include "Pt/Math.h"

#include "DrawWideDashPolyline.h"
#include "LineFace.h"
#include "LineSlope.h"
#include "LineEdge.h"
#include "Dash.h"


namespace Pt{
namespace Gfx{

DrawWideDashPolyline::DrawWideDashPolyline()
{ }


DrawWideDashPolyline::~DrawWideDashPolyline()
{ }

void DrawWideDashPolyline::draw( ARgbImage& image, const Pen& pen, const  Gfx::Point* pPts, size_t npt )
{
    int	      x1, y1, x2, y2;
    int	      dashNum;					// Absolute number of dash, starts with 0
    int       dashIndex;				// index into array (i.e. dashNum % length)
    int       dashOffset;				// Offset into selected dash */
    int       startPaintType, endPaintType = 0, prevEndPaintType = 0;
    int       firstPaintType = 0;		// Used only for closed polylines; will be 1
    int       numPixels;
    bool	  selfJoin;					// Polyline is closed?
    bool	  first;					// First line segment of polyline
    bool	  somethingDrawn = false;
    bool	  projectLeft, projectRight;
    LineFace  leftFace, rightFace, prevRightFace;
    LineFace  firstFace;

    //Define the dash patter.
    unsigned  int dashes[2];
    dashes[0] = pen.size() * 2; // Length of `on' dashes.
    dashes[1] = pen.size() * 2;	// Length of `off' dashes.

    // Ensure we have >=1 points
    if( npt <= 0 )
        return;

    x2 = pPts->x();
    y2 = pPts->y();
    first = true;	// first line segment of polyline

    /* determine whether polyline is closed */
    selfJoin = false;

    if( x2 == pPts[npt-1].x() && y2 == pPts[npt-1].y() )
        selfJoin = true;

    // Dash segments (except for the last) will not project right; and
    // (except for the first) will not project left
    projectLeft  = (pen.capStyle() == Pen::ProjectingCap) && !selfJoin;
    projectRight = false;

    // perform initial offsetting into the dash sequence
    dashNum     = 0; // absolute number of dash
    dashIndex   = 0; // index into dash array
    dashOffset  = 0; // index into selected dash

    Dash::stepDash( 0, &dashNum, &dashIndex, dashes, 2, &dashOffset );

    // How many paint types?  (Will cycle through 0..numPixels-1, beginning
    // with 1, with `off' dashes defined as those with paint type #0.)
    numPixels = 2; //pGC->numPixels;

    // Iterate through points, drawing a dashed segment for each line segment
    // of nonzero length
    while( --npt )
    {
        x1 = x2;
        y1 = y2;

        ++pPts;

        x2 = pPts->x();
        y2 = pPts->y();

        // Have a line segment of nonzero length.
        if( x1 != x2 || y1 != y2 )
        {
            int prevDashNum;
            int lastPaintedDashNum;

            // Final point; and need a projecting cap here.
            if( npt == 1 && pen.capStyle() ==Pen::ProjectingCap  && (!selfJoin || (firstPaintType == 0)))
                projectRight = true;

            prevDashNum = dashNum;

            // Draw dashed segment, updating dashNum, dashIndex and dashOffset, returning faces
            dashSegment( image, pen,  &dashNum, &dashIndex, &dashOffset, x1, y1, x2, y2, projectLeft, projectRight, &leftFace, &rightFace, dashes);

            // Determine paint types used at start and end of just-drawn segment
            startPaintType = ((dashNum & 1) ? 0 : 1 + ((dashNum / 2) % (numPixels - 1)));

            lastPaintedDashNum = (dashOffset != 0 ? dashNum : dashNum - 1);

            endPaintType = ((lastPaintedDashNum & 1) ? 0 : 1 + ((dashNum / 2) % (numPixels - 1)));

            // Add round cap or line join at left end of just-drawn segment;
            // if DashStyle, do so only if segment began with an `on' dash
            if( pen.style() == Pen::DoubleDash || (startPaintType != 0))
            {
                // Draw cap at left end, unless this is first segment of a closed polyline
                if( first || (pen.style() == Pen::DashStyle && prevEndPaintType == 0 ) )
                {
                    if( first && selfJoin )
                    {
                        firstFace = leftFace;
                        firstPaintType = startPaintType;
                    }
                    else if( pen.capStyle() == Pen::RoundCap || pen.capStyle() == Pen::TriangularCap )
                    {
                        lineArc( image, pen, &leftFace, (LineFace *)NULL, (double)0.0, (double)0.0, true);
                    }
                }
                else
                {
                    // Draw join at left end.
                    lineJoin( image, pen, &leftFace, &prevRightFace );
                }
            }

            somethingDrawn = true;
            first = false;
            prevRightFace = rightFace;
            prevEndPaintType = endPaintType;
            projectLeft = false;
        }

        // Last point of a nonempty polyline, so add line join or round cap
        // if appropriate, i.e. if we're doing DashStyle and ended on an
        // `on' dash, or if we're doing DoubleDash
        if( npt == 1 && somethingDrawn )
        {
            if( pen.style() == Pen::DoubleDash || (endPaintType != 0) )
            {
                // closed, so draw a join
                if (selfJoin && (pen.style() == Pen::DoubleDash  || (firstPaintType != 0)))
                {
                    lineJoin( image, pen, &firstFace, &rightFace );
                }
                else
                {
                    if( pen.capStyle() == Pen::RoundCap || pen.capStyle() == Pen::TriangularCap )
                        lineArc( image, pen, (LineFace *)NULL, &rightFace, (double)0.0, (double)0.0, true );
                }
            }
            else // we're doing OnOffDash, and final segment of polyline ended with an (undrawn) `off' dash
            {
                if( selfJoin && (firstPaintType != 0 ) )  // closed; if projecting or round caps are being used, draw one on the first face
                {
                    if( pen.capStyle() == Pen::ProjectingCap)
                        lineProjectingCap( image, pen,  &firstFace, true, true);
                    else if (pen.capStyle() == Pen::RoundCap || pen.capStyle() == Pen::TriangularCap )
                        lineArc(image, pen, &firstFace, (LineFace *)NULL, (double)0.0, (double)0.0, true);
                }
            }
        }
    }

    // Handle `all points coincident' crock, nothing yet drawn
    if( !somethingDrawn && (pen.style() == Pen::DoubleDash || !(dashNum & 1)) )
    {
        unsigned int w1;

        switch( pen.capStyle() )
        {
            case Pen::RoundCap:
            case Pen::TriangularCap:
                lineArc( image, pen, (LineFace *)NULL, (LineFace *)NULL, (double)x2, (double)y2, false );
            break;

            case Pen::ProjectingCap: // Draw a square box with edge size equal to line width
                w1 = pen.size();
                fillRect(image, pen, (int)(x2 - (w1 >> 1)), (int)(y2 - (w1 >> 1)), w1, w1);
            break;

            case Pen::ButtCap:
            default:

            break;
        }
    }
}

void DrawWideDashPolyline::dashSegment( ARgbImage& image, const Pen& pen, int *pDashNum, int *pDashIndex, int *pDashOffset, int x1, int y1, int x2, int y2, bool projectLeft, bool projectRight, LineFace *leftFace, LineFace *rightFace, unsigned int* dash)
{
    int		            dashNum, dashIndex, dashRemain;
    double	            L, l;
    double	            k;
    Pt::Gfx::PointF	vertices[4];
    Pt::Gfx::PointF    saveRight, saveBottom;
    LineSlope	        slopes[4];
    LineEdge	        left[2], right[2];
    LineFace	        lcapFace, rcapFace;
    int		            nleft, nright;
    unsigned int	    h;
    int		            y;
    int		            dy, dx;
    double	            LRemain;
    double	            r;
    double	            rdx, rdy;
    double	            dashDx, dashDy;
    double	            saveK = 0.0;
    bool	    	    first = true;
    double	            lcenterx, lcentery, rcenterx = 0.0, rcentery = 0.0;
    int    	            numPixels, paintType;
    int                 numInDashList = 2;

    dx          = x2 - x1;
    dy          = y2 - y1;
    dashNum     = *pDashNum;
    dashIndex   = *pDashIndex;

    // Determine portion of current dash remaining (i.e. the portion after
    // the current offset.
    dashRemain = (int)(dash[dashIndex]) - *pDashOffset;

    // compute color of current dash
    numPixels = 2;
    paintType = (dashNum & 1) ? 0 : 1 + ((dashNum / 2) % (numPixels - 1));

    // Compute e.g. L, the distance to go (for dashing).
    l = 0.5 * ((double) pen.size() );

    // Vertical segment.
    if( dx == 0 )
    {
        L   = dy;
        rdx = 0;
        rdy = l;

        if( dy < 0  )
        {
            L = -dy;
            rdy = -l;
        }
    }
    else if( dy == 0 ) //Horizontal segment.
    {
        L = dx;
        rdx = l;
        rdy = 0;

        if (dx < 0)
        {
            L = -dx;
            rdx = -l;
        }
    }
    else // Neither horizontal nor vertical.
    {
        L = Pt::hypot((double) dx, (double) dy);
        r = l / L;		// this is ell / L, not 1 / L
        rdx = r * dx;
        rdy = r * dy;
    }

    k = l * L; // this is ell * L, not 1 * L.

    // All position comments are relative to a line with dx and dy > 0,
    // but the code does not depend on this.

    // top
    slopes[V_TOP].setDX( dx );
    slopes[V_TOP].setDY( dy );
    slopes[V_TOP].setK( k );
    // right
    slopes[V_RIGHT].setDX( -dy );
    slopes[V_RIGHT].setDY( dx );
    slopes[V_RIGHT].setK( 0 );
    // bottom
    slopes[V_BOTTOM].setDX( -dx );
    slopes[V_BOTTOM].setDY( -dy );
    slopes[V_BOTTOM].setK( k );
    // left
    slopes[V_LEFT].setDX( dy );
    slopes[V_LEFT].setDY( -dx );
    slopes[V_LEFT].setK( 0 );

    // preload the start coordinates
    vertices[V_RIGHT].setX( rdy) ;
    vertices[V_TOP].setX( rdy );
    vertices[V_RIGHT].setY( -rdx );
    vertices[V_TOP].setY( -rdx );

    vertices[V_BOTTOM].setX( -rdy );
    vertices[V_LEFT].setX( -rdy );
    vertices[V_BOTTOM].setY( rdx );
    vertices[V_LEFT].setY( rdx );

    // Offset the vertices appropriately
    if (projectLeft)
    {
        vertices[V_TOP].setX( vertices[V_TOP].x() - rdx );
        vertices[V_TOP].setY( vertices[V_TOP].y() - rdy );

        vertices[V_LEFT].setX( vertices[V_LEFT].x() - rdx );
        vertices[V_LEFT].setY( vertices[V_LEFT].y() - rdy );

        slopes[V_LEFT].setK( rdx * dx + rdy * dy );
    }

    // Starting point for first dash (floating point)
    lcenterx = x1;
    lcentery = y1;

    // Keep track of starting face (need only in OnOff case)
    if( pen.capStyle() == Pen::RoundCap  || pen.capStyle() == Pen::TriangularCap )
    {
        lcapFace.setDX( dx );
        lcapFace.setDY( dy );
        lcapFace.setX( x1 );
        lcapFace.setY( y1 );

        rcapFace.setDX( -dx );
        rcapFace.setDY( -dy );
        rcapFace.setX( x1 );
        rcapFace.setY( y1 );
    }

    // Draw dashes until end of line segment is reached, and no additional
    // (complete) dash can be drawn.
    LRemain = L;

    while( LRemain > dashRemain )
    {
        dashDx = (dashRemain * dx) / L;
        dashDy = (dashRemain * dy) / L;

        // Ending point for dash
        rcenterx = lcenterx + dashDx;
        rcentery = lcentery + dashDy;

        vertices[V_RIGHT].setX( vertices[V_RIGHT].x() + dashDx );
        vertices[V_RIGHT].setY( vertices[V_RIGHT].y() + dashDy );

        vertices[V_BOTTOM].setX( vertices[V_BOTTOM].x() + dashDx );
        vertices[V_BOTTOM].setY( vertices[V_BOTTOM].y() + dashDy );

        slopes[V_RIGHT].setK( vertices[V_RIGHT].x() * dx + vertices[V_RIGHT].y() * dy );

        // Draw dash (if OnOffDash, don't draw `off' dashes)
        if( pen.style() == Pen::DoubleDash  || !(paintType == 0))
        {
            if( pen.style() == Pen::DashStyle && pen.capStyle() == Pen::ProjectingCap )
            {
                saveRight = vertices[V_RIGHT];
                saveBottom = vertices[V_BOTTOM];
                saveK = slopes[V_RIGHT].k();

                if( !first )
                {
                    vertices[V_TOP].setX( vertices[V_TOP].x() - rdx );
                    vertices[V_TOP].setY( vertices[V_TOP].y()- rdy );

                    vertices[V_LEFT].setX( vertices[V_LEFT].x() - rdx );
                    vertices[V_LEFT].setY( vertices[V_LEFT].y() - rdy );

                    slopes[V_LEFT].setK( vertices[V_LEFT].x() * slopes[V_LEFT].dy() - vertices[V_LEFT].y() * slopes[V_LEFT].dx() );
                }

                vertices[V_RIGHT].setX( vertices[V_RIGHT].x() + rdx );
                vertices[V_RIGHT].setY( vertices[V_RIGHT].y() + rdy );

                vertices[V_BOTTOM].setX( vertices[V_BOTTOM].x() + rdx );
                vertices[V_BOTTOM].setY( vertices[V_BOTTOM].y() + rdy );

                slopes[V_RIGHT].setK( vertices[V_RIGHT].x() * slopes[V_RIGHT].dy() - vertices[V_RIGHT].y() * slopes[V_RIGHT].dx() );
            }

            // Build lists of left and right edges for the dash, using the
            // just-computed array of slopes
            y = polyBuildPoly( vertices, slopes, 4, x1, y1, left, right, &nleft, &nright, &h );

            // Fill the dash, with either fg or bg color (alternates)
            fillLine( image, pen, y, h, left, right, nleft, nright);

            // If doing DashStyle, add caps if any
            if( pen.style() == Pen::DashStyle )
            {
                switch( pen.capStyle() )
                {
                    case Pen::ButtCap:
                    default:
                    break;
                    // Use saved vertices
                    case Pen::ProjectingCap:
                        vertices[V_BOTTOM] = saveBottom;
                        vertices[V_RIGHT]  = saveRight;
                        slopes[V_RIGHT].setK( saveK );
                    break;

                    case Pen::RoundCap:
                    case Pen::TriangularCap:
                        if( !first )
                        {
                            if( dx < 0 )
                            {
                                lcapFace.setXA( -vertices[V_LEFT].x() );
                                lcapFace.setYA( -vertices[V_LEFT].y() );
                                lcapFace.setK( slopes[V_LEFT].k() );
                            }
                            else
                            {
                                lcapFace.setXA( vertices[V_TOP].x() );
                                lcapFace.setYA(  vertices[V_TOP].y() );
                                lcapFace.setK( -slopes[V_LEFT].k() );
                            }
                            // Invoke miLineArc, isInt = false, to draw half-disk
                            // on left end of dash (only if dash is not first)
                            lineArc(image, pen, &lcapFace, (LineFace *) NULL, lcenterx, lcentery, false);
                        }

                        if (dx < 0)
                        {
                            rcapFace.setXA( vertices[V_BOTTOM].x() );
                            rcapFace.setYA( vertices[V_BOTTOM].y() );
                            rcapFace.setK( slopes[V_RIGHT].k() );
                        }
                        else
                        {
                            rcapFace.setXA( -vertices[V_RIGHT].x() );
                            rcapFace.setYA( -vertices[V_RIGHT].y() );
                            rcapFace.setK( -slopes[V_RIGHT].k() );
                        }

                        // Invoke miLineArc, isInt = false, to draw half-disk on
                        // right end of dash
                        lineArc(image, pen,  (LineFace *)NULL, &rcapFace, rcenterx, rcentery, false);
                    break;
                }
            }
        }

        // We just drew a dash, or (in the OnOff case) we either drew a dash
        // or we didn't

        // Decrement float by int (distance over which we just drew, i.e. the remainder
        // of current dash)

        LRemain -= dashRemain;

        // Bump absolute dash number, and index of dash in array (cyclically)
        ++dashNum;
        ++dashIndex;

        if( dashIndex == numInDashList )
            dashIndex = 0;

        dashRemain = (int)(dash[dashIndex]); // whole new dash now `remains'

        // Compute color of next dash
        paintType = (dashNum & 1) ? 0 : 1 + ((dashNum / 2) % (numPixels - 1));

        // next dash will start where previous one ended
        lcenterx = rcenterx;
        lcentery = rcentery;

        vertices[V_TOP] = vertices[V_RIGHT];
        vertices[V_LEFT] = vertices[V_BOTTOM];
        slopes[V_LEFT].setK( -slopes[V_RIGHT].k() );
        // No longer first dash of line segment
        first = false;
    }

    // Final portion of segment is dashed specially, with an incomplete dash
    if( pen.style() == Pen::DoubleDash || !(paintType == 0))
    {
        vertices[V_TOP].setX( vertices[V_TOP].x() - dx );
        vertices[V_TOP].setY( vertices[V_TOP].y() - dy );

        vertices[V_LEFT].setX( vertices[V_LEFT].x() - dx );
        vertices[V_LEFT].setY( vertices[V_LEFT].y() - dy );

        vertices[V_RIGHT].setX( rdy );
        vertices[V_RIGHT].setY( -rdx );

        vertices[V_BOTTOM].setX( -rdy );
        vertices[V_BOTTOM].setY( rdx );

        // Offset appropriately
        if( projectRight )
        {
            vertices[V_RIGHT].setX( vertices[V_RIGHT].x() + rdx );
            vertices[V_RIGHT].setY( vertices[V_RIGHT].y() + rdy );

            vertices[V_BOTTOM].setX( vertices[V_BOTTOM].x() + rdx );
            vertices[V_BOTTOM].setY( vertices[V_BOTTOM].y() + rdy );
            slopes[V_RIGHT].setK( vertices[V_RIGHT].x() * slopes[V_RIGHT].dy() - vertices[V_RIGHT].y() * slopes[V_RIGHT].dx() );
        }
        else
        {
            slopes[V_RIGHT].setK( 0 );
        }

        // If DashStyle line style and cap mode is projecting, offset the
        // face, so as to draw a projecting cap
        if( !first && ( pen.style() == Pen::DashStyle)  && ( pen.capStyle() == Pen::ProjectingCap ) )
        {
            vertices[V_TOP].setX( vertices[V_TOP].x() - rdx );
            vertices[V_TOP].setY( vertices[V_TOP].y() - rdy );

            vertices[V_LEFT].setX( vertices[V_LEFT].x() - rdx );
            vertices[V_LEFT].setY( vertices[V_LEFT].y() - rdy );

            slopes[V_LEFT].setK( vertices[V_LEFT].x() * slopes[V_LEFT].dy() - vertices[V_LEFT].y() * slopes[V_LEFT].dx() );
        }
        else
        {
            //Slopes[V_LEFT].k += dx * dx + dy * dy;
            slopes[V_LEFT].setK( slopes[V_LEFT].k() + ( dx * dx + dy * dy ) );
        }

        // Build lists of left and right edges for the final incomplete dash,
        // using the just-computed vertices and slopes
        y = polyBuildPoly( vertices, slopes, 4, x2, y2, left, right, &nleft, &nright, &h);

        // Fill the final dash
        fillLine( image, pen,  y, h, left, right, nleft, nright);

        // If DashStyle line style and cap mode is round, draw a round cap
        if( ( !first && ( pen.style() == Pen::DashStyle) && ( pen.capStyle() == Pen::RoundCap) ) || ( pen.capStyle() == Pen::TriangularCap) )
        {
            lcapFace.setX( x2 );
            lcapFace.setY( y2 );

            if( dx < 0 )
            {
                lcapFace.setXA( -vertices[V_LEFT].x() );
                lcapFace.setYA( -vertices[V_LEFT].y() );
                lcapFace.setK( slopes[V_LEFT].k() );
            }
            else
            {
                lcapFace.setXA( vertices[V_TOP].x() );
                lcapFace.setYA( vertices[V_TOP].y() );
                lcapFace.setK( -slopes[V_LEFT].k() );
            }

            // Invoke miLineArc, isInt = false, to draw disk on end
            lineArc(image, pen, &lcapFace, (LineFace *) 0, rcenterx, rcentery, false);
        }
    }

    // Work out left and right faces of the dashed segment, to pass back
    leftFace->setX( x1 );
    leftFace->setY( y1 );
    leftFace->setDX( dx );
    leftFace->setDY( dy );
    leftFace->setXA( rdy );
    leftFace->setYA( -rdx );
    leftFace->setK( k );

    rightFace->setX( x2 );
    rightFace->setY( y2 );
    rightFace->setDX( -dx );
    rightFace->setDY( -dy );
    rightFace->setXA( -rdy );
    rightFace->setYA( rdx );
    rightFace->setK( k );

    // Update absolute dash number, dash index, dash offset
    dashRemain = (int)(((double) dashRemain) - LRemain);

    // On to next dash in array
    if( dashRemain == 0 )
    {
        dashNum++;		// bump absolute dash number
        dashIndex++;

        if (dashIndex == numInDashList) // wrap
            dashIndex = 0;

        dashRemain = (int)(dash[dashIndex]);
    }

    *pDashNum = dashNum;
    *pDashIndex = dashIndex;
    *pDashOffset = (int)(dash[dashIndex]) - dashRemain;
}

}// namespace Gfx
}// namespace Pt
