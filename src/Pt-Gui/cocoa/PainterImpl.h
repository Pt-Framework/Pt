/***************************************************************************
 *   Copyright (C) 2006 Marc Boris Duerner                                 *
 *   Copyright (C) 2005-2007 by Aloysius Indrayanto                        *
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
#ifndef Pt_Gui_cocoa_PainterImpl_h
#define Pt_Gui_cocoa_PainterImpl_h

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

namespace Gui {

	class Pixmap;

    class PainterImpl
    {
        class Paint;
        class DrawLine;
        class DrawRect;
        class DrawPixmap;
        class FillRect;

        public:
            PainterImpl();

            virtual ~PainterImpl();

            virtual void begin();

            virtual void end();

            void setPen(const Gfx::Pen& pen);

            const Gfx::Pen& pen() const;

            void setBrush(const Gfx::Brush& brush);

            const Gfx::Brush& brush() const;

            const Gfx::Font& font() const;

            void setFont(const Gfx::Font& font);

            Gfx::FontMetrics fontMetrics() const;

            Gfx::FontMetrics fontMetrics(const Pt::String& text) const;

            const std::list<std::string>& fontFamilyNames();

            virtual void drawPixel(const Gfx::Point& to);

            virtual void drawLine(const Gfx::Point& from, const Gfx::Point& to);

            virtual void drawText(const Gfx::Point& to, const Pt::String& text);

            virtual void drawRect(const Gfx::Rect& rect);

            virtual void drawEllipse(const Gfx::Point& topLeft, const Gfx::Size& size);

            virtual void drawPolyline(const Gfx::Point* points, const size_t pointCount);

            virtual void fillRect(const Gfx::Rect& rect);

            virtual void fillEllipse(const Gfx::Point& topLeft, const Gfx::Size& size);

            virtual void fillPolygon(const Gfx::Point* points, const size_t pointCount);

            virtual void drawPixmap(const Gfx::Point& to, Pixmap& pm);

            virtual void drawPixmap(const Gfx::Point& to, Pixmap& pm, const Gfx::Region& pmRegion);

            virtual void drawImage(const Gfx::Point& to, const Gfx::ARgbImage& image);

            virtual void drawImage(const Gfx::Point& to, const Gfx::ARgbImage& image,
                                       const Gfx::Region& imageRegion);

        protected:
            template <typename Iterator>
            void drawImage(ssize_t toX, ssize_t toY, Iterator begin, Iterator end, size_t width, size_t height)
            {

            }

            void copyImageData(ssize_t toX, ssize_t toY, const char* data, size_t fromWidth, size_t fromHeight);

		protected:
            Gfx::Pen _pen;
            Gfx::Brush _brush;
            Gfx::Font  _font;
            std::vector<Paint*> _paintQueue;
    };

    class PainterImpl::Paint
    {
        public:
            virtual ~Paint() {}
            virtual void paint() = 0;
    };

    class PainterImpl::DrawLine : public PainterImpl::Paint
    {
        public:
            DrawLine(const Gfx::Point& from, const Gfx::Point& to, const Gfx::Pen& pen);

            virtual ~DrawLine();

            virtual void paint();

        private:
            Gfx::Point _from;
            Gfx::Point _to;
            Gfx::Pen _pen;
    };


    class PainterImpl::DrawRect : public PainterImpl::Paint
    {
        public:
            DrawRect(const Gfx::Rect& rect, const Gfx::Pen& pen);

            virtual ~DrawRect();

            virtual void paint();

        private:
            const Gfx::Rect _rect;
            Gfx::Pen _pen;
    };


    class PainterImpl::DrawPixmap : public PainterImpl::Paint
    {
        public:
            DrawPixmap(const Gfx::Point& to, Pixmap& pm, const Gfx::Region& region);

            virtual ~DrawPixmap();

            virtual void paint();

        private:
            Gfx::Point _to;
            Gfx::Region _region;
            NSImage* _image;
    };


    class PainterImpl::FillRect : public PainterImpl::Paint
    {
        public:
            FillRect(const Gfx::Rect& rect, const Gfx::Brush& brush);

            virtual ~FillRect();

            virtual void paint();

        private:
            const Gfx::Rect _rect;
            Gfx::Brush _brush;
    };

} // namespace Gui

} // namespace Pt


#endif
