/***************************************************************************
 *   Copyright (C) 2006 Marc Boris Drner                                   *
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
#ifndef Pt_Gui_photon_ApplicationImpl_h
#define Pt_Gui_photon_ApplicationImpl_h

#include <Pt/Gui/Api.h>
#include <Pt/Singleton.h>
#include <Pt/Signal.h>
#include <Pt/Event.h>
#include <Pt.h>


namespace Pt {

namespace Gui {

	class Application;
	class Widget;

	class PT_GUI_API EventLoopImpl : public Singleton<EventLoopImpl>
	{
		friend class Singleton<EventLoopImpl>;
		
		public:
			void setApp(Application& app);
			
			int run();
			
			void exit();
			
			static int photonEvent(PtWidget_t* widget, void* data, PtCallbackInfo_t* info);
		
		protected:
			EventLoopImpl();
			
			void pointerMotion(Pt::Gui::Widget& widget, PhEvent_t& pev);

			void buttonPress(Pt::Gui::Widget& widget, PhEvent_t& pev);
			
			void buttonRelease(Pt::Gui::Widget& widget, PhEvent_t& pev);

			void exposeEvent(Pt::Gui::Widget& widget, PhEvent_t& ev);

			void windowEvent(Pt::Gui::Widget& widget, PhWindowEvent_t& ev);
		private:
			Application* _app;
	};

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
    };

} // namespace Gui

} // namespace Pt

#endif
