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

#ifndef Pt_Hmi_PaintSurfaceImpl_h
#define Pt_Hmi_PaintSurfaceImpl_h

#include <Pt/Hmi/Window.h>
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Size.h>
#include <Pt/Gfx/Image.h>
#include <Windows.h>

namespace Pt {

namespace Hmi {

class PaintSurfaceImpl
{
    public:        
        virtual ~PaintSurfaceImpl()
        {}   

        virtual const Gfx::SizeF& size() const = 0;
    
        virtual HDC deviceContext() const = 0;

        virtual Pt::Gfx::Point toDevice(const Pt::Gfx::PointF&) const = 0;

        virtual Gfx::Rect toDevice(const Gfx::RectF&) const = 0;

        virtual Gfx::Size toDevice(const Gfx::SizeF&) const = 0;

    protected:
        PaintSurfaceImpl()
        {}
};


class PaintAreaImpl : public PaintSurfaceImpl
{
    public:        
        PaintAreaImpl();
        
        virtual ~PaintAreaImpl();   

        void set(PaintSurface& surface, const Gfx::RectF& area); 

        virtual const Gfx::SizeF& size() const
        {
            return _area.size();
        }
    
        virtual HDC deviceContext() const;

        virtual Gfx::Point toDevice(const Gfx::PointF& p) const;

        virtual Gfx::Rect toDevice(const Gfx::RectF& p) const;

        virtual Gfx::Size toDevice(const Gfx::SizeF&) const;

    private:
        PaintSurface* _surface;
        Gfx::RectF _area;
};


class PixmapSurfaceImpl : public PaintSurfaceImpl
{
    public:        
        PixmapSurfaceImpl();
        
        virtual ~PixmapSurfaceImpl();    
    
        void resize(const Gfx::SizeF& size);    

        void clear()
        {
        }

        virtual const Gfx::SizeF& size() const
        {
            return _size;
        }

        virtual HDC deviceContext() const;

        virtual Gfx::Point toDevice(const Gfx::PointF& p) const;

        virtual Gfx::Rect toDevice(const Gfx::RectF& p) const;

        virtual Gfx::Size toDevice(const Gfx::SizeF&) const;

    private:
        Gfx::SizeF _size;
        HDC        _deviceContext;
        HBITMAP    _bitmapHandle;            
        HPEN       _oldPen;
        HBRUSH     _oldBrush;
        HFONT      _oldFont;
};

} // namespace

} // namespace

#endif
