/*
 * Copyright (C) 2006 Marc Boris D�rner
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef PT_GUI_IPAINTER_H
#define PT_GUI_IPAINTER_H

#include <Pt/Gfx/Gfx.h>
#include <Pt/Gfx/Painter.h>
#include <Pt/Gui/Api.h>
#include <cstddef>


namespace Pt {

namespace Gui {
    class Pixmap;

    /**
     * @brief Generic Painter interface
     *
     * This interface derives from Gfx::Painter and adds 2 additional methods
     * for drawing of pixmaps. This interface can be implemented by platform specific
     * or platform independent painter classes, which for example may allow painting
     * of widgets, pixmaps, images or other devices like printers.
     *
     * For a more precise description of the painter interface and its other
     * methods please refer to Gfx::Painter.
     *
     * @see Gfx::Painter
     */
    class PT_GUI_API IPainter : public Gfx::Painter {
        public:
            /**
             * @brief Draws a sub-segment of a pixmap at the specified point on the painter's drawing area.
             *
             * The Pixmap (pixmap) is drawn at the specified Point (to) of the painter's drawing
             * area. The third parameter 'fromRect' specifies the rectangular section of the pixmap
             * which is supposed to be drawn. The coordinates of the Rect object are relative to
             * the pixmap's top-left corner and in pixmap coordinates.
             *
             * The pixmap sub-segment is drawn from left to right und top to bottom.
             *
             * The position may be outside of the drawing area. If it is, only the parts that
             * are still inside of the drawing area are drawn.
             *
             * If the pixmap-segment is bigger than the drawing area or does not completely fit for
             * other reasons, the pixmap is clipped (but not stretched or shrunk).
             *
             * To not only draw a sub-segment of the image but all of it use the method
             * drawPixmap(const Gfx::Point& to, Pixmap& pixmap) instead.
             *
             * @param to The pixmap-segment is drawn at this location on the painter's area.
             * @param from A sub-segment of this pixmap is drawn.
             * @param fromRect Specifies the rectangular segment/section of the pixmap that
             * will be drawn.
             * @see drawPixmap(const Gfx::Point& to, Pixmap& pixmap)
             */
            virtual void drawPixmap(const Gfx::Point& to, Pixmap& from, const Gfx::Region& fromRect) = 0;

            /**
             * @brief Draws a pixmap at the specified point on the painter's drawing area.
             *
             * The Pixmap (pixmap) is drawn at the specified Point (to) of the painter's drawing
             * area. The position describes the upper left corner of the pixmap. The pixmap is
             * drawn from left to right und top to bottom.
             *
             * The position may be outside of the drawing area. If it is, only the parts that
             * are still inside of the drawing area are drawn.
             *
             * If the pixmap is bigger than the drawing area or does not completely fit for
             * other reasons, the pixmap is clipped (but not stretched or shrunk).
             *
             * @param to The pixmap is drawn at this location on the painter's area.
             * @param pixmap This pixmap is drawn.
             */
            virtual void drawPixmap(const Gfx::Point& to, Pixmap& pixmap) = 0;
    };

} // namespace Gui

} // namespace Pt

#endif
