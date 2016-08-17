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
    
        void resize(const Gfx::SizeF& size);    

        virtual const Gfx::SizeF& size() const;

        virtual void setClip( const Gfx::RectF& clip);
        
        virtual  const Gfx::RectF& clip();
         
        virtual void setCompositionMode(const Gfx::CompositionMode& mode)
        {
            _compositionMode = mode; 
        }

        virtual const Gfx::CompositionMode& compositionMode() const
        {
            return _compositionMode;
        }

        virtual void setPen(const Gfx::Pen& pen);

        virtual void setBrush(const Gfx::Brush& brush);

        virtual void setFont(const Gfx::Font& font);

        virtual Gfx::FontMetrics fontMetrics(const Pt::String& text) const;

        virtual void drawLine(const Gfx::PointF& from, const Gfx::PointF& to);

        virtual void drawText(const Gfx::PointF& to, const Pt::String& Text);

        virtual void drawRect(const Gfx::RectF& rectangle);

        virtual void fillRect(const Gfx::RectF& rectangle);

        virtual void drawEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size);

        virtual void fillEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size);

        virtual void drawPolyline(const Gfx::PointF* points, size_t pointCount);

        virtual void fillPolygon(const Gfx::PointF* points, size_t pointCount);

        virtual void drawSurface(const Gfx::PointF& toF, const PixmapSurface& surface);

        virtual void drawSurface(const Gfx::PointF& toF, 
                                 const PixmapSurface& pm,
                                 const Gfx::RectF& pmRect);

        virtual void drawImage(const Gfx::PointF& to, const Gfx::Image& image);

        virtual void drawImage(const Gfx::PointF& to, const Gfx::Image& image, const Gfx::RectF& imgRect);

        virtual void drawPicture(const Gfx::PointF& to, const Picture& pic);


        HDC deviceContext() const;

    private: 
        void bitBlit( const Gfx::PointF& pos, size_t width, size_t height, HBITMAP bitmap, DWORD op );

    private:
        Gfx::SizeF     _size;
        HDC            _deviceContext;
        HBITMAP        _bitmapHandle;
        HPEN           _oldPen;
        HBRUSH         _oldBrush;
        HFONT          _oldFont;
        std::wstring   _text;
        
        bool                  _gradientBrush;
        Gfx::Brush::FillStyle _gradientStyle;
        Gfx::Color            _gradientStart;
        Gfx::Color            _gradientStop;
        Gfx::CompositionMode  _compositionMode;
        Gfx::RectF            _clip;
};

}}

#endif
