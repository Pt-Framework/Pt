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

#ifndef PT_GFX_ImageSurface_H
#define PT_GFX_ImageSurface_H

#include <Pt/Gfx/Api.h>
#include <Pt/Gfx/Paint.h>
#include <Pt/Gfx/Canvas.h>
#include <Pt/Gfx/PaintSurface.h>
#include <Pt/Gfx/Rect.h>
#include <Pt/System/Path.h>

namespace Pt {

namespace Gfx {

class Rasterizer;
class ImageSurface;
class ImageCanvas;

class ImagePaint : public PaintContext
{
    public:
        ImagePaint();

        ~ImagePaint();

        const Pen& pen() const
        {
            return _pen;
        }

        const Brush& brush() const
        {
            return _brush;
        }

        const Font& font() const
        {
            return _font;
        }

    protected:
        virtual void onSetPen(const Gfx::Pen& pen) override;

        virtual void onSetBrush(const Gfx::Brush& brush) override;

        virtual void onSetFont(const Gfx::Font& font) override;
   
    private:
        Pen                  _pen;
        Gfx::Brush           _brush;
        Gfx::Font            _font;
};


class ImageCanvas : public Gfx::Canvas
{
  public:
    ImageCanvas(PaintSurface& surface);

    ImageCanvas(PaintSurface& surface, 
                const Gfx::Size& size, std::size_t stride = 0);

    virtual ~ImageCanvas();

    void reset(const Gfx::Image& image);

    void reset(const Gfx::Size& size, std::size_t stride = 0);

    const Gfx::Image& image() const;

    void setScaleFactor(double scaleFactor);

    const Gfx::SizeF& imageSize() const;

    void resize(const Gfx::SizeF& size);

    const Gfx::SizeF& imageLogicalSize() const;

    const Gfx::ImageFormat& format() const;

  protected:
    virtual const Gfx::Scaling& onGetScaling() const override;

    virtual bool onBeginPaint(Gfx::PaintContext* context) override;

    virtual Gfx::PaintContext* onBeginPaint() override;

    virtual void onReleasePaint() override;
  
  protected:
      virtual void onSetCompositionMode(const CompositionMode& mode) override;

      virtual void onApplyPen(PaintContext& paint) override;

      virtual void onApplyBrush(PaintContext& paint) override;

      virtual void onApplyFont(PaintContext& paint) override;

  protected:
      virtual void onSetClip(const Gfx::RectF& clip) override;

      virtual void onResetClip() override;

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

    virtual void onDrawPath(const Gfx::Path& path, float smoothness) override
    {}

    virtual void onFillPath(const Gfx::Path& path, float smoothness) override
    {}

  protected:
    virtual Gfx::FontMetrics onGetFontMetrics(const Pt::String& text) const override;

    virtual void onDrawText(const Gfx::PointF& to, 
                            const Pt::String& text, 
                            const Gfx::Transform* trans)override;

  protected:
    virtual Gfx::Image onGetImage() const override;

    virtual void onDrawImage(const Gfx::PointF& to, 
                             const Gfx::Image& image) override;

    virtual void onDrawImage(const Gfx::PointF& to, 
                             const Gfx::Image& image, 
                             const Gfx::RectF& imgRect) override;

    virtual void onDrawCanvas(const Gfx::PointF& to, 
                              const Gfx::Canvas& surface) override;

    virtual void onDrawCanvas(const Gfx::PointF& to, 
                              const Gfx::Canvas& canvas, 
                              const Gfx::RectF& pmRect) override;

  private:
    Rasterizer*   _rasterizer;
    ImagePaint*   _paint;
    Scaling       _scaling;
    SizeF         _size;
    SizeF         _imageSize;
};

/** @brief Image drawing surface.
*/
class PT_GFX_API ImageSurface : public Gfx::PaintSurface
{
  public:
    ImageSurface();

    ImageSurface(const Gfx::Size& size, std::size_t stride = 0);

    virtual ~ImageSurface();

    void reset(const Gfx::Image& image);

    void reset(const Gfx::Size& size, std::size_t stride = 0);

    const Gfx::Image& image() const;

    /** @brief Returns the size of the image in device pixel. 
    */
    const Gfx::SizeF& size() const;

    void resize(const Gfx::SizeF& size);

    void setScaleFactor(double scaleFactor);    

  protected:
    virtual const Canvas* onGetCanvas() const override;

    virtual Gfx::PaintContext* onBeginPaint(Gfx::PaintContext* context) override;

    virtual const Gfx::ImageFormat& onGetFormat() const override;

    virtual const Gfx::SizeF& onGetLogicalSize() const override;

    virtual const Scaling& onGetScaling() const override;

  public:
    static void setFontDir(const System::Path& path);

    static const std::string& defaultFont();

    static void setDefaultFont(const std::string& name);

    static std::vector<std::string> fontNames();

  private:
    ImageCanvas*  _canvas;
    Scaling       _scaling;
};

} // namespace

} // namespace

#endif

