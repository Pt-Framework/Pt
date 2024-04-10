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
#include <Pt/Gfx/PaintSurface.h>
#include <Pt/Gfx/Rect.h>
#include <Pt/System/Path.h>

namespace Pt {

namespace Gfx {

class Rasterizer;

class PT_GFX_API ImageSurface : public PaintSurface
{
  public:
    ImageSurface();

    ImageSurface(const Gfx::Size& size, std::size_t stride = 0);

    virtual ~ImageSurface();

    void reset(const Gfx::Size& size, std::size_t stride = 0);

    const Gfx::Image& image() const;

  protected:
    virtual double onScaleFactor() const;

    virtual const Gfx::SizeF& onSize() const;

    virtual void onBegin(Painter& painter);

    virtual void onFinish();

  public:
    virtual const Gfx::ImageFormat& format() const;

    virtual Image toImage() const;

    virtual void setClip(const Gfx::RectF& clip);

    virtual void resetClip();

    virtual void setCompositionMode(const Gfx::CompositionMode& mode);

    virtual void setPen(const Gfx::Pen& pen);

    virtual void setBrush(const Gfx::Brush& brush);

    virtual void setFont(const Gfx::Font& font);

    virtual Gfx::FontMetrics fontMetrics(const Pt::String& text) const;

    virtual void drawLine(const Gfx::PointF& from, const Gfx::PointF& to);

    virtual void drawText(const Gfx::PointF& to, const Pt::String& Text);

    virtual void drawText(const Gfx::PointF& to, const Pt::String& Text, const Gfx::Transform& trans);

    virtual void drawRect(const Gfx::RectF& rectangle);

    virtual void fillRect(const Gfx::RectF& rectangle);

    virtual void drawEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size);

    virtual void fillEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size);

    virtual void drawPolyline(const Gfx::PointF* points, size_t pointCount);

    virtual void fillPolygon(const Gfx::PointF* points, size_t pointCount);

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

  public:
    static void setFontDir(const System::Path& path);

    static const std::string& defaultFont();

    static void setDefaultFont(const std::string& name);

    static std::vector<std::string> fontNames();

    static FontMetrics fontMetrics( const Font& font, const Pt::String& text );

  private:
    Rasterizer* _rasterizer;
    mutable SizeF   _size;
};

} // namespace

} // namespace

#endif

