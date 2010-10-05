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

#include "Drawable.h"
#include "PainterImpl.h"
#include "ApplicationImpl.h"
#include <Pt/Gui/Api.h>
#include <Pt/Gui/Painter.h>
#include <Pt/Gfx/Size.h>

class CFbsBitmap;
class CFbsBitGc;
class CFbsBitmapDevice;

namespace Pt {

namespace Gui {

    /**
     * @brief Pixmap implementation using symbian CFbsBitmap
     *
     * The underlying resources can only be allocated from the OS
     * when there is a connection to the bitmap server available.
     * This is usually provided by the Eikon application framework.
     */
    class PixmapImpl : public Drawable, public Resource
    {
        public:
            /**
             * @brief Construct pixmap with given size
             */
            PixmapImpl(size_t width, size_t height);

            /**
             * @brief Construct pixmap from existing pixmap.
             */
            PixmapImpl(const PixmapImpl& pimpl);

            /**
             * @brief Destruct pixmap.
             */
            virtual ~PixmapImpl();

            /**
             * @brief Get size of pixmap.
             */
            const Gfx::Size& size() const { return _size; }

            /**
             * @brief Provide painter to pixmap.
             */
            Painter painter();

            /**
             * @brief From Drawable: Enable drawing to native graphics context.
             * This will be called by the painer to retrieve context information.
             */
            virtual PainterImpl::ContextInfo beginDraw();

            /**
             * @brief From Drawable: Disable drawing to native gfx context.
             * This will be called by the painer to end drawing.
             */
            virtual void endDraw();

            /**
             * @brief Get native bitmap.
             * This might return 0 if the bitmap has not been constructed properly.
             */
            const CFbsBitmap* getNativeBitmap() const { return _bitmap; }

            /**
             * @brief From Resource: Get type of resource.
             */
            virtual Types Type() const { return TypePixmap; }

        private:
            /**
             * @brief Construct underlying symbian resources (bitmap etc.)
             */
            void construct();

            /**
             * @brief Destruct underlying symbian resources.
             */
            void destruct();

            Pt::Gfx::Size _size;
            ConcretePainter _painter;

            // default font
            const class CFont* _defaultFont;

            // TODO: Use auto_ptr? Feasible with symbian classes?
            CFbsBitmap* _bitmap;
            CFbsBitGc* _bitmapGc;
            CFbsBitmapDevice* _bitmapDevice;

            // some global error flag
            int _lastError;
    };

} // namespace Gui

} // namespace Pt

#endif
