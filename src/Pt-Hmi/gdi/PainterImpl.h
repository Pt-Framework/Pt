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
#include <Pt/Ui/Painter.h>
#include <Pt/Ui/Font.h>
#include <Pt/Ui/Pen.h>
#include <Pt/Ui/Brush.h>
#include <Pt/Ui/Image.h>
#include <Pt/Ui/Region.h>


#include <windows.h>

namespace Pt {
namespace Hmi {

class PaintSurfaceImpl;
class PaintSurface;

class PainterImpl
{
    public:
        PainterImpl(PaintSurfaceImpl* surface);

        virtual ~PainterImpl();

				void setRenderMode(Ui::RenderMode::Type mode);

        void setPen(const Ui::Pen& pen);

        const Ui::Pen& pen() const;

        void setBrush(const Ui::Brush& brush);

        const Ui::Brush& brush() const;

        void setFont(const Ui::Font& font);

        const Ui::Font& font() const;

        Ui::FontMetrics fontMetrics() const;

        Ui::FontMetrics fontMetrics(Pt::String Text) const;

        const std::list<std::string>& fontFamilyNames();

        int depth() const;

        void drawPixel(const Ui::PointF& to);

        void drawLine(const Ui::PointF& from, const Ui::PointF& to);

				void drawText( const Ui::PointF& to, const Pt::String& text, const Ui::Color* outline );

        void drawText(const Ui::PointF& to, const Pt::String& Text);

        void drawRect(const Ui::RectF& rectangle);

        void fillRect(const Ui::RectF& rectangle);

        void drawEllipse(const Ui::PointF& topLeft, const Ui::SizeF& size);

        void fillEllipse(const Ui::PointF& topLeft, const Ui::SizeF& size);

        void drawPolyline(const Ui::PointF* points, const size_t pointCount);

        void fillPolygon(const Ui::PointF* points, const size_t pointCount);

        void drawSurface(const Ui::PointF& to, PaintSurface& pm, const Ui::Region& pmRegion);

        void drawSurface(const Ui::PointF& to, PaintSurface& pm);
		
        void drawImage(const Ui::PointF& to, const Ui::Image& image);

        void drawImage(const Ui::PointF& to, const Ui::Image& image, const Ui::Region& imageRegion);

        void addFontName(const std::string& fontName);

				void setSurface(PaintSurface& surface);


    protected:
        void drawCompatibleImage(size_t x, size_t y, size_t depth, const char* data, size_t width, size_t height);
        void drawIndependentImage(size_t x, size_t y, const char* data, size_t width, size_t height);

        std::string determinePlatformDefaultFontName();

        void updatePen();
        void updateFont();
        void updateBrush();
        
        Ui::PointF fromOrigin(const Ui::PointF& p);

        Ui::RectF fromOrigin(const Ui::RectF& p);

    protected:
        PaintSurfaceImpl*  _surface;
        Ui::Pen   _pen;
        Ui::Brush _brush;
        Ui::Font  _font;
        mutable std::wstring _text;
        std::list<std::string> _fontNamesList;
				Ui::RenderMode::Type _renderMode;

    private:
        static DWORD toGdiPenStyle( const Ui::Pen& pen );
};

}}

#endif
