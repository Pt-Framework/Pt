/* Copyright (C) 2016 Marc Boris Duerner 
   Copyright (C) 2016 Laurentiu-Gheorghe Crisan

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   As a special exception, you may use this file as part of a free
   software library without restriction. Specifically, if other files
   instantiate templates or use macros or inline functions from this
   file, or you compile this file and link it with other files to
   produce an executable, this file does not by itself cause the
   resulting executable to be covered by the GNU General Public
   License. This exception does not however invalidate any other
   reasons why the executable file might be covered by the GNU Library
   General Public License.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
   MA 02110-1301 USA
*/

#ifndef Pt_Forms_InputMethod_H
#define Pt_Forms_InputMethod_H

#include <Pt/Forms/Api.h>
#include <Pt/Forms/KeyEvent.h>
#include <Pt/Forms/MouseEvent.h>

#include <Pt/Connectable.h>

namespace Pt {

namespace Forms {

class Window;
class PushButton;
class FlowLayout;
class Application;

/** @brief Extension for platform or custom text entry.

    An input method begins and finishes a session for a %Control, may
    provide an active window, and sends %KeyEvent objects to its current
    receiver. The caller does not own the receiver. Derive from this class
    only to implement a text-entry interface; ordinary controls use the
    method installed on %Application.

    @ingroup Pt-Forms-Input
*/
class PT_FORMS_API InputMethod
{
    friend class Application;

    public:
        /** @brief Creates an inactive input method.
        */
        InputMethod();

        /** @brief Destructor.
        */
        virtual ~InputMethod();

        /** @brief Returns true when the method is showing UI.
        */
        bool isVisible() const;

        /** @brief Returns the method's active window, or 0.
        */
        Window* activeWindow();

        /** @brief Begins a session for @a control.

            The control remains owned by its caller.
        */
        void begin(Control& control);

        /** @brief Ends the current session.
        */
        void finish();

        /** @brief Returns the current receiver, or 0.
        */
        Widget* receiver() const;

        /** @brief Sends @a ev to the current receiver.
        */
        void sendEvent(const KeyEvent& ev);

    protected:
        virtual void onBegin() = 0;

        virtual void onFinish() = 0;

        virtual Window* onActiveWindow() = 0;

    private:
        void registerApplication(Application& app);
        
        void unregisterApplication(Application& app);

    private:
        Application* _app;
        Pt::uint64_t _receiver;
        KeyEvent     _keyEvent;
        bool         _isVisible;
};


/** @brief Application-owned default input method.
*/
class DefaultInputMethod : public InputMethod
                         , public Connectable
{
    public:
        DefaultInputMethod();

        ~DefaultInputMethod();

    protected:
        virtual void onBegin();

        virtual void onFinish();

        virtual Window* onActiveWindow();

    private:
        void onKeyPress();

    private:
        Window*     _window;
        FlowLayout* _layout;
        PushButton* _keyButtonA;
        PushButton* _keyButtonB;
};

} // namespace

} // namespace

#endif
