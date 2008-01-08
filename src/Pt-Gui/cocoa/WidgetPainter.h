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
#ifndef Pt_WidgetPainter_h
#define Pt_WidgetPainter_h

#include "PainterImpl.h"

#ifdef __OBJC__
    #import <AppKit/NSView.h>
#else
    struct NSView;
#endif

namespace Pt {

namespace Gui {

	class Widget;

    class WidgetPainter : public PainterImpl
    {
        public:
            WidgetPainter();

            ~WidgetPainter();

            void setView(NSView* view)
            { _view = view; }

            virtual void begin();

            virtual void end();

        private:
            NSView* _view;
    };

} // namespace Gui

} // namespace Pt

#endif
