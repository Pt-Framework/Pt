/***************************************************************************
 *   Copyright (C) 2008 Marc Boris Duerner                                 *
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
#ifndef Pt_Gui_cocoa_PixmapPainter_h
#define Pt_Gui_cocoa_PixmapPainter_h

#include "PainterImpl.h"

namespace Pt {

namespace Gui {

	class PixmapImpl;

    class PixmapPainterImpl : public PainterImpl 
    {
        public:
            PixmapPainterImpl();

            virtual ~PixmapPainterImpl();

            void set(PixmapImpl& pixmap);

            void begin();

            void end();

            void drawLine(const Math::Point& from, const Math::Point& to);

            void drawRect(const Gfx::Rect& rect);

            void fillRect(const Gfx::Rect& rect);

            void drawPixmap(const Math::Point& to, Pixmap& pm);

            void drawPixmap(const Math::Point& to, Pixmap& pm, const Gfx::Region& pmRegion);

        private:
            PixmapImpl* _pixmap;
    };

} // namespace Gui

} // namespace Pt

#endif
