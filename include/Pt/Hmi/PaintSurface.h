/* Copyright (C) 2015 Laurentiu-Gheorghe Crisan
 
 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.
 
 As a special exception, you may use this file as part of a free
 software library without restriction. Specifically, if other files
 instantiate templates or use macros or inline functions from this
 file, or you compile this file and link it with other files to
 produce an executable, this file does not by itself cause the
 resulting executable to be covered by the GNU General Public
 License. This exception does not however invalidate any other
 reasons why the executable file might be covered by the GNU Library
 General Public License.
 
 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
 MA  02110-1301  USA
*/

#ifndef Pt_Hmi_PaintSurface_h
#define Pt_Hmi_PaintSurface_h

#include <Pt/Hmi/Api.h>
#include <Pt/Gfx/Size.h>
#include <Pt/Gfx/Rect.h>

namespace Pt {

namespace Hmi {

class PaintSurfaceImpl;

class PT_HMI_API PaintSurface
{
    public:
        virtual ~PaintSurface();
        
        virtual PaintSurfaceImpl* impl() = 0;

        virtual const PaintSurfaceImpl* impl() const = 0;
    
    protected:
        PaintSurface();
};

class PaintRegionImpl;

/** @brief Drawing region on another surface.
*/
class PaintRegion : public PaintSurface
{
    public:
        PaintRegion(PaintSurface& surface, const Gfx::RectF& rect);

        PaintRegion();

        virtual ~PaintRegion();

        void set(PaintSurface& surface, const Gfx::RectF& rect);

        virtual PaintSurfaceImpl* impl();

        virtual const PaintSurfaceImpl* impl() const;

    private:
        PaintRegionImpl* _impl;
};

class PixmapSurfaceImpl;

/** @brief A back buffer drawing surface.
*/
class PixmapSurface : public PaintSurface
{
    public:
        PixmapSurface();

        virtual ~PixmapSurface();

        void resize(const Gfx::SizeF& size);        

        Gfx::SizeF size() const;

        void clear();

        virtual PaintSurfaceImpl* impl();

        virtual const PaintSurfaceImpl* impl() const;

        PixmapSurfaceImpl* pixmapImpl() const;

    private:
        PixmapSurfaceImpl* _impl;
};

} // namespace

} // namespace

#endif
