/***************************************************************************
 *   Copyright (C) 2006 Marc Boris Duerner                                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef PT_HMI_GDI_PAINTERIMPL_H
#define PT_HMI_GDI_PAINTERIMPL_H

#include <Pt/Api.h>
#include <Pt/Gfx/Painter.h>
#include <Pt/Gfx/Font.h>
#include <Pt/Gfx/Pen.h>
#include <Pt/Gfx/Brush.h>
#include <Pt/Gfx/Image.h>
#include <windows.h>

namespace Pt {
namespace Hmi {

class PaintSurfaceImpl;
class PaintSurface;
class Screen;

class PainterImpl : public Gfx::Painter
{
    public:
        PainterImpl(PaintSurfaceImpl* surface);

        virtual ~PainterImpl();

        void setSurface(PaintSurface& surface);

        void setOrigin(const Gfx::PointF& p)
        {
            _origin = p;
        }

        void setClip( const Gfx::RectF& clip )
        {
            _clip = clip;
        }
        
        const Gfx::RectF& clip() const
        {
            return _clip;
        }

        void setRenderMode(Gfx::RenderMode::Type mode);

        void setPen(const Gfx::Pen& pen);

        const Gfx::Pen& pen() const;

        void setBrush(const Gfx::Brush& brush);

        const Gfx::Brush& brush() const;

        void setFont(const Gfx::Font& font);

        const Gfx::Font& font() const;        

        Gfx::FontMetrics fontMetrics(const Pt::String& Text) const;

        static Gfx::FontMetrics fontMetrics(const Gfx::Font& font, const Pt::String& text);

        const std::list<std::string>& fontFamilyNames();        

        void drawLine(const Gfx::PointF& from, const Gfx::PointF& to);                

        void drawText(const Gfx::PointF& to, const Pt::String& Text);

        void drawRect(const Gfx::RectF& rectangle);

        void fillRect(const Gfx::RectF& rectangle);

        void drawEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size);

        void fillEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size);

        void drawPolyline(const Gfx::PointF* points, const size_t pointCount);

        void fillPolygon(const Gfx::PointF* points, const size_t pointCount);

        void drawSurface(const Gfx::PointF& to, const PaintSurface& pm);
        
        void drawImage(const Gfx::PointF& to, const Gfx::Image& image);

        void addFontName(const std::string& fontName);

        void flush()
        {
        }
        
        void clear(const Gfx::Color& color);

    protected:
        void drawCompatibleImage(size_t x, size_t y, size_t depth, const char* data, size_t width, size_t height);
        void drawIndependentImage(size_t x, size_t y, const char* data, size_t width, size_t height);

        std::string determinePlatformDefaultFontName();

        void updatePen();
        void updateFont();
        void updateBrush();
        
    private:
        Screen&                _screen;
        PaintSurfaceImpl*      _surface;

        Gfx::PointF            _origin;
        Gfx::RectF             _clip;

        Gfx::Pen               _pen;
        Gfx::Brush             _brush;
        Gfx::Font              _font;

        mutable std::wstring   _text;
        std::list<std::string> _fontNamesList;
        Gfx::RenderMode::Type  _renderMode;
};

}

}

#endif
