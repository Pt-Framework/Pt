/*
 * Copyright (C) 2014 Laurentiu-Gheorghe Crisan
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
 * MA 02110-1301 USA
 */

#ifndef Pt_Hmi_PixmalSurfaceImpl_h
#define Pt_Hmi_PixmalSurfaceImpl_h

#include "PaintData.h"

#include <Pt/Gfx/PaintSurface.h>
#include <Pt/Gfx/Painter.h>
#include <Pt/Gfx/Brush.h>
#include <Pt/Gfx/Color.h>
#include <vector>
#include <Windows.h>

namespace Pt {

namespace Hmi {

class PixmapSurfaceImpl
{
    public:
        PixmapSurfaceImpl();

        virtual ~PixmapSurfaceImpl();
        
        void clear(const Gfx::Color& c);

        void resize(const Gfx::SizeF& size);

        const Gfx::SizeF& size() const;

        void begin(Gfx::Painter& painter);
        
        void finish();
        
        const Gfx::ImageFormat& format() const;

        void setClip( const Gfx::RectF& clip);

        void resetClip();
         
        void setCompositionMode(const Gfx::CompositionMode& mode);

        void setPen(const Gfx::Pen& pen);

        void setBrush(const Gfx::Brush& brush);

        void setFont(const Gfx::Font& font);

        Gfx::FontMetrics fontMetrics(const Pt::String& text) const;

        void drawLine(const Gfx::PointF& from, const Gfx::PointF& to);

        void drawText(const Gfx::PointF& to, const Pt::String& text, 
                      const Gfx::Transform& trans);

        void drawRect(const Gfx::RectF& rectangle);

        void fillRect(const Gfx::RectF& rectangle);

        void drawEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size);

        void fillEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size);

        void drawPolyline(const Gfx::PointF* points, size_t pointCount);

        void fillPolygon(const Gfx::PointF* points, size_t pointCount);

        void drawPath(const Gfx::Path& path, float smoothness)
        {}

        void fillPath(const Gfx::Path& path, float smoothness)
        {}

        void drawSurface(const Gfx::PointF& toF, const PixmapSurface& surface);

        void drawSurface(const Gfx::PointF& toF, const PixmapSurface& pm, const Gfx::RectF& pmRect);


        void drawImage(const Gfx::PointF& to, const Gfx::Image& image);

        void drawImage(const Gfx::PointF& to, 
                       const Gfx::Image& image, 
                       const Gfx::RectF& imgRect);

        Gfx::Image toImage(const Gfx::ImageFormat& format) const;

        void set(const Gfx::Image& image);

        static std::string defaultFont();

        static void setDefaultFont(const std::string& name);

        static std::string& getDefaultFont();

        static std::vector<std::string> fontNames();

        static void setFontDir(const System::Path& path);

        static Gfx::FontMetrics fontMetrics(const Gfx::Font& font, const Pt::String& text);

        HDC deviceContext() const;

    private: 
        void bitBlit(const Gfx::Point& pos, size_t width, size_t height, 
                     HBITMAP bitmap, DWORD op);

        static void toPreMulAlpha(const Pt::Gfx::Image& image, 
                                  std::vector<Pt::uint8_t>& preMul);

    private:
        Gfx::SizeF     _size;
        PaintData*     _paintData;
        Gfx::Painter*  _painter;
        HDC            _dc;
        HBITMAP        _bitmap;
        HPEN           _oldPen;
        HBRUSH         _oldBrush;
        HFONT          _oldFont;
        HBITMAP        _oldBitmap;
        std::wstring   _text;

        bool                      _gradientBrush;
        Gfx::Brush::GradientStyle _gradient;
        Gfx::Color                _gradientStart;
        Gfx::Color                _gradientStop;
        Gfx::CompositionMode      _compositionMode;
};

} // namespace

} // namespace

#endif
