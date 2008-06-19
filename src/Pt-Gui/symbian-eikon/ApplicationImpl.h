/***************************************************************************
 *   Copyright (C) 2006 Marc Boris Duerner                                 *
 *   Copyright (C) 2008 Peter Barth                                        *
 *   Copyright (C) 2006-2008 PTV AG                                        *
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
#include <list>
#include <assert.h>

#include <Pt/Singleton.h>
#include <Pt/Signal.h>

#include <Pt/Event.h>
#include <Pt/System/Mutex.h>

class SymbAppUi;
class SymbEventLoop;
class CApaApplication;
class CEikonEnv;

namespace Pt {

namespace Gui {

    class Application;
    class WidgetImpl;
    class PixmapImpl;
    class Event;
    
    /**
     * @brief This class is an internal exit event used to wake the event loop
     * and exit the application.
     */ 
    class ExitEvent : public Pt::Event
    {
        public:
            ExitEvent()
            { }
    
            virtual const std::type_info& typeInfo() const
            {
                return typeid(ExitEvent);
            }
    
            virtual Event* clone() const
            {
                return new ExitEvent(*this);
            }
    };
    
    class Resource
    {
        public:
            enum Types 
            {
                TypeApplication,
                TypeWidget,
                TypePixmap
            };
            
            virtual ~Resource()
            { }
            
            virtual Types Type() const = 0;
    };
    
    /**
     * @brief This class will manage backend resources like widgets and pixmaps.
     * Pt-Gui allows to create Widgets and Pixmaps before the actual application
     * is running. This is not yet possible when using the Eikon-framework on Symbian.
     *
     * Explanation:
     * To access windows, bitmaps or similar resources it is necessary to connect
     * to their respective servers (i.e. window server, bitmap server etc.).
     * The connection to these servers is created when the application framework
     * instantiates the necessary MVC classes.
     * 
     * The solution is to keep track of all resources and instantiate their
     * backends when the application instance is fully running.
     * Until then widgets and pixmaps will behave gracefully when invoking
     * their painter methods even if no symbian backend is available.
     * 
     */     
    class ResourceRegistry : public Pt::Singleton<ResourceRegistry>
    {
        public:
            /**
             * @brief Constructor used to initialize the registry.
             * Note that this is a singleton class.
             */
            ResourceRegistry();

            /**
             * @brief Destructor used to destroy the registry.
             */
            ~ResourceRegistry();

            void registerResource(Resource* resource);
            void unregisterResource(Resource* resource);
            
            SymbAppUi& symbAppUi() const;
            
            void startWaitLoop();
            void stopWaitLoop();

            /**
             * @brief Dispatch events to their receivers using signal.
             */
            void dispatchEvent(const Pt::Event& event);
            
            /**
             * @brief Get all resources.
             * Kept public for easier handling within implementation scope. 
             */
            std::list<Resource*>& resources() { return _resources; }
            
            /**
             * @brief Signal to dispatch events. 
             * Kept public for easier handling within implementation scope. 
             */
            Signal<const Pt::Event&> eventQueueSignal;
                        
        private:
            std::list<Resource*> _resources;
            
            // Eikon environment
            CEikonEnv* _coe;
            // Eikon main UI
            SymbAppUi* _ui;        
            
            /**
             * @brief Used to initialize the Eikon framework environment.
             */
            void initFramework();
            
            /**
             * @brief Used to destroy the Eikon framework environment.
             */
            void destroyFramework();
    };    
    
    /**
     * @brief Basic Application implementation for GUI application.
     * 
     * Note that on symbian only one application instance can be running
     * at the same time. Creating a second application instance will block
     * upon construction.
     * 
     * @see Application
     */
    class ApplicationImpl : public Resource
    {
        public:
            /**
             * @brief Construct application impl.
             * Note that on symbian only one application instance can be running
             * at the same time. Creating a second application instance will block
             * upon construction.
             *
             * @param app Reference to application instance
             * @throws std::runtime_exception If initialization of event loop fails 
             */
            ApplicationImpl(Application& app);
            
            /**
             * @brief Destruct application impl
             */
            virtual ~ApplicationImpl();

            /**
             * @see Application::commitEvent(Pt::Event&)
             */
            void commitEvent(const Pt::Event& e);

            /**
             * @see Application::queueEvent(Pt::Event&)
             */
            void queueEvent(const Pt::Event& e);

            /**
             * @see Application::processEvents()
             */
            void processEvents();

            /**
             * @see Application::run()
             */
            int run();

            /**
             * @see Application::exit()
             */
            int exit();
            
            /**
             * @brief From Resource: Get type of resource.
             */
            virtual Types Type() const { return TypeApplication; }

        private:
            // application frontend
            Application& _app;
            
            // event loop
            SymbEventLoop* _eventLoop;

            // mutex used to protect static "singleton" instance
            static System::Mutex _mutex;

            // used to protect static "singleton" instance using the mutex from above.
            static void lockAppInstance();
            static void unlockAppInstance();
    };

} // namespace Gui

} // namespace Pt

#endif
