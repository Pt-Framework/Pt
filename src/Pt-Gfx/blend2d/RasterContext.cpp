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

#include "RasterContext.h"
#include "DrawText.h"
#include "Dasher.h"

#include <Pt/Gfx/Image.h>
#include <Pt/Gfx/PaintLayer.h>
#include <Pt/Gfx/ImageSurface.h>

#include <vector>
#include <cmath>
#include <algorithm>

namespace Pt {

namespace Gfx {

RasterContext::RasterContext()
: PaintContext()
, _rasterImage(0)
, _image()
, _text( new DrawText() )
, _hasClip(false)
{
}


RasterContext::~RasterContext()
{
    delete _text;
}


void RasterContext::init(BLImage& rasterImage, Image& image)
{
    _rasterImage = &rasterImage;
    _image = &image;
}


void RasterContext::onBeginPaint(const Gfx::Paint& paint)
{
    if( ! _rasterImage )
        return;

    _context.begin(*_rasterImage);
    
    Transform tx = transform();
    BLMatrix2D m( tx.m11(), tx.m12(),
                  tx.m21(), tx.m22(), 
                  tx.dx(), tx.dy() );

    _context.reset_transform();
    _context.set_transform(m);
}


void RasterContext::onResetPaint()
{
    // NOTE: this might be called from the attached canvas base class destructor

    _context.end();

    if(_rasterImage)
        _rasterImage = 0;

    if(_image)
        _image = 0;
}


void RasterContext::onSetCompositionMode(const Gfx::CompositionMode& mode) 
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

    _context.set_comp_op(compOp);
    
    _compositionMode = mode;
}


void RasterContext::onApplyCompositionMode(const Gfx::CompositionMode& mode)
{
    _compositionMode = mode;
}


void RasterContext::onSetPen(const Gfx::Pen& pen)
{
    _context.set_stroke_width( pen.size() );

    Pt::Gfx::Color penColor = pen.color();
    BLRgba32 strokecolor(penColor.red() / 257, 
                         penColor.green() / 257, 
                         penColor.blue()  / 257, 
                         penColor.alpha() / 257);

    switch( pen.style() )
    {
        case Gfx::Pen::Solid:
            _context.set_stroke_style(strokecolor);
            break;
                
        case Gfx::Pen::Dash:
        {
            BLArray<double> dash;

            if(pen.capStyle() == Gfx::Pen::RoundCap ||
               pen.capStyle() == Gfx::Pen::SquareCap)
            {
                dash.append( 2.0 * pen.size() ); // stroke
                dash.append( 2.0 * pen.size() ); // gap
            }
            else
            {
                dash.append( 3.0 * pen.size() ); // stroke
                dash.append( 1.0 * pen.size() ); // gap
            }
            
            _context.set_stroke_style(strokecolor);
            _context.set_stroke_dash_array(dash);
            _context.set_stroke_dash_offset(0.0);
            break;
        }

        case Gfx::Pen::Dot:
            break;
    }
}


void RasterContext::onApplyPen(const Gfx::Pen& pen)
{
    _pen = pen;
    _text->setPen(pen);

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
              _dashPattern.push_back( _pen.size() );
              _dashPattern.push_back( _pen.size() );
          }
    }
}


void RasterContext::onSetBrush(const Gfx::Brush& brush)
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
            _context.set_fill_style(fillColor);
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


void RasterContext::onApplyBrush(const Gfx::Brush& brush)
{

}


void RasterContext::onSetFont(const Gfx::Font& font)
{
}


void RasterContext::onApplyFont(const Gfx::Font& font)
{
    _text->setFont(font);
}



void RasterContext::onSetClip(const Gfx::RectF* clip)
{
    _hasClip = clip != 0;

    if( ! clip )
        _context.restore_clipping();
    else
        _context.clip_to_rect( clip->x(), clip->y(), 
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
}


void RasterContext::onApplyClip(const Gfx::RectF* clip) 
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

    Rect clipRect = Rect( Point( lround( _clip.x() ),
                                 lround( _clip.y() ) ), 
                          Size( lround( _clip.width() ),
                                lround( _clip.height() ) ) );

    if( clipRect.isNull() ) // crashes otherwise
        clipRect = Rect( Point(0, 0), Size(1, 1) );

    _currentClip =  clipRect.intersect(imageRect);
}


void RasterContext::onDrawLine(const Gfx::PointF& from, const Gfx::PointF& to)
{
    _context.stroke_line( from.x(), from.y(), to.x(), to.y() );
}


void RasterContext::onDrawPolyline(const Gfx::PointF* pts, const size_t n)
{
    if(n == 0)
        return;

    std::vector<BLPoint> points(n);

    for(unsigned i = 0; i < n; i++)
    {
        const Gfx::PointF& p = pts[i];
        points[i] = BLPoint( p.x(), p.y() );
    }
    
    _context.stroke_polyline( points.data(), points.size() );
}


void RasterContext::onFillPolygon(const Gfx::PointF* pts, const size_t n)
{
    if(n == 0)
        return;

    std::vector<BLPoint> points(n);

    for(unsigned i = 0; i < n; i++)
    {
        const Gfx::PointF& p = pts[i];
        points[i] = BLPoint( p.x(), p.y() );
    }
    
    _context.fill_polygon( points.data(), points.size() );
}


void RasterContext::onDrawRect(const Gfx::RectF& r)
{
    if( _pen.style() ==  Gfx::Pen::Dash ||
        _pen.style() == Gfx::Pen::Dot )
    {
        std::vector<double> _dashPattern2;
        _dashPattern2.push_back( 45 );
        _dashPattern2.push_back( 40 );

        Path path;
        path.moveTo( r.topLeft() );
        path.lineTo( r.topRight() );
        path.lineTo( r.bottomRight() );
        path.lineTo( r.bottomLeft() );
        path.lineTo( r.topLeft() );

        Dasher dasher(_dashPattern2);
        std::vector<PointF> points;
        for(PathIterator it = path.begin(); it != path.end(); ++it)
        {
            it->flatten(points);
            dasher.push(points.data(), points.size());
            points.clear();
        }

        dasher.finish();

        std::vector<Dasher::Dash> dashes = dasher.getDashes();
        for(Dasher::Dash dash : dashes)
            onDrawPolyline(dash.data(), dash.size());
      
        return;
    }

    _context.stroke_rect( r.x(), r.y(), r.width(),r.height() );
}


void RasterContext::onFillRect(const Gfx::RectF& r)
{
     _context.fill_rect( r.x(), r.y(), r.width(),r.height() );
}


void RasterContext::onDrawEllipse(const PointF& topLeft, const SizeF& size)
{
    double radiusX = size.width() / 2.0;
    double radiusY = size.height() / 2.0;
    double centerX = topLeft.x() + radiusX;
    double centerY = topLeft.y() + radiusY;

    _context.stroke_ellipse( centerX, centerY, radiusX, radiusY );
}


void RasterContext::onFillEllipse(const PointF& topLeft, const SizeF& size)
{
    double radiusX = size.width() / 2.0;
    double radiusY = size.height() / 2.0;
    double centerX = topLeft.x() + radiusX;
    double centerY = topLeft.y() + radiusY;

    _context.fill_ellipse( centerX, centerY, radiusX, radiusY );
}


void RasterContext::onBeginPath()
{
}


void RasterContext::onMoveTo(const PointF& to)
{
}


void RasterContext::onLineTo(const PointF& to)
{
}


void RasterContext::onCurveTo(const PointF &cp, const PointF& to)
{
}


void RasterContext::onCurveTo(const PointF &cp1, const PointF &cp2, const PointF& to)
{
}


void RasterContext::onClosePath()
{
}


void RasterContext::onSetPath(const Gfx::Path& path)
{
}


void RasterContext::onDrawPath(const Path& path)
{
}


void RasterContext::onFillPath(const Path& path)
{
}


TextMetrics RasterContext::onGetTextMetrics(const String& text) const
{
    return _text->textMetrics(text);
}


void RasterContext::onDrawText(const PointF& to, const Pt::String& text, 
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

void RasterContext::onDrawImage(const PointF& toF, const Image& image, 
                                const RectF* imageRect)
{
    _context.save();
    _context.reset_transform();

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

        _context.blit_image(pos, view, srcRect);
    }
    else
    {
        _context.blit_image(pos, view);
    }
    
    _context.restore();
}

#else

void RasterContext::onDrawImage(const PointF& toF, const Image& image, 
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


void RasterContext::putImage( const Point& to, const Image& img)
{
    Rect imageRect;
    imageRect.setWidth( img.width() );
    imageRect.setHeight( img.height() );

    putImage(to, img, imageRect);
}


void RasterContext::putImage(const Point& to, const Image& image, const Rect& imageRect)
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

    _image->view().copy(toRect.x(), toRect.y(),
                        image.view(), fromRect.x(), fromRect.y(), 
                        fromRect.width(), fromRect.height(), _compositionMode);

    //_image.format().copy(_image.view(), toRect.x(), toRect.y(),
    //                     image.view(), fromRect.x(), fromRect.y(), 
    //                     fromRect.width(), fromRect.height(), _compositionMode);
}

#endif // USE_BLEND2d_BLIT

bool RasterContext::onDrawLayer(const Gfx::PointF& to,
                                const Gfx::PaintLayer& layer,
                                const Gfx::RectF* rect)
{
    const PaintSurface* layerSurface = layer.surface();
    const ImageSurface* imageSurface = dynamic_cast<const ImageSurface*>(layerSurface);
    if(imageSurface)
    {
        const Gfx::Image& image = imageSurface->image();
        
        if(rect)
        {
            Gfx::RectF imageRect = scaling().toPhysical(*rect);
            drawImage(to, image, &imageRect);
        }
        else
        {
            drawImage(to, image);
        }
        
        return true;
    }

    return false;
}

} // namespace

} // namespace
