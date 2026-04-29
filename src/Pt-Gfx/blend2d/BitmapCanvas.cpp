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
#include "Dasher.h"

#include <Pt/Gfx/Rgb32.h>
#include <Pt/Gfx/Image.h>
#include <Pt/Gfx/Bitmap.h>

#include <vector>
#include <cmath>
#include <algorithm>

namespace Pt {

namespace Gfx {

BitmapCanvas::BitmapCanvas()
: Canvas()
, _surface(0)
, _image(0)
, _hasClip(false)
{
}


BitmapCanvas::~BitmapCanvas()
{
}


void BitmapCanvas::init(BitmapSurface& surface)
{
    _surface = &surface;
    _context = &surface.rasterContext();
    _image = &surface.rgb32Image();
}


void BitmapCanvas::onBeginPaint(const Gfx::Paint& paint)
{
    if(_context)
        _context->save(_cookie);
}


void BitmapCanvas::onFinishPaint()
{
    if(_context)
        _context->restore(_cookie);

    _surface = 0;
    _context = 0;
    _image = 0;
}


void BitmapCanvas::onSetCompositionMode(const Gfx::CompositionMode& mode) 
{
    _compositionMode = mode;
}


void BitmapCanvas::onApplyCompositionMode()
{
    if( ! _context )
        return;

    BLCompOp compOp = BL_COMP_OP_SRC_OVER;
    
    if(_compositionMode == CompositionMode::SourceOver)
    {
        compOp = BL_COMP_OP_SRC_OVER;
    }
    else // CompositionMode::SourceCopy
    {
        compOp = BL_COMP_OP_SRC_COPY;
    }

    _context->set_comp_op(compOp);
}


void BitmapCanvas::onApplyTransform()
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


void BitmapCanvas::onSetTransform(const Gfx::Transform& tx)
{
    if(_context && isActive())
    {
        _context->restore(_cookie);
        _context->save(_cookie);
        invalidate(DirtyAll & ~DirtyTransform);
    }
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


void BitmapCanvas::onApplyPen()
{
    if( ! _context )
        return;

    _context->set_stroke_width( static_cast<double>( _pen.size() ) );

    Pt::Gfx::Color penColor = _pen.color();
    BLRgba32 strokecolor(penColor.red(), 
                         penColor.green(), 
                         penColor.blue(), 
                         penColor.alpha());

    _context->set_stroke_style(strokecolor);
}


void BitmapCanvas::onSetBrush(const Gfx::Brush& brush)
{
    _brush = brush;
}


void BitmapCanvas::onApplyBrush()
{
    if( ! _context )
        return;

    Pt::Gfx::Color brushColor = _brush.color();
    BLRgba32 fillColor(brushColor.red(), 
                       brushColor.green(), 
                       brushColor.blue(), 
                       brushColor.alpha());

    switch( _brush.fillStyle() ) 
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
    _font = font;
    _fontRenderer.setFont(_font);
    _fontMetrics = _fontRenderer.fontMetrics();
}


void BitmapCanvas::onApplyFont()
{
}


void BitmapCanvas::onSetClip(const Gfx::RectF* clip)
{
    if(_context && isActive())
    {
        _context->restore(_cookie);
        _context->save(_cookie);
        invalidate(DirtyAll & ~DirtyClip);
    }

    _hasClip = clip != 0;

    if(clip)
        _clip = *clip;
    else
        _clip.clear();
}


void BitmapCanvas::onApplyClip() 
{
    if( ! _image )
        return;

    if(_hasClip)
        _context->clip_to_rect( _clip.x(), _clip.y(), 
                               _clip.width(), _clip.height() );

    RectI imageRect;
    imageRect.setWidth( _image->width() );
    imageRect.setHeight( _image->height() );

    if( ! _hasClip )
    {
        _currentClip = imageRect;
        return;
    }

    Gfx::PointF origin = transform() * _clip.origin();
    Gfx::SizeF size = transform() * _clip.size();
    Gfx::RectF clipP(origin, size);

    RectI clipRect = RectI( PointI( lround( clipP.x() ),
                                    lround( clipP.y() ) ), 
                            SizeI( lround( clipP.width() ),
                                   lround( clipP.height() ) ) );

    if( clipRect.isNull() ) // crashes otherwise
        clipRect = RectI( PointI(0, 0), SizeI(1, 1) );

    _currentClip = clipRect.intersect(imageRect);
}


void BitmapCanvas::onDrawLine(const Gfx::PointF& from, const Gfx::PointF& to)
{
    if( ! _context )
        return;

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
    if(n == 0 || ! _context)
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
    if(n == 0 || ! _context)
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
    if( ! _context )
        return;

    if( _pen.style() ==  Gfx::Pen::Dash ||
        _pen.style() == Gfx::Pen::Dot )
    {
        Path rectPath;
        rectPath.addRect(r);
        drawDashed(rectPath);
        return;
    }

    _context->stroke_rect( r.x(), r.y(), r.width(),r.height() );
}


void BitmapCanvas::onFillRect(const Gfx::RectF& r)
{
    if( ! _context )
        return;

     _context->fill_rect( r.x(), r.y(), r.width(),r.height() );
}


void BitmapCanvas::onDrawEllipse(const PointF& topLeft, const SizeF& size)
{
    if( ! _context )
        return;

    if( _pen.style() == Gfx::Pen::Dash || _pen.style() == Gfx::Pen::Dot )
    {
        Path ellipsePath;
        ellipsePath.addEllipse(topLeft, size);
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
    if( ! _context )
        return;

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
    if( ! _context )
        return;

    if( _pen.style() == Gfx::Pen::Dash || _pen.style() == Gfx::Pen::Dot )
    {
        drawDashed(_ptPath);
        return;
    }

    _context->stroke_path(_blPath);
}


void BitmapCanvas::onDrawPath(const Path& path)
{
    if( ! _context )
        return;

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
    if( ! _context )
        return;

    _context->fill_path(_blPath);
}


void BitmapCanvas::onFillPath(const Gfx::Path& path)
{
    if( ! _context )
        return;

    BLPath blPath;
    addPath(blPath, path);
    _context->fill_path(blPath);
}



const FontMetrics& BitmapCanvas::onGetFontMetrics() const
{
    return _fontMetrics;
}


TextMetrics BitmapCanvas::onGetTextMetrics(const String& text) const
{
    return _fontRenderer.textMetrics(text);
}


void BitmapCanvas::onDrawText(const PointF& to, const Pt::String& text, 
                               const Transform* tform)
{
    if( ! _image )
        return;

    Gfx::Transform tf = transform();
    tf.translate( to.x(), to.y() );

    if(tform)
        tf *= *tform;

    _fontRenderer.draw(*_image, 0, 0, text, _pen.color(), _currentClip,
                       _compositionMode, &tf);
}


void BitmapCanvas::onDrawImage(const PointF& toF, const Image& image, 
                               const RectF* imageRect)
{
    if( ! _surface )
        return;

    Gfx::PointF toP = transform() * toF;
    PointI to = round(toP);

    if(imageRect)
    {
        _surface->putImage(to, image, round(*imageRect),
                           _currentClip, _compositionMode);
    }
    else
    {
        RectI srcRect;
        srcRect.setWidth( image.width() );
        srcRect.setHeight( image.height() );

        _surface->putImage(to, image, srcRect,
                           _currentClip, _compositionMode);
    }
}

} // namespace

} // namespace
