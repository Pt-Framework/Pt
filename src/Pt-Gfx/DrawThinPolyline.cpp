/*
 * Copyright (C) 2006-2007 Laurentiu-Gheorghe Crisan
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
#include <cmath>

#include "DrawThinPolyline.h"
#include "Dash.h"

namespace Pt{
namespace Gfx{

DrawThinPolyline::DrawThinPolyline()
{
    _dashPattern.push_back(true);
    _dashPattern.push_back(true);
    _dashPattern.push_back(true);
    _dashPattern.push_back(false);
}

DrawThinPolyline::~DrawThinPolyline()
{ }

void DrawThinPolyline::draw( ARgbImage& image, const Pen& pen, const Gfx::Point* points,  size_t pointCount )
{
    switch( pen.style() )
    {
        case Pen::SolidStyle:
            drawSolid( image, pen, points, pointCount );
        break;

        case Pen::DashStyle:
        case Pen::DoubleDash:
            drawDash( image, pen, points, pointCount );
        break;

        default:
            throw std::runtime_error("Style is not implemented yet!" + PT_SOURCEINFO);
    }
}


void DrawThinPolyline::drawDash( ARgbImage& image, const Pen& pen, const Gfx::Point* points,  size_t pointCount)
{
    if (points <= 0)
        return;

    const Gfx::Point* ppt = points;
    int xstart, ystart;
    int x1, x2, y1, y2;

    int  dashNum    = 0;
    int  dashIndex  = 0;
    int  dashOffset = 0;
    bool isDoubleDash = (pen.style() == Pen::DoubleDash);

    std::vector<unsigned int> dashes(2);
    dashes[0] = pen.size() * 3; // Length of `on' dashes.
    dashes[1] = pen.size();		// Length of `off' dashes.

    Dash::stepDash( 0, &dashNum, &dashIndex, &dashes[0], dashes.size(), &dashOffset);

    // Loop through points, drawing a dashed Bresenham segment for each line
    // segment of nonzero length.

    xstart = ppt->x();
    ystart = ppt->y();
    x2 = xstart;
    y2 = ystart;

    while (--pointCount)
    {
        x1 = x2;
        y1 = y2;

        ++ppt;

        x2 = ppt->x();
        y2 = ppt->y();

        // Use Bresenham algorithm for sloped lines (no special treatment for
        // horizontal or vertical lines, unlike the undashed case)
        {
            int adx;		// abs values of dx and dy
            int ady;
            int signdx;		// sign of dx and dy
            int signdy;
            int e, e1, e2;	// Bresenham error and increments
            int axis;		// major axis
            int len;

            absDeltaAndSign(x2, x1, adx, signdx);
            absDeltaAndSign(y2, y1, ady, signdy);

            if (adx > ady)
            {
                axis = xAxis;
                e1 = ady << 1;
                e2 = e1 - (adx << 1);
                e = e1 - adx;
                len = adx;
                e -= ( signdx < 0);
            }
            else
            {
                axis = yAxis;
                e1 = adx << 1;
                e2 = e1 - (ady << 1);
                e = e1 - ady;
                len = ady;
                e -= ( signdy < 0);
            }

            // We have Bresenham parameters and two points, so all we need to
            // do now is draw (updating dashNum, dashIndex and dashOffset).
            bresenhamDasheLineSegment( image, pen, &dashNum, &dashIndex, &dashes[0], dashes.size(),
                                       &dashOffset, isDoubleDash, signdx, signdy, axis, x1, y1,
                                        e, e1, e2, len);
        }
    }

    // paint the last point if the end style isn't CapNotLast.
    // (Assume that a projecting, butt, or round cap that is one
    // pixel wide is the same as the single pixel of the endpoint.) */
    if (pen.capStyle() != Pen::NotLastCap && (xstart != x2 || ystart != y2 || ppt == points + 1))
    {
        if (dashNum & 1)
        {// Background dash. paint, in paint type #0

            /*
                if (isDoubleDash)
                    _stroke->stroke( image, pen, x2, y2);
            */
        }
        else
        {// Foreground dash
            /* use a paint type that cycles through 1..(numPixels-1) */

            /*
            int numPixels = pGC->numPixels;
            int paintType = 1 + ((dashNum / 2) % (numPixels - 1));
            MI_PAINT_POINT(paintedSet, pGC->pixels[paintType], x2, y2);
            */

            _stroke->stroke( image, pen, x2, y2);
        }
    }
}

// Internal: draw dashed Bresenham line segment. Called by miZeroDash().
// Endpoint semantics are used.
void DrawThinPolyline::bresenhamDasheLineSegment(ARgbImage& image, const Pen& pen, int *pdashNum, int *pdashIndex, const unsigned int *pDash, int numInDashList, int *pdashOffset, bool isDoubleDash, int signdx, int signdy, int axis, int x1, int y1, int e, int e1, int e2, int len)
{
    std::vector<Gfx::Point>  ptInit_bg;
    Gfx::Point *pptLast_fg,  *pptLast_bg = 0;
    std::vector<unsigned int>  widthInit_bg;
    unsigned int *pwidthLast_fg, *pwidthLast_bg = 0;
    int		x, y;
    int 	e3;
    int		dashNum, dashIndex;
    int		dashOffset;
    int		dashRemaining;
    int		thisDash;

    // Variables in span generation code, i.e. in addPoint()
    int numSpans_fg, numSpans_bg = 0;
    int ycurr_fg, ycurr_bg = 0;

    Gfx::Point *ppt_fg, *ppt_bg = 0;
    unsigned int *pwidth_fg, *pwidth_bg = 0;
    bool firstspan_fg, firstspan_bg = false;

    // Set up work arrays
    std::vector<Gfx::Point> ptInit_fg(len);
    std::vector<unsigned int> widthInit_fg(len);

    pptLast_fg = &ptInit_fg[len - 1];
    pwidthLast_fg = &widthInit_fg[len - 1];

    if (isDoubleDash)
    {
        ptInit_bg.resize(len);
        widthInit_bg.resize(len);

        pptLast_bg = &ptInit_bg [len - 1];
        pwidthLast_bg = &widthInit_bg[len - 1];
    }

    dashNum = *pdashNum;		/* absolute number of current dash */
    dashIndex = *pdashIndex;	/* index of current dash */
    dashOffset = *pdashOffset;	/* offset into current dash */
    dashRemaining = (int)(pDash[dashIndex]) - dashOffset;	/* how much is left */

    if (len <= (thisDash = dashRemaining))
    { // line segment will be solid, not dashed
        thisDash = len;
        dashRemaining -= len;
    }

    e3 = e2-e1;
    e = e - e1;

    //Point to first point
    x = x1;
    y = y1;

    // loop, generating dashes (in the absence of dashing, would
    // generate len pixels in all)
    for ( ; ; )
    {
        len -= thisDash;

        /* reset variables used in MI_ADD_POINT() */
        numSpans_fg = 0;
        ycurr_fg = 0;
        firstspan_fg = true;

        if (signdy >= 0)
        {
            ppt_fg = &ptInit_fg[0];
            pwidth_fg = &widthInit_fg[0];
        }
        else
        {
            ppt_fg  = pptLast_fg;
            pwidth_fg = pwidthLast_fg;
        }

        if (isDoubleDash)
        {
            numSpans_bg = 0;
            ycurr_bg = 0;
            firstspan_bg = true;

            ppt_bg  = pptLast_bg;
            pwidth_bg = pwidthLast_bg;

            if (signdy >= 0)
            {
                ppt_bg  = &ptInit_bg[0];
                pwidth_bg = &widthInit_bg[0];
            }
        }

        switch (axis)
        {
            case xAxis:
            default:
            if (dashIndex & 1)
            {
                if (isDoubleDash)
                {
                    while (thisDash--)
                    {
                        addPoint( x, y, &ppt_bg, &pwidth_bg, numSpans_bg, ycurr_bg, firstspan_bg, signdy );

                        if ((e += e1) >= 0)
                        {
                            e += e3;
                            y += signdy;
                        }

                        x += signdx;
                    }
                }
                else
                {
                    /* not double dashing; no background dash */
                    while (thisDash--)
                    {
                        if ((e += e1) >= 0)
                        {
                            e += e3;
                            y += signdy;
                        }

                        x += signdx;
                    }
                }
            }
            else
            {
                /* create foreground dash */
                while (thisDash--)
                {
                    addPoint(x, y, &ppt_fg, &pwidth_fg, numSpans_fg, ycurr_fg, firstspan_fg, signdy);

                    if ((e += e1) >= 0)
                    {
                        e += e3;
                        y += signdy;
                    }

                    x += signdx;
                }
            }
        break;
        case yAxis:
            if (dashIndex & 1)
            {
                if (isDoubleDash)
                {
                    /* create background dash */
                    while (thisDash--)
                    {
                        addPoint(x, y, &ppt_bg, &pwidth_bg, numSpans_bg, ycurr_bg, firstspan_bg, signdy);

                        if ((e += e1) >= 0)
                        {
                            e += e3;
                            x += signdx;
                        }

                        y += signdy;
                    }
                }
                else
                {
                    /* not double dashing; no background dash */
                    while (thisDash--)
                    {
                        if ((e += e1) >= 0)
                        {
                            e += e3;
                            x += signdx;
                        }

                        y += signdy;
                    }
                }
            }
            else
            {
                /* create foreground dash */
                while (thisDash--)
                {
                    addPoint(x, y, &ppt_fg, &pwidth_fg, numSpans_fg, ycurr_fg, firstspan_fg, signdy);

                    if ((e += e1) >= 0)
                    {
                        e += e3;
                        x += signdx;
                    }

                    y += signdy;
                }
            }
            break;
        }

        if (numSpans_fg > 0)
        { // Have a foreground dash to paint.
            Gfx::Point *pptStart_fg;
            unsigned int *pwidthStart_fg;

            if (signdy >= 0)
            {
                pptStart_fg = &ptInit_fg[0];
                pwidthStart_fg = &widthInit_fg[0];
            }
            else
            {
                pptStart_fg = pptLast_fg - (numSpans_fg - 1);
                pwidthStart_fg = pwidthLast_fg - (numSpans_fg - 1);
            }
            /* for foreground dash, use a paint type that cycles through
            1..(numPixels-1) */
            {
                /*
                int numPixels = pGC->numPixels;
                int paintType = 1 + ((dashNum / 2) % (numPixels - 1));

                MI_COPY_AND_PAINT_SPANS(paintedSet, pGC->pixels[paintType], numSpans_fg, pptStart_fg, pwidthStart_fg)
                */
                for( int i = 0;  i < numSpans_fg; ++i)
                    _stroke->stroke( image, pen, pptStart_fg[i].x(), pptStart_fg[i].y(), pwidthStart_fg[i]);
            }
        }

        if (isDoubleDash && numSpans_bg > 0)
        {// Have a background dash to paint.

            Gfx::Point *pptStart_bg;
            unsigned int *pwidthStart_bg;

            if (signdy >= 0)
            {
                pptStart_bg = &ptInit_bg[0];
                pwidthStart_bg = &widthInit_bg[0];
            }
            else
            {
                pptStart_bg = pptLast_bg - (numSpans_bg - 1);
                pwidthStart_bg = pwidthLast_bg - (numSpans_bg - 1);
            }

            /* for background dash, use paint type #0 */
            for( int i = 0;  i < numSpans_fg; ++i)
                _stroke->stroke( image, pen, pptStart_bg[i].x(), pptStart_bg[i].y(), pwidthStart_bg[i]);
        }

        if (len == 0)
            break;			/* break out of dash-generating loop */

        dashNum++;
        dashIndex++;

        if (dashIndex == numInDashList)
            dashIndex = 0;

        dashRemaining = (int)(pDash[dashIndex]);

        if ((thisDash = dashRemaining) >= len)
        {
            dashRemaining -= len;
            thisDash = len;
        }

    } /* end infinite dash-generating loop */

    *pdashNum    = dashNum;
    *pdashIndex  = dashIndex;
    *pdashOffset = (int)(pDash[dashIndex]) - dashRemaining;
}

void DrawThinPolyline::drawSolid( ARgbImage& image, const Pen& pen, const Gfx::Point* points,  size_t pointCount)
{
    const Gfx::Point *ppt;

    int xstart;
    int ystart;
    int x1;
    int x2;
    int y1;
    int y2;

    // Ensure we have >=1 points.
    if (pointCount <= 0)
        return;

    // Loop through points, drawing a solid Bresenham segment for each line segment.
    ppt = points;
    xstart = ppt->x();
    ystart = ppt->y();
    x2 = xstart;
    y2 = ystart;

    while( --pointCount )
    {
        x1 = x2;
        y1 = y2;
        ++ppt;

        x2 = ppt->x();
        y2 = ppt->y();

        if (x1 == x2)  // Vertical line.
        {
            if (y1 > y2)
            { 	// Make line go top to bottom, keeping endpoint semantics.
                int tmp;

                tmp = y2;
                y2 = y1 + 1;
                y1 = tmp + 1;
            }

            // Draw line.
            if (y1 != y2)
            {
                for( int i = y1; i < y2; ++i)
                    _stroke->stroke( image, pen, x1, i);
            }

            // Restore final point.
            y2 = ppt->y();
        }
        else if (y1 == y2)  // Horizontal line.
        {
            if (x1 > x2)
            { // Force line from left to right, keeping endpoint semantics.
                int tmp;

                tmp = x2;
                x2 = x1 + 1;
                x1 = tmp + 1;
            }

            // Draw line
            if (x1 != x2)
                _stroke->stroke(image, pen, x1, y1, x2 - x1);

            // Restore final point.
            x2 = ppt->x();
        }
        else
        { // Sloped line.
            int adx;		// Abs values of dx and dy
            int ady;
            int signdx;		// Sign of dx and dy
            int signdy;
            int e, e1, e2;	// Bresenham error and increments
            int axis;		// Major axis
            int len;		// Length of segment

            absDeltaAndSign(x2, x1, adx, signdx);
            absDeltaAndSign(y2, y1, ady, signdy);

            if (adx > ady)
            {
                axis = xAxis;
                e1 = ady << 1;
                e2 = e1 - (adx << 1);
                e = e1 - adx;
                e -= ( signdx < 0);
            }
            else
            {
                axis = yAxis;
                e1 = adx << 1;
                e2 = e1 - (ady << 1);
                e = e1 - ady;
                e -= ( signdy < 0);
            }

            // We have Bresenham parameters and two points, so all we need to do now is draw.
            len = (axis == xAxis) ? adx : ady;

            bresenhamLineSegment(image, pen, signdx, signdy, axis, x1, y1, e, e1, e2, len );
        }
    }

    // Paint the last point if the end style isn't CapNotLast.  (I.e. assume
    // that a round/butt/projecting/triangular cap that is one pixel wide is
    // the same as the single pixel of the endpoint.)
    if (pen.capStyle() != Pen::NotLastCap && (xstart != x2 || ystart != y2 || ppt == points + 1) )
        _stroke->stroke( image, pen, x2, y2);
}

void DrawThinPolyline::bresenhamLineSegment(ARgbImage& image, const Pen& pen, int signdx, int signdy, int axis, int x1, int y1, int e, int e1, int e2, int len)
{
    if (len == 0)
        return;

    std::vector<Gfx::Point> ptInit(len);
    std::vector<unsigned int> widthInit(len);

    Gfx::Point* pptLast     = &ptInit[len - 1];
    unsigned int *pwidthLast = &widthInit[len - 1];

    int x, y;
    int e3;
    int numSpans = 0;
    int ycurr = 0;
    Gfx::Point  *ppt = pptLast;
    unsigned int *pwidth = pwidthLast;
    bool firstspan = true;

    if (signdy >= 0)
    {
        ppt  = &ptInit[0];
        pwidth = &widthInit[0];
    }

    e3 = e2 - e1;
    e = e - e1;

    // Point to first point, and generate len pixels.
    x = x1;
    y = y1;

    switch (axis)
    {
        case xAxis:
        default:
            while (len--)
            {
                addPoint(x, y, &ppt, &pwidth, numSpans, ycurr, firstspan, signdy);

                if ((e += e1) >= 0)
                {
                    e += e3;
                    y+=signdy;
                }

                x+=signdx;
            }
        break;
        case yAxis:
            while (len--)
            {
                addPoint(x, y, &ppt, &pwidth, numSpans, ycurr, firstspan, signdy);

                if ((e += e1) >= 0)
                {
                    e += e3;
                    x += signdx;
                }

                y += signdy;
            }
        break;
    }

    if (numSpans > 0)
    {
        if (signdy < 0)
        {// Spans are offset, so shift downward.
            Gfx::Point *ppt_src	 = pptLast - (numSpans - 1);
            Gfx::Point *ppt_dst	 = &ptInit[0];
            unsigned int *pwidth_src = pwidthLast - (numSpans - 1);
            unsigned int *pwidth_dst = &widthInit[0];

            int count = numSpans;

            while (count--)
            {
                *ppt_dst++    = *ppt_src++;
                *pwidth_dst++ = *pwidth_src++;
            }
        }

        for( int i = 0; i < numSpans; ++i)
            _stroke->stroke( image, pen, ptInit[i].x(), ptInit[i].y(), widthInit[i]);
    }
}

} // namespace Gfx
} // namespace Pt
