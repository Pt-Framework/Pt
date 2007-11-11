/***************************************************************************
 *   Copyright (C) 2006 Marc Boris Duerner                                 *
 *   Copyright (C) 2007 Sebastian Pieck                                    *
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

#include "PainterImpl.h"
#include <Pt/Gui/Api.h>
#include <Pt/Gui/Painter.h>
#include <Pt/String.h>
#include <Pt/Math/Point.h>
#include <Pt/Math/Rect.h>
#include <Pt.h>

namespace Pt {

namespace Gui {
;
    class Widget;
    class ResizeEvent;

    class WidgetImpl
    {
        public:
            WidgetImpl(Widget& apiWidget, Widget* parent, const Math::Point& at = Math::Point(0, 0),
                                                          const Math::Size& size = Math::Size(400, 300));

            virtual ~WidgetImpl();

            void setTitle(const Pt::String& text);

            Pt::String title() const
            { return L""; }

            Painter painter();

            void setParent(Widget* parent);

            void move(size_t x, size_t y);

            void resize(size_t width, size_t height);

            void show();

            void hide();
						
						PhRid_t rid() const
						{ return PtWidgetRid(_ptwidget);}
        
        private:
            Widget& _apiWidget;
            PtWidget_t* _ptwidget;
            PainterImpl _painter;
            bool _isShown;
    };

} // namespace Gui

} // namespace Pt

#endif

#/** PhEDIT attribute block
#-11:16777215
#0:1674:monospace9:-3:-3:0
#1674:1689:monospace9:0:-1:0
#1689:2680:monospace9:-3:-3:0
#**  PhEDIT attribute block ends (-0000175)**/
