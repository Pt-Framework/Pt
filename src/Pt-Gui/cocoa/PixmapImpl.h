/***************************************************************************
 *   Copyright (C) 2006 Marc Boris Duerner                                  *
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
#ifndef Pt_Gui_cocoa_PixmapImpl_h
#define Pt_Gui_cocoa_PixmapImpl_h

#include "PixmapPainter.h"
#include <Pt/Gui/Api.h>
#include <Pt/Gui/Painter.h>
#include <Pt/Math/Size.h>

#ifdef __OBJC__
    #import <AppKit/NSImage.h>
    #import <AppKit/NSColor.h>
#else
    struct NSImage;
#endif

namespace Pt {

namespace Gui {

    class PixmapImpl {
        public:
            PixmapImpl(size_t width, size_t height);

            PixmapImpl(const PixmapImpl& pimpl);

            virtual ~PixmapImpl();

            const Math::Size& size() const
            { return _size; }

            Painter painter();

            NSImage* image() const
            { return _image; }
        
        private:
            Pt::Math::Size _size;
            NSImage* _image;
            PixmapPainter _painter;
    };

} // namespace Gui

} // namespace Pt

#endif
