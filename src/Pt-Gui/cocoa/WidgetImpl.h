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
#ifndef Pt_Gui_WidgetImpl_h
#define Pt_Gui_WidgetImpl_h

//#include "WidgetPainterImpl.h"
#include <Pt/Gui/Api.h>
//#include <Pt/Gui/Widget.h>
//#include <Pt/Gui/Painter.h>
#include <Pt/Math/Point.h>
#include <Pt/Math/Rect.h>
#include <Pt/String.h>

#ifdef __OBJC__

    #import <AppKit/NSView.h>
    #import <AppKit/NSWindow.h>
    
    @interface PtGuiView : NSView            - (void)mouseDown:(NSEvent*)event;
        @end

#else

    struct PtGuiView;
    struct NSWindow;

#endif

namespace Pt {

namespace Gui {

    class Widget;

    class WidgetImpl
    {
        public:
            WidgetImpl( /*Widget& apiWidget, Widget* parent,*/
                         const Math::Point& at = Math::Point(0, 0),
                         const Math::Size& size = Math::Size(400, 300) );

            virtual ~WidgetImpl();

            void setTitle(const Pt::String& text);

            Pt::String title() const;

            //Painter painter();

            void setParent(Widget* parent);

            void move(size_t x, size_t y);

            void resize(size_t width, size_t height);

            void show();

            void hide();

        private:
            //Widget& _apiWidget;
            NSWindow* window; 
            PtGuiView* view;
            //WidgetPainterImpl _painter;
            //bool _isShown;
    };

} // namespace Gui

} // namespace Pt

#endif

