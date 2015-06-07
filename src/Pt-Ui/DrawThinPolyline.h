/* Copyright (C) 2006-2015 Laurentiu-Gheorghe Crisan
 * Copyright (C) 2006-2015 Marc Boris Duerner
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA*/
#ifndef PT_UI_DRAWTHINPOLYLINE_H
#define PT_UI_DRAWTHINPOLYLINE_H

#include "DrawPolyline.h"
#include <Pt/Ui/ClipLine.h>
#include <algorithm>

namespace Pt {
namespace Ui {

/** @brief Draw thin lines on an image

    This class implements DrawPolyline and is specialised for the drawing
    of thin lines.
 */
class DrawThinPolyline : public DrawPolyline
{
    public:

        DrawThinPolyline();
        ~DrawThinPolyline();
        /** @brief Draw a line on an image

            @see DrawLine::draw
        */
        void draw( Image& image, const Pen& pen, const  PointF* points, size_t pointCount );

    private:
        void bresenhamLineSegment(Image& image, const Pen& pen, int signdx, int signdy, int axis, int x1, int y1, int e, int e1, int e2, int len);
        void bresenhamDasheLineSegment(Image& image, const Pen& pen, int *pdashNum, int *pdashIndex, const unsigned int *pDash, int numInDashList, int *pdashOffset, bool isDoubleDash, int signdx, int signdy,
                                       int axis, int x1, int y1, int e, int e1, int e2, int len);

        void drawLine( Image& image, const Pen& pen, const PointF& from, const PointF& to );
        void drawSolid( Image& image, const Pen& pen, const PointF* points,  size_t pointCount );
        void drawDash( Image& image, const Pen& pen, const PointF* points,  size_t pointCount);

        inline void outputSpan(Image& image, const Pen& pen, size_t x, size_t y, size_t length )
        {
            _stroke->stroke( image, pen, x, y, length );
        }

        inline void outputPixel( Image& image, const Pen& pen, size_t x, size_t y )
        {
            image.setColor( x,  y, pen.color() );
        }

        inline void absDeltaAndSign( int p2, int p1, int& absdelta, int& sign)
        {
            sign = 1;
            absdelta = p2 - p1;

            if ( absdelta < 0)
            {
                absdelta = -absdelta;
                sign = -1;
            }
        }

        inline void addPoint(int xx, int yy, PointF** ppt, unsigned int** pwidth, int& numSpans, int& ycurr, bool& firstspan, int signdy)
        {
            if (!firstspan && yy == ycurr)
            {
                int xdelta = xx - (*ppt)->x();

                if (xdelta < 0)
                {
                    (**pwidth) -= xdelta;
                    (*ppt)->setX(xx);
                }
                else if (xdelta > 0)
                {
                    unsigned int widthcurr = **pwidth;
                    (**pwidth) = std::max( widthcurr, (unsigned int)(1 + xdelta));
                }
            }
            else
            {
                if (!firstspan)
                {
                    *ppt += signdy;
                    *pwidth += signdy;
                }
                else
                    firstspan = false;

                (*ppt)->setX(xx);
                (*ppt)->setY(yy);
                **pwidth = 1;
                ycurr = yy;
                ++numSpans;
            }
        }

        std::vector<bool>	_dashPattern;
        ClipLine			_clipLine;
        enum{  xAxis = 0, yAxis = 1 };
};

}} // namespace

#endif
