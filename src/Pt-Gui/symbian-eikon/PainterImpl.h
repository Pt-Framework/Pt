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
class CEikonEnv;

#include <e32std.h>

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

            // will be called from Widget/Pixmap when the connection
            // to the window/font server is about to be destroyed
            void destructResources();
            
            virtual void begin();

            virtual void end();

            virtual void cleanUp();
            
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

            struct ContextInfo
            {
                CGraphicsContext* _gc;
                CGraphicsDevice* _device;
                const CFont* _nativeFont;
                CEikonEnv* _coeEnv;
                TPoint _offset;
                TRect _clipRect;
                
                ContextInfo() 
                : _gc(0)
                , _device(0)
                , _nativeFont(0)
                , _coeEnv(0)
                , _offset(0, 0)
                , _clipRect(0, 0, 0, 0)
                {                    
                }
            };
                        
        protected:
            void applyContextInfo(const ContextInfo& contextInfo);
            
            template <typename Iterator>
            void drawImage(ssize_t x, ssize_t y, Iterator begin, Iterator end, size_t width, size_t height)
            {
                Gfx::Rgb888Image rgb32Image(width, height);
                assign(begin, end, rgb32Image.begin());
                drawCompatibleImage(x, y, (char*)rgb32Image.data(), rgb32Image.width(), rgb32Image.height());
            }

            void drawCompatibleImage(size_t x, size_t y, const char* data, size_t width, size_t height);

            void updatePen();
            void activatePen();

            void updateFont();
            void activateFont();

            void updateBrush();
            void activateBrush();

            bool ensureActiveContext();
            
            void freeFont();
            
        protected:
            Gfx::Pen _pen;
            Gfx::Brush _brush;
            Gfx::Font  _font;

            Gfx::Pen _oldPen;
            Gfx::Brush _oldBrush;
            Gfx::Font  _oldFont;

            Gfx::Pen* _oldPenRef;
            Gfx::Brush* _oldBrushRef;
            Gfx::Font*  _oldFontRef;
            
            CGraphicsContext* _gc;
            CGraphicsDevice* _device;
            const CFont* _nativeFont;
            bool _fontOwner;
            CEikonEnv* _coeEnv;
            TPoint _offset;
            TRect _clipRect;
            
            // TODO: Use auto_ptr
            CFbsBitmap* _brushBitmap;
            CFbsBitmap* _drawBitmap;            
    };

} // namespace Gui

} // namespace Pt
#endif
