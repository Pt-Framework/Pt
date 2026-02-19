/* Copyright (C) 20234 Marc Boris Duerner

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
#include "DrawText.h"
#include "Dasher.h"

#include <Pt/Gfx/Argb32.h>
#include <Pt/Gfx/Image.h>
#include <Pt/Gfx/Bitmap.h>

#include <vector>
#include <cmath>
#include <algorithm>

namespace Pt {

namespace Gfx {

BitmapCanvas::BitmapCanvas()
: Canvas()
, _image()
, _text( new DrawText() )
, _hasClip(false)
{
}


BitmapCanvas::~BitmapCanvas()
{
    delete _text;
}


void BitmapCanvas::init(BLContext& rasterContext, Image& image)
{
    _context = &rasterContext;
    _image = &image;
    _imageView.reset(*_image);
}


void BitmapCanvas::onBeginPaint(const Gfx::Paint& paint)
{
    if( ! _context )
        return;
    
    Transform tx = transform();
    BLMatrix2D m( tx.m11(), tx.m12(),
                  tx.m21(), tx.m22(), 
                  tx.dx(), tx.dy() );

    _context->reset_transform();
    _context->set_transform(m);
}


void BitmapCanvas::onFinishPaint()
{
    if(_context)
        _context = 0;

    if(_image)
        _image = 0;
}


void BitmapCanvas::onSetCompositionMode(const Gfx::CompositionMode& mode) 
{
    _compositionMode = mode;
}


void BitmapCanvas::onApplyCompositionMode(const Gfx::CompositionMode& mode)
{
    BLCompOp compOp = BL_COMP_OP_SRC_OVER;
    
    if(mode == CompositionMode::SourceOver)
    {
        compOp = BL_COMP_OP_SRC_OVER;
    }
    else // CompositionMode::SourceCopy
    {
        compOp = BL_COMP_OP_SRC_COPY;
    }

    _context->set_comp_op(compOp);
}


void BitmapCanvas::onSetPen(const Gfx::Pen& pen)
{
    _pen = pen;

    _dashPattern.clear();

    if(_pen.style() == Gfx::Pen::Dash)
    {
          if(_pen.capStyle() == Gfx::Pen::RoundCap ||
              _pen.capStyle() == Gfx::Pen::SquareCap)
          {
              _dashPattern.push_back( 2.0 * _pen.size() );
              _dashPattern.push_back( 2.0 * _pen.size() );
          }
          else
          {
              _dashPattern.push_back( 3.0 * _pen.size() );
              _dashPattern.push_back( 1.0 * _pen.size() );
          }
    }
    else if( _pen.style() == Gfx::Pen::Dot )
    {
          if(_pen.capStyle() == Gfx::Pen::RoundCap ||
              _pen.capStyle() == Gfx::Pen::SquareCap)
          {
              _dashPattern.push_back( 1.0 );
              _dashPattern.push_back( 2.0 * _pen.size() );
          }
          else
          {
              _dashPattern.push_back( 1.0 * _pen.size() );
              _dashPattern.push_back( 1.0 * _pen.size() );
          }
    }
}


void BitmapCanvas::onApplyPen(const Gfx::Pen& pen)
{
    _text->setPen(pen);

    _context->set_stroke_width( static_cast<double>( pen.size() ) );

    Pt::Gfx::Color penColor = pen.color();
    BLRgba32 strokecolor(penColor.red() / 257, 
                         penColor.green() / 257, 
                         penColor.blue()  / 257, 
                         penColor.alpha() / 257);

    _context->set_stroke_style(strokecolor);
}


void BitmapCanvas::onSetBrush(const Gfx::Brush& brush)
{
}


void BitmapCanvas::onApplyBrush(const Gfx::Brush& brush)
{
    Pt::Gfx::Color brushColor = brush.color();
    BLRgba32 fillColor(brushColor.red() / 257, 
                       brushColor.green() / 257, 
                       brushColor.blue()  / 257, 
                       brushColor.alpha() / 257);

    switch( brush.fillStyle() ) 
    {
        case Gfx::Brush::Solid: 
        {
            _context->set_fill_style(fillColor);
            break;
        }

        case Gfx::Brush::Texture: 
        {
            break;     
        }
                
        case Gfx::Brush::Gradient:
        {
            break;
        }
                
        default:
            break;
    }
}


void BitmapCanvas::onSetFont(const Gfx::Font& font)
{
}


void BitmapCanvas::onApplyFont(const Gfx::Font& font)
{
    _text->setFont(font);
}



void BitmapCanvas::onSetClip(const Gfx::RectF* clip)
{
}


void BitmapCanvas::onApplyClip(const Gfx::RectF* clip) 
{
    if( ! _image )
        return;

    _hasClip = clip != 0;

    if( ! clip )
        _context->restore_clipping();
    else
        _context->clip_to_rect( clip->x(), clip->y(), 
                               clip->width(), clip->height() );

    if(clip)
    {
        Gfx::PointF origin =  transform() * clip->origin();
        Gfx::SizeF size =  transform() * clip->size();
        Gfx::RectF clipP(origin, size);
        
        _clip = clipP;
    }
    else
        _clip.clear();

    Rect imageRect;
    imageRect.setWidth( _image->width() );
    imageRect.setHeight( _image->height() );

    if( ! _hasClip )
    {
        _currentClip = imageRect;
        return;
    }

    Rect clipRect = Rect( Point( lround( _clip.x() ),
                                 lround( _clip.y() ) ), 
                          Size( lround( _clip.width() ),
                                lround( _clip.height() ) ) );

    if( clipRect.isNull() ) // crashes otherwise
        clipRect = Rect( Point(0, 0), Size(1, 1) );

    _currentClip =  clipRect.intersect(imageRect);
}


void BitmapCanvas::onDrawLine(const Gfx::PointF& from, const Gfx::PointF& to)
{
    if( _pen.style() == Gfx::Pen::Dash || _pen.style() == Gfx::Pen::Dot )
    {
        Gfx::PointF pts[2];
        pts[0] = from;
        pts[1] = to;
        drawDashed(pts, 2);
        return;
    }

    _context->stroke_line( from.x(), from.y(), to.x(), to.y() );
}


void BitmapCanvas::onDrawPolyline(const Gfx::PointF* pts, const size_t n)
{
    if(n == 0)
        return;

    switch( _pen.style() )
    {
        default:
        case Gfx::Pen::Solid:
            drawSolid(pts, n);
            break;

        case Gfx::Pen::Dash:
        case Gfx::Pen::Dot:
            drawDashed(pts, n);
            break;
    }
}


void BitmapCanvas::drawDashed(const Gfx::PointF* pts, const size_t n)
{
    Dasher dasher(_dashPattern);
    dasher.push(pts, n);
    dasher.finish();
    
    const std::vector<Polygon>& dashes = dasher.getDashes();
    for(const Polygon& dash : dashes)
        drawSolid( dash.points(), dash.size() );
}


void BitmapCanvas::drawSolid(const Gfx::PointF* pts, const size_t n)
{
    _points.resize(n);

    for(unsigned i = 0; i < n; i++)
    {
        const Gfx::PointF& p = pts[i];
        _points[i] = BLPoint( p.x(), p.y() );
    }
    
    _context->stroke_polyline( _points.data(), _points.size() );
}


void BitmapCanvas::onFillPolygon(const Gfx::PointF* pts, const size_t n)
{
    if(n == 0)
        return;

    _points.resize(n);

    for(unsigned i = 0; i < n; i++)
    {
        const Gfx::PointF& p = pts[i];
        _points[i] = BLPoint( p.x(), p.y() );
    }
    
    _context->fill_polygon( _points.data(), _points.size() );
}


void BitmapCanvas::onDrawRect(const Gfx::RectF& r)
{
    if( _pen.style() ==  Gfx::Pen::Dash ||
        _pen.style() == Gfx::Pen::Dot )
    {
        Path rectPath;
        rectPath.moveTo( r.topLeft() );
        rectPath.addRect( r.size() );
        drawDashed(rectPath);
        return;
    }

    _context->stroke_rect( r.x(), r.y(), r.width(),r.height() );
}


void BitmapCanvas::onFillRect(const Gfx::RectF& r)
{
     _context->fill_rect( r.x(), r.y(), r.width(),r.height() );
}


void BitmapCanvas::onDrawEllipse(const PointF& topLeft, const SizeF& size)
{
    if( _pen.style() == Gfx::Pen::Dash || _pen.style() == Gfx::Pen::Dot )
    {
        Path ellipsePath;
        ellipsePath.moveTo(topLeft);
        ellipsePath.addEllipse(size);
        drawDashed(ellipsePath);
        return;
    }

    double radiusX = size.width() / 2.0;
    double radiusY = size.height() / 2.0;
    double centerX = topLeft.x() + radiusX;
    double centerY = topLeft.y() + radiusY;

    _context->stroke_ellipse( centerX, centerY, radiusX, radiusY );
}


void BitmapCanvas::onFillEllipse(const PointF& topLeft, const SizeF& size)
{
    double radiusX = size.width() / 2.0;
    double radiusY = size.height() / 2.0;
    double centerX = topLeft.x() + radiusX;
    double centerY = topLeft.y() + radiusY;

    _context->fill_ellipse( centerX, centerY, radiusX, radiusY );
}


void BitmapCanvas::onSetPath(const Gfx::Path& path)
{
    _ptPath = path;
    _blPath.clear();
    addPath(_blPath, path);
}


void BitmapCanvas::addPath(BLPath& path, const Gfx::Path& other)
{
    for(Gfx::PathIterator it = other.begin(); it != other.end(); ++it)
    {
        switch( it->type() )
        {
            default:
                break;

            case Gfx::Path::Close:
                path.close();
                break;

            case Gfx::Path::MoveTo:
            {
                const Gfx::PointF& to = it->point(0);
                path.move_to( to.x(), to.y() );
                break;
            }

            case Gfx::Path::LineTo:
            {
                const Gfx::PointF& to = it->point(0);
                path.line_to( to.x(), to.y() );
                break;
            }

            case Gfx::Path::QuadTo:
            {
                const Gfx::PointF& c1 = it->point(0);
                const Gfx::PointF& to = it->point(1);
                
                path.quad_to( c1.x(), c1.y(), to.x(), to.y() );
                break;
            }
            
            case Gfx::Path::CubicTo:
            {
                const Gfx::PointF& c1 = it->point(0);
                const Gfx::PointF& c2 = it->point(1);
                const Gfx::PointF& to = it->point(2);

                path.cubic_to( c1.x(), c1.y(), c2.x(), c2.y(), to.x(), to.y() );
                break;
            }
        }
    }
}


void BitmapCanvas::onDrawPath()
{
    if( _pen.style() == Gfx::Pen::Dash || _pen.style() == Gfx::Pen::Dot )
    {
        drawDashed(_ptPath);
        return;
    }

    _context->stroke_path(_blPath);
}


void BitmapCanvas::onDrawPath(const Path& path)
{
    if( _pen.style() == Gfx::Pen::Dash || _pen.style() == Gfx::Pen::Dot )
    {
        drawDashed(path);
        return;
    }

    BLPath blPath;
    addPath(blPath, path);
    _context->stroke_path(blPath);
}


void BitmapCanvas::drawDashed(const Path& path)
{
    Dasher dasher(_dashPattern);
    _polygon.clear();

    for(PathIterator it = path.begin(); it != path.end(); ++it)
    {
        if(it->type() == Gfx::Path::Close)
        {
            dasher.finish();
        }
        else
        {
            if( ! _polygon.empty() )
                _polygon.pop_back();

            it->flatten(_polygon);
            dasher.push( _polygon.points(), _polygon.size() );
            _polygon.clear();
        }

        const std::vector<Polygon>& dashes = dasher.getDashes();
        for(const Polygon& dash : dashes)
            drawSolid( dash.points(), dash.size() );

        dasher.pop();
    }

    dasher.finish();
    const std::vector<Polygon>& dashes = dasher.getDashes();
    for(const Polygon& dash : dashes)
        drawSolid( dash.points(), dash.size() );
}


void BitmapCanvas::onFillPath()
{
    _context->fill_path(_blPath);
}


void BitmapCanvas::onFillPath(const Gfx::Path& path)
{
    BLPath blPath;
    addPath(blPath, path);
    _context->fill_path(blPath);
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

#if USE_BLEND2D_BLIT

void BitmapCanvas::onDrawImage(const PointF& toF, const Image& image, 
                                const RectF* imageRect)
{
    _context->save();
    _context->reset_transform();

    Gfx::PointF toP = transform() * toF;
    BLPoint pos( toP.x(), toP.y() );

    if( image.empty() )
        return;

    void* data = const_cast<Pt::uint8_t*>( image.data() );
    std::size_t stride = image.format().imageSize( image.width(), 1, image.padding() );

    BLImage view;

    if(_compositionMode == CompositionMode::SourceCopy)
    {
        view.create_from_data(image.width(), image.height(), BL_FORMAT_XRGB32,
                              data, stride, BL_DATA_ACCESS_READ);
    }
    else
    {
        view.create_from_data(image.width(), image.height(), BL_FORMAT_PRGB32,
                              data, stride, BL_DATA_ACCESS_READ);
    }

    if(imageRect)
    {
        BLRectI srcRect(lround( imageRect->x() ),
                        lround( imageRect->y() ), 
                        lround( imageRect->width() ),
                        lround( imageRect->height() ) );

        _context->blit_image(pos, view, srcRect);
    }
    else
    {
        _context->blit_image(pos, view);
    }
    
    _context->restore();
}

#else

void BitmapCanvas::onDrawImage(const PointF& toF, const Image& image, 
                                const RectF* imageRect)
{
    Gfx::PointF toP = transform() * toF;
    Point to = Point( lround(toP.x()), lround(toP.y()) );

    if(imageRect)
    {
        Rect srcRect = Rect( Point( lround( imageRect->x() ),
                                    lround( imageRect->y() ) ), 
                             Size( lround( imageRect->width() ),
                                   lround( imageRect->height() ) ) );

        putImage(to, image, srcRect);
    }
    else
    {
        putImage(to, image);
    }
}


void BitmapCanvas::putImage( const Point& to, const Image& img)
{
    Rect imageRect;
    imageRect.setWidth( img.width() );
    imageRect.setHeight( img.height() );

    putImage(to, img, imageRect);
}


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

#endif // USE_BLEND2d_BLIT

} // namespace

} // namespace
