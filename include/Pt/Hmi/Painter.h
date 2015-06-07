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
#include <Pt/Ui/FontMetrics.h>
#include <Pt/Ui/Painter.h>

namespace Pt {
namespace Hmi {

class PainterImpl;
class PaintSurface;

class PT_HMI_API Painter : public Pt::Ui::Painter
{
public:
	Painter(PaintSurface& surface);

    virtual ~Painter();

		virtual void setRenderMode(Ui::RenderMode::Type mode);

    virtual void setPen(const Ui::Pen& pen);

    virtual const Ui::Pen& pen() const;

    virtual void setBrush(const Ui::Brush& brush);

    virtual const Ui::Brush& brush() const;

    virtual void setFont(const Ui::Font& font);

    virtual const Ui::Font& font() const;

    virtual Ui::FontMetrics fontMetrics() const;

    virtual Ui::FontMetrics fontMetrics(Pt::String Text) const;

    virtual const std::list<std::string>& fontFamilyNames();

    virtual int depth() const;

    virtual void drawPixel(const Ui::PointF& to);

    virtual void drawLine(const Ui::PointF& from, const Ui::PointF& to);

	virtual void drawText( const Ui::PointF& to, const Pt::String& text, const Ui::Color* outline );

    virtual void drawText(const Ui::PointF& to, const Pt::String& Text);

    virtual void drawRect(const Ui::RectF& rectangle);

    virtual void fillRect(const Ui::RectF& rectangle);

    virtual void drawEllipse(const Ui::PointF& topLeft, const Ui::SizeF& size);

    virtual void fillEllipse(const Ui::PointF& topLeft, const Ui::SizeF& size);

    virtual void drawPolyline(const Ui::PointF* points, const size_t pointCount);

    virtual void fillPolygon(const Ui::PointF* points, const size_t pointCount);

    virtual void drawSurface(const Ui::PointF& to, PaintSurface& pm, const Ui::Region& pmRegion);

    virtual void drawSurface(const Ui::PointF& to, PaintSurface& pm);
		
    virtual void drawImage(const Ui::PointF& to, const Ui::Image& image);

    virtual void drawImage(const Ui::PointF& to, const Ui::Image& image, const Ui::Region& imageRegion);      

    virtual void addFontName(const std::string& fontName);

    void setSurface(PaintSurface& surface);

private:
	PainterImpl* _impl;
};

}}

#endif
