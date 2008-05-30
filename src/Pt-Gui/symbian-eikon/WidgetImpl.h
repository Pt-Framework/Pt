/***************************************************************************
 *   Copyright (C) 2008 Marc Boris Duerner                                 *
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
#ifndef PT_GUI_SYMBIAN_WIDGETIMPL_H
#define PT_GUI_SYMBIAN_WIDGETIMPL_H

#include "WidgetPainter.h"
#include <Pt/Gui/Api.h>
#include <Pt/Gui/Painter.h>
#include <Pt/Gui/Widget.h>
#include <Pt/Math/Point.h>
#include <Pt/Math/Rect.h>
#include <Pt/String.h>

class CControl;
class CGraphicsContext;
class CGraphicsDevice;
class CFont;

namespace Pt {

namespace Gui {

    class WidgetImpl
    {
        public:
            static const ssize_t KPositionUnused;
            
            WidgetImpl( Widget& apiWidget, Widget* parent,
                         const Math::Point& at = Math::Point(KPositionUnused, KPositionUnused),
                         const Math::Size& size = Math::Size(KPositionUnused, KPositionUnused) );

            virtual ~WidgetImpl();

            void setTitle(const Pt::String& text);

            Pt::String title() const;

            Painter painter();

            void setParent(Widget* parent);

            void move(size_t x, size_t y);

            void resize(size_t width, size_t height);

            void show();

            void hide();
            
            bool isVisible() const;

            void repaint();

            // construct symbian backend control
            void construct();
            
            // destruct
            void destruct();
            
            bool isConstructed() { return _control != 0; }
            
            // dispatch events to slots
            void dispatchEvent(Pt::Event& event);    
            
            Widget* parent() { return _parent; }
            
            // get backend control
            CControl* nativeControl() { return _control; }
            
            // enable drawing to native graphics context                        
            void beginDraw();            
            
            // disable drawing to native gfx context
            void endDraw();
            
            void synchronize(bool initial = false);
            
            Widget& apiWidget() const { return _apiWidget; }
            
        private:
            Widget& _apiWidget;
            Widget* _parent;
            Pt::Math::Point _initialLocation;
            Pt::Math::Size _initialSize;

            WidgetPainter _painter;
            
            // symbian control
            CControl* _control;
            
    };

} // namespace Gui

} // namespace Pt

#endif
