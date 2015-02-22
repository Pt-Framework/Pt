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
#include <Pt/Hmi/Painter.h>
#include <Pt/Gfx/FontMetrics.h>
#include <Pt/Gfx/Painter.h>

namespace Pt {
namespace Hmi {

class PainterImpl;
class PaintSurface;

class PT_HMI_API Painter : public Pt::Gfx::Painter
{
public:
	Painter(PaintSurface& surface);

    virtual ~Painter();

    virtual void setPen(const Gfx::Pen& pen);

    virtual const Gfx::Pen& pen() const;

    virtual void setBrush(const Gfx::Brush& brush);

    virtual const Gfx::Brush& brush() const;

    virtual void setFont(const Gfx::Font& font);

    virtual const Gfx::Font& font() const;

    virtual Gfx::FontMetrics fontMetrics() const;

    virtual Gfx::FontMetrics fontMetrics(Pt::String Text) const;

    virtual const std::list<std::string>& fontFamilyNames();

    virtual int depth() const;

    virtual void drawPixel(const Pt::Gfx::PointF& to);

    virtual void drawLine(const Pt::Gfx::PointF& from, const Pt::Gfx::PointF& to);

	virtual void drawText( const Gfx::PointF& to, const Pt::String& text, const Gfx::ARgbColor* outline );

    virtual void drawText(const Pt::Gfx::PointF& to, const Pt::String& Text);

    virtual void drawRect(const Pt::Gfx::RectF& rectangle);

    virtual void fillRect(const Pt::Gfx::RectF& rectangle);

    virtual void drawEllipse(const Pt::Gfx::PointF& topLeft, const Pt::Gfx::SizeF& size);

    virtual void fillEllipse(const Pt::Gfx::PointF& topLeft, const Pt::Gfx::SizeF& size);

    virtual void drawPolyline(const Pt::Gfx::PointF* points, const size_t pointCount);

    virtual void fillPolygon(const Pt::Gfx::PointF* points, const size_t pointCount);

    virtual void drawSurface(const Pt::Gfx::PointF& to, PaintSurface& pm, const Pt::Gfx::Region& pmRegion);

    virtual void drawSurface(const Pt::Gfx::PointF& to, PaintSurface& pm);
		
    virtual void drawImage(const Pt::Gfx::PointF& to, const Gfx::ARgbImage& image);

    virtual void drawImage(const Pt::Gfx::PointF& to, const Gfx::ARgbImage& image, const Pt::Gfx::Region& imageRegion);      

    virtual void addFontName(const std::string& fontName);

    void setSurface(PaintSurface& surface);

private:
	PainterImpl* _impl;
};

}}

#endif
