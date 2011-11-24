/***************************************************************************
 *   Copyright (C) 2006 Marc Boris D�rner                                  *
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

#ifndef Pt_Gui_ApplicationImpl_h
#define Pt_Gui_ApplicationImpl_h

#include <Pt/Api.h>
#include <map>
#include <iostream>

#include <Pt/Singleton.h>
#include <Pt/Signal.h>

#include <Pt/Event.h>
#include <Pt/Allocator.h>
#include <Pt/System/Mutex.h>

#include <Pt/Gui/MouseEvent.h>
#include <Pt/Gui/KeyEvent.h>

#include <windows.h>


namespace Pt {

namespace Gui {

    class Application;
    class Widget;
    class Event;

    /**
     * @brief Global singleton class which provides access to the applications's instance
     * handle and registered windows/widgets.
     *
     * This singleton class provides access to the application's instance, registers the
     * Windows-specific window classes for this framework and provides methods to register,
     * deregister and find widgets by the associated Windows' window handle (HWND).
     *
     * You can access the application's instance handle by calling getInstanceHandle().
     *
     * To register a widget the method registerWidget(HWND, Widget&) can be called. To
     * deregister a widget unregisterWidget(HWND) is used. A registered widget can be
     * found be the associated window handle (HWND) by using findWidget(HWND).
     * Every widget which is created by the framework has to be registered using these
     * method of this singleton class. It creates the essential association between the
     * Windows' HWND and the application's widget.
     */
    class GDIRegistry : public Pt::Singleton<GDIRegistry>
    {
        public:
            //! @brief Initializes the instance handle and registers the window classes.
            GDIRegistry();

            //! @brief Deregisters the window classes.
            ~GDIRegistry();

            //! @brief Returns this application's instance handle (Windows).
            HINSTANCE getInstanceHandle()
            { return _instanceHandle; }

            /**
             * @brief Associates the given Window handle (HWND) with a PPR widget.
             *
             * Every widget that is created has to be registered using this method and
             * deregistered using unregisterWidget as soon as the widget is destroyed.
             * The given window handle is internally associated with the widget. Clients
             * who only know the windows handle can find out what PPR widget is associated
             * with it, for example to send events to the widget. The method findWidget
             * can be used to find the Widget to a Window handle.
             *
             * @param windowHandle The Window handle to be associated with the given widget
             * @param widget The widget to be associated with the given Window handle.
             * @see unregisterWidget
             * @see findWidget
             */
            void registerWidget(HWND windowHandle, Widget& widget);

            /**
             * @brief Unregisters the given Window handle (HWND).
             *
             * Every widget that is destroyed has to be deregistered with this method.
             * It clears the association between the given Window handle (HWND) and the
             * widget object which is going to be destroyed.
             * If the given Window handle can not be found, nothing happens.
             *
             * @param windowHandle The Window handle which is going to be unregistered.
             */
            void unregisterWidget(HWND windowHandle);

            /**
             * @brief Returns the PPR widget that is associated with the given Window handle.
             *
             * If a PPR widget is associated with the given Window handle, a pointer to this
             * widget is returned. If there is no widget currently associated with the Window
             * handle 0 is returned. To associate a Window handle with a PPR widget use the
             * registerWidget method. To release the association again use unregisterWidget.
             *
             * @param windowHandle The Window handle to which the associated widget shall be
             * found.
             * @return A pointer to the widget which was found; or 0 if no widget was found.
             * @see registerWidget
             * @see unregisterWidget
             */
            Widget* findWidget(HWND windowHandle);

            /**
             * @brief Returns the first Window handle in the list of association between Window
             * handles and widgets.
             *
             * If there is currently no association available 0 is returned.
             *
             * @return The first Window handle.
             */
            HWND getFirstHWND();

        public:
            //! @brief Window class name for top level windows.
            static const LPCSTR TOP_WINDOW_CLASS_NAME;

            //! @brief Window class name for child windows.
            static const LPCSTR CHILD_WINDOW_CLASS_NAME;

        private:
            //! @brief Registers the top level and child window classes with Windows for later use.
            void registerWindowClasses();

            //! @brief Unregisters the top level and child window classes.
            void unregisterWindowClasses();

        private:
            //! @brief Instance handle of this application
            HINSTANCE _instanceHandle;

            //! @brief Map for associations between Window handles and widgets.
            std::map<HWND, Widget*> _windowHandle2Widget;
    };


    /**
     * @brief Central EventLoop-Singleton which addiitionally processes all Windows messages.
     *
     * This class contains the actual Event Loop of the application, which includes the Windows
     * message queue and the applicatio's event queue. To ensure a single-threaded model both
     * queues are processed in only one loop. This loop waits for events or message and is only
     * awoken as soon as a Windows message or application event was added to the queue. While
     * Windows message automatically wake the loop, application events have to explicitely call
     * wake() to initiate the event processing.
     *
     * Events can be added by using commitEvent(Event) or queueEvent(Event), where the first method
     * also wakes up the loop to process the event as soon as possible, in contrary to the second
     * method, which only queues the event without waking up the loop.
     *
     * All Windows messages that are relevant for this application are converted to GUI events
     * before they are dispatched to a slot. The process?????Message() methos are responsible for
     * doing those.
     *
     * As usual for Windows application the processing of the Messages is done in the WndProc
     * function. It corresponds to the static method wndProc(HWND, unsigned int, unsigned int, long)
     * of this class. Messages are passed to dispatchGDIEvent(HWND, unsigned int, unsigned int, long),
     * where they are processed, possibly converted to a GUI event and dispatched to any registered
     * slot.
     *
     */
    class GDIEventLoop : public Pt::Singleton<GDIEventLoop>
    {
        public:
            //! @brief Empty constructor
            GDIEventLoop();

            //! @brief Empty destructor
            ~GDIEventLoop();

            //! @see Pt::Application::commitEvent(Pt::Event)
            //! @see wake()
            void commitEvent(const Pt::Event& e);

            //! @see Pt::Application::queueEvent(Pt::Event)
            void queueEvent(const Pt::Event& e);

            //! @see Pt::Application::processEvents()
            void processEvents();

            /**
             * @brief Wakes up the Window message queue to process queued application events.
             *
             * The application's event queue is combined with the Windows message queue in a
             * way that application events are processed in the same thread as the Windows messages.
             * To trigger the delivery of the application events a custom message, specific to
             * PPR is sent to the Windows message queue of this thread. There the event queue is
             * processed.
             */
            void wake();

            /**
             * \brief Starts this application and processes the Windows message queue.
             *
             * This method processes the Windows message queue in a loop until a quit message
             * is received. It processes the Window messages by translating and dispatching them
             * to the WndProc method (wndProc(HWND, unsigned int, unsigned int, long)). Besides
             * the "usual" Window message the custom PPR message (WM_MESSAGE_QUEUE_WAKE_UP) is
             * processed. It's posted by wake() to wake up the Windows message queue to process
             * PPR application events. If this Message is received during the processing of the
             * message queue only processEvents() is called.
             *
             * This method only returns after the Windows quit message (WM_QUIT) was received.
             *
             * @see wake()
             * @see processEvents()
             * @see wndProc()
             */
            int run();

            /**
             * @brief Exits the application by making the run() method to exit.
             *
             * The Windows quit message (WM_QUIT) is posted to the message queue. This causes
             * the run() method to return and therefore basically exiting the application.
             *
             * Before exiting all remaining application events are processed.
             *
             * @see run()
             */
            int exit();

            /**
             * @brief The window callback function which only delegates to dispatchGDIEvent().
             *
             * This function is called for every Windows message that occures for a specific
             * window of this application. It does not necessarily have to be processed before
             * by the message queue in run(). Some messages are posted directly to the wndProc
             * function for a window.
             *
             * In our application all windows use this wndProc function. So the message for which
             * the message was posted has to be determined using the first parameter 'hwnd'.
             *
             * @param hwnd The window for which this message is posted.
             * @param message The actual message.
             * @param wParam Additional information to this message.
             * @param lParam Additional information to this message.
             * @see dispatchGDIEvent(HWND, message, wParam, lParam)
             */
            static long CALLBACK wndProc(HWND hwnd, unsigned int message, unsigned int wParam, long lParam);

            /**
             * @brief Translation and dispatches Windows messages.
             *
             * This method is called by the wndProc() method directly. It processes all relevant
             * Windows messages and converts them to the corresponding application GUI events.
             * The actual conversion of the Windows message to an event happens in one of the
             * process****Message methods. There the message and additional parameters are used
             * to create an appropriate event from the message. These events are sent to the signal
             * eventQueueSignal which is in turn used to send the event to the associated widget.
             *
             * The widget is determined by using GDIRegistry::findWidget().
             *
             * All messages that are not used by the application are passed to the Windows function
             * DefWindowProc for standard processing.
             */
            static LRESULT dispatchGDIEvent(HWND hwnd, unsigned int message, unsigned int wParam, long lParam);

        public:
            /**
             * @brief A signal to post GUI application events to.
             *
             * The events will be delivered to Gui::Application::dispatchEvent() where they will
             * eventually be delivered to the widget for which the event was created.
             *
             * @see dispatchGDIEvent(HWND, unsigned int, unsigned int, long)
             */
            static Pt::Signal<const Pt::Event&> eventQueueSignal;

        private:
            /**
             * @brief Creates a CloseEvent from a GDI Destroy message and sends it to
             * the application event queue.
             *
             * @param widget The widget for which this message was created.
             */
            static void processDestroyMessage(Widget& widget);

            /**
             * @brief Creates a KeyEvent from a GDI virtual key down or key up message
             * and sends it to the application event queue.
             *
             * @param widget The widget for which this message was created.
             * @param wParam Windows parameters containing additional information to the message.
             * @param lParam Windows parameters containing additional information to the message.
             * @param type The type of the key message, for example Pressed or Relased.
             */
            static void processVirtualKeyMessage(Widget& widget, int wParam, int lParam, KeyEvent::Type type);

            /**
             * @brief Creates a KeyEvent from a GDI character key down message
             * and sends it to the application event queue.
             *
             * @param widget The widget for which this message was created.
             * @param wParam Windows parameters containing additional information to the message.
             * @param lParam Windows parameters containing additional information to the message.
             */
            static void processCharacterKeyMessage(Widget& widget, int wParam, int lParam);

            /**
             * @brief Creates a MouseMoveEvent from a GDI mouse move message
             * and sends it to the application event queue.
             *
             * Also does the "mouse entered"-handling. When the mouse is moved inside window
             * that the mouse was not previously in, processMouseEntered() is called.
             *
             * @param widget The widget for which this message was created.
             * @param wParam Windows parameters containing additional information to the message.
             * @param lParam Windows parameters containing additional information to the message.
             * @see processMouseEntered
             */
            static void processMouseMoveMessage(Widget& widget, int wParam, int lParam);

            /**
             * @brief Creates a MouseMoveEvent for entering a window
             * and sends it to the application event queue.
             *
             * Also the TrackMouseEvent-API of Windows is used to initiate WM_MOUSELEAVE message
             * delivery. As soon as the mouse leaves the window, for which the TrackMouseEvent
             * function was called, a WM_MOUSELEAVE message is sent, which is processed in
             * processMouseLeaveMessage().
             *
             * @param widget The widget for which this message was created.
             * @param wParam Windows parameters containing additional information to the message.
             * @param lParam Windows parameters containing additional information to the message.
             * @see processMouseLeaveMessage()
             */
            static void processMouseEntered(Widget& widget, int wParam, int lParam);

            /**
             * @brief Creates a MouseEvent from a GDI mouse button press, release or double-click message
             * and sends it to the application event queue.
             *
             * @param widget The widget for which this message was created.
             * @param wParam Windows parameters containing additional information to the message.
             * @param lParam Windows parameters containing additional information to the message.
             * @param button Specifies which button was pressed, released or double-clicked.
             * @param action Specifies the action: button was pressed, released or double-clicked.
             */
            static void processMouseButtonMessage(Widget& widget, int wParam, int lParam,
                                                  MouseEvent::Button button, MouseEvent::Action action);

            /**
             * @brief Creates a MouseEvent from a GDI mouse wheel message
             * and sends it to the application event queue.
             *
             * @param widget The widget for which this message was created.
             * @param wParam Windows parameters containing additional information to the message.
             * @param lParam Windows parameters containing additional information to the message.
             */
            static void processMouseWheelMessage(Widget& widget, int wParam, int lParam);

            /**
             * @brief Creates a MouseMoveEvent from a GDI mouse leave message
             * and sends it to the application event queue.
             *
             * @param widget The widget for which this message was created.
             */
            static void processMouseLeaveMessage(Widget& widget);

            /**
             * @brief Creates PaintEvents from a GDI paint message
             * and sends it to the application event queue.
             *
             * @param hwnd The window handle to the window for which this message was created.
             * @param widget The widget for which this message was created.
             * @param wParam Windows parameters containing additional information to the message.
             * @param lParam Windows parameters containing additional information to the message.
             */
            static void processPaintMessage(HWND hwnd, Widget& widget);

            /**
             * @brief Creates a MoveEvent from a GDI move message
             * and sends it to the application event queue.
             *
             * @param widget The widget for which this message was created.
             * @param wParam Windows parameters containing additional information to the message.
             * @param lParam Windows parameters containing additional information to the message.
             */
            static void processMoveMessage(Widget& widget, int wParam, int lParam);

            /**
             * @brief Creates a ResizeEvent from a GDI size message
             * and sends it to the application event queue.
             *
             * @param widget The widget for which this message was created.
             * @param wParam Windows parameters containing additional information to the message.
             * @param lParam Windows parameters containing additional information to the message.
             */
            static void processSizeMessage(Widget& widget, int wParam, int lParam);

            /**
             * @brief Creates a bitwise ORed mouse modifier list from the passed parameter.
             *
             * The parameter wParam contains Windows specific information about modifier keys pressed
             * while the mouse message happened. Supported keys are: shift, control, left mouse
             * button, right mouse button, middle mouse button. The value which is returned is
             * an integer with ORed values of MouseEvent::Modifier.
             *
             * @param wParam Contains Windows specific information about the modifier keys which
             * were pressed when the mouse message happened.
             * @return An integer with ORed values of MouseEvent::Modifier containing information
             * about the modifiers which were pressed when the mouse message happened.
             */
            static unsigned int createModifiersFromMouseMessage(int wParam);

        private:
            /**
             * Contains the Event objects of the event queue that are delivered one
             * by one in method "processEvents()".
             * @see processEvents()
             */
			Pt::Allocator         _allocator;
            std::list<Pt::Event*> _eventQueue;
            System::Mutex         _queueMutex;
            DWORD                 _messageLoopThreadId;

            /**
             * A custom message to wake up the Windows Message Queue when a PPR event was
             * added to the application event queue, which would normally not wake up the Windows
             * Message Queue.
             */
            static const UINT WM_MESSAGE_QUEUE_WAKE_UP;

            /**
             * Is set to true when the Windows TrackMouseEvent-API was used for a window to
             * being notified as soon as the mouse leaves this window. When it's false, no
             * tracking is currently done, because the mouse just left a window or the mouse
             * is not currently located inside any window of this application.
             * @see processMouseMoveMessage()
             * @see processMouseEntered()
             * @see processMouseLeaveMessage()
             */
            static bool _trackingMouseEvent;
    };

    /**
     * @brief Basic Application implementation for GUI application.
     *
     * This is the implementation of the GUI Application class. It basically delegates all method
     * calls to the method of the same name of GDIEventLoop, where the acutal event loop and event
     * processing is located.
     *
     * All events, which are sent by GDIEventLoop are tranfered to Gui::Application::event by
     * connecting the signal GDIEventLoop::eventQueueSignal to the Gui::Application::event slot.
     */
    class ApplicationImpl
    {
        public:
            //! Connects the event signal to Application::event and stores the Application object.
            ApplicationImpl(Application& app);

            //! @brief Delegates to GDIEventLoop::commitEvent(Pt::Event)
            //! @see Pt::Application::commitEvent(Pt::Event)
            void commitEvent(const Pt::Event& e);

            //! @brief Delegates to GDIEventLoop::queueEvent(Pt::Event)
            //! @see Pt::Application::queueEvent(Pt::Event)
            void queueEvent(const Pt::Event& e);

            //! @brief Starts to process the events (by calling GDIEventLoop::processEvents())
            //! @see GDIEventLoop::processEvents()
            void processEvents();

            //! @brief Delegates to GDIEventLoop::run()
            //! @see Pt::Application::run()
            int run();

            //! @brief Delegates to GDIEventLoop::exit()
            //! @see Pt::Application::exit()
            int exit();
    };

    /*class MainLoop : public Pt::System::EventLoop
    {
        public:
            MainLoop();

            MainLoop(Allocator& a);

            virtual ~MainLoop();

        protected:
            virtual void onAttach(Selectable&);

            virtual void onDetach(Selectable&);

            virtual void onEnable( Selectable& s );

            virtual void onDisable( Selectable& s );

            virtual void onReinit(Selectable& s);

            virtual void onChanged(Selectable& s);

        private:
            class MainLoopImpl* _impl;
    };

    class MainLoopImpl : public EventLoopImpl
    {
        public:
            MainLoopImpl();
    
            MainLoopImpl(Allocator& a);
    
            ~MainLoopImpl();
    
            void attach( Selectable& s );
    
            void detach( Selectable& s );
    
            void enable( Selectable& s );
    
            void disable( Selectable& s );
    
            void changed(Selectable& s);
    
        protected:
            virtual void onRun();
    
            virtual void onWake();
    
            void waitNext(std::size_t timeout, bool& isActive);
    
        private:
            HANDLE _wakeEvent;
            HANDLE _ioEvent;
            HandleMap _handles;
            std::set<Selectable*>::iterator _current;
            std::set<Selectable*>::iterator _currentAvail;
            std::set<Selectable*> _attached;
            std::set<Selectable*> _devices;
            std::set<Selectable*> _dirty;
            std::set<Selectable*> _avail;
    };*/

} // namespace Gui

} // namespace Pt

#endif
