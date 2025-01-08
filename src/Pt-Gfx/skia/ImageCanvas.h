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
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
  MA 02110-1301 USA
*/

#ifndef PT_GFX_SKIA_IMAGE_CANVAS_H
#define PT_GFX_SKIA_IMAGE_CANVAS_H

#include <Pt/Gfx/Canvas.h>
#include <Pt/Gfx/Image.h>
#include <Pt/Gfx/Scaling.h>
#include <Pt/Gfx/Size.h>

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

namespace Pt {

namespace Gfx {

class SkiaContext;
class DrawText;

class ImageCanvas : public Canvas
{
  public:
    ImageCanvas(PaintSurface& surface);

    ~ImageCanvas();

    const Image& image() const;

    void reset(const Gfx::Image& image);

    void reset(Pt::ssize_t width, Pt::ssize_t height, 
               std::size_t stride);

    void setScaleFactor(double scaleFactor);

    const Size& physicalSize() const;

    const SizeF& logicalSize() const;

  protected:
    virtual const Gfx::ImageFormat& onGetFormat() const;

    virtual const Gfx::SizeF& onGetSize() const;

    virtual const Scaling& onGetScaling() const;

  protected:
    virtual bool onSetPaint(Gfx::PaintContext* context) override;

    virtual Gfx::PaintContext* onCreatePaint() override;

    virtual void onReleasePaint() override;


  protected:
    virtual void onCompositionModeChanged() override;

    virtual void onPenChanged() override;

    virtual void onBrushChanged() override;

    virtual void onFontChanged() override;

    virtual void onClipChanged() override;

  protected:
    virtual void onDrawLine(const Gfx::PointF& from, const Gfx::PointF& to) override;

    virtual void onDrawPolyline(const Gfx::Polyline& line) override;

    virtual void onFillPolygon(const Gfx::Polyline& line) override;

    virtual void onDrawRect(const Gfx::RectF& rectangle) override;

    virtual void onFillRect(const Gfx::RectF& rectangle) override;

    virtual void onDrawEllipse(const Gfx::PointF& topLeft, 
                               const Gfx::SizeF& size) override;

    virtual void onFillEllipse(const Gfx::PointF& topLeft, 
                               const Gfx::SizeF& size) override;

    virtual void onDrawArc(const Gfx::PointF& topLeft, const Gfx::SizeF& size, 
                           float degBegin, float degEnd) override
    {}

    virtual void onFillChord(const Gfx::PointF& topLeft, const Gfx::SizeF& size, 
                             float degBegin, float degEnd) override
    {}

    virtual void onFillPie(const Gfx::PointF& topLeft, const Gfx::SizeF& size, 
                           float degBegin, float degEnd) override
    {}

    virtual void onDrawPath(const Gfx::Path& path, float smoothness) override;

    virtual void onFillPath(const Gfx::Path& path, float smoothness) override;

  protected:
    virtual Gfx::FontMetrics onGetFontMetrics(const Pt::String& text) const override;

    virtual void onDrawText(const Gfx::PointF& to, 
                            const Pt::String& text, 
                            const Gfx::Transform* trans) override;

  protected:
    virtual void onDrawImage(const Gfx::PointF& to, 
                             const Gfx::Image& image, 
                             const Gfx::RectF* imgRect) override;

    virtual bool onDrawLayer(const Gfx::PointF& to,
                             const Gfx::PaintLayer& layer,
                             const Gfx::RectF* rect) override;

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
      void init(Image& image);

      void updateClip() const;

  private:
    Image            _image;
    SkiaContext*     _paint;

    Gfx::Size        _physicalSize;
    Gfx::SizeF       _logicalSize;
    Gfx::Scaling     _scaling;

    sk_sp<SkSurface> _surface;
    SkCanvas*        _canvas;
    DrawText*        _text;
    CompositionMode  _compositionMode;
    RectF            _clip;
};

} //namespace

} //namespace

#endif
