/***************************************************************************
 *   Copyright (C) 2006 Marc Boris Duerner                                 *
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

#ifndef PT_HMI_X11_PAINTERIMPL_H
#define PT_HMI_X11_PAINTERIMPL_H

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

class PainterImpl
{
    public:
        PainterImpl();

        ~PainterImpl();

        void setPen(const Gfx::Pen& pen);

        GC& pen();

        void setBrush(const Gfx::Brush& brush);

        GC& brush();

        void setClip(const Gfx::RectF& rectF);

        void resetClip();

        void setCompositionMode(const Gfx::CompositionMode& mode)
        {
        }

        void setFont(const Gfx::Font& font);

        _XftFont* font();

        static Gfx::FontMetrics fontMetrics(const Gfx::Font& font, 
                                            const Pt::String& text);

        static const std::string& defaultFont()
        {
            return getDefaultFont();
        }

        static void setDefaultFont(const std::string& f)
        {
            getDefaultFont() = f;
        }

        static std::string& getDefaultFont()
        { 
            static std::string _defaultFont;
            return _defaultFont; 
        }

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
