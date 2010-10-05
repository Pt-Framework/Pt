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

#include "Drawable.h"
#include "PainterImpl.h"
#include "ApplicationImpl.h"
#include <Pt/Gui/Api.h>
#include <Pt/Gui/Painter.h>
#include <Pt/Gui/Widget.h>
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Rect.h>
#include <Pt/String.h>

// forward declarations
class CControl;
class CGraphicsContext;
class CGraphicsDevice;
class CFont;

namespace Pt {

namespace Gui {

    /**
     * @brief Widget implementation using symbian CCoeControl derived class.
     *
     * The underlying resources can only be allocated from the OS
     * when there is a connection to the window server available.
     * This is usually provided by the Eikon application framework.
     *
     * Nested controls share the graphic context of the root window.
     *
     * Note that accessing widgets from other threads than the main creation
     * thread is not possible on Symbian. You will cause a panic if you try.
     */
    class WidgetImpl : public Drawable, public Resource
    {
        public:
            /**
             * @brief Indicate default position and size values.
             */
            static const ssize_t KUnused;

            /**
             * @brief Construct widget with default location and size.
             */
            WidgetImpl( Widget& apiWidget, Widget* parent,
                         const Gfx::Point& at = Gfx::Point(KUnused, KUnused),
                         const Gfx::Size& size = Gfx::Size(KUnused, KUnused) );

            /**
             * @brief Regular constructor.
             */
            virtual ~WidgetImpl();

            /**
             * @brief Set title of window. Not used on symbian yet. Window does not
             * provide a caption.
             */
            void setTitle(const Pt::String& text);

            /**
             * @brief Get title of the window. Not used on symbian. See above.
             */
            Pt::String title() const;

            /**
             * @brief Provide painter to paint into widget.
             */
            Painter painter();

            /**
             * @brief Set new parent of a widget.
             * Note that detaching a control into a parentless window does not work.
             */
            void setParent(Widget* parent);

            /**
             * @brief Move the widget to a specific location.
             */
            void move(size_t x, size_t y);

            /**
             * @brief Set new widget size.
             */
            void resize(size_t width, size_t height);

            /**
             * @brief Show widget if it is hidden.
             */
            void show();

            /**
             * @brief Hide widget if it is shown.
             */
            void hide();

            /**
             * @brief Is widget visible?
             */
            bool isVisible() const;

            /**
             * Force redraw of widget.
             */
            void repaint();

            /**
             * @brief See whether the widget has a symbian CCoeControl attached to it.
             */
            bool isConstructed() const { return _control != 0; }

            /**
             * @brief Dispatch event using ApplicationImpl::dispatchEvent().
             */
            void dispatchEvent(Pt::Event& event);

            /**
             * @brief Provide access to parent widget.
             */
            Widget* parent() { return _parent; }

            /**
             * @brief Provide access to backend control.
             */
            CControl* nativeControl() { return _control; }

            /**
             * @brief From Drawable: Enable drawing to native graphics context.
             * This will be called by the painer to retrieve context information.
             */
            virtual PainterImpl::ContextInfo beginDraw();

            /**
             * @brief From Drawable: Disable drawing to native gfx context.
             * This will be called by the painer to end drawing.
             */
            virtual void endDraw();

            /**
             * @brief This will synchronize the position/size attributes to the
             * ApiWidget (make sure they reflect valid position/size).
             */
            void synchronize(bool initial = false);

            /**
             * @brief Provide access to api widget.
             */
            Widget& apiWidget() const { return _apiWidget; }

            /**
             * @brief From Resource: Get type of resource.
             */
            virtual Types Type() const { return TypeWidget; }

        private:
            /**
             * @brief Construct underlying symbian resources (CCoeControl etc.)
             */
            void construct();

            /**
             * @brief Destruct underlying symbian resources.
             */
            void destruct();

            Widget& _apiWidget;
            Widget* _parent;
            Pt::Gfx::Point _initialLocation;
            Pt::Gfx::Size _initialSize;
            bool _initialVisibility;

            ConcretePainter _painter;

            // symbian control
            CControl* _control;

    };

} // namespace Gui

} // namespace Pt

#endif
