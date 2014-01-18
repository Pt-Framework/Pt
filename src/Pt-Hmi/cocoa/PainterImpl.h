/***************************************************************************
 *   Copyright (C) 2006 Marc Boris Duerner                                 *
 *   Copyright (C) 2005-2007 Aloysius Indrayanto                           *
 *   Copyright (C) 2014 Laurentiu-Gheorghe Crisan                          *
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
#ifndef Pt_Hmi_cocoa_PainterImpl_h
#define Pt_Hmi_cocoa_PainterImpl_h

#include <Pt/Gui/Api.h>
#include <Pt/Gfx/Pen.h>
#include <Pt/Gfx/Brush.h>
#include <Pt/Gfx/Font.h>
#include <Pt/Gfx/Rect.h>
#include <Pt/Gfx/Region.h>
#include <Pt/Gfx/Algorithm.h>
#include <Pt/Gfx/ARgbImage.h>
#include <Pt/Gfx/Rgb888Image.h>
#include <Pt/Gfx/Rgb565Image.h>
#include <Pt/Gfx/Rgb555Image.h>
#include <Pt/String.h>
#include <vector>
#include <list>

#ifdef __OBJC__
    #import <Foundation/NSGeometry.h>
    #import <AppKit/NSGraphicsContext.h>
    #import <AppKit/NSBezierPath.h>
    #import <AppKit/NSImage.h>
#else
    struct NSBezierPath;
    struct NSImage;
#endif

namespace Pt {
namespace Hmi {

class PaintSurfaceImpl;
class PaintSurface;

class PainterImpl
{
    public:
        PainterImpl(PaintSurfaceImpl* surface);

        virtual ~PainterImpl();

        void setPen(const Gfx::Pen& pen);

        const Gfx::Pen& pen() const;

        void setBrush(const Gfx::Brush& brush);

        const Gfx::Brush& brush() const;

        const Gfx::Font& font() const;

        void setFont(const Gfx::Font& font);

        Gfx::FontMetrics fontMetrics() const;

        Gfx::FontMetrics fontMetrics(const Pt::String& text) const;

        const std::list<std::string>& fontFamilyNames();

        virtual void drawPixel(const Gfx::PointF& to);

        virtual void drawLine(const Gfx::PointF& from, const Gfx::PointF& to);

        virtual void drawText(const Gfx::PointF& to, const Pt::String& text);

        virtual void drawRect(const Gfx::RectF& rect);

        virtual void drawEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size);

        virtual void drawPolyline(const Gfx::PointF* points, const size_t pointCount);

        virtual void fillRect(const Gfx::RectF& rect);

        virtual void fillEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size);

        virtual void fillPolygon(const Gfx::PointF* points, const size_t pointCount);

        virtual void drawSurface(const Gfx::PointF& to, PaintSurface& pm);

        virtual void drawSurface(const Gfx::PointF& to, PaintSurface& pm, const Gfx::Region& pmRegion);

        virtual void drawImage(const Gfx::PointF& to, const Gfx::ARgbImage& image);

        virtual void drawImage(const Gfx::PointF& to, const Gfx::ARgbImage& image, const Gfx::Region& imageRegion);

	protected:
        Gfx::Pen _pen;
        Gfx::Brush _brush;
        Gfx::Font  _font;  
};

}}

#endif
