/***************************************************************************
 *   Copyright (C) 2006 Marc Boris Dürner                                  *
 *   Copyright (C) 2008 Peter Barth                                        *
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

#ifndef PT_GUI_SYMBIAN_PAINTERIMPL_H
#define PT_GUI_SYMBIAN_PAINTERIMPL_H

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

class CGraphicsContext;
class CGraphicsDevice;
class CFont;
class CFbsBitmap;

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

            virtual void drawPixel(const Math::Point& to);

            virtual void drawLine(const Math::Point& from, const Math::Point& to);

            virtual void drawText(const Math::Point& to, const Pt::String& text);

            virtual void drawRect(const Gfx::Rect& rect);

            virtual void drawEllipse(const Math::Point& topLeft, const Math::Size& size);

            virtual void drawPolyline(const Math::Point* points, const size_t pointCount);

            virtual void fillRect(const Gfx::Rect& rect);

            virtual void fillEllipse(const Math::Point& topLeft, const Math::Size& size);

            virtual void fillPolygon(const Math::Point* points, const size_t pointCount);

            virtual void drawPixmap(const Math::Point& to, Pixmap& pm);

            virtual void drawPixmap(const Math::Point& to, Pixmap& pm, const Gfx::Region& pmRegion);

            void drawImage(const Pt::Math::Point& to, const Gfx::ARgbImage& image);
            
            void drawImage(const Pt::Math::Point& to, const Gfx::ARgb8888Image& image);

            void drawImage(const Pt::Math::Point& to, const Gfx::ARgbImage& image, const Pt::Gfx::Region& imageRegion);
            
            void drawImage(const Pt::Math::Point& to, const Gfx::ARgb8888Image& image, const Pt::Gfx::Region& imageRegion);

            void setGc(CGraphicsContext* gc) { _gc = gc; }
            void setDevice(const CGraphicsDevice* device) { _device = device; }
            void setNativeFont(const CFont* font) { _nativeFont = font; }
            void setOffset(const TPoint& offset) { _offset = offset; }
            
        protected:
            template <typename Iterator>
            void drawImage(ssize_t x, ssize_t y, Iterator begin, Iterator end, size_t width, size_t height)
            {
                Gfx::Rgb888Image rgb32Image(width, height);
                assign(begin, end, rgb32Image.begin());
                drawCompatibleImage(x, y, (char*)rgb32Image.data(), rgb32Image.width(), rgb32Image.height());
            }

            void drawCompatibleImage(size_t x, size_t y, const char* data, size_t width, size_t height);

            void updatePen();
            void updateFont();
            void updateBrush();

            bool ensureActiveContext();
            
        protected:
            Gfx::Pen _pen;
            Gfx::Brush _brush;
            Gfx::Font  _font;
            
            CGraphicsContext* _gc;
            const CGraphicsDevice* _device;
            const CFont* _nativeFont;
            TPoint _offset;
            
            // TODO: Use auto_ptr
            CFbsBitmap* _brushBitmap;
            CFbsBitmap* _drawBitmap;
            
            //std::vector<Paint*> _paintQueue;
    };

//    class PainterImpl::Paint
//    {
//        public:
//            virtual ~Paint() {}
//            virtual void paint() = 0;
//    };
//
//    class PainterImpl::DrawLine : public PainterImpl::Paint
//    {
//        public:
//            DrawLine(const Math::Point& from, const Math::Point& to, const Gfx::Pen& pen);
//            
//            virtual ~DrawLine();
//            
//            virtual void paint();
//        
//        private:
//            Math::Point _from;
//            Math::Point _to;
//            Gfx::Pen _pen;
//    };
//
//
//    class PainterImpl::DrawRect : public PainterImpl::Paint
//    {
//        public:
//            DrawRect(const Gfx::Rect& rect, const Gfx::Pen& pen);
//            
//            virtual ~DrawRect();
//            
//            virtual void paint();
//        
//        private:
//            const Gfx::Rect _rect;
//            Gfx::Pen _pen;
//    };
//
//
//    class PainterImpl::DrawPixmap : public PainterImpl::Paint
//    {
//        public:
//            DrawPixmap(const Math::Point& to, Pixmap& pm, const Gfx::Region& region);
//            
//            virtual ~DrawPixmap();
//            
//            virtual void paint();
//        
//        private:
//            Math::Point _to;
//            Gfx::Region _region;
//    };
//
//
//    class PainterImpl::FillRect : public PainterImpl::Paint
//    {
//        public:
//            FillRect(const Gfx::Rect& rect, const Gfx::Brush& brush);
//            
//            virtual ~FillRect();
//            
//            virtual void paint();
//        
//        private:
//            const Gfx::Rect _rect;
//            Gfx::Brush _brush;
//    };

} // namespace Gui

} // namespace Pt
#endif
