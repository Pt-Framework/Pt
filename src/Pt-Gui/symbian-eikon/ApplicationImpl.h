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
    
    /**
     * @brief This class is used to provide an interface to identify a resource.
     * There are three types of resources which are registered to the Environment
     * singleton class:
     * 1. Application 
     * 2. Widget
     * 3. Pixmap
     * 
     * The Environment will keep track of these resources.
     * 
     * @see Environment
     */
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
     * @brief This class will provide the environment necessary to create
     * GUI applications using the Eikon (Uikon) framework.
     * 
     * Additionally the environment keeps track of resources and dispatches
     * events to widgets. It is also used to start the application wait loop.
     */     
    class Environment : public Pt::Singleton<Environment>
    {
        public:
            /**
             * @brief Constructor used to initialize the environment.
             * Note that this is a singleton class.
             */
            Environment();

            /**
             * @brief Destructor used to destroy the registry.
             */
            ~Environment();

            /**
             * @brief Register a resource in with the environment.
             * This should happen in the constructor of the resource.
             */
            void registerResource(Resource* resource);

            /**
             * @brief Unregister a resource in with the environment.
             * This should happen in the destructor of the resource.
             */
            void unregisterResource(Resource* resource);
            
            /**
             * @brief Get access to the UI provided by the Eikon framework.
             */
            SymbAppUi& symbAppUi() const;
            
            /**
             * @brief Start application wait loop.
             * This starts the application wait loop by starting the active scheduler
             * provided by the Eikon framework.
             */
            void startWaitLoop();

            /**
             * @brief Stop application wait loop.
             * This stops the application wait loop by stopping the active scheduler
             * provided by the Eikon framework.
             */
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
            /**
             * @brief Used to initialize the Eikon framework environment.
             */
            void initFramework();
            
            /**
             * @brief Used to destroy the Eikon framework environment.
             */
            void destroyFramework();

            std::list<Resource*> _resources;
            
            // Eikon environment
            CEikonEnv* _coe;
            // Eikon main UI
            SymbAppUi* _ui;        
            
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
