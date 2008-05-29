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

#ifndef PT_GUI_SYMBIAN_APPLICATIONIMPL_H
#define PT_GUI_SYMBIAN_APPLICATIONIMPL_H

#include <Pt/Api.h>
#include <memory>
#include <iostream>
#include <list>

#include <Pt/Singleton.h>
#include <Pt/Signal.h>

#include <Pt/Event.h>
//#include <Pt/System/EventLoop.h>
#include <Pt/System/Mutex.h>
//#include <Pt/System/Thread.h>

#include <Pt/Gui/MouseEvent.h>
#include <Pt/Gui/KeyEvent.h>

class SymbApp;

namespace Pt {

namespace Gui {

    class Application;
    class WidgetImpl;
    class PixmapImpl;
    class Event;

    class ResourceRegistry : public Pt::Singleton<ResourceRegistry>
    {
        public:
            ResourceRegistry();

            ~ResourceRegistry();

            // Widget backend construction becomes delayed until 
            // application instance is running and MVC hierachy is built
            // (window server connection is made)
            void registerWidget(WidgetImpl* widget);
            void unregisterWidget(WidgetImpl* widget);
            void constructWidgets();             
            void destructWidgets();

            // same goes for the pixmaps
            void registerPixmap(PixmapImpl* pixmap);
            void unregisterPixmap(PixmapImpl* pixmap);
            void constructPixmaps();             
            void destructPixmaps();
            
            std::list<WidgetImpl*>& getWidgets() { return _widgets; }         
            
        private:
            // Widgets we need to construct when the application is run
            std::list<WidgetImpl*> _widgets;
            // Pixmaps we need to construct when the application is run
            std::list<PixmapImpl*> _pixmaps;
            
            template<class type>
            static void registerResource(std::list<type*>& container, type* resource)
            {
                container.push_back(resource);                
            }
            
            template<class type>
            static void unregisterResource(std::list<type*>& container, type* resource)
            {
                container.remove(resource);
            }

            template<class type>
            static void constructResources(std::list<type*>& container)
            {
                typename std::list<type*>::iterator i;
                for (i = container.begin(); i != container.end(); ++i)
                    (*i)->construct();
            }
            
            template<class type>
            static void destructResources(std::list<type*>& container)
            {
                typename std::list<type*>::iterator i;
                for (i = container.begin(); i != container.end(); ++i)
                    (*i)->destruct();
            }
            
    };    
    
    /**
     * @brief Basic Application implementation for GUI application.
     */
    class ApplicationImpl
    {
        public:
            ApplicationImpl(Application& app);
            ~ApplicationImpl();

            //! @see Pt::Application::commitEvent(Pt::Event)
            void commitEvent(const Pt::Event& e);

            //! @see Pt::Application::queueEvent(Pt::Event)
            void queueEvent(const Pt::Event& e);

            //! @see Pt::Application::processEvents()
            void processEvents();

            //! @see Pt::Application::run()
            int run();

            //! @see Pt::Application::exit()
            int exit();
            
        private:
            Application& _app;
            System::Mutex _eventMutex;
            
            //System::EventLoop _eventLoop;
            //System::Thread _eventLoopThread;

            static System::Mutex _mutex;

            friend class Widget;
            
        public:
            Signal<const Pt::Event&> eventQueueSignal;
            
            void dispatchEvent(const Pt::Event& event);
            
            SymbApp* _symbApp;
            static ApplicationImpl* _self;

            static void lockAppInstance();
            static void unlockAppInstance();
    };

} // namespace Gui

} // namespace Pt

#endif
