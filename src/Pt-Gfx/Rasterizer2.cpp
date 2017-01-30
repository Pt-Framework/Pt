/* Copyright (C) 2017-2017 Aloysius Indrayanto
   Copyright (C) 2006-2015 Marc Boris Duerner
   Copyright (C) 2006-2015 Laurentiu-Gheorghe Crisan

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  As a special exception, you may use this file as part of a free
  software library without restriction. Specifically, if other files
  instantiate templates or use macros or inline functions from this
  file, or you compile this file and link it with other files to
  produce an executable, this file does not by itself cause the
  resulting executable to be covered by the GNU General Public
  License. This exception does not however invalidate any other
  reasons why the executable file might be covered by the GNU Library
  General Public License.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
  02110-1301 USA
*/

#include "ClipPolygon.h"
#include "LineSlope.h"
#include "LineEdge.h"
#include "LineFace.h"
#include "EdgeTable.h"
#include "DrawText.h"

#include "Rasterizer2.h"
#include <Pt/Gfx/ImagePainter.h>
#include <Pt/Gfx/Image.h>
#include <Pt/Gfx/Algorithm.h>
#include <Pt/Math.h>
#include <algorithm>
#include <cmath>

namespace Pt {

namespace Gfx {

inline void addPoint(int xx, int yy, Point** ppt,  int** pwidth, int& numSpans, int& ycurr, bool& firstspan, int signdy)
{
  if( !firstspan && yy == ycurr )
  {
    int xdelta = xx - (int)(*ppt)->x();

    if (xdelta < 0)
    {
      (**pwidth) -= xdelta;
      (*ppt)->setX(xx);
    }
    else if (xdelta > 0)
    {
       int widthcurr = **pwidth;
      (**pwidth) = std::max( widthcurr, (1 + xdelta));
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
    {
        firstspan = false;
    }

    (*ppt)->setX(xx);
    (*ppt)->setY(yy);
    **pwidth = 1;
    ycurr = yy;
    ++numSpans;
  }
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





Rasterizer2::Rasterizer2(Image& image)
: _image(&image)
, _text( new DrawText() )
, _font()
, _compositionMode(CompositionMode::SourceCopy)
, _penPixel(_image->view(), 0, 0)
, _brushPixel(_image->view(), 0, 0)
{
    _text->setFont(_font);
    updateClip();
}

Rasterizer2::~Rasterizer2()
{
    delete _text;
}

void Rasterizer2::setImage( Image& image )
{
    _image = &image;
    _brushBuffer.reset(_image->format(), _brushBuffer.size());
    updateClip();
}

const ImageFormat& Rasterizer2::format() const
{
    return _image->format();
}

void Rasterizer2::setPen( const Pen& pen )
{
    _pen = pen;
    _penBuffer.reset(_image->format(), Size(64, 1));
    Gfx::fill(_penBuffer.begin(), _penBuffer.end(), pen.color());

    _penPixel.reset(_penBuffer.view(), 0, 0);
}

void Rasterizer2::setBrush( const Brush& brush )
{
    _brush = brush;
    _isGradient = false;

    switch( brush.fillStyle() )
    {
        case Brush::Solid:
            _brushBuffer.reset( _image->format(), Size(64, 1) );
            Gfx::fill(_brushBuffer.begin(), _brushBuffer.end(), brush.color());

            _brushImage = &_brushBuffer;
            break;

        case Brush::Texture:
            if( brush.texture().format() != _image->format() )
            {
                _brushBuffer.reset( _image->format(), brush.texture().size() );
                Gfx::copy( brush.texture().begin(), brush.texture().end(), _brushBuffer.begin() );

                _brushImage = &_brushBuffer;
            }
            else
            {
                _brushImage = &_brush.texture();
            }
            break;

        case Brush::HorizontalGradient:
        case Brush::VerticalGradient:
            _isGradient = true;
            _brushImage = &_brushBuffer;
            break;
    }

    _brushPixel.reset(_brushImage->view(), 0, 0);
}

void Rasterizer2::setFont(const Font& font)
{
    _font = font;
    _text->setFont(_font);
}

FontMetrics Rasterizer2::fontMetrics( const String& text ) const
{
    return _text->fontMetrics( text );
}

FontMetrics Rasterizer2::fontMetrics( const Font& font, const Pt::String& text )
{
    DrawText textRender;
    textRender.setFont(font);

    return textRender.fontMetrics(text);
}

void Rasterizer2::setClip( const Rect& clip )
{
    _clip = clip;
    updateClip();
}

void Rasterizer2::image( const Point& to, const Image& img)
{
    Rect imageRect( Point(0,0), img.size() );
    image( to, img, imageRect );
}

void Rasterizer2::image(const Point& to, const Image& from, const Rect& fromRect)
{
    // clip fromRect to fit into the clip/image rect
    Point d = _currentClip.topLeft() - to;
    Point fromPos = fromRect.topLeft() + d;

    Rect fromClip( fromPos, _currentClip.size() );
    fromClip = fromRect.intersect(fromClip);

    if( fromClip.isNull() )
      return;

    // account for smaller fromRect
    Point toClip = to + (fromClip.topLeft() - fromRect.topLeft());

    _image->format().copy(_image->view(), toClip, from.view(), fromClip, _compositionMode);
}

void Rasterizer2::strokeText( const Point& to, const Pt::String& text )
{
    _text->setClip(_currentClip);
    _text->draw( *_image, _pen.color(), to, text );
}



void Rasterizer2::stroke(const Point* points,  size_t pointCount)
{
    switch( _pen.style() )
    {
        case Pen::Solid:
            drawThinSolidPolyline( points, pointCount );
            /*
            if( _pen.size() == 1 )
                drawThinSolidPolyline( points, pointCount );
            else
                drawWideSolidPolyline( points, pointCount );
            */
            break;

        case Pen::Dash:
        case Pen::DoubleDash:
            /*
            if( _pen.size() == 1 )
                drawThinDashPolyline(points, pointCount );
            else
                drawWideDashPolyline( points, pointCount );
            */
            break;
    }
}




void Rasterizer2::drawThinSolidPolyline( const Point* points,  int pointCount)
{
    const Point *ppt;

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
    xstart =(int) ppt->x();
    ystart = (int)ppt->y();
    x2 = xstart;
    y2 = ystart;

    while( --pointCount )
    {
        x1 = x2;
        y1 = y2;
        ++ppt;

        x2 = (int)ppt->x();
        y2 = (int)ppt->y();

        if (x1 == x2)  // Vertical line.
        {
            if (y1 > y2)
            {   // Make line go top to bottom, keeping endpoint semantics.
                int tmp;

                tmp = y2;
                y2 = y1 + 1;
                y1 = tmp + 1;
            }

            // Draw line.
            if (y1 != y2)
            {
                for( int i = y1; i < y2; ++i)
                   stroke(x1, i);
            }

            // Restore final point.
            y2 =(int) ppt->y();
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
                stroke( x1, y1, x2 - x1  );

            // Restore final point.
            x2 = (int)ppt->x();
        }
        else
        { // Sloped line.
            int adx;        // Abs values of dx and dy
            int ady;
            int signdx;     // Sign of dx and dy
            int signdy;
            int e, e1, e2;  // Bresenham error and increments
            int axis;       // Major axis
            int len;        // Length of segment

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

            bresenhamLineSegment( signdx, signdy, axis, x1, y1, e, e1, e2, len);
        }
    }
}

void Rasterizer2::bresenhamLineSegment( int signdx, int signdy, int axis, int x1, int y1, int e, int e1, int e2, int len )
{
    if (len == 0)
        return;

    std::vector<Point> ptInit(len);
    std::vector< int> widthInit(len);

    Point* pptLast     = &ptInit[len - 1];
     int *pwidthLast = &widthInit[len - 1];

    int x, y;
    int e3;
    int numSpans = 0;
    int ycurr = 0;
    Point  *ppt = pptLast;
     int *pwidth = pwidthLast;
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
            Point *ppt_src   = pptLast - (numSpans - 1);
            Point *ppt_dst   = &ptInit[0];
             int *pwidth_src = pwidthLast - (numSpans - 1);
             int *pwidth_dst = &widthInit[0];

            int count = numSpans;

            while (count--)
            {
                *ppt_dst++    = *ppt_src++;
                *pwidth_dst++ = *pwidth_src++;
            }
        }

        for( int i = 0; i < numSpans; ++i)
            stroke( (int)ptInit[i].x(), (int)ptInit[i].y(), widthInit[i]);
    }
}

void Rasterizer2::stroke(int x, int y)
{
    if( x < _currentClip.x() || x >= _clipRight ||
        y < _currentClip.y() || y >= _clipBottom)
        return;

    Pixel pixel(_image->view(), x, y);
    _image->format().setPixel(pixel, _penPixel, _compositionMode);
}

void Rasterizer2::stroke(int xpos, int ypos, int length)
{

    clipSpan( xpos, ypos, length );

    int bufferWidth = _penBuffer.width();


    while(length > 0)
    {
        int n = std::min(length, bufferWidth);
        if( n )
        {
             Pixel destPixel( _image->view(), xpos, ypos);
            _image->format().copy(destPixel, _penPixel, n, _compositionMode);
        }

        length -= n;
        xpos   += n;
    }
}


void Rasterizer2::clipSpan( int& xpos, int& ypos, int& length )
{
    if( ypos < _currentClip.y() )
    {
        length = 0;
        return;
    }

    if( ypos >= _clipBottom )
    {
        length = 0;
        return;
    }

    if( xpos >= _clipRight )
    {
        length = 0;
        return;
    }


    if(xpos < _currentClip.x() )
    {
        length -= (_currentClip.x()- xpos);
        xpos = _currentClip.x();
    }

    if( (xpos + length) >= _clipRight )
    length =  _clipRight - xpos;
}

void Rasterizer2::updateClip()
{
    Rect imageRect(Point(0,0) , _image->size());
    _currentClip =  _clip.isNull() ? imageRect : _clip.intersect( imageRect);
    _clipRight = _currentClip.x() + _currentClip.width();
    _clipBottom = _currentClip.y() + _currentClip.height();
}

} // namespace

} // namespace
