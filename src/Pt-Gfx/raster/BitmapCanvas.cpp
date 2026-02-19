/* Copyright (C) 2015 Marc Boris Duerner
   Copyright (C) 2015 Laurentiu-Gheorghe Crisan

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

#include "BitmapCanvas.h"
#include "ClipPolygon.h"
#include "LineSlope.h"
#include "LineEdge.h"
#include "LineFace.h"
#include "EdgeTable.h"
#include "DrawText.h"

#include <Pt/Gfx/Image.h>
#include <Pt/Gfx/Argb32.h>
#include <Pt/Gfx/Paint.h>
#include <Pt/Gfx/Algorithm.h>
#include <Pt/Math.h>
#include <algorithm>
#include <cmath>

namespace {

void sourceOver(Pt::uint8_t* to, const Pt::uint8_t* from)
{
    Pt::uint8_t alpha = from[3];
    Pt::uint32_t alphaSrc = alpha + 1;
    Pt::uint32_t alphaInv = 256 - alpha;

    to[0] = (unsigned char)((alphaSrc * from[0] + alphaInv * to[0]) >> 8);
    to[1] = (unsigned char)((alphaSrc * from[1] + alphaInv * to[1]) >> 8);
    to[2] = (unsigned char)((alphaSrc * from[2] + alphaInv * to[2]) >> 8);
    to[3] = (unsigned char)((alphaSrc * from[3] + alphaInv * to[3]) >> 8);
}

class EllipseSpan
{
  public:
      EllipseSpan()
      : x1( 0 )
      , len1( 0 )
      , x2( 0 )
      , len2( 0 )
      {}

      ~EllipseSpan()
      { }

      int x1;
      int len1;
      int x2;
      int len2;
};


inline void addPoint(int xx, int yy, Pt::Gfx::BitmapCanvas::Point** ppt, int** pwidth, 
                     int& numSpans, int& ycurr, bool& firstspan, int signdy)
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


inline int stepAround( int v, int incr, int max )
{
  return (((v) + (incr) < 0) ? (max - 1) : ((v) + (incr) == max) ? 0 : ((v) + (incr)));
}

} // namespace

namespace Pt {

namespace Gfx {

BitmapCanvas::BitmapCanvas()
: Canvas()
, _text( new DrawText() )
, _image(0)
, _lastScaleFactor(1.0)
, _penView(_penBuffer)
, _penPixel(_penView, 0, 0)
, _brushView(_brushBuffer)
, _brushPixel(_brushView, 0, 0)
, _isGradient(false)
, _hasClip(false)
{
}


BitmapCanvas::~BitmapCanvas()
{
    delete _text;
}


void BitmapCanvas::init(Image& image)
{
    _image = &image;
    _imageView.reset(*_image);
}


void BitmapCanvas::onBeginPaint(const Gfx::Paint& paint)
{
    double scaleFactor = scaling().scaleFactor();
    if( std::abs(_lastScaleFactor - scaleFactor) < 0.0001 )
    {
        onSetPen( paint.pen() );
    }
}


void BitmapCanvas::onFinishPaint()
{
    // NOTE: this might be called from the attached surface base class destructor

    if(_image)
        _image = 0;
}


void BitmapCanvas::onSetCompositionMode(const Gfx::CompositionMode& mode) 
{
    _compositionMode = mode;
}


void BitmapCanvas::onApplyCompositionMode(const Gfx::CompositionMode& mode)
{
    _compositionMode = mode;
}


void BitmapCanvas::onSetPen(const Gfx::Pen& pen)
{
    _lastScaleFactor = scaling().scaleFactor();

    size_t penSize = pen.size();

    // keep pen size when downscaling
    penSize = _lastScaleFactor < 1.0 ? penSize 
                                     : static_cast<size_t>( penSize * _lastScaleFactor );
    _pen = pen;
    _pen.setSize(penSize);
}


void BitmapCanvas::onApplyPen(const Gfx::Pen& pen)
{
    if( ! _image )
        return;
    
    _penBuffer.reset(_image->format(), 64, 1);

    Gfx::PixelView fillView(_penBuffer);
    Gfx::fill( fillView.begin(), fillView.end(), pen.color() );

    _penView.reset(_penBuffer);
    _penPixel.reset(_penView, 0, 0);

    _text->setPen(pen);
}


void BitmapCanvas::onSetBrush(const Gfx::Brush& brush)
{
    _brush = brush;
}


void BitmapCanvas::onApplyBrush(const Gfx::Brush& brush)
{
    if( ! _image )
        return;

    _isGradient = false;

    switch( brush.fillStyle() )
    {
        case Brush::Solid:
        {
            _brushBuffer.reset( _image->format(), 64, 1);
            _brushView.reset(_brushBuffer);

            Gfx::PixelView fillView(_brushBuffer);
            Gfx::fill(fillView.begin(), fillView.end(), brush.color());
            break;
        }

        case Brush::Texture:
            if( brush.texture().format() != _image->format() )
            {
                _brushBuffer.reset( _image->format(), 
                                    brush.texture().width(), brush.texture().height() );
                _brushView.reset(_brushBuffer);

                Gfx::ConstPixelView textureView( brush.texture() );
                Gfx::PixelView brushView(_brushBuffer);

                Gfx::copy( textureView.begin(), textureView.end(), brushView.begin() );
            }
            else
            {
                _brushView.reset( _brush.texture() );
            }
            break;

        case Brush::Gradient:
            _isGradient = true;
            _brushView.reset(_brushBuffer);
            break;
    }

    _brushPixel.reset(_brushView, 0, 0);
}


void BitmapCanvas::updateGradientBrush(int width, int height)
{
    if( ! _image )
        return;

    Color gradientStart = _brush.color();
    Color gradientStop = _brush.gradientColor();

    switch( _brush.gradient() )
    {
        case Pt::Gfx::Brush::Horizontal:
          _brushBuffer.reset(_image->format(), width, 1);
          height = 1;
          break;

        case Pt::Gfx::Brush::Vertical:
          _brushBuffer.reset(_image->format(), 1, height);
          width = 1;
          std::swap(gradientStart, gradientStop);
          break;

        case Pt::Gfx::Brush::Linear:
        case Pt::Gfx::Brush::Radial:
          return;
    }

    int length = width + height - 1;
    Pt::uint8_t* pixel = _brushBuffer.data();

    for(int n = 0; n < length; ++n)
    {
        float f1 = (length - n) / float(length);
        float f2 = n / float(length);

        float r1 = gradientStart.red() * f1;
        float r2 = gradientStop.red() * f2;

        float g1 = gradientStart.green() * f1;
        float g2 = gradientStop.green() * f2;

        float b1 = gradientStart.blue() * f1;
        float b2 = gradientStop.blue() * f2;


        pixel[0] = (b1 + b2) / 257;
        pixel[1] = (g1 + g2) / 257;
        pixel[2] = (r1 + r2) / 257;
        pixel[3] = 0;

        pixel += 4;
    }
}


void BitmapCanvas::onSetFont(const Gfx::Font& font)
{
    _font = font;
}


void BitmapCanvas::onApplyFont(const Gfx::Font& font)
{
    _text->setFont(font);
}


void BitmapCanvas::onSetClip(const Gfx::RectF* clip)
{
    _hasClip = clip != 0;

    if(clip)
    {
        Gfx::PointF origin =  transform() * clip->origin();
        Gfx::SizeF size =  transform() * clip->size();
        Gfx::RectF clipP(origin, size);
        
        _clip = clipP;
    }
    else
        _clip.clear();
}


void BitmapCanvas::onApplyClip(const Gfx::RectF* clip) 
{
    if( ! _image )
        return;

    Rect imageRect;
    imageRect.setWidth( _image->width() );
    imageRect.setHeight( _image->height() );

    if( ! _hasClip )
    {
        _currentClip = imageRect;
        return;
    }

    Rect clipRect = round(_clip);

    if( clipRect.isNull() ) // crashes otherwise
        clipRect = Rect( Point(0, 0), Size(1, 1) );

    _currentClip =  clipRect.intersect(imageRect);
}


void BitmapCanvas::onDrawLine(const Gfx::PointF& from, const Gfx::PointF& to)
{
    if( ! _image )
        return;

    Point points[2];

    points[0] = toLocal(from);
    points[1] = toLocal(to);

    stroke(points, 2, _currentClip);
}


void BitmapCanvas::onDrawPolyline(const Gfx::PointF* pts, const size_t n)
{
    std::vector<Point> points(n);

    for(size_t i = 0; i < n; ++i)
    {
        const Gfx::PointF& p = pts[i];
        points[i] = toLocal(p);
    }

    stroke(&points[0], points.size(), _currentClip);
}


void BitmapCanvas::onFillPolygon(const Gfx::PointF* pts, const size_t n)
{
    std::vector<Point> points(n);

    for(size_t i = 0; i < n; ++i)
    {
        const Gfx::PointF& p = pts[i];
        points[i] = toLocal(p);
    }

    fill(&points[0], points.size(), _currentClip);
}


void BitmapCanvas::onDrawRect(const Gfx::RectF& r)
{
    // TODO: this works only if the transform is simple 
    //       and the shape is stable
    
    Gfx::PointF origin =  transform() * r.topLeft();
    Gfx::SizeF size =  transform() * r.size();
    Gfx::RectF rect(origin, size);

    Rect rectangle( lround(rect.left()   - 0.4999), 
                    lround(rect.right()  - 0.4999),
                    lround(rect.top()    - 0.4999), 
                    lround(rect.bottom() - 0.4999)) ;

    Point points[5] = { rectangle.topLeft(),
                        rectangle.topRight(),
                        rectangle.bottomRight(),
                        rectangle.bottomLeft(),
                        rectangle.topLeft() };

    stroke(points, 5, _currentClip);
}


void BitmapCanvas::onFillRect(const Gfx::RectF& r)
{
    // TODO: this works only if the transform is simple 
    //       and the shape is stable

    Gfx::PointF origin =  transform() * r.topLeft();
    Gfx::SizeF size =  transform() * r.size();
    Gfx::RectF rect(origin, size);

    Rect rectangle( lround( rect.left() ),
                    lround( rect.right() + 0.001 ), 
                    lround( rect.top() ),
                    lround( rect.bottom() + 0.001 ) );

    fillRect(rectangle, _currentClip);
}


void BitmapCanvas::onDrawEllipse(const PointF& topLeftF, const SizeF& sizeF)
{
    Gfx::PointF p = transform() * topLeftF;
    Gfx::SizeF s = transform() * sizeF;

    Point topLeft( Pt::lround(p.x() - 0.5),
                   Pt::lround(p.y() - 0.5) );

    Size size( lround(s.width() - 0.5),
               lround(s.height() - 0.5) );

    strokeEllipse(topLeft, size, _currentClip);
}


void BitmapCanvas::onFillEllipse(const PointF& topLeftF, const SizeF& sizeF)
{
    Gfx::PointF p = transform() * topLeftF;
    Gfx::SizeF s = transform() * sizeF;

    Point topLeft = round(p);
    Size size = round(s);

    fillEllipse(topLeft, size, _currentClip);
}


void BitmapCanvas::onSetPath(const Gfx::Path& path)
{
    _flatPath.clear();
    path.toPolygons(_flatPath);
}


void BitmapCanvas::onDrawPath()
{
    strokePolygons(_flatPath, _currentClip);
}


void BitmapCanvas::onFillPath()
{
    fillPolygons(_flatPath, _currentClip);
}


void BitmapCanvas::onDrawPath(const Gfx::Path& path)
{
    std::vector<Polygon> flatPath;
    path.toPolygons(flatPath);
    strokePolygons(flatPath, _currentClip);
}


void BitmapCanvas::onFillPath(const Gfx::Path& path)
{
    std::vector<Polygon> flatPath;
    path.toPolygons(flatPath);
    fillPolygons(flatPath, _currentClip);
}


TextMetrics BitmapCanvas::onGetTextMetrics(const String& text) const
{
    return _text->textMetrics(text);
}


void BitmapCanvas::onDrawText(const PointF& to, const Pt::String& text, 
                               const Transform* tform)
{
    if( ! _image )
        return;

    Gfx::Transform tf;
    if(tform)
        tf *= *tform;

    tf.translate( to.x(), to.y() );
    tf *= transform();

    _text->setClip(_currentClip);
    _text->draw(*_image, 0, 0, text, _compositionMode, &tf);
}


void BitmapCanvas::onDrawImage(const PointF& toF, const Image& image, 
                               const RectF* imageRect)
{
    Gfx::PointF toP = transform() * toF;
    Point to = round(toP);

    if(imageRect)
        putImage( to, image, round(*imageRect) );
    else
        putImage(to, image);
}


void BitmapCanvas::clipSpan(int& xpos, int& ypos, int& length, const Rect& clip)
{

  if( ypos < clip.y() )
  {
    length = 0;
    return;
  }

  if( ypos >= clip.bottom() )
  {
    length = 0;
    return;
  }

  if( xpos >= clip.right() )
  {
    length = 0;
    return;
  }

  if(xpos < clip.x() )
  {
      length -= clip.x() - xpos;
      xpos = clip.x();
  }

  if( (xpos + length) >= clip.right() )
    length = clip.right() - xpos;
}


void BitmapCanvas::stroke(const Point* points,  size_t n, const Rect& currentClip)
{
  switch( _pen.style() )
  {
    case Pen::Solid:
      if( _pen.size() == 1 )
        drawThinSolidPolyline(points, n, currentClip);
      else
        drawWideSolidPolyline(points, n, currentClip);
       break;

    case Pen::Dot:
      if( _pen.size() == 1 )
        drawThinDashPolyline(points, n, _pen.size(), _pen.size(), currentClip);
      else
        drawWideDashPolyline(points, n, _pen.size(), _pen.size(), currentClip);
      break;

    case Pen::Dash:
    //case Pen::DoubleDash:
      if( _pen.size() == 1 )
        drawThinDashPolyline( points, n, _pen.size() * 3, _pen.size(), currentClip);
      else
        drawWideDashPolyline (points, n, _pen.size() * 3, _pen.size(), currentClip);
      break;
  }
}


void BitmapCanvas::stroke(const Point& pixel, const Rect& currentClip)
{
  stroke( (int) pixel.x(),(int) pixel.y(), currentClip );
}


void BitmapCanvas::stroke(int x, int y, const Rect& clip)
{
    if( x < clip.x() || x >= clip.right() ||
        y < clip.y() || y >= clip.bottom() )
        return;

    if( ! _image )
        return;

    std::size_t off = _image->size( _image->width(), y, _image->padding() );
    off += x * pixelStride( _image->format() );

    Pt::uint8_t* to = _image->data() + off;
    const Pt::uint8_t* from = _penPixel.base();

    switch(_compositionMode) 
    {
        default:
        case CompositionMode::SourceCopy:
            Argb32::sourceCopy(to, from);
            break;

        case CompositionMode::SourceOver:
            Argb32::sourceOver(to, from);
            break;
    }
}


void BitmapCanvas::stroke(int xpos, int ypos, int length, const Rect& currentClip)
{
    if( ! _image )
        return;

    clipSpan(xpos, ypos, length, currentClip);

    int bufferWidth = _penBuffer.width();

    while(length > 0)
    {
        int n = std::min(length, bufferWidth);
        if( n )
        {
            PixelView::Pixel destPixel( _imageView, xpos, ypos);

            switch(_compositionMode) 
            {
                default:
                case CompositionMode::SourceCopy:
                    Argb32::sourceCopy(destPixel.base(), _penPixel.base(), n);
                    break;

                case CompositionMode::SourceOver:
                    Argb32::sourceOver(destPixel.base(), _penPixel.base(), n);
                    break;
            }
        }

        length -= n;
        xpos   += n;
    }
}


void BitmapCanvas::fill(const Point* pts, size_t pointCount, const Rect& currentClip)
{
    EdgeSet           globalEdgeTable;
    ActiveEdgeTable   activeEdgeTable;
    EdgeSet::iterator currentPos;
    std::vector<Point> points( pts, pts + pointCount );

    ClipPolygon clipper;
    clipper(points, currentClip);

    // find unclipped origin coordinates
    Point origin( std::numeric_limits<int>::max(), std::numeric_limits<int>::max() );

    int leftPos = std::numeric_limits<int>::max();
    int topPos = std::numeric_limits<int>::max();
    int rightPos = 0;
    int bottomPos = 0;

    for(size_t n = 0; n < points.size(); ++n)
    {
        const Point& p = points[n];
        origin.setX( std::min( origin.x(), p.x() ) );
        origin.setY( std::min( origin.y(), p.y() ) );

        if( ! _isGradient )
          continue;

        if( p.y() < topPos)
            topPos = p.y();

        if( p.y() > bottomPos)
            bottomPos = p.y();

        if( p.x() < leftPos)
            leftPos = p.x();

        if( p.x() > rightPos)
            rightPos = p.x();
    }

    if( points.empty() )
        return;

    if( points.back() != points.front() )
        points.push_back( points[0] );

    if( _isGradient )
        updateGradientBrush(rightPos - leftPos, bottomPos - topPos);

    // might as well create a new table here...
    globalEdgeTable.clear();

    // Fill the global edge table. Two points yield an edge.
    Edge   edge;
    Point* bottom = 0;
    Point* top = 0;

    for( size_t i = 1; i < points.size(); ++i )
    {
        // Find out which point is above and which is below
        if ( points[i-1].y() > points[i].y() )
        {
            bottom = &( points[i-1] );
            top = &( points[i] );
        }
        else
        {
            bottom = &(points[i]);
            top = &(points[i-1]);
        }

        // Omit horizontal edges, add others to global edge table. The GET
        // is sorted by primarily by the edges ymin and secondarily by
        // the x value of the edge

        if( top->y() != bottom->y())
        {
            const int dy   = (int)bottom->y() - (int)top->y();
            const int dx   = (int)bottom->x() - (int)top->x();

            edge.ymax = (int)bottom->y();
            edge.ymin = (int)top->y();
            edge.x    = (int)top->x();

            // Bresenham stuff...
            if (dx < 0)
            {
                edge.m = dx / dy;
                edge.m1 = edge.m - 1;
                edge.incr1 = -2 * dx + 2 * dy * edge.m1;
                edge.incr2 = -2 * dx + 2 * dy * edge.m;
                edge.d = 2 * edge.m * dy - 2 * dx - 2 * dy;
            }
            else
            {
                edge.m = dx / dy;
                edge.m1 = edge.m + 1;
                edge.incr1 = 2 * dx - 2 * dy * edge.m1;
                edge.incr2 = 2 * dx - 2 * dy * edge.m;
                edge.d = -2 * edge.m * dy + 2 * dx;
            }

            globalEdgeTable.insert( edge );
        }
    }

    // if all polygon points are on one line the GET will be empty
    if( globalEdgeTable.empty() )
        return;

    // Start at ymin of the first entry in the GET.
    int scanLine = globalEdgeTable.begin()->ymin;

    // move active edges to AET for current scanline. Keep iterator where
    // we stopped for later use.
    EdgeSet::iterator it = globalEdgeTable.begin();

    for( ; it != globalEdgeTable.end() && it->ymin == scanLine; ++it )
        activeEdgeTable.addEdge( *it );

    ///ActiveEdgeTable last;

    do
    {
        ///last = activeEdgeTable;

        // fill every even span, starting at even (even-odd-rule)
        outputEdges(activeEdgeTable, origin, scanLine);

        // now we are done with the current active edges and can update
        // them for the next scanline.
        scanLine++;

        activeEdgeTable.update(scanLine);

        // move active edges to AET for current scanline
        for( ; it != globalEdgeTable.end() && it->ymin == scanLine; ++it )
        {
            activeEdgeTable.addEdge(*it);
        }

        // Need to resort the AET, because of update and new edges
        activeEdgeTable.sort();
    }
    while( ! activeEdgeTable.empty() );

    //last.update();

    //outputEdges(last, origin, scanLine);
}


void BitmapCanvas::fill(const Point& origin, const Point& pos, int length)
{
  switch( _brush.fillStyle() )
  {
    default:
    case Brush::Solid:
        fillSolid(pos, length);
        break;

    case Brush::Texture:
        fillTexture(origin, pos, length);
        break;

    case Brush::Gradient:
        if(_brush.gradient() == Brush::Vertical)
            fillVerticalGradient(origin, pos, length);
        else
            fillHorizontalGradient(origin, pos, length);
        break;
  }
}


void BitmapCanvas::fillRect(const Rect& rectIn, const Rect& currentClip)
{
    Rect rect = currentClip.intersect( rectIn );

    if( rect.isNull() )
        return;

    if( _isGradient )
        updateGradientBrush( rectIn.width(), rectIn.height() );

    int length = rect.width();

    Point linePos = rect.topLeft();

    for(int y = 0; y < rect.height(); y++)
    {
        fill(rect.topLeft(), linePos, length);
        linePos.addY(1);
    }
}


void BitmapCanvas::fillSolid(const Point& pos, int length)
{
    if( ! _image )
        return;

    int xpos = pos.x();
    int ypos = pos.y();

    if( length <= 0)
        return;

    int bufferWidth = _brushView.width();

    while(length > 0)
    {
        int n = std::min(length, bufferWidth);

        if( n )
        {
            PixelView::Pixel destPixel(_imageView, xpos, ypos);

            switch(_compositionMode) 
            {
                default:
                case CompositionMode::SourceCopy:
                    Argb32::sourceCopy(destPixel.base(), _brushPixel.base(), n);
                    break;

                case CompositionMode::SourceOver:
                    Argb32::sourceOver(destPixel.base(), _brushPixel.base(), n);
                    break;
            }
        }

        length -= n;
        xpos   += n;
    }
}


void BitmapCanvas::fillVerticalGradient( const Point& origin, const Point& pos,  int length )
{
    fillTexture(origin, pos, length);
}


void BitmapCanvas::fillHorizontalGradient( const Point& origin, const Point& pos,  int length )
{
    fillTexture(origin, pos, length);
}


void BitmapCanvas::fillTexture(const Point& origin, const Point& pos,  int length)
{
    if( ! _image )
        return;

    ConstPixelView& texture = _brushView;
    int xpos = pos.x();
    int ypos = pos.y();
    int originx =  origin.x();
    int originy = origin.y();

    while(length)
    {
        // x position in the texture to copy from
        const int textureXPos = (int)( xpos - originx ) % texture.width();

        // determine the scanline of the texture to copy from
        const int textureYPos = (int) ( ypos - originy ) % texture.height();

        // number of pixels to copy from texture
        const int fillLength = std::min( length, (int)texture.width() - textureXPos );

        // Copy pixels from textrure to image
        if(fillLength)
        {
            PixelView::ConstPixel sourcePixel(texture,  textureXPos, textureYPos);
            PixelView::Pixel destPixel(_imageView, xpos, ypos);

            switch(_compositionMode)
            {
                default:
                case CompositionMode::SourceCopy:
                    Argb32::sourceCopy(destPixel.base(), sourcePixel.base(), fillLength);
                    break;

                case CompositionMode::SourceOver:
                    Argb32::sourceOver(destPixel.base(), sourcePixel.base(), fillLength);
                    break;
            }
        }

        // Remaining unfilled pixels of the span
        length -= fillLength;
        xpos   += fillLength;
    }
}


void BitmapCanvas::outputEdges(const ActiveEdgeTable& edges, const Point& origin, int scanLine)
{
    // fill every even span, starting at even (even-odd-rule)
    for( size_t i = 1; i < edges.size(); i += 2 )
    {
        const int xend    = std::max(edges[i].x, edges[i-1].x);
        const int xbegin  = std::min(edges[i].x, edges[i-1].x);
        const int length  = xend - xbegin;

        fill(Point((int)origin.x(), (int)origin.y() ), Point(xbegin, scanLine), length);
    }
}


void BitmapCanvas::outputSpan( const Point& topLeft, int x, int y, int width )
{
    if( ! _image )
        return;

    const int imageWidth = static_cast<int>( _image->width() );
    const int imageHeight = static_cast<int>( _image->height() );

    if( y >= imageHeight )
        return;

    if( y < 0 )
        return;

    const int xend = std::min( x + width, imageWidth );
    int       xpos = std::max( 0, x );

    if( xend > xpos )
        fill( topLeft, Point(xpos, y), xend-xpos );
}


void BitmapCanvas::strokePolygons(const std::vector<Polygon>& polygons, const Rect& currentClip)
{
    for(const Polygon& poly : polygons)
    {
      std::size_t n = poly.size();
      std::vector<Point> points(n);

      for(size_t i = 0; i < n; ++i)
      {
          //Gfx::PointF pComp = scaling().toPhysical( _paint->origin() + poly.at(i) );
          Gfx::PointF p = transform() * poly.at(i);
          //Gfx::PointF p = poly.at(i);

          points[i] = Point( Pt::lround(p.x() - 0.4999),
                             Pt::lround(p.y() - 0.4999) );
      }

      stroke(&points[0], points.size(), currentClip);
    }
}


void BitmapCanvas::fillPolygons(const std::vector<Polygon>& polygons, const Rect& currentClip)
{
    EdgeSet           globalEdgeTable;
    ActiveEdgeTable   activeEdgeTable;
    EdgeSet::iterator currentPos;
    
    // find unclipped origin coordinates
    Point origin( std::numeric_limits<int>::max(), std::numeric_limits<int>::max() );

    // might as well create a new table here...
    globalEdgeTable.clear();

    int leftPosG = std::numeric_limits<int>::max();
    int topPosG = std::numeric_limits<int>::max();
    int rightPosG = 0;
    int bottomPosG = 0;

    for(const Polygon& poly : polygons)
    {
        std::size_t n = poly.size();
        std::vector<Point> points(n);

        for (size_t i = 0; i < n; ++i)
        {
            //Gfx::PointF pComp = scaling().toPhysical( _paint->origin() + poly.at(i) );
            Gfx::PointF p = transform() * poly.at(i);
            //Gfx::PointF p = poly.at(i);

            points[i] = Point( Pt::lround(p.x() - 0.4999),
                               Pt::lround(p.y() - 0.4999) );
        }

        ClipPolygon clipper;
        clipper(points, currentClip);

        for(size_t n = 0; n < points.size(); ++n)
        {
            const Point& p = points[n];
            origin.setX( std::min( origin.x(), p.x() ) );
            origin.setY( std::min( origin.y(), p.y() ) );

            if( ! _isGradient )
              continue;

            if( p.y() < topPosG)
                topPosG = p.y();

            if( p.y() > bottomPosG)
                bottomPosG = p.y();

            if( p.x() < leftPosG)
                leftPosG = p.x();

            if( p.x() > rightPosG)
                rightPosG = p.x();
        }

        if( points.empty() )
            continue;

        if( points.back() != points.front() )
            points.push_back( points[0] );

        // Fill the global edge table. Two points yield an edge.
        Edge   edge;
        Point* bottom = 0;
        Point* top = 0;

        for( size_t i = 1; i < points.size(); ++i )
        {
            // Find out which point is above and which is below
            if ( points[i-1].y() > points[i].y() )
            {
                bottom = &( points[i-1] );
                top = &( points[i] );
            }
            else
            {
                bottom = &(points[i]);
                top = &(points[i-1]);
            }

            // Omit horizontal edges, add others to global edge table. The GET
            // is sorted by primarily by the edges ymin and secondarily by
            // the x value of the edge

            if( top->y() != bottom->y())
            {
                const int dy   = (int)bottom->y() - (int)top->y();
                const int dx   = (int)bottom->x() - (int)top->x();

                edge.ymax = (int)bottom->y();
                edge.ymin = (int)top->y();
                edge.x    = (int)top->x();

                // Bresenham stuff...
                if (dx < 0)
                {
                    edge.m = dx / dy;
                    edge.m1 = edge.m - 1;
                    edge.incr1 = -2 * dx + 2 * dy * edge.m1;
                    edge.incr2 = -2 * dx + 2 * dy * edge.m;
                    edge.d = 2 * edge.m * dy - 2 * dx - 2 * dy;
                }
                else
                {
                    edge.m = dx / dy;
                    edge.m1 = edge.m + 1;
                    edge.incr1 = 2 * dx - 2 * dy * edge.m1;
                    edge.incr2 = 2 * dx - 2 * dy * edge.m;
                    edge.d = -2 * edge.m * dy + 2 * dx;
                }

                globalEdgeTable.insert( edge );
            }
        }
    }

    if( _isGradient )
        updateGradientBrush(rightPosG - leftPosG, bottomPosG - topPosG);

    // if all polygon points are on one line the GET will be empty
    if( globalEdgeTable.empty() )
        return;

    // Start at ymin of the first entry in the GET.
    int scanLine = globalEdgeTable.begin()->ymin;

    // move active edges to AET for current scanline. Keep iterator where
    // we stopped for later use.
    EdgeSet::iterator it = globalEdgeTable.begin();

    for( ; it != globalEdgeTable.end() && it->ymin == scanLine; ++it )
        activeEdgeTable.addEdge( *it );

    ///ActiveEdgeTable last;

    do
    {
        ///last = activeEdgeTable;

        // fill every even span, starting at even (even-odd-rule)
        outputEdges(activeEdgeTable, origin, scanLine);

        // now we are done with the current active edges and can update
        // them for the next scanline.
        scanLine++;

        activeEdgeTable.update(scanLine);

        // move active edges to AET for current scanline
        for( ; it != globalEdgeTable.end() && it->ymin == scanLine; ++it )
        {
            activeEdgeTable.addEdge(*it);
        }

        // Need to resort the AET, because of update and new edges
        activeEdgeTable.sort();
    }
    while( ! activeEdgeTable.empty() );

    //last.update();

    //outputEdges(last, origin, scanLine);
}


void BitmapCanvas::strokeEllipse(const Point& topLeft, const Size& size, 
                               const Rect& currentClip)
{
    if( size.width() <= 1 || size.height() <= 1 )
        return;

    int errorx = 1;
    int errory = 1;

    if( (int)size.width()%2 != 0 )
        errorx  =  0;

    if( (int)size.height()%2 != 0)
        errory  = 0;

    int  a      = (int) size.width()/2;
    int  b      = (int)size.height()/2;

    int  xc     = (int)topLeft.x() + a;
    int  yc     = (int)topLeft.y() + b;

    int  x      = 0;
    int  y      = b;
    long a2     = (long) a*a;
    long b2     = (long) b*b;
    long crit1  = -( a2/4 + a%2 + b2 );
    long crit2  = -( b2/4 + b%2 + a2 );
    long crit3  = -( b2/4 + b%2 );
    long t      = -a2*y; /* e(x+1/2,y-1/2) - (a^2+b^2)/4 */
    long dxt    = 2*b2*x;
    long dyt    = -2*a2*y;
    long d2xt   = 2*b2;
    long d2yt   = 2*a2;

    while( y >= 0 && x <= a )
    {
        stroke(xc+x -errorx, yc+y - errory, currentClip);

        if( x!=0 || y!=0 )
            stroke( xc-x, yc-y, currentClip);

        if( x!=0 && y!=0 )
        {
            stroke( xc+x -errorx, yc-y, currentClip);
            stroke( xc-x, yc+y -errory, currentClip);
        }

        if( t + b2*x <= crit1 /* e(x+1,y-1/2) <= 0 */ || t + a2*y <= crit3 /* e(x+1/2,y) <= 0 */)
        {
            //inc x
            x++;
            dxt += d2xt;
            t   += dxt;
        }
        else if( t - a2*y > crit2 ) /* e(x+1/2,y-1) > 0 */
        {
            //inc y
            y--;
            dyt += d2yt;
            t   += dyt;
        }
        else
        {
            //inc x
            x++;
            dxt += d2xt;
            t   += dxt;

            //inc y
            y--;
            dyt += d2yt;
            t   += dyt;
        }
    }
}


void BitmapCanvas::fillEllipse( const Point& topLeftIn, const Size& size, const Rect& currentClip)
{
    const Point topLeft( (int) topLeftIn.x(), (int) topLeftIn.y() );

    if( size.width() == 0 || size.height() == 0 )
        return;

    if( size.width() ==  1 && size.height() == 1 )
        return;

    if( _isGradient )
        updateGradientBrush(size.width(), size.height() );

    /* e(x,y) = b^2*x^2 + a^2*y^2 - a^2*b^2 */
    int errorx = 1;
    int errory = 1;

    if( (int)size.width()%2 != 0 )
        errorx  =  0;

    if( (int) size.height()%2 != 0)
        errory  = 0;

    const int       a      = (int)size.width() /2;
    const int       b      = (int)size.height() /2;
    const int       xc     = (int)topLeft.x() + a;
    const int       yc     = (int)topLeft.y() + b;
    int             x      = 0;
    int             y      = b;
     int    width  = 1;
    long            a2     = (long)a*a;
    long            b2     = (long)b*b;
    long            crit1  = -(a2/4 + a%2 + b2);
    long            crit2  = -(b2/4 + b%2 + a2);
    long            crit3  = -(b2/4 + b%2);
    long            t      = -a2*y; /* e(x+1/2,y-1/2) - (a^2+b^2)/4 */
    long            dxt    = 2*b2*x;
    long            dyt    = -2*a2*y;
    long            d2xt   = 2*b2;
    long            d2yt   = 2*a2;

    while( y >= 0 && x <= a )
    {
        if( t + b2*x <= crit1 /* e(x+1,y-1/2) <= 0 */ || t + a2*y <= crit3 /* e(x+1/2,y) <= 0 */ )
        {
            //Increment x
            x++;
            dxt += d2xt;
            t   += dxt;

            width += 2;
        }
        else if( t - a2*y > crit2 ) /* e(x+1/2,y-1) > 0 */
        {
            outputSpan( topLeft, xc-x, yc-y, width  - errorx);

            if( y!=0 )
                outputSpan( topLeft, xc-x, yc+y - errory, width  - errorx);

             //Increment Y
            y--;
            dyt += d2yt;
            t   += dyt;
        }
        else
        {
            outputSpan(  topLeft, xc-x, yc-y, width -errorx );

            if( y != 0 )
                outputSpan( topLeft, xc-x, yc+y -errory, width - errorx );

             //Increment x
            x++;
            dxt += d2xt;
            t   += dxt;

            //Increment Y
            y--;
            dyt += d2yt;
            t   += dyt;

            width += 2;
        }
    }

    if( b == 0 )
        outputSpan(  topLeft, xc-a, yc, 2*a );
}


void BitmapCanvas::drawThinSolidPolyline(const Point* points, int pointCount, 
                                          const Rect& currentClip)
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
                   stroke(x1, i, currentClip);
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
                stroke( x1, y1, x2 - x1 , currentClip );

            // Restore final point.
            x2 = (int)ppt->x();
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

            bresenhamLineSegment( signdx, signdy, axis, x1, y1, e, e1, e2, len, currentClip);
        }
    }

    // Paint the last point if the end style isn't CapNotLast.  (I.e. assume
    // that a round/butt/projecting/triangular cap that is one pixel wide is
    // the same as the single pixel of the endpoint.)
/*
    if (_pen.capStyle() != Pen::NotLastCap && (xstart != x2 || ystart != y2 || ppt == points + 1) )
        stroke( x2, y2);
*/
}


void BitmapCanvas::drawThinDashPolyline(const Point* points,  int pointCount,
                                         int dashOn, int dashOff, const Rect& currentClip)
{
    const Point* ppt = points;
    int xstart, ystart;
    int x1, x2, y1, y2;

    int  dashNum    = 0;
    int  dashIndex  = 0;
    int  dashOffset = 0;
    bool isDoubleDash = false; //(_pen.style() == Pen::DoubleDash);

    std::vector<int> dashes(2);
    dashes[0] = dashOn; // Length of `on' dashes.
    dashes[1] = dashOff; // Length of `off' dashes.

    stepDash( 0, &dashNum, &dashIndex, &dashes[0], dashes.size(), &dashOffset);

    // Loop through points, drawing a dashed Bresenham segment for each line
    // segment of nonzero length.

    xstart =(int) ppt->x();
    ystart =(int) ppt->y();
    x2 = xstart;
    y2 = ystart;

    while (--pointCount)
    {
        x1 = x2;
        y1 = y2;

        ++ppt;

        x2 = (int)ppt->x();
        y2 = (int)ppt->y();

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
            bresenhamDasheLineSegment(&dashNum, &dashIndex, &dashes[0], dashes.size(), &dashOffset, 
                                       isDoubleDash, signdx, signdy, axis, x1, y1, 
                                       e, e1, e2, len, currentClip);
        }
    }

    // paint the last point if the end style isn't CapNotLast.
    // Assume that a projecting, butt, or round cap that is one
    // pixel wide is the same as the single pixel of the endpoint
    if( /*_pen.capStyle() != Pen::NotLastCap && */ (xstart != x2 || ystart != y2 || ppt == points + 1))
    {
        if (dashNum & 1)
        {// Background dash. paint, in paint type #0

            /*
                if (isDoubleDash)
                    stroke(  x2, y2);
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
            stroke(x2, y2, currentClip);
        }
    }
}


void BitmapCanvas::stepDash( int dist, int* pDashNum, int* pDashIndex, 
                           const int* pDash, int numInDashList, int *pDashOffset )
{
    int	dashNum, dashIndex, dashOffset;
    int totallen;
    int	i;

    dashNum     = *pDashNum;
    dashIndex   = *pDashIndex;
    dashOffset  = *pDashOffset;

    // Offset won't take us beyond end of present dash.
    if( dashOffset + dist < (int)(pDash[dashIndex]) )
    {
        *pDashOffset = dashOffset + dist;
        return;
    }

    // Move to next dash.
    dist -= (int)(pDash[dashIndex]) - dashOffset;
    dashNum++;
    dashIndex++;

    // Wrap to beginning of dash list.
    if( dashIndex == numInDashList )
        dashIndex = 0;

    // Make it easy on ourselves: work modulo iteration interval.
    totallen = 0;

    for (i = 0; i < numInDashList; i++)
        totallen += (int)(pDash[i]);

    if (totallen <= dist)
        dist = dist % totallen;

    while (dist >= (int)(pDash[dashIndex]))
    {
        dist -= (int)(pDash[dashIndex]);
        dashNum++;
        dashIndex++;

         // Wrap to beginning of dash list.
        if( dashIndex == numInDashList )
          dashIndex = 0;
    }

    *pDashNum = dashNum;
    *pDashIndex = dashIndex;
    *pDashOffset = dist;
}


// Internal: draw dashed Bresenham line segment. Called by miZeroDash().
// Endpoint semantics are used.
void BitmapCanvas::bresenhamDasheLineSegment(int *pdashNum, int *pdashIndex, const int *pDash, int numInDashList, int *pdashOffset, 
                                              bool isDoubleDash, int signdx, int signdy, int axis, int x1, int y1,
                                              int e, int e1, int e2, int len, const Rect& currentClip)
{
    std::vector<Point>  ptInit_bg;
    Point *pptLast_fg,  *pptLast_bg = 0;
    std::vector< int>  widthInit_bg;
     int *pwidthLast_fg, *pwidthLast_bg = 0;
    int		x, y;
    int 	e3;
    int		dashNum, dashIndex;
    int		dashOffset;
    int		dashRemaining;
    int		thisDash;

    // Variables in span generation code, i.e. in addPoint()
    int numSpans_fg, numSpans_bg = 0;
    int ycurr_fg, ycurr_bg = 0;

    Point *ppt_fg, *ppt_bg = 0;
     int *pwidth_fg, *pwidth_bg = 0;
    bool firstspan_fg, firstspan_bg = false;

    // Set up work arrays
    std::vector<Point> ptInit_fg(len);
    std::vector< int> widthInit_fg(len);

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
            Point *pptStart_fg;
             int *pwidthStart_fg;

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
                    stroke((int)pptStart_fg[i].x(), (int)pptStart_fg[i].y(), pwidthStart_fg[i], currentClip);
            }
        }

        if (isDoubleDash && numSpans_bg > 0)
        {// Have a background dash to paint.

            Point *pptStart_bg;
             int *pwidthStart_bg;

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
                stroke((int)pptStart_bg[i].x(), (int)pptStart_bg[i].y(), pwidthStart_bg[i], currentClip);
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


void BitmapCanvas::bresenhamLineSegment( int signdx, int signdy, int axis, int x1, int y1,
                                          int e, int e1, int e2, int len, const Rect& currentClip)
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
            Point *ppt_src	 = pptLast - (numSpans - 1);
            Point *ppt_dst	 = &ptInit[0];
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
            stroke( (int)ptInit[i].x(), (int)ptInit[i].y(), widthInit[i], currentClip);
    }
}


int BitmapCanvas::polyBuildPoly( const Point *vertices, const LineSlope *slopes, int count, int xi, int yi, LineEdge *left, LineEdge *right, int *pnleft, int *pnright, int *h)
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

    // Compute min, max y values for polygon (floating-point); also location
    // of corresponding vertices in vertex array
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

    // Compute integer y-value for bottom of polygon (round up).
    bottomy = static_cast<int>( ceil( maxy ) + yi );

    // Determine whether should go `clockwise' or `counterclockwise'
    // to move down the right side of the polygon
    i = top;
    j = stepAround (top, -1, count);

    clockwise = 1;
    slopeoff = 0;

    if( ( slopes[j].dy() *  slopes[i].dx() )  > (  slopes[i].dy() *  slopes[j].dx()  ) )
    {
        clockwise = -1;
        slopeoff = -1;
    }

    // step around right side of polygon from top to bottom, building array
    // of `right' edges (horizontal edges are ignored)
    i = top;
    s = stepAround( top, slopeoff, count );
    nright = 0;

    while (i != bottom)
    {
        if (slopes[s].dy() != 0)
        {
            y = buildLineEdge( vertices[i].x(), vertices[i].y(), slopes[s].k(), slopes[s].dx(), slopes[s].dy(), xi, yi, false, &right[nright]);

            if( nright != 0 )
                right[nright-1].setHeight( y - lasty );
            else			// y is top of first edge.
                topy = y;

            nright++;
            lasty = y;
        }

        i = stepAround (i, clockwise, count);
        s = stepAround (s, clockwise, count);
    }

    if( nright != 0 )
        right[nright-1].setHeight( bottomy - lasty );

    // step around left side of polygon from top to bottom, building array of
    //`left' edges (horizontal edges are ignored)
    if (slopeoff == 0)
        slopeoff = -1;
    else
        slopeoff = 0;

    i = top;
    s = stepAround (top, slopeoff, count);
    nleft = 0;

    while (i != bottom)
    {
        if( slopes[s].dy() != 0 )
        {
            y = buildLineEdge( vertices[i].x(), vertices[i].y(), slopes[s].k(), slopes[s].dx(),  slopes[s].dy(), xi, yi, true, &left[nleft]);

            if( nleft != 0 )
                left[nleft-1].setHeight( y - lasty );

            nleft++;
            lasty = y;
        }

        i = stepAround( i, -clockwise, count );
        s = stepAround( s, -clockwise, count );
    }

    if (nleft != 0)
        left[nleft-1].setHeight( bottomy - lasty );

    // return number of left-side and right-side edges; also height (vertical
    // range, an  int) and the vertical location of the top vertex
    // (an integer)
    *pnleft = nleft;
    *pnright = nright;
    *h = bottomy - topy;

    return topy;
}


int BitmapCanvas::buildLineEdge( double x0, double y0, double k, int dx, int dy, int xi, int yi, bool left, LineEdge *edge )
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


void BitmapCanvas::fillSpans(int x, int y,  int w,  int h, const Rect& currentClip)
{
    int ypos = std::max( 0, y );
    int yend = 0;

    if( (y + h) > 0 )
        yend = std::min<int>(currentClip.bottom(), y + h ) ;

    for( ; ypos < yend; ypos++ )
        stroke(x, ypos, w, currentClip );
}


void BitmapCanvas::fillLine(int y,  int overall_height, LineEdge *left, LineEdge *right, int left_count, int right_count, const Rect& currentClip)
{
    int left_x		= 0;
    int left_e		= 0;
    int left_stepx	= 0;
    int left_signdx = 0;
    int left_dy		= 0;
    int left_dx		= 0;

    int right_x      = 0;
    int right_e		 = 0;
    int right_stepx  = 0;
    int right_signdx = 0;
    int right_dy     = 0;
    int right_dx     = 0;

    int left_height = 0;
    int right_height = 0;

    if( ! _image )
        return;

    while( (left_count || left_height) && (right_count || right_height) )
    {
         int height;

        if (!left_height && left_count)
        { // Load fields from next left edge, right edge

            left_height = left->height();
            left_x		= left->x();
            left_stepx	= left->stepx();
            left_signdx = left->signdx();
            left_e		= left->e();
            left_dy		= left->dy();
            left_dx		= left->dx();

            --left_count;
            ++left;
        }

        // load fields from next right edge
        if (!right_height && right_count)
        {
            right_height = right->height();
            right_x		 = right->x();
            right_stepx  = right->stepx();
            right_signdx = right->signdx();
            right_e		 = right->e();
            right_dy	 = right->dy();
            right_dx     = right->dx();

            --right_count;
            ++right;
        }

        height = std::min(left_height, right_height);
        left_height -= height;
        right_height -= height;

        // walk down to end of left or right edge, whichever comes first
        while (height--)
        {
            // generate a span (omitting point on right end, see above)
            if (right_x >= left_x)
            {
                int xpos = std::max( left_x, 0 );
                const int endx = std::min<int>( right_x, _image->width() -1);
                stroke( xpos, y, endx - xpos + 1, currentClip );
            }

            y++;

            // Update left_x, right_x by stepping along left and right edges,
            // using midpoint line algorithm.
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


/* From two line faces, construct clipping edges that will be used by
   miLineArcD when drawing a pie wedge.  The line faces may be modified. */
void BitmapCanvas::roundJoinClip (LineFace *pLeft, LineFace *pRight, LineEdge *edge1, LineEdge *edge2, int *y1, int *y2, bool *left1, bool *left2)
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


/* From a line face, construct a clipping edge that will be used by
   miLineArcD when drawing a half-disk.  */
int BitmapCanvas::roundCapClip( const LineFace *face, bool isInt, LineEdge *edge, bool *leftEdge )
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
        left = false;
    }

    if( dx == 0 && dy == 0 )
        dy = 1;

    if( dy == 0 )
    {
        y = static_cast<int>( ceil( face->ya() ) + face->y() );
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
        edge->setHeight( std::numeric_limits< int>::max() );	/* number of scanlines to process */
    }

    *leftEdge = (left ? false : true);

    return y;
}


void BitmapCanvas::lineArc( LineFace *leftFace, LineFace *rightFace, double xorg, double yorg, bool isInt, const Rect& currentClip)
{
    std::vector<Point>    points;
    std::vector<int>   widths;

    int      xorgi = 0;
    int		 yorgi = 0;
    int 	 n;
    LineEdge edge1;
    LineEdge edge2;
    int		 edgey1;
    int		 edgey2;
    bool	 edgeleft1;
    bool	 edgeleft2;

    if (isInt)
    { //In integer case, take (xorgi,yorgi) from face; otherwise (0,0).
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

    if( (_pen.style() != Pen::Solid || _pen.size() > 2) && ((_pen.capStyle() == Pen::RoundCap && _pen.joinStyle() != Pen::RoundJoin)  ||
        ( _pen.joinStyle() == Pen::RoundJoin && _pen.capStyle() == Pen::FlatCap)))
    { // Construct clipping edges from the passed line faces (otherwise,
      // ignore them; will just draw a disk).

        if (isInt)
        {
            xorg = (double) xorgi;
            yorg = (double) yorgi;
        }

        if (leftFace && rightFace) // Have two faces, so construct clipping edges for pie wedge.
            roundJoinClip (leftFace, rightFace, &edge1, &edge2, &edgey1, &edgey2, &edgeleft1, &edgeleft2);

        else if (leftFace)// Will draw half-disk on left face, so construct clipping edge.
            edgey1 = roundCapClip( leftFace, isInt, &edge1, &edgeleft1 );

        else if (rightFace)
            /* will draw half-disk on right face, so construct clipping edge */
            edgey2 = roundCapClip (rightFace, isInt, &edge2, &edgeleft2);

        /* due to clipping, switch to using floating-point coordinates */
        isInt = false;
    }

    points.resize( _pen.size() );
    widths.resize( _pen.size() );

    // Construct a Spans by calling integer or floating point routine.
    if (isInt) // Integer routine, no clipping: just draw a disk.
        n = lineArcI(xorgi, yorgi, points, widths );
    else //Call floating point routine, supporting clipping by edge(s).
        n = lineArcD( xorg, yorg, points, widths, &edge1, edgey1, edgeleft1, &edge2, edgey2, edgeleft2);

    //Stroke the span.
    for( int i = 0; i < n; i++)
        stroke((int)points[i].x(), (int)points[i].y(), widths[i], currentClip );
}


int BitmapCanvas::lineArcI( int xorg, int yorg, std::vector<Point>& points, std::vector<int>& widths )
{
    Point *tpts, *bpts;
    int* twids, *bwids;
    int x, y, e, ex;
    int slw;

    tpts = &points[0];
    twids = &widths[0];
    slw = (int)(_pen.size());

    if (slw == 1) // 'disk' is a single pixel.
    {
        tpts->setX( xorg );
        tpts->setY( yorg );
        *twids = 1;
        return 1;
    }

    // Otherwise, draw the disk scanline by scanline.
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
    return (int)( _pen.size() );
}


/* Draw as a Spans a filled disk of diameter equal to the linewidth, paying
   attention to one or two clipping edges.  This is used for round caps and
   round joins, respectively (it respectively yields a half-disk or a pie
   wedge).  Floating point coordinates are used.  Returns number of spans
   in the Spans.  The clipping edges may be modified. */
int BitmapCanvas::lineArcD( double xorg, double yorg, std::vector<Point>& points, std::vector<int>& widths, LineEdge *edge1, int edgey1, bool edgeleft1, LineEdge *edge2, int edgey2, bool edgeleft2)
{
    Point *pts;
    int *wids;
    double radius, x0, y0, el, er, yk, xlk, xrk, k;
    int xbase, ybase, y, boty, xl, xr, xcl, xcr;
    int ymin, ymax;
    bool edge1IsMin, edge2IsMin;
    int ymin1, ymin2;

    pts = &points[0];
    wids = &widths[0];
    xbase = (int)(floor(xorg));
    x0 = xorg - xbase;
    ybase = static_cast<int>( ceil(yorg) );
    y0 = yorg - ybase;
    xlk = x0 + x0 + 1.0;
    xrk = x0 + x0 - 1.0;
    yk = y0 + y0 - 1.0;
    radius = 0.5 * ((double)_pen.size());
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

        clipStepEdge(ybase,xcl,xcr, edgey1, edge1,edgeleft1);
        clipStepEdge(ybase,xcl,xcr, edgey2, edge2,edgeleft2);

        if (xcr >= xcl)
        {
            pts->setX( xcl );
            pts->setY( ybase );
            pts++;
            *wids++ = ( int)(xcr - xcl + 1);
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

        clipStepEdge(ybase, xcl, xcr, edgey1, edge1, edgeleft1);
        clipStepEdge(ybase, xcl, xcr, edgey2, edge2, edgeleft2);

        if (xcr >= xcl)
        {
            pts->setX( xcl );
            pts->setY( ybase );
            pts++;
            *wids++ = ( int)(xcr - xcl + 1);
        }
    }

    /* return number of spans in the Spans */
    return (pts - &points[0]);
}


/* helper function called by the preceding */
int BitmapCanvas::roundJoinFace( const LineFace *face, LineEdge *edge, bool *leftEdge )
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
        y = static_cast<int>( ceil( face->ya() ) + face->y() );
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

        edge->setHeight( std::numeric_limits< int>::max() );	/* number of scanlines to process */
    }

    *leftEdge = (left ? false : true);

    return y;
}

/* Paint all types of line join: round/miter/bevel/triangular.  Called by
   both miWideLine() and miWideDash().  Left and right line faces are
   supplied, each with its own value of k.  They may be modified. */
void BitmapCanvas::lineJoin(  LineFace *pLeft, LineFace *pRight, const Rect& currentClip)
{
    double	            mx = 0.0, my = 0.0;
    int		            denom = 0;
    Point    vertices[4];
    LineSlope           slopes[4];
    int		            edgecount;
    LineEdge            left[4], right[4];
    int                 nleft, nright;
    int                 y;
    int        height;
    bool		        swapslopes;
    int		            lw = _pen.size();

    if( _pen.joinStyle() == Pen::RoundJoin )
    { // invoke miLineArc to fill the round join, isInt = true-
        lineArc( pLeft, pRight, (double)0.0, (double)0.0, true, currentClip );
        return;
    }

    denom = - pLeft->dx() * pRight->dy() + pRight->dx() * pLeft->dy();

    if( denom == 0 )
        return; // No join to draw.

    // Now must handle cases where line join is a small polygon to be filled;
    // specify its vertices clockwise.

    // Swap slopes if cross product of line faces has wrong sign.
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

    // vertex #0 is at the right end of the right face.
    vertices[0].setX( pRight->xa() );
    vertices[0].setY( pRight->ya() );
    slopes[0].setDX( - pRight->dy() );
    slopes[0].setDY( pRight->dx() );
    slopes[0].setK( 0 );

    // vertex #1 is the nominal join point (i.e. halfway across both the.
    // right face and the left face).
    vertices[1].setX( 0 );
    vertices[1].setY( 0 );
    slopes[1].setDX( pLeft->dy() );
    slopes[1].setDY( -pLeft->dx() );
    slopes[1].setK( 0 );

    // vertex #2 is at the left end of the left face.
    vertices[2].setX( pLeft->xa() );
    vertices[2].setY( pLeft->ya() );
    Pen::JoinStyle joinStyle =_pen.joinStyle();

    if(_pen.joinStyle() == Pen::MiterJoin )
    {
        double miterlimit = 10.43; // default = 10.43, as in X11.

        // compute vertex (mx,my) of miter quadrilateral
        my = (pLeft->dy()  * (pRight->xa() * pRight->dy() - pRight->ya() * pRight->dx()) -
             pRight->dy() * (pLeft->xa()  * pLeft->dy()  - pLeft->ya()  * pLeft->dx() )) / (double) denom;

        if (pLeft->dy() != 0)
            mx = pLeft->xa() + (my - pLeft->ya()) * (double) pLeft->dx() / (double) pLeft->dy();
        else
            mx = pRight->xa() + (my - pRight->ya()) *(double) pRight->dx() / (double) pRight->dy();

        // if miter limit violated, switch to bevelled join
        if ((mx * mx + my * my) * 4 > miterlimit * miterlimit * lw * lw)
            joinStyle = Pen::BevelJoin;
    }


    double scale, dx, dy, adx, ady;

    switch( joinStyle )
    {
        default: 

        case Pen::MiterJoin: // join by adding a quadrilateral
            edgecount = 4;
            slopes[2].setDX( pLeft->dx() );
            slopes[2].setDY( pLeft->dy() );
            slopes[2].setK( pLeft->k() );

            if( swapslopes )
            {
                slopes[2].setDX( -slopes[2].dx() );
                slopes[2].setDY( -slopes[2].dy() );
                slopes[2].setK( -slopes[2].k() );
            }

            /* vertex #3 is miter vertex (mx,my) */
            vertices[3].setX( mx );
            vertices[3].setY( my );
            slopes[3].setDX( pRight->dx() );
            slopes[3].setDY( pRight->dy() );
            slopes[3].setK( pRight->k() );

            if( swapslopes )
            {
                slopes[3].setDX( -slopes[3].dx() );
                slopes[3].setDY( -slopes[3].dy() );
                slopes[3].setK( -slopes[3].k() );
            }
            break;

        case Pen::BevelJoin: //join by adding a triangle
        {
            Point midpoint;
            edgecount = 3;

            // Third edge of triangle will pass through midpoint.
            midpoint.setX( 0.5 * (pLeft->xa() + pRight->xa() ) );
            midpoint.setY( 0.5 * (pLeft->ya() + pRight->ya() ) );

            // Vector along third edge of triangle.
            dx = pRight->xa() - pLeft->xa();
            dy = pRight->ya() - pLeft->ya();

            // Compute scale = max(|dx|,|dy|).
            adx = dx;
            ady = dy;

            if (adx < 0)
                adx = -adx;

            if (ady < 0)
                ady = -ady;

            scale = ady;

            if (adx > ady)
                scale = adx;

            // Use integer dx, dy in range -65536..65536.
            slopes[2].setDX(  (int)((dx * 65536) / scale) );
            slopes[2].setDY( (int)((dy * 65536) / scale) );
            slopes[2].setK( midpoint.x() * slopes[2].dy() - midpoint.y() * slopes[2].dx() );
            break;
        }

        /*
        case Pen::TriangularJoin: // join by adding a stubby quadrilateral
        {
            Point midpoint, newpoint;
            double mid2, mid, dx2, dy2, dx3, dy3;

            edgecount = 4;

            // compute additional vertex, offset by linewidth/2
            midpoint.setX( 0.5 * (pLeft->xa() + pRight->xa()) );
            midpoint.setY( 0.5 * (pLeft->ya() + pRight->ya()) );
            mid2 = midpoint.x() * midpoint.x() + midpoint.y() * midpoint.y();
            mid = sqrt (mid2);
            newpoint.setX( 0.5 * lw * midpoint.x() / mid );
            newpoint.setY( 0.5 * lw * midpoint.y() / mid );
            vertices[3] = newpoint;

            // offset from vertices[2] to vertices[3]
            dx2 = vertices[3].x() - vertices[2].x();
            dy2 = vertices[3].y() - vertices[2].y();

            // offset from vertices[3] back to vertices[0]
            dx3 = vertices[0].x() - vertices[3].x();
            dy3 = vertices[0].y() - vertices[3].y();

            // compute scale = max(|dx|,|dy|), where (dx,dy) is offset between
            // the two corners, i.e. vertices[0] and vertices[2]
            dx = pRight->xa() - pLeft->xa();
            dy = pRight->ya() - pLeft->ya();
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
            slopes[2].setDX( (int)((dx2 * 65536) / scale) );
            slopes[2].setDY( (int)((dy2 * 65536) / scale) );
            slopes[2].setK( newpoint.x() * slopes[2].dy() - newpoint.y() * slopes[2].dx() );

            // use integer dx, dy in range -65536..65536
            slopes[3].setDX( (int)((dx3 * 65536) / scale) );
            slopes[3].setDY( (int)((dy3 * 65536) / scale) );
            slopes[3].setK(  newpoint.x() * slopes[3].dy() - newpoint.y() * slopes[3].dx() );
            break;
        }
        */
    }

    // Compute lists of left and right edges for the small polygon, using the
    // just-computed slopes array.
    y = polyBuildPoly( vertices, slopes, edgecount, pLeft->x(), pLeft->y(), left, right, &nleft, &nright, &height);

    // Fill the small polygon.
    fillLine( y, height, left, right, nleft, nright, currentClip);
}


/* Paint a projecting rectangular cap on a line face.  Called only by
   miWideDash (with isInt = true); not by miWideLine. */
void BitmapCanvas::lineProjectingCap( const LineFace *face, bool isLeft, bool isInt, const Rect& currentClip)
{
    int		    xorgi = 0, yorgi = 0;
    int	       	lw;
    LineEdge	lefts[2], rights[2];
    int		    lefty, righty, topy, bottomy;
    LineEdge    *left, *right;
    LineEdge    *top, *bottom;
    double	    xa,ya;
    double	    k;
    double	    xap, yap;
    int		    dx, dy;
    double	    projectXoff, projectYoff;
    double	    maxy;
    int		    finaly;

    // in integer case, take (xorgi,yorgi) from face; otherwise (0,0)
    if (isInt)
    {
        xorgi = face->x();
        yorgi = face->y();
    }

    lw = (int)(_pen.size() );
    dx = face->dx();
    dy = face->dy();
    k = face->k();

    // special case: line face is horizontal
    if( dy == 0 )
    {
        lefts[0].setHeight( ( int)lw );
        lefts[0].setX( xorgi );

        if( isLeft )
            lefts[0].setX( lefts[0].x()  - (lw >> 1) );

        lefts[0].setStepX( 0 );
        lefts[0].setSignDX( 1 );
        lefts[0].setE( -lw );
        lefts[0].setDX( 0 );
        lefts[0].setDY( lw );

        rights[0].setHeight( ( int)lw );
        rights[0].setX( xorgi );

        if( !isLeft )
            rights[0].setX( rights[0].x() + ((lw + 1) >> 1) );

        rights[0].setStepX( 0 );
        rights[0].setSignDX( 1 );
        rights[0].setE( -lw );
        rights[0].setDX( 0 );
        rights[0].setDY( lw );

        // fill the rectangle (1 left edge, 1 right edge)
        fillLine( yorgi - (lw >> 1), ( int)lw,  lefts, rights, 1, 1, currentClip);
    }
    else if( dx == 0 ) // special case: line face is vertical
    {
        topy = yorgi;
        bottomy = yorgi + dy;

        if (isLeft)
            topy -= (lw >> 1);
        else
            bottomy += (lw >> 1);

        lefts[0].setHeight( ( int)(bottomy - topy) );
        lefts[0].setX( xorgi - (lw >> 1) );
        lefts[0].setStepX( 0 );
        lefts[0].setSignDX( 1 );
        lefts[0].setE( -dy );
        lefts[0].setDX( dx );
        lefts[0].setDY( dy );

        rights[0].setHeight( ( int)(bottomy - topy) );
        rights[0].setX( lefts[0].x() + (lw - 1) );
        rights[0].setStepX( 0 );
        rights[0].setSignDX( 1 );
        rights[0].setE( -dy );
        rights[0].setDX(  dx );
        rights[0].setDY( dy );

        // fill the rectangle (1 left edge, 1 right edge)
        fillLine( topy, ( int)(bottomy - topy), lefts, rights, 1, 1, currentClip);
    }
    else // general case: line face is neither horizontal nor vertical
    {
        xa = face->xa();
        ya = face->ya();
        projectXoff = -ya;
        projectYoff = xa;

        if( dx < 0 )
        {
            right   = &rights[1];
            left    = &lefts[0];
            top     = &rights[0];
            bottom  = &lefts[1];
        }
        else
        {
            right   = &rights[0];
            left    = &lefts[1];
            top     = &lefts[0];
            bottom  = &rights[1];
        }

        if( isLeft ) // Cap goes left; build four edges.
        {
            righty = buildLineEdge( xa, ya, k, dx, dy,  xorgi, yorgi, false, right );
            xa = -xa;
            ya = -ya;
            k = -k;

            lefty = buildLineEdge (xa - projectXoff, ya - projectYoff, k, dx, dy, xorgi, yorgi, true, left);

            if( dx > 0 )
            {
                ya = -ya;
                xa = -xa;
            }

            xap = xa - projectXoff;
            yap = ya - projectYoff;
            topy = buildLineEdge( xap, yap,  xap * dx + yap * dy, -dy, dx, xorgi, yorgi, (dx > 0 ? true : false), top );
            bottomy = buildLineEdge( xa, ya, 0.0, -dy, dx,  xorgi, yorgi, (dx < 0 ? true : false), bottom );
            maxy = -ya;
        }
        else  // cap goes right; build four edges
        {
            righty = buildLineEdge( xa - projectXoff, ya - projectYoff, k, dx, dy,  xorgi, yorgi, false, right );

            xa = -xa;
            ya = -ya;
            k = -k;

            lefty = buildLineEdge( xa, ya, k, dx, dy,  xorgi, yorgi, true, left );

            if( dx > 0 )
            {
                ya = -ya;
                xa = -xa;
            }

            xap = xa - projectXoff;
            yap = ya - projectYoff;

              topy = buildLineEdge( xa, ya, 0.0, -dy, dx, xorgi, xorgi, (dx > 0 ? true : false), top);
              bottomy = buildLineEdge( xap, yap,  xap * dx + yap * dy, -dy, dx, xorgi, xorgi, (dx < 0 ? true : false), bottom );
              maxy = -ya + projectYoff;
          }

        finaly = static_cast<int>( ceil(maxy) + yorgi );

        if (dx < 0)
        {
            left->setHeight( ( int)(bottomy - lefty) );
            right->setHeight( ( int)(finaly - righty) );
            top->setHeight( ( int)(righty - topy) );
        }
        else
        {
            right->setHeight( ( int)(bottomy - righty) );
            left->setHeight( ( int)(finaly - lefty) );
            top->setHeight( ( int)(lefty - topy) );
        }
            bottom->setHeight( ( int)(finaly - bottomy) );

        // fill the rectangle (2 left edges, 2 right edges)
        fillLine( topy, ( int)(bottom->height() + bottomy - topy), lefts, rights, 2, 2, currentClip );
    }
}


void BitmapCanvas::clipStepEdge( int ybase, int& xcl, int& xcr, int& edgey,  LineEdge* edge, bool edgeleft )
{
  if (ybase != edgey)
      return;

  if (edgeleft)
  {
      if (edge->x() > xcl)
          xcl = edge->x();
  }
  else
  {
      if (edge->x() < xcr)
          xcr = edge->x();
  }

  edgey++;
  edge->setX( edge->x() + edge->stepx() );
  edge->setE( edge->e() + edge->dx());

  if (edge->e() > 0)
  {
      edge->setX( edge->x() + edge->signdx() );
      edge->setE( edge->e() - edge->dy() );
  }
}


void BitmapCanvas::drawWideSolidPolyline( const  Point* pPts, int npt, const Rect& currentClip)
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

    x2 = (int)pPts->x();
    y2 = (int) pPts->y();
    first = true;

    // Determine whether polyline is closed.
    selfJoin = false;

    if( npt > 1 )
        if (x2 == pPts[npt-1].x() && y2 == pPts[npt-1].y())
            selfJoin = true;

    // Line segments (except for the last) will not project right; they'll
    // project left if the cap mode is "projecting".
    projectLeft = (_pen.capStyle() == Pen::SquareCap && ! selfJoin);
    projectRight = false;

    // Iterate through points, drawing all line segments of nonzero length.
    while( --npt )
    {
        x1 = x2;
        y1 = y2;

        ++pPts;

        x2 = (int)pPts->x();
        y2 = (int)pPts->y();

        if (x1 != x2 || y1 != y2)
        {
            somethingDrawn = true;

            if (npt == 1 && _pen.capStyle() == Pen::SquareCap && ! selfJoin) // last point; and need a projecting cap here
                projectRight = true;

            // Draw segment (pixel=1), returning faces.
            drawSegment(  Point(x1, y1), Point(x2, y2), projectLeft, projectRight, &leftFace, &rightFace, currentClip );

            if (first)
            { //First line segment, draw round cap if needed.
                if (selfJoin)
                {
                    firstFace = leftFace;
                }
                else if (_pen.capStyle() == Pen::RoundCap /*|| _pen.capStyle() == Pen::TriangularCap*/ )
                {
                    // Invoke miLineArc, isInt = true, to draw a round cap on left face in paint type #1.
                    lineArc(  &leftFace, 0,(double)0.0, (double)0.0, true, currentClip );
                }
            }
            else
            {
                // General case: draw join at beginning of segment (pixel=1).
                lineJoin(  &leftFace, &prevRightFace, currentClip );
            }

            prevRightFace = rightFace;
            first = false;
            projectLeft = false;
        }

        // Final point of polyline.
        if (npt == 1 && somethingDrawn)
        {
            if (selfJoin) // Add line join to close the polyline, pixel=1.
                lineJoin(  &firstFace, &rightFace, currentClip);

            else if ( _pen.capStyle() == Pen::RoundCap /*|| _pen.capStyle() == Pen::TriangularCap*/ )
                // Invoke miLineArc, isInt = true, to draw round cap on right face, pixel=1.
                lineArc( 0, &rightFace, (double)0.0, (double)0.0, true, currentClip );
        }
    }

    // handle crock where all points are coincident
    if( !somethingDrawn )
    {
        projectLeft = (_pen.capStyle() == Pen::SquareCap );

        drawSegment(  Point(x2, y2), Point(x2, y2), projectLeft, projectRight, &leftFace, &rightFace, currentClip );

        if( _pen.capStyle() == Pen::RoundCap /*|| _pen.capStyle() == Pen::TriangularCap*/ )
        {
            // invoke miLineArc, isInt = true, to draw round cap in paint type #1
            lineArc( &leftFace, (LineFace *)NULL, (double)0.0, (double)0.0, true, currentClip );

            // invoke miLineArc, isInt = true, to draw other round cap in paint type #1
            rightFace.setDX( -1);	// sleazy hack to make it work
            lineArc( (LineFace *) NULL, &rightFace, (double)0.0, (double)0.0, true, currentClip );
        }
    }
}


void BitmapCanvas::drawSegment( Point from, Point to, bool projectLeft, bool projectRight, LineFace* leftFace, LineFace* rightFace, const Rect& currentClip)
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
    int		 lw = _pen.size();

    if (to.y() < from.y() || (to.y() == from.y() && to.x() < from.x()))
    {	// Interchange, so as always to draw top-to-bottom, or left-to-right if horizontal.
        int tx, ty;
        bool tbool;
        LineFace *tface;

        tx = (int)from.x();
        from.setX( to.x());
        to.setX(tx);

        ty = (int)from.y();
        from.setY(to.y());
        to.setY(ty);

        tbool = projectLeft;
        projectLeft = projectRight;
        projectRight = tbool;

        tface = leftFace;
        leftFace = rightFace;
        rightFace = tface;
    }

    dy = (int)(to.y() - from.y());
    signdx = 1;
    dx = (int)(to.x() - from.x());

    if (dx < 0)
        signdx = -1;

    leftFace->setX((int)from.x());
    leftFace->setY((int)from.y());
    leftFace->setDX(dx);
    leftFace->setDY(dy);

    rightFace->setX((int)to.x());
    rightFace->setY((int)to.y());
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

        x = (int)from.x();

        if (projectLeft)
            x -= (lw >> 1);

        y = (int)from.y() - (lw >> 1);

        dx = (int)to.x() - x;

        if (projectRight)
            dx += ((lw + 1) >> 1);

        dy = lw;

        fillSpans( x, y,( int)dx, ( int)dy, currentClip );
    }
    else if (dx == 0) // Segment is vertical.
    {
        leftFace->setXA(0.5 * (double)lw );
        leftFace->setYA(0);
        leftFace->setK( 0.5 * (double)(lw * dy) ); // k = xa * dy - ya * dx
        rightFace->setXA( -leftFace->xa());
        rightFace->setYA( 0 );
        rightFace->setK( leftFace->k()); // k = xa * dy - ya * dx
        y = (int)from.y();

        if (projectLeft)
        y -= lw >> 1;

        x = (int)from.x() - (lw >> 1);
        dy = (int)to.y() - y;

        if (projectRight)
        dy += ((lw + 1) >> 1);

        dx = lw;

        fillSpans(  x, y, dx, dy, currentClip );
    }
    else
    { // General case: segment is neither horizontal nor vertical.
        l = 0.5 * ((double) lw);
        L = Pt::hypot((double) dx, (double) dy);

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
            righty = buildLineEdge( xa - projectXoff, ya - projectYoff, k, dx, dy, (int)from.x(), (int)from.y(), false, right);
        else
            righty = buildLineEdge( xa, ya, k, dx, dy, (int)from.x(), (int)from.y(), false, right);

          // Build second long edge.
          ya = -ya;
          xa = -xa;
          k = -k; // xa * dy - ya * dx

        if (projectLeft)
            lefty = buildLineEdge(xa - projectXoff, ya - projectYoff, k, dx, dy, (int)from.x(), (int)from.y(), true, left);
        else
            lefty = buildLineEdge(xa, ya, k, dx, dy, (int)from.x(), (int)from.y(), true, left);

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
            topy = buildLineEdge( xap, yap, xap * dx + yap * dy, -dy, dx, (int)from.x(), (int)from.y(), dx > 0 , top );
        }
        else
        {
            topy = buildLineEdge( xa, ya, 0.0, -dy, dx,(int) from.x(), (int)from.y(), dx > 0, top );
        }

        // Build second short edge, on right end.
        if (projectRight)
        {
            double xap = xa + projectXoff;
            double yap = ya + projectYoff;
            bottomy = buildLineEdge( xap, yap, xap * dx + yap * dy, -dy, dx, (int)to.x(), (int)to.y(),dx < 0, bottom );
            maxy = -ya + projectYoff;
        }
        else
        {
            bottomy = buildLineEdge( xa, ya, 0.0, -dy, dx,(int) to.x(), (int)to.y(), (dx < 0 ? true : false), bottom );
            maxy = -ya;
        }

        finaly = static_cast<int>(std::ceil(maxy)) + (int)to.y();

        if (dx < 0)
        {
            left->setHeight( ( int)(bottomy - lefty));
            right->setHeight( ( int)(finaly - righty) );
            top->setHeight( ( int)(righty - topy) );
        }
        else
        {
            right->setHeight( ( int)(bottomy - righty));
            left->setHeight( ( int)(finaly - lefty) );
            top->setHeight( ( int)(lefty - topy) );
        }

        bottom->setHeight( ( int)(finaly - bottomy) );

        // Fill the rectangle (2 left edges, 2 right edges).
        fillLine( topy, ( int)(bottom->height() + bottomy - topy), lefts, rights, 2, 2, currentClip );
    }
}


void BitmapCanvas::drawWideDashPolyline( const Point* pPts, int npt,
                                         int dashOn, int dashOff, const Rect& currentClip)
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
     int dashes[2];
    dashes[0] = dashOn; // Length of `on' dashes.
    dashes[1] = dashOff;	// Length of `off' dashes.

    // Ensure we have >=1 points
    if( npt <= 0 )
        return;

    x2 = (int)pPts->x();
    y2 = (int)pPts->y();
    first = true;	// first line segment of polyline

    /* determine whether polyline is closed */
    selfJoin = false;

    if( x2 == pPts[npt-1].x() && y2 == pPts[npt-1].y() )
        selfJoin = true;

    // Dash segments (except for the last) will not project right; and
    // (except for the first) will not project left
    projectLeft  = (_pen.capStyle() == Pen::SquareCap) && ! selfJoin;
    projectRight = false;

    // perform initial offsetting into the dash sequence
    dashNum     = 0; // absolute number of dash
    dashIndex   = 0; // index into dash array
    dashOffset  = 0; // index into selected dash

    stepDash( 0, &dashNum, &dashIndex, dashes, 2, &dashOffset );

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

        x2 = (int)pPts->x();
        y2 =(int) pPts->y();

        // Have a line segment of nonzero length.
        if( x1 != x2 || y1 != y2 )
        {
            int lastPaintedDashNum;

            // Final point; and need a projecting cap here.
            if( npt == 1 && _pen.capStyle() == Pen::SquareCap  && ( ! selfJoin || (firstPaintType == 0) ))
                projectRight = true;

            // Draw dashed segment, updating dashNum, dashIndex and dashOffset, returning faces
            dashSegment( &dashNum, &dashIndex, &dashOffset, x1, y1, x2, y2, projectLeft, projectRight, &leftFace, &rightFace, dashes, currentClip);

            // Determine paint types used at start and end of just-drawn segment
            startPaintType = ((dashNum & 1) ? 0 : 1 + ((dashNum / 2) % (numPixels - 1)));

            lastPaintedDashNum = (dashOffset != 0 ? dashNum : dashNum - 1);

            endPaintType = ((lastPaintedDashNum & 1) ? 0 : 1 + ((dashNum / 2) % (numPixels - 1)));

            // Add round cap or line join at left end of just-drawn segment;
            // if DashStyle, do so only if segment began with an `on' dash
            if( /*_pen.style() == Pen::DoubleDash || */ (startPaintType != 0) )
            {
                // Draw cap at left end, unless this is first segment of a closed polyline
                if( first || (_pen.style() == Pen::Dash && prevEndPaintType == 0 ) )
                {
                    if( first && selfJoin )
                    {
                        firstFace = leftFace;
                        firstPaintType = startPaintType;
                    }
                    else if( _pen.capStyle() == Pen::RoundCap /* || _pen.capStyle() == Pen::TriangularCap */ )
                    {
                        lineArc( &leftFace, (LineFace *)NULL, (double)0.0, (double)0.0, true, currentClip);
                    }
                }
                else
                {
                    // Draw join at left end.
                    lineJoin( &leftFace, &prevRightFace, currentClip );
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
            if( /*_pen.style() == Pen::DoubleDash ||*/ (endPaintType != 0) )
            {
                // closed, so draw a join
                if (selfJoin && (/*_pen.style() == Pen::DoubleDash ||*/ (firstPaintType != 0)) )
                {
                    lineJoin( &firstFace, &rightFace, currentClip );
                }
                else
                {
                    if( _pen.capStyle() == Pen::RoundCap /*|| _pen.capStyle() == Pen::TriangularCap*/ )
                        lineArc( (LineFace *)NULL, &rightFace, (double)0.0, (double)0.0, true, currentClip );
                }
            }
            else // we're doing OnOffDash, and final segment of polyline ended with an (undrawn) `off' dash
            {
                if( selfJoin && (firstPaintType != 0 ) )  // closed; if projecting or round caps are being used, draw one on the first face
                {
                    if( _pen.capStyle() == Pen::SquareCap)
                        lineProjectingCap( &firstFace, true, true, currentClip);
                    else if (_pen.capStyle() == Pen::RoundCap /*|| _pen.capStyle() == Pen::TriangularCap*/ )
                        lineArc( &firstFace, (LineFace *)NULL, (double)0.0, (double)0.0, true, currentClip);
                }
            }
        }
    }

    // Handle `all points coincident' crock, nothing yet drawn
    if( ! somethingDrawn && (/*_pen.style() == Pen::DoubleDash ||*/ !(dashNum & 1)) )
    {
         int w1;

        switch( _pen.capStyle() )
        {
            //case Pen::TriangularCap:
            case Pen::RoundCap:
                lineArc( (LineFace *)NULL, (LineFace *)NULL, (double)x2, (double)y2, false, currentClip );
                break;

            case Pen::SquareCap: // Draw a square box with edge size equal to line width
                w1 = _pen.size();
                fillSpans( (int)(x2 - (w1 >> 1)), (int)(y2 - (w1 >> 1)), w1, w1, currentClip);
                break;

            case Pen::FlatCap:
            default:
                break;
        }
    }
}


void BitmapCanvas::dashSegment( int *pDashNum, int *pDashIndex, int *pDashOffset, int x1, int y1, int x2, int y2, 
                                 bool projectLeft, bool projectRight, LineFace *leftFace, LineFace *rightFace,  int* dash, const Rect& currentClip)
{
  int		            dashNum, dashIndex, dashRemain;
    double	            L, l;
    double	            k;
    Point	vertices[4];
    Point    saveRight, saveBottom;
    LineSlope	        slopes[4];
    LineEdge	        left[2], right[2];
    LineFace	        lcapFace, rcapFace;
    int		            nleft, nright;
     int	    h;
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
    l = 0.5 * ((double) _pen.size() );

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
    if( _pen.capStyle() == Pen::RoundCap /*|| _pen.capStyle() == Pen::TriangularCap*/ )
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
        if( /*_pen.style() == Pen::DoubleDash ||*/ ! (paintType == 0) )
        {
            if( _pen.style() == Pen::Dash && _pen.capStyle() == Pen::SquareCap )
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
            fillLine( y, h, left, right, nleft, nright, currentClip);

            // If doing DashStyle, add caps if any
            if( _pen.style() == Pen::Dash )
            {
                switch( _pen.capStyle() )
                {
                    case Pen::FlatCap:
                    default:
                    break;
                    // Use saved vertices
                    case Pen::SquareCap:
                        vertices[V_BOTTOM] = saveBottom;
                        vertices[V_RIGHT]  = saveRight;
                        slopes[V_RIGHT].setK( saveK );
                    break;

                    //case Pen::TriangularCap:
                    case Pen::RoundCap:
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
                            lineArc( &lcapFace, (LineFace *) NULL, lcenterx, lcentery, false, currentClip);
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
                        lineArc((LineFace *)NULL, &rcapFace, rcenterx, rcentery, false, currentClip);
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

        dashRemain = dash[dashIndex]; // whole new dash now `remains'

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
    if( /*_pen.style() == Pen::DoubleDash ||*/ ! (paintType == 0) )
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
        if( !first && ( _pen.style() == Pen::Dash)  && (_pen.capStyle() == Pen::SquareCap ) )
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
        fillLine( y, h, left, right, nleft, nright, currentClip);

        // If DashStyle line style and cap mode is round, draw a round cap
        if( ( ! first && (_pen.style() == Pen::Dash) && (_pen.capStyle() == Pen::RoundCap) ) 
            /*|| (_pen.capStyle() == Pen::TriangularCap)*/ )
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
            lineArc( &lcapFace, (LineFace *) 0, rcenterx, rcentery, false, currentClip);
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


void BitmapCanvas::putImage( const Point& to, const Image& img)
{
    Rect imageRect;
    imageRect.setWidth( img.width() );
    imageRect.setHeight( img.height() );

    putImage(to, img, imageRect);
}


//void BitmapCanvas::putImage(const Point& to, const Image& from, const Rect& fromRect)
//{
//  // clip fromRect to fit into the clip/image rect
//  Point d = _currentClip.topLeft() - to;
//  Point fromPos = fromRect.topLeft() + d;
//
//  Rect fromClip( fromPos, _currentClip.size() );
//  fromClip = fromRect.intersect(fromClip);
//
//  if( fromClip.isNull() )
//      return;
//
//  // account for smaller fromRect
//  Point toClip = to + (fromClip.topLeft() - fromRect.topLeft());
//
//  _image.format().copy(_image.view(), toClip, from.view(), fromClip, _compositionMode);
//}


void BitmapCanvas::putImage(const Point& to, const Image& image, const Rect& imageRect)
{
    if( ! _image )
        return;

    // clip against source boundaries
    Rect fromRect( image.width(), image.height() );
    fromRect = fromRect.intersect(imageRect);

    // update target position if rect got smaller
    Point toPos = to;
    toPos += fromRect.topLeft() - imageRect.topLeft();

    // clip against target boundaries
    Rect toRect = Rect( toPos, fromRect.size() );
    toRect = toRect.intersect(_currentClip);

    // update source position if rect got smaller
    Point fromPos = fromRect.topLeft();
    fromPos += toRect.topLeft() - toPos;
    fromRect.setOrigin(fromPos);

    // update source size if rect got smaller
    fromRect.setSize( toRect.size() );

    Gfx::PixelView::Iterator toIter = _imageView.pixel( toRect.x(), toRect.y() );

    Gfx::ConstPixelView fromView(image);
    Gfx::ConstPixelView::Iterator fromIter = fromView.pixel( fromRect.x(), fromRect.y() );

    switch(_compositionMode)
    {
        default:
        case CompositionMode::SourceCopy:
            Argb32::sourceCopy(toIter->base(), _imageView.stride(),
                               fromIter->base(), fromView.stride(), 
                               fromRect.width(), fromRect.height());

            //Argb32::sourceCopy(toView.base(), toRect.x(), toRect.y(),
            //                   fromView, fromRect.x(), fromRect.y(), 
            //                   fromRect.width(), fromRect.height());
            break;

        case CompositionMode::SourceOver:
            Argb32::sourceOver(toIter->base(), _imageView.stride(),
                               fromIter->base(), fromView.stride(), 
                               fromRect.width(), fromRect.height());

            //Argb32::sourceOver(toView.base(), toRect.x(), toRect.y(),
            //                   fromView, fromRect.x(), fromRect.y(), 
            //                   fromRect.width(), fromRect.height());
            break;
    }
}

} // namespace

} // namespace
