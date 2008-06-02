/***************************************************************************
 *   Copyright (C) 2006 Marc Boris Duerner                                 *
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

#ifndef PT_GUI_SYMBIAN_PIXMAPIMPL_H
#define PT_GUI_SYMBIAN_PIXMAPIMPL_H

#include "PixmapPainter.h"
#include <Pt/Gui/Api.h>
#include <Pt/Gui/Painter.h>
#include <Pt/Math/Size.h>

class CFbsBitmap;
class CFbsBitGc;
class CFbsBitmapDevice;

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
            
            // enable drawing to native graphics context                        
            void beginDraw();            
            
            // disable drawing to native gfx context
            void endDraw();            

            // This can return 0 if the bitmap has not been constructed properly
            CFbsBitmap* getNativeBitmap() { return _bitmap; }
            
            void construct();
            void destruct();

            PixmapImpl& operator =(const PixmapImpl& pimpl);
            
        private:            
            Pt::Math::Size _size;
            PixmapPainter _painter;

            // TODO: Use auto_ptr
            CFbsBitmap* _bitmap;
            CFbsBitGc* _bitmapGc;
            CFbsBitmapDevice* _bitmapDevice;
            
            int _lastError;
    };

} // namespace Gui

} // namespace Pt

#endif
