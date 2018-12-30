/***************************************************************************
 *   Copyright (C) 2006 Marc Boris Duerner                                 *
 *   Copyright (c) 2014 Laurentiu-Gheorghe Crisan                          *
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

#ifndef Pt_Hmi_PixmapSurfaceImpl_h
#define Pt_Hmi_PixmapSurfaceImpl_h

#include <Pt/Gfx/Size.h>

#include <CoreGraphics/CGBitmapContext.h>

#ifdef __OBJC__
    #import <AppKit/NSImage.h>
    #import <AppKit/NSColor.h>
#else
    struct NSImage;
#endif

namespace Pt {

namespace Hmi {

class PixmapSurfaceImpl 
{
    public:
        PaintSurfaceImpl();            

        virtual ~PaintSurfaceImpl();

        void clear(const Gfx::Color& c);

        void resize(const Pt::Gfx::SizeF& size);

        inline const Gfx::SizeF& size() const
        { return _size; }

        const Gfx::ImageFormat& format() const;

        inline CGContextRef context() const
        { return _context; }

    private:
        void create();
    
        void destroy();
    
    private:
        Pt::Gfx::SizeF _size;
        CGContextRef _context;
};

}}

#endif
