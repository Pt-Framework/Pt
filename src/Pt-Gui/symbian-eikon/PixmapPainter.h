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

#ifndef PT_GUI_SYMBIAN_PIXMAPPAINTER_H
#define PT_GUI_SYMBIAN_PIXMAPPAINTER_H

#include "PainterImpl.h"
#include <Pt/Gui/Painter.h>

namespace Pt {

namespace Gui {

    class PixmapImpl;

    class PixmapPainter : public PainterImpl
    {
        public:
            PixmapPainter(PixmapImpl& parentPixmapImpl);

            virtual ~PixmapPainter();

            virtual void begin();

            virtual void end();
            
        private:
            PixmapImpl& _parentPixmapImpl;
            bool _active;            
    };

} // namespace Gui

} // namespace Pt

#endif
