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

#ifndef Ptv_Gui_Pixmap_h
#define Ptv_Gui_Pixmap_h

#include <Pt/Gfx/Size.h>
#include <Pt/Gui/Api.h>
#include <cstddef>


namespace Pt {

namespace Gui {
    class Painter;

    /**
     * @brief A pixmap is a hardware accelerated device-dependent image.
     *
     * A pixmap may be used instead of an image for performance reason. Its internal
     * format is device-specific and depends primarily on the color depth of the device
     * this pixmap was created for. As no conversion is necessary and the pixmap can
     * be bit-blitted directly to the graphics card's memory (and may even be stored
     * inside the graphics card's memory) the usage of pixmap is the fastest way to
     * draw complex graphics on a graphical device.
     *
     * Just like a widget a Pixmap uses the Gfx::Painter interface for drawing to the
     * pixmap's drawing area. A finished pixmap can be bit-blitted to a device by using
     * Painter::drawPixmap().
     *
     * A common usage of a pixmap is as the backbuffer of a widget. All graphical operations
     * are done to the widget and the pixmap. When a widget has to be repainted, for example
     * because the widget was hidden, the backbuffer-pixmap can be drawn directly to the widget
     * instead of doing a (probably complex) redraw by painting all graphic primitives again.
     */
    class PT_GUI_API Pixmap
    {
        private:
            //! @brief The platform-specific implementation of the Pixmap object.
            class PixmapImpl* _impl;

        public:
            /**
             * @brief Creates a Pixmap with the given size for the current display.
             *
             * A pixmap with the given width and height is created for the current
             * display device. The pixmap is compatible to this device.
             *
             * Both parameters width and height are optinal and default to 0.
             *
             * @param width Optional width for this Pixmap (default = 0);
             * @param height Optional height for this Pixmap (default = 0);
             */
            Pixmap(size_t width = 0, size_t height = 0);

            /**
             * @brief Copies the given pixmap and its content.
             *
             * This copy-constructor copies the given pixmap including its context.
             *
             * @param pixmap The pixmap to be cloned.
             */
            Pixmap(const Pixmap& pixmap);

            /**
             * @brief Destroys the pixmap by freeing any resources associated with it.
             */
            ~Pixmap();

            /**
             * @brief Returns the size of this pixmap.
             * @return The size of this pixmap.
             */
            const Gfx::Size& size() const;

            /**
             * @brief Returns a painter for this pixmap to draw to its surface.
             *
             * The returned Painter must not be stored for longer usage! Only when
             * a Painter is freed after its usage, it is guaranteed to function
             * properly. The creation of a Painter is slightly expensive, so a
             * painter should not be created for every single graphical operation
             * but may be stored for the time it is needed, for example to do a
             * complete redraw. After that, the Painter should be freed by destroying
             * the Painter object.
             *
             * @return A painter for this pixmap.
             */
            Painter painter();

            /**
             * @brief Returns the platform-specific implementation of this Pixmap.
             * @return The platform-specific implementation of this Pixmap.
             */
            PixmapImpl& impl()
            { return *_impl; }

            /**
             * @brief Returns the platform-specific implementation of this Pixmap.
             * @return The platform-specific implementation of this Pixmap.
             */
            const PixmapImpl& impl() const
            { return *_impl; }
    };


} // namespace Gui

} // namespace Pt

#endif
