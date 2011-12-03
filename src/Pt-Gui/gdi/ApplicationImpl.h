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

#include <Pt/Gui/Api.h>
#include <Pt/Gui/MouseEvent.h>
#include <Pt/Gui/KeyEvent.h>
#include <Pt/Singleton.h>

#include "win32/MainLoopImpl.h"

#include <map>
#include <iostream>

#include <windows.h>

namespace Pt {

namespace Gui {

class Widget;

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
class MainLoopImpl : public Pt::System::MainLoopImpl
{
    public:
        MainLoopImpl();

        ~MainLoopImpl();

    protected:
        virtual DWORD waitFor(DWORD numHandles, const HANDLE *handles, DWORD msecs, bool& isTimeout);

        void processMessage();
};


class MainLoop : public Pt::System::EventLoop
               , public Pt::Singleton<MainLoop>
{
    public:
        //! @brief Window class name for top level windows.
        static const LPCSTR TOP_WINDOW_CLASS_NAME;

        //! @brief Window class name for child windows.
        static const LPCSTR CHILD_WINDOW_CLASS_NAME;

    public:
        MainLoop();

        ~MainLoop();

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

    protected:
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
        LRESULT dispatchGDIEvent(HWND hwnd, unsigned int message, unsigned int wParam, long lParam);

        /**
         * @brief Creates a CloseEvent from a GDI Destroy message and sends it to
         * the application event queue.
         *
         * @param widget The widget for which this message was created.
         */
        void processDestroyMessage(Widget& widget);

        /**
         * @brief Creates a KeyEvent from a GDI virtual key down or key up message
         * and sends it to the application event queue.
         *
         * @param widget The widget for which this message was created.
         * @param wParam Windows parameters containing additional information to the message.
         * @param lParam Windows parameters containing additional information to the message.
         * @param type The type of the key message, for example Pressed or Relased.
         */
        void processVirtualKeyMessage(Widget& widget, int wParam, int lParam, KeyEvent::Type type);

        /**
         * @brief Creates a KeyEvent from a GDI character key down message
         * and sends it to the application event queue.
         *
         * @param widget The widget for which this message was created.
         * @param wParam Windows parameters containing additional information to the message.
         * @param lParam Windows parameters containing additional information to the message.
         */
        void processCharacterKeyMessage(Widget& widget, int wParam, int lParam);

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
        void processMouseMoveMessage(Widget& widget, int wParam, int lParam);

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
        void processMouseEntered(Widget& widget, int wParam, int lParam);

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
        void processMouseButtonMessage(Widget& widget, int wParam, int lParam,
                                       MouseEvent::Button button, MouseEvent::Action action);

        /**
         * @brief Creates a MouseEvent from a GDI mouse wheel message
         * and sends it to the application event queue.
         *
         * @param widget The widget for which this message was created.
         * @param wParam Windows parameters containing additional information to the message.
         * @param lParam Windows parameters containing additional information to the message.
         */
        void processMouseWheelMessage(Widget& widget, int wParam, int lParam);

        /**
         * @brief Creates a MouseMoveEvent from a GDI mouse leave message
         * and sends it to the application event queue.
         *
         * @param widget The widget for which this message was created.
         */
        void processMouseLeaveMessage(Widget& widget);

        /**
         * @brief Creates PaintEvents from a GDI paint message
         * and sends it to the application event queue.
         *
         * @param hwnd The window handle to the window for which this message was created.
         * @param widget The widget for which this message was created.
         * @param wParam Windows parameters containing additional information to the message.
         * @param lParam Windows parameters containing additional information to the message.
         */
        void processPaintMessage(HWND hwnd, Widget& widget);

        /**
         * @brief Creates a MoveEvent from a GDI move message
         * and sends it to the application event queue.
         *
         * @param widget The widget for which this message was created.
         * @param wParam Windows parameters containing additional information to the message.
         * @param lParam Windows parameters containing additional information to the message.
         */
        void processMoveMessage(Widget& widget, int wParam, int lParam);

        /**
         * @brief Creates a ResizeEvent from a GDI size message
         * and sends it to the application event queue.
         *
         * @param widget The widget for which this message was created.
         * @param wParam Windows parameters containing additional information to the message.
         * @param lParam Windows parameters containing additional information to the message.
         */
        void processSizeMessage(Widget& widget, int wParam, int lParam);

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
        unsigned int createModifiersFromMouseMessage(int wParam);

     protected:
        virtual void onAttach(System::Selectable&);

        virtual void onDetach(System::Selectable&);

        virtual void onEnable(System::Selectable& s);

        virtual void onDisable(System::Selectable& s);

        virtual void onReinit(System::Selectable& s);

        virtual void onChanged(System::Selectable& s);

    private:
        //! @brief Registers the top level and child window classes with Windows for later use.
        void registerWindowClasses();

        //! @brief Unregisters the top level and child window classes.
        void unregisterWindowClasses();

    private:
        MainLoopImpl _impl;

        //! @brief Instance handle of this application
        HINSTANCE _instanceHandle;
        bool _trackingMouseEvent;

        //! @brief Map for associations between Window handles and widgets.
        std::map<HWND, Widget*> _windowHandle2Widget;  
};

} // namespace Gui

} // namespace Pt

#endif

