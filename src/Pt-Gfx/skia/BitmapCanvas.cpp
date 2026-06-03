/* Copyright (C) 2015-2024 Marc Boris Duerner

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

#include <Pt/Gfx/Rgb32.h>
#include <Pt/Gfx/Image.h>
#include <Pt/Gfx/Bitmap.h>

#include <SkMatrix.h>

#include <vector>
#include <cmath>
#include <algorithm>

namespace Pt {

namespace Gfx {

BitmapCanvas::BitmapCanvas()
: Canvas()
, _surface(0)
, _canvas(0)
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
    _canvas = surface.skCanvas();
    _image = &surface.rgb32Image();
}


void BitmapCanvas::onBeginPaint(const Gfx::Paint& paint)
{
    if(_canvas)
        _canvas->save();
}


void BitmapCanvas::onFinishPaint()
{
    if(_canvas)
        _canvas->restore();

    _surface = 0;
    _canvas = 0;
    _image = 0;
}


void BitmapCanvas::onSetCompositionMode(const Gfx::CompositionMode& mode)
{
    _compositionMode = mode;
}


void BitmapCanvas::onApplyCompositionMode()
{
}


void BitmapCanvas::onApplyTransform()
{
    if( ! _canvas )
        return;

    Transform tx = transform();
    SkMatrix m;
    m.setAll( tx.m11(), tx.m12(), tx.dx(),
              tx.m21(), tx.m22(), tx.dy(),
              0, 0, 1 );

    _canvas->setMatrix(m);
}


void BitmapCanvas::onSetTransform(const Gfx::Transform& tx)
{
    if(_canvas && isActive())
    {
        _canvas->restore();
        _canvas->save();
        invalidate(DirtyAll & ~DirtyTransform);
    }
}


void BitmapCanvas::onSetPen(const Gfx::Pen& pen)
{
    _pen = pen;

    _skPen = SkPaint();
    _skPen.setColor( toSkColor(pen.color()) );
    _skPen.setStyle( SkPaint::kStroke_Style );
    _skPen.setAntiAlias(true);
    _skPen.setStrokeWidth( pen.size() );
    _skPen.setStrokeJoin( toSkJoin(pen.joinStyle()) );
    _skPen.setStrokeCap( toSkCap(pen.capStyle()) );

    switch( pen.style() )
    {
        default:
        case Pt::Gfx::Pen::Solid:
            break;

        case Pt::Gfx::Pen::Dash:
        {
            if(pen.capStyle() == Gfx::Pen::RoundCap ||
               pen.capStyle() == Gfx::Pen::SquareCap)
            {
                SkScalar dashes[] = { 2.0f * pen.size(), 2.0f * pen.size() };
                _skPen.setPathEffect( SkDashPathEffect::Make(dashes, 2, 0) );
            }
            else
            {
                SkScalar dashes[] = { 3.0f * pen.size(), 1.0f * pen.size() };
                _skPen.setPathEffect( SkDashPathEffect::Make(dashes, 2, 0) );
            }
            break;
        }

        case Pt::Gfx::Pen::Dot:
        {
            if(pen.capStyle() == Gfx::Pen::RoundCap ||
               pen.capStyle() == Gfx::Pen::SquareCap)
            {
                SkScalar dashes[] = { 1.0f, 2.0f * pen.size() };
                _skPen.setPathEffect( SkDashPathEffect::Make(dashes, 2, 0) );
            }
            else
            {
                SkScalar dashes[] = { 1.0f * pen.size(), 1.0f * pen.size() };
                _skPen.setPathEffect( SkDashPathEffect::Make(dashes, 2, 0) );
            }
            break;
        }
    }
}


void BitmapCanvas::onApplyPen()
{
}


void BitmapCanvas::onSetBrush(const Gfx::Brush& brush)
{
    _brush = brush;

    _skBrush = SkPaint();
    _skBrush.setAntiAlias(true);
    _skBrush.setStyle( SkPaint::kFill_Style );
    _skBrush.setColor( toSkColor(brush.color()) );
}


void BitmapCanvas::onApplyBrush()
{
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
    if(_canvas && isActive())
    {
        _canvas->restore();
        _canvas->save();
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
    if( ! _canvas || ! _image )
        return;

    RectI imageRect;
    imageRect.setWidth( _image->width() );
    imageRect.setHeight( _image->height() );

    if( ! _hasClip )
    {
        _currentClip = imageRect;
    }
    else
    {
        // clip in logical coordinates, applied before the transform
        SkRect skClip = SkRect::MakeLTRB( _clip.x(), _clip.y(),
                                           _clip.x() + _clip.width(),
                                           _clip.y() + _clip.height() );
        _canvas->clipRect(skClip);

        // _currentClip in physical pixels for software blit/text
        Gfx::PointF origin = transform() * _clip.origin();
        Gfx::SizeF size = transform() * _clip.size();
        Gfx::RectF clipP(origin, size);

        RectI clipRect = RectI( PointI( lround( clipP.x() ),
                                        lround( clipP.y() ) ),
                                SizeI( lround( clipP.width() ),
                                       lround( clipP.height() ) ) );

        if( clipRect.isEmpty() )
        {
            RectI outsideClip( PointI(imageRect.right(), imageRect.bottom()),
                               SizeI(1, 1) );
            _currentClip = outsideClip.toIntersected(imageRect);
            return;
        }

        _currentClip = clipRect.toIntersected(imageRect);
    }
}


void BitmapCanvas::onDrawLine(const Gfx::PointF& from, const Gfx::PointF& to)
{
    if( ! _canvas )
        return;

    _canvas->drawLine( from.x(), from.y(), to.x(), to.y(), _skPen );
}


void BitmapCanvas::onDrawPolyline(const Gfx::PointF* pts, const size_t n)
{
    if( ! _canvas || n < 2 )
        return;

    SkPath path;
    path.moveTo( pts[0].x(), pts[0].y() );

    for(size_t i = 1; i < n; ++i)
        path.lineTo( pts[i].x(), pts[i].y() );

    _canvas->drawPath( path, _skPen );
}


void BitmapCanvas::onFillPolygon(const Gfx::PointF* pts, const size_t n)
{
    if( ! _canvas || n == 0 )
        return;

    SkPath path;
    path.moveTo( pts[0].x(), pts[0].y() );

    for(size_t i = 1; i < n; ++i)
        path.lineTo( pts[i].x(), pts[i].y() );

    path.close();

    _canvas->drawPath( path, _skBrush );
}


void BitmapCanvas::onDrawRect(const Gfx::RectF& r)
{
    if( ! _canvas )
        return;

    SkRect sr = SkRect::MakeLTRB( r.left(), r.top(), r.right(), r.bottom() );
    _canvas->drawRect( sr, _skPen );
}


void BitmapCanvas::onFillRect(const Gfx::RectF& r)
{
    if( ! _canvas )
        return;

    SkRect sr = SkRect::MakeLTRB( r.left(), r.top(), r.right(), r.bottom() );
    _canvas->drawRect( sr, _skBrush );
}


void BitmapCanvas::onDrawEllipse(const PointF& topLeft, const SizeF& size)
{
    if( ! _canvas )
        return;

    SkRect sr = SkRect::MakeXYWH( topLeft.x(), topLeft.y(),
                                   size.width(), size.height() );
    _canvas->drawOval( sr, _skPen );
}


void BitmapCanvas::onFillEllipse(const PointF& topLeft, const SizeF& size)
{
    if( ! _canvas )
        return;

    SkRect sr = SkRect::MakeXYWH( topLeft.x(), topLeft.y(),
                                   size.width(), size.height() );
    _canvas->drawOval( sr, _skBrush );
}


void BitmapCanvas::onSetPath(const Gfx::Path& path)
{
    _ptPath = path;
    _skPath = toSkPath(path);
}


void BitmapCanvas::onDrawPath()
{
    if( ! _canvas )
        return;

    _canvas->drawPath( _skPath, _skPen );
}


void BitmapCanvas::onDrawPath(const Path& path)
{
    if( ! _canvas )
        return;

    SkPath skPath = toSkPath(path);
    _canvas->drawPath( skPath, _skPen );
}


void BitmapCanvas::onFillPath()
{
    if( ! _canvas )
        return;

    _canvas->drawPath( _skPath, _skBrush );
}


void BitmapCanvas::onFillPath(const Gfx::Path& path)
{
    if( ! _canvas )
        return;

    SkPath skPath = toSkPath(path);
    _canvas->drawPath( skPath, _skBrush );
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
    PointI to = PointI( lround(toP.x()), lround(toP.y()) );

    if(imageRect)
    {
        RectI srcRect = RectI( PointI( lround( imageRect->x() ),
                                       lround( imageRect->y() ) ),
                               SizeI( lround( imageRect->width() ),
                                      lround( imageRect->height() ) ) );

        _surface->putImage(to, image, srcRect, _currentClip, _compositionMode);
    }
    else
    {
        RectI srcRect;
        srcRect.setWidth( image.width() );
        srcRect.setHeight( image.height() );

        _surface->putImage(to, image, srcRect, _currentClip, _compositionMode);
    }
}


SkPath BitmapCanvas::toSkPath(const Gfx::Path& path)
{
    SkPath skPath;

    for(Gfx::PathIterator it = path.begin(); it != path.end(); ++it)
    {
        switch( it->type() )
        {
            default:
                break;

            case Gfx::Path::Close:
                skPath.close();
                break;

            case Gfx::Path::MoveTo:
            {
                const Gfx::PointF& to = it->point(0);
                skPath.moveTo( to.x(), to.y() );
                break;
            }

            case Gfx::Path::LineTo:
            {
                const Gfx::PointF& to = it->point(0);
                skPath.lineTo( to.x(), to.y() );
                break;
            }

            case Gfx::Path::QuadTo:
            {
                const Gfx::PointF& c1 = it->point(0);
                const Gfx::PointF& to = it->point(1);

                skPath.quadTo( c1.x(), c1.y(), to.x(), to.y() );
                break;
            }

            case Gfx::Path::CubicTo:
            {
                const Gfx::PointF& c1 = it->point(0);
                const Gfx::PointF& c2 = it->point(1);
                const Gfx::PointF& to = it->point(2);

                skPath.cubicTo( c1.x(), c1.y(), c2.x(), c2.y(), to.x(), to.y() );
                break;
            }
        }
    }

    return skPath;
}


SkColor BitmapCanvas::toSkColor(const Gfx::Color& c)
{
    return SkColorSetARGB( static_cast<U8CPU>(c.alpha()),
                           static_cast<U8CPU>(c.red()),
                           static_cast<U8CPU>(c.green()),
                           static_cast<U8CPU>(c.blue()) );
}


SkPaint::Join BitmapCanvas::toSkJoin(Gfx::Pen::JoinStyle s)
{
    switch(s)
    {
        case Gfx::Pen::MiterJoin: return SkPaint::kMiter_Join;
        case Gfx::Pen::RoundJoin: return SkPaint::kRound_Join;
        case Gfx::Pen::BevelJoin: return SkPaint::kBevel_Join;
    }

    return SkPaint::kMiter_Join;
}


SkPaint::Cap BitmapCanvas::toSkCap(Gfx::Pen::CapStyle s)
{
    switch(s)
    {
        case Gfx::Pen::RoundCap:  return SkPaint::kRound_Cap;
        case Gfx::Pen::FlatCap:   return SkPaint::kButt_Cap;
        case Gfx::Pen::SquareCap: return SkPaint::kSquare_Cap;
    }

    return SkPaint::kSquare_Cap;
}

} // namespace

} // namespace
