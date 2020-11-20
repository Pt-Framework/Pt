/*
 * Copyright (C) 2015 Marc Boris Duerner 
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

#ifndef PT_HMI_X11_PAINTDATA_H
#define PT_HMI_X11_PAINTDATA_H

#include <Pt/Hmi/Api.h>
#include <Pt/Gfx/Pen.h>
#include <Pt/Gfx/Brush.h>
#include <Pt/Gfx/Font.h>
#include <Pt/Gfx/FontMetrics.h>
#include <Pt/Gfx/CompositionMode.h>
#include <Pt/Gfx/Rect.h>
#include <Pt/String.h>

#include <X11/X.h>
#include <X11/Xlib.h>

struct _XftFont;

namespace Pt {

namespace Hmi {

class PaintData
{
    public:
        PaintData();

        ~PaintData();

        void setPen(const Gfx::Pen& pen);

        GC& pen();

        void setBrush(const Gfx::Brush& brush);

        GC& brush();

        void setClip(const Gfx::RectF& rectF);

        void resetClip();

        void setFont(const Gfx::Font& font);

        _XftFont* font();

        static Gfx::FontMetrics fontMetrics(const Gfx::Font& font, 
                                            const Pt::String& text);

    protected:
        void create();
        
        void destroy();

        long toXColor(const Gfx::Color& color);

    private:
        GC         _penGc;
        GC         _brushGc;
        _XftFont*  _xftFont;
};

} // namespace

} // namespace

#endif // include guard
