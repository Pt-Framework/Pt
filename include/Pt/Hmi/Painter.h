/***************************************************************************
 *   Copyright (C) 2006 Marc Boris Duerner                                 *
 *   Copyright (C) 2014 Laurentiu-Gheorghe Crisan						   * 
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
#ifndef PT_HMI_PAINTER_H
#define PT_HMI_PAINTER_H

#include <Pt/Hmi/Api.h>
#include <Pt/Gfx/Painter.h>
#include <Pt/Gfx/FontMetrics.h>

namespace Pt {
namespace Hmi {

class PainterImpl;
class PaintSurface;

class PT_HMI_API Painter : public Gfx::Painter
{
public:
    Painter(PaintSurface& surface);

    virtual ~Painter();

    void setPen(const Gfx::Pen& pen);

    const Gfx::Pen& pen() const;

    void setBrush(const Gfx::Brush& brush);

    const Gfx::Brush& brush() const;

    void setFont(const Gfx::Font& font);

    const Gfx::Font& font() const;

    Gfx::FontMetrics fontMetrics() const;

    Gfx::FontMetrics fontMetrics(Pt::String Text) const;

    const std::list<std::string>& fontFamilyNames();

    int depth() const;

    void drawPixel(const Pt::Gfx::Point& to);

    void drawLine(const Pt::Gfx::Point& from, const Pt::Gfx::Point& to);

	void drawText( const Gfx::Point& to, const Pt::String& text, const Gfx::ARgbColor* outline );

    void drawText(const Pt::Gfx::Point& to, const Pt::String& Text);

    void drawRect(const Pt::Gfx::Rect& rectangle);

    void fillRect(const Pt::Gfx::Rect& rectangle);

    void drawEllipse(const Pt::Gfx::Point& topLeft, const Pt::Gfx::Size& size);

    void fillEllipse(const Pt::Gfx::Point& topLeft, const Pt::Gfx::Size& size);

    void drawPolyline(const Pt::Gfx::Point* points, const size_t pointCount);

    void fillPolygon(const Pt::Gfx::Point* points, const size_t pointCount);

    void drawSurface(const Pt::Gfx::Point& to, PaintSurface& pm, const Pt::Gfx::Region& pmRegion);

    void drawSurface(const Pt::Gfx::Point& to, PaintSurface& pm);
		
    void drawImage(const Pt::Gfx::Point& to, const Gfx::ARgbImage& image);

    void drawImage(const Pt::Gfx::Point& to, const Gfx::ARgbImage& image, const Pt::Gfx::Region& imageRegion);      

    void addFontName(const std::string& fontName);

private:
	PainterImpl* _impl;
};

}}

#endif
