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

#ifndef PT_GFX_SKIA_RASTERIZER_H
#define PT_GFX_SKIA_RASTERIZER_H

#include <Pt/Gfx/Api.h>
#include <Pt/Gfx/FontMetrics.h>
#include <Pt/Gfx/Path.h>
#include <Pt/Gfx/ImageFormat.h>
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Rect.h>
#include <Pt/Gfx/Pen.h>
#include <Pt/Gfx/Brush.h>
#include <Pt/Gfx/Font.h>
#include <Pt/String.h>
#include <Pt/System/Path.h>

#include <SkSurface.h>
#include <SkImage.h>
#include <SkCanvas.h>
#include <SkFont.h>
#include <SkPath.h>
#include <SkDashPathEffect.h>

namespace Pt {

namespace Gfx {

class DrawText;
class Image;
class Painter;
class SkiaPaintData;

class Rasterizer
{
  public:
    Rasterizer( Image& image );

    ~Rasterizer();

    const Image& image() const
    {
        return *_image;
    }

    void setImage(Image& image);

    void begin(Painter& painter);

    void finish();

    const ImageFormat& format() const;

    void setCompositionMode(const CompositionMode& mode);

    void setClip(const RectF& clip);

    void resetClip();

    void setPen( const Pen& pen );

    void setBrush( const Brush& brush );

    void setFont(const Font& font);

    FontMetrics fontMetrics(const String& text) const;

    static FontMetrics fontMetrics(const Font& font, const Pt::String& text);

    void drawLine(const Gfx::PointF& from, const Gfx::PointF& to);

    void drawText(const Gfx::PointF& to, const Pt::String& Text);

    void drawText(const Gfx::PointF& to, const Pt::String& Text, const Gfx::Transform& trans);

    void drawRect(const Gfx::RectF& rectangle);

    void fillRect(const Gfx::RectF& rectangle);

    void drawEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size);

    void fillEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size);

    void drawPolyline(const Gfx::PointF* points, size_t pointCount);

    void fillPolygon(const Gfx::PointF* points, size_t pointCount);

    void drawImage(const Gfx::PointF& to, const Gfx::Image& image);

    void drawImage(const Gfx::PointF& to, const Gfx::Image& image, const Gfx::RectF& imgRect);

    void drawPath(const Gfx::Path& path, float smoothness);

    void fillPath(const Path& path, float smoothness);

    void drawChord(const PointF& topLeft, const SizeF& size, float degBegin, float degEnd);

    void fillChord(const PointF& topLeft, const SizeF& size, float degBegin, float degEnd);

    void drawPie(const PointF& topLeft, const SizeF& size, float degBegin, float degEnd);

    void fillPie(const PointF& topLeft, const SizeF& size, float degBegin, float degEnd);

    void drawArc(const PointF& topLeft, const SizeF& size, float degBegin, float degEnd);

    Image toImage() const;

    public:
      static void setFontDir(const System::Path& path);

      static const std::string& defaultFont();

      static void setDefaultFont(const std::string& name);

      static std::vector<std::string> fontNames();

    private:
        static SkPoint toSkia(const Gfx::PointF& p)
        {
            SkPoint sp;

            sp.fX = p.x();
            sp.fY = p.y();
            return sp;
        }

        static SkRect toSkia(const Gfx::RectF& r)
        {
            SkRect sr;

            sr.fLeft = r.left();
            sr.fRight = r.right();
            sr.fTop = r.top();
            sr.fBottom = r.bottom();

            return sr;
        }
  
        static SkRect toSkia(const Gfx::PointF& p, const Gfx::SizeF& s)
        {
            SkRect sr;

            sr.fLeft = p.x();
            sr.fRight = p.x() + s.width();
            sr.fTop = p.y();
            sr.fBottom = p.y() + s.height();

            return sr;
        }

        static SkPath toSkia(const Gfx::Path& p);

        static bool equals(const Gfx::PointF& p1, const Gfx::PointF& p2)
        {

            double dt = std::abs(p1.x() - p2.x());

            if (dt > 0.1)
                return false;


            dt = std::abs(p1.y() - p2.y());

            if (dt > 0.1)
                return false;

            return true;
        }
  private:
      void updateClip() const;

  private:
    Image*           _image;
    sk_sp<SkSurface> _surface;
    SkCanvas*        _canvas;
    DrawText*        _text;
    CompositionMode  _compositionMode;
    SkiaPaintData*   _paintData;
    Painter*         _painter;
    RectF            _clip;
};

} //namespace

} //namespace

#endif
