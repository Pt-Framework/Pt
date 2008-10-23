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
#ifndef PT_GFX_DRAWTHINPOLYLINE_H
#define PT_GFX_DRAWTHINPOLYLINE_H

#include "DrawPolyline.h"
#include "Pt/Gfx/ClipLine.h"

namespace Pt {
namespace Gfx {

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
        void draw( ARgbImage& image, const Pen& pen, const  Math::Point* points, size_t pointCount );

    private:
        void bresenhamLineSegment(ARgbImage& image, const Pen& pen, int signdx, int signdy, int axis, int x1, int y1, int e, int e1, int e2, int len);
        void bresenhamDasheLineSegment(ARgbImage& image, const Pen& pen, int *pdashNum, int *pdashIndex, const unsigned int *pDash, int numInDashList, int *pdashOffset, bool isDoubleDash, int signdx, int signdy, 
                                       int axis, int x1, int y1, int e, int e1, int e2, int len);

        void drawLine( ARgbImage& image, const Pen& pen, const Math::Point& from, const Math::Point& to );  
        void drawSolid( ARgbImage& image, const Pen& pen, const Math::Point* points,  size_t pointCount );
        void drawDash( ARgbImage& image, const Pen& pen, const Math::Point* points,  size_t pointCount);

        inline void outputSpan(ARgbImage& image, const Pen& pen, size_t x, size_t y, size_t length )
        {
            _stroke->stroke( image, pen, x, y, length );
        }

        inline void outputPixel( ARgbImage& image, const Pen& pen, size_t x, size_t y )
        {
            image.pixel( x,  y ) = pen.color();
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

        inline void addPoint(int xx, int yy, Math::Point** ppt, unsigned int** pwidth, int& numSpans, int& ycurr, bool& firstspan, int signdy)
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

} // namespace gfx

} // namespace Pt

#endif
