/***************************************************************************
 *   Copyright (C) 2006 Marc Boris Duerner                                 *
 *   Copyright (C) 2005-2007 by Aloysius Indrayanto                        *
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
#ifndef Pt_WidgetPainterImpl_h
#define Pt_WidgetPainterImpl_h

#include "PainterImpl.h"
#include <Pt.h>

namespace Pt {

namespace Gui {

	class Widget;

    class WidgetPainterImpl : public PainterImpl
    {
        public:
            WidgetPainterImpl();

            virtual ~WidgetPainterImpl();

            virtual void begin();

            virtual void end();

            void setRid(PhRid_t rid)
            { _rid = rid; }

			void setClipping(PhTile_t* tiles);

            void drawLine(const Math::Point& from, const Math::Point& to);

            void drawRect(const Gfx::Rect& rect);

            void fillRect(const Gfx::Rect& rect);

            void drawPixmap(const Math::Point& to, Pixmap& pm);

            void drawPixmap(const Math::Point& to, Pixmap& pm, const Gfx::Region& pmRegion);

        private:
            PhRid_t _rid;
            PhDrawContext_t* _dc;
    };

} // namespace Gui

} // namespace Pt

#endif
