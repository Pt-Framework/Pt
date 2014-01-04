/*
 * Copyright (C) 2006-2011 Marc Boris Duerner
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef Pt_Gui_Application_h
#define Pt_Gui_Application_h

#include <Pt/Gui/Api.h>
#include <Pt/System/Application.h>

namespace Pt {

namespace Gui {

/** @brief %Application with a GUI event-loop.
 */ 
class PT_GUI_API Application : public Pt::System::Application
{
    public:
        Application(int argc = 0, char** argv = 0);

        ~Application();

        static Application& instance();

    protected:
        /** @brief Dispatched GUI events to the widgets. 
         
            The GUI event is sent to the method Widget::event(). From there it may
            be dispatched to more specific event handling methods. If the event is
            not a GUI event, it is ignored. 
            
            @param event An event that will be dispatched to the corresponding widget.
        */
        void dispatchGuiEvent(const Pt::Event& ev);

    private:     
        class MainLoop* _guiloop; 
};

} // namespace gui

} // namespace Pt

#endif
