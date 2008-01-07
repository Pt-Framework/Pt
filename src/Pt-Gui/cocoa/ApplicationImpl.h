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
#ifndef Pt_Gui_cocoa_ApplicationImpl_h
#define Pt_Gui_cocoa_ApplicationImpl_h

#include <Pt/Gui/Api.h>
#include <Pt/Gui/Event.h>
#include <Pt/Gui/Application.h>

#ifdef __OBJC__
    #import "Application.h"
#else
    struct PtGuiApplication;
#endif

namespace Pt {

namespace Gui {

class PT_GUI_API ApplicationImpl
{
    public:
        ApplicationImpl(Application& app);

        ~ApplicationImpl();

        void commitEvent(const Pt::Event& event);

        void queueEvent(const Pt::Event& event);

        void processEvents();

        int run();

        void wake();

        void exit();
	
	private:
		Application* app;
};

} // namespace Gui

} // namespace Pt

#endif
