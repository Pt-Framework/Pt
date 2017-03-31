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

#include <Pt/Hmi/PaintSurface.h>
#include <Pt/Hmi/Picture.h>
#include <Pt/Gfx/Brush.h>
#include <Pt/Gfx/Color.h>
#include <Windows.h>
#include "PaintSurfaceImpl.h"

namespace Pt {
namespace Hmi {

class PixmapSurfaceImpl : public PaintSurfaceImpl
{
    public:        
        PixmapSurfaceImpl();
        
        virtual ~PixmapSurfaceImpl();  
        
        void clear(const Gfx::Color& c);  
    
        void resize(const Gfx::Size& size);
        
        const Gfx::Size& size() const;

        void begin(Painter& painter);  
        
        void finish();           
        
        const Gfx::ImageFormat& format() const;

        void setClip( const Gfx::Rect& clip);
         
        void setCompositionMode(const Gfx::CompositionMode& mode);

        void setPen(const Gfx::Pen& pen);

        void setBrush(const Gfx::Brush& brush);

        void setFont(const Gfx::Font& font);

        Gfx::FontMetrics fontMetrics(const Pt::String& text) const;

        void drawLine(const Gfx::Point& from, const Gfx::Point& to);

        void drawText(const Gfx::Point& to, const Pt::String& Text);

        void drawRect(const Gfx::Rect& rectangle);

        void fillRect(const Gfx::Rect& rectangle);

        void drawEllipse(const Gfx::Point& topLeft, const Gfx::Size& size);

        void fillEllipse(const Gfx::Point& topLeft, const Gfx::Size& size);

        void drawPolyline(const Gfx::Point* points, size_t pointCount);

        void fillPolygon(const Gfx::Point* points, size_t pointCount);

        void drawSurface(const Gfx::Point& toF, const PixmapSurface& surface);

        void drawSurface(const Gfx::Point& toF, 
                                 const PixmapSurface& pm,
                                 const Gfx::Rect& pmRect);

        void drawImage(const Gfx::Point& to, const Gfx::Image& image);

        void drawImage(const Gfx::Point& to, const Gfx::Image& image, const Gfx::Rect& imgRect);

        void drawPicture(const Gfx::Point& to, const Picture& pic);

        HDC deviceContext() const;

    private: 
        void bitBlit( const Gfx::Point& pos, size_t width, size_t height, HBITMAP bitmap, DWORD op );

    private:
        Gfx::Size     _size;
        Painter*       _painter;
        HDC            _dc;
        HBITMAP        _bitmap;
        HPEN           _oldPen;
        HBRUSH         _oldBrush;
        HFONT          _oldFont;
        HBITMAP        _oldBitmap;
        std::wstring   _text;
        
        bool                  _gradientBrush;
        Gfx::Brush::FillStyle _gradientStyle;
        Gfx::Color            _gradientStart;
        Gfx::Color            _gradientStop;
};

} // namespace

} // namespace

#endif
