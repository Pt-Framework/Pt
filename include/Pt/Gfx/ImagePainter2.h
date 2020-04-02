/* Copyright (C) 2017-2017 Aloysius Indrayanto
   Copyright (C) 2006-2017 Marc Boris Duerner
   Copyright (C) 2006-2015 Laurentiu-Gheorghe Crisan

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

#ifndef PT_GFX_IMAGEPAINTER_2_H
#define PT_GFX_IMAGEPAINTER_2_H

#include <Pt/Gfx/Painter.h>
#include <Pt/Gfx/Transform.h>
#include <Pt/Gfx/Path.h>
#include <Pt/System/Path.h>

namespace Pt {

namespace Gfx {

class Rasterizer2;

class PT_GFX_API ImagePainter2 : public Painter
{
    public:
        ImagePainter2(Image& image);

        virtual ~ImagePainter2();

        void setImage(Image& image);

        virtual const ImageFormat& format() const;

        virtual void setCompositionMode(const CompositionMode& mode);

        virtual const CompositionMode& compositionMode() const;

        virtual void setClip(const RectF& clip);

        virtual void resetClip();

        virtual void setPen(const Pen& pen);

        virtual const Pen& pen() const;

        virtual void setBrush(const Brush& brush);

        virtual const Brush& brush() const;

        virtual void setFont(const Font& font);

        virtual const Font& font() const;

        virtual FontMetrics fontMetrics(const Pt::String& text) const;

        virtual void drawLine(const PointF& from, const PointF& to);

        virtual void drawPolyline(const PointF* points, const size_t pointCount);

        virtual void fillPolygon(const PointF* points, const size_t pointCount);

        virtual void fillPolygon_NR(const PointF* points, const size_t pointCount);

        virtual void drawText(const PointF& to, const Pt::String& text);

        virtual void drawText(const PointF& to, const Pt::String& text, const Transform& t);

        virtual void drawRect(const RectF& rect);

        virtual void fillRect(const RectF& rect);

        virtual void drawEllipse(const PointF& topLeft, const SizeF& size);

        virtual void fillEllipse(const PointF& topLeft, const SizeF& size);

        virtual void drawImage(const PointF& to, const Image& image);

        virtual void drawImage(const PointF& to, const Image& image, const RectF& imageRect);



        virtual bool isAntiAliasing() const;

        virtual void setAntiAliasing(bool on);


        virtual void drawRoundedRect(const RectF& rect, float radius);


        // NOTE: The begin and end angle must move in counter-clockwise (CCW) direction or something wrong may be drawn!
        virtual void drawArc(const PointF& topLeft, const SizeF& size,
                             float degBegin, float degEnd);

        // NOTE: The begin and end angle must move in counter-clockwise (CCW) direction or something wrong may be drawn!
        virtual void drawChord(const PointF& topLeft, const SizeF& size,
                               float degBegin, float degEnd);

        // NOTE: The begin and end angle must move in counter-clockwise (CCW) direction or something wrong may be drawn!
        virtual void drawPie(const PointF& topLeft, const SizeF& size,
                             float degBegin, float degEnd);

        // maybe better have ImagePainter::setSmoothness
        virtual void drawPath(const Path& path, float smoothness = 1.0f, bool nonZeroFillingRule = false);


        virtual void fillRoundedRect(const RectF& rect, float radius);


        virtual void fillPie(const PointF& topLeft, const SizeF& size,
                             float degBegin, float degEnd);

        virtual void fillChord(const PointF& topLeft, const SizeF& size,
                              float degBegin, float degEnd);

        // maybe better have a separate ImagePainter::setSmoothness
        virtual void fillPath(const Path& path, float smoothness = 1.0f);

    public:
        static FontMetrics fontMetrics(const Font& font, const Pt::String& text);

        static void setFontDir(const System::Path& path);

        static void setDefaultFont(const std::string& name);

        static std::string defaultFont();

        static std::vector<std::string> fontNames();

    private:
      Rasterizer2* _rasterizer;
};

} // namespace

} // namespace

#endif
