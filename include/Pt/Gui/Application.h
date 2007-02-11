/***************************************************************************
 *   Copyright (C) 2006 Marc Boris Dürner                                  *
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
#ifndef Pt_Gui_Application_h
#define Pt_Gui_Application_h

#include <Pt/Application.h>
#include <Pt/Gui/Api.h>

#include <typeinfo>


namespace Pt {

namespace Gui {

    class PT_GUI_API Application : public Pt::Application
    {
        private:
            //! Pointer to the implementation of Application.
            class ApplicationImpl* _impl;

        public:
            /**
             * @brief Initializes this application.
             *
             * Creates the platform specific Application object and stores it in _impl.
             * Connects the event signal to dispatchEvent().
             */
            Application();

            //! @brief Deletes the platform specific Application object.
            ~Application();

            //! @brief Returns a reference to the platform specific Application object.
            ApplicationImpl& impl();

            // inheritdoc
            void commitEvent(const Pt::Event& event);

            // inheritdoc
            void queueEvent(const Pt::Event& event);

            // inheritdoc
            void processEvents();

            // inheritdoc
            int run();

            // inheritdoc
            void exit();

            /**
             * @brief Receives GUI events for widgets and delivers them to the widget.
             *
             * General events which are sent to the 'event' signal of ptv::Application are
             * passed to this method. If the event is a GUI event, it is sent to the method
             * Widget::event(). From there it may be dispatched to more specific event handling
             * methods.
             * If the event is not a GUI event, it is ignored.
             *
             * @param event An event that will be dispatched to the corresponding widget.
             */
            void dispatchEvent(const Pt::Event& ev) const;
    };

} // namespace gui

} // namespace ptv

#endif
