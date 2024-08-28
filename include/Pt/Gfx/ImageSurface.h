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
        ImagePaint(ImageCanvas& canvas);

        ~ImagePaint();

        void reset(ImageCanvas& canvas);

        const Pen& pen() const
        {
            return _pen;
        }

    protected:
        virtual void onSetPainter(Painter& painter) override;

        virtual void onReleasePainter(Painter& painter) override;

        virtual void onSetCanvas(Canvas& canvas) override;

        virtual void onReleaseCanvas(Canvas& canvas) override;

        virtual void onBeginPaint(const Paint& paint) override;

        virtual void onFinishPaint() override;

    protected:
        virtual void onSetCompositionMode(const Gfx::CompositionMode& mode);

        virtual void onSetPen(const Gfx::Pen& pen);

        virtual void onSetBrush(const Gfx::Brush& brush);

        virtual void onSetFont(const Gfx::Font& font);

        virtual void onSetClip(const Gfx::RectF& rectF);

        virtual void onResetClip();

    protected:
        virtual void onDrawLine(const Gfx::Line& line) override;

        virtual void onDrawPolyline(const Gfx::Polyline& line) override;

        virtual void onFillPolygon(const Gfx::Polyline& line) override;

        virtual void onDrawRect(const Gfx::RectF& rectangle) override;

        virtual void onFillRect(const Gfx::RectF& rectangle) override;

        virtual void onDrawEllipse(const Gfx::PointF& topLeft, 
                                   const Gfx::SizeF& size) override;

        virtual void onFillEllipse(const Gfx::PointF& topLeft, 
                                   const Gfx::SizeF& size) override;

    protected:
        virtual Gfx::FontMetrics onGetFontMetrics(const Pt::String& text) const override;

        virtual void onDrawText(const Gfx::PointF& to, const Pt::String& text) override;

        virtual void onDrawText(const Gfx::PointF& to, const Pt::String& text, 
                                const Gfx::Transform& trans) override;

    protected:
        virtual void onDrawImage(const Gfx::PointF& to, 
                                 const Gfx::Image& image) override;

        virtual void onDrawImage(const Gfx::PointF& to, 
                                 const Gfx::Image& image, 
                                 const Gfx::RectF& imgRect) override;

        virtual void onDrawSurface(const Gfx::PointF& to, 
                                   const Gfx::PaintSurface& surface) override;

        virtual void onDrawSurface(const Gfx::PointF& to,
                                   const Gfx::PaintSurface& surface,
                                   const Gfx::RectF& rect) override;

    private:
        void updatePen(const Gfx::Pen& pen);
    
    private:
        ImageCanvas*   _canvas;
        bool           _invalid;
        Gfx::Scaling   _scaling;
        Pen            _pen;
};


class PT_GFX_API ImageCanvas : public Gfx::Canvas
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

    void resize(const Gfx::SizeF& size);

    void setPen(const ImagePaint& paint);

    const Gfx::ImageFormat& format() const;

    const Gfx::SizeF& size() const;

    const Scaling& scaling() const;

  protected:
    virtual PaintContext* onBeginPaint(Gfx::PaintContext* context) override;

    virtual void onFinishPaint() override;
  
  public:
    virtual void setClip(const Gfx::RectF& clip);

    virtual void resetClip();

    virtual void setCompositionMode(const Gfx::CompositionMode& mode);

    //virtual void setPen(const Gfx::Pen& pen);

    virtual void setBrush(const Gfx::Brush& brush);

    virtual void setFont(const Gfx::Font& font);

    virtual Gfx::FontMetrics fontMetrics(const Pt::String& text) const;

    virtual void drawLine(const Gfx::PointF& from, const Gfx::PointF& to);

    virtual void drawLine(const Gfx::Line& line);

    virtual void drawText(const Gfx::PointF& to, const Pt::String& Text);

    virtual void drawText(const Gfx::PointF& to, const Pt::String& Text, const Gfx::Transform& trans);

    virtual void drawRect(const Gfx::RectF& rectangle);

    virtual void fillRect(const Gfx::RectF& rectangle);

    virtual void drawEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size);

    virtual void fillEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size);

    virtual void drawPolyline(const Gfx::PointF* points, size_t pointCount);

    virtual void fillPolygon(const Gfx::PointF* points, size_t pointCount);

    virtual void drawPolyline(const Gfx::Polyline& line);

    virtual void fillPolygon(const Gfx::Polyline& line);

    virtual void drawImage(const Gfx::PointF& to, const Gfx::Image& image);

    virtual void drawImage(const Gfx::PointF& to, const Gfx::Image& image, const Gfx::RectF& imgRect);

    virtual void drawPath(const Gfx::Path& path, float smoothness);

    virtual void fillPath(const Path& path, float smoothness);

    virtual void drawChord(const PointF& topLeft, const SizeF& size, float degBegin, float degEnd);

    virtual void fillChord(const PointF& topLeft, const SizeF& size, float degBegin, float degEnd);

    virtual void drawPie(const PointF& topLeft, const SizeF& size, float degBegin, float degEnd);

    virtual void fillPie(const PointF& topLeft, const SizeF& size, float degBegin, float degEnd);

    virtual void drawArc(const PointF& topLeft, const SizeF& size, float degBegin, float degEnd);

    virtual void drawSurface(const Gfx::PointF& toF, const PaintSurface& surface);

    virtual void drawSurface(const Gfx::PointF& toF, const PaintSurface& pm, const Gfx::RectF& pmRect);

  private:
    Rasterizer*   _rasterizer;
    Scaling       _scaling;
    SizeF         _size;
};

//
// TODO: Image in ImageSurface used by Canvas to draw on
//

class PT_GFX_API ImageSurface : public Gfx::PaintSurface
{
  public:
    ImageSurface();

    ImageSurface(const Gfx::Size& size, std::size_t stride = 0);

    ImageSurface(ImageCanvas& canvas);

    virtual ~ImageSurface();

    void reset(const Gfx::Image& image);

    void reset(const Gfx::Size& size, std::size_t stride = 0);

    const Gfx::Image& image() const;

    void resize(const Gfx::SizeF& size);

    void setScaleFactor(double scaleFactor);    

  protected:
    virtual const Gfx::ImageFormat& onGetFormat() const override;

    virtual const Gfx::SizeF& onGetSize() const override;

    virtual const Scaling& onGetScaling() const override;

    virtual PaintContextPtr onBeginPaint(Gfx::PaintContext* context) override;

    virtual Image onGetImage() const override;

  public:
    static void setFontDir(const System::Path& path);

    static const std::string& defaultFont();

    static void setDefaultFont(const std::string& name);

    static std::vector<std::string> fontNames();

    static FontMetrics fontMetrics( const Font& font, const Pt::String& text );

  private:
    ImageCanvas*  _canvas;
    ImageCanvas*  _owner;
    Scaling       _scaling;
    SizeF         _size;
};

} // namespace

} // namespace

#endif

