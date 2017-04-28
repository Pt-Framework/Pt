/* Copyright (C) 2015 Laurentiu-Gheorghe Crisan
 
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

#ifndef Pt_Hmi_Application_h
#define Pt_Hmi_Application_h

#include <Pt/Hmi/Api.h>
#include <Pt/Hmi/Screen.h>
#include <Pt/Hmi/ResizeEvent.h>
#include <Pt/Hmi/MouseEvent.h>
#include <Pt/Hmi/TouchEvent.h>
#include <Pt/Hmi/ScrollEvent.h>
#include <Pt/Hmi/PaintEvent.h>
#include <Pt/Hmi/MoveEvent.h>
#include <Pt/Hmi/ActivateEvent.h>
#include <Pt/Hmi/EnableEvent.h>
#include <Pt/Hmi/ShowEvent.h>
#include <Pt/Hmi/KeyEvent.h>
#include <Pt/Hmi/CloseEvent.h>
#include <Pt/Hmi/EnterEvent.h>
#include <Pt/Hmi/LeaveEvent.h>
#include <Pt/Hmi/FocusEvent.h>
#include <Pt/Hmi/InvalidateEvent.h>
#include <Pt/Hmi/LayoutEvent.h>
#include <Pt/Hmi/Style.h>
#include <Pt/Hmi/StyleOptions.h>
#include <Pt/Hmi/PlatinumStyle.h>
#include <Pt/Gfx/Font.h>
#include <Pt/System/Application.h>
#include <Pt/Hmi/InputMethod.h>

namespace Pt {

namespace Hmi {

class ApplicationImpl;
class Window;
class Widget;
class Visual;
class WindowStateEvent;

class PT_HMI_API Application : public Pt::System::Application
{
    friend class Visual;
    friend class Window;
    friend class ApplicationImpl;

    public:
        Application(int argc = 0, char** argv = 0);

        virtual ~Application();

        static Application& instance();

        const Screen& screen() const;

        Screen& screen();

        Window* pointerWindow();

        const Window* pointerWindow() const;

        Widget* pointerWidget();

        const Widget* pointerWidget() const;

        Visual* pointerGrabber();

        void grabPointer(Window& w);

        void releasePointer(Window& w);

        void grabPointer(Widget& w);

        void releasePointer(Widget& w);

        void setCursor(const Cursor* cursor = 0);

        const Style& style() const;

        void setStyle(const Style& s);

        const StyleOptions& styleOptions() const;

        StyleOptions& styleOptions();

        InputMethod& inputMethod();

        void setInputMethod(InputMethod& im);

        void removeInputMethod(InputMethod& im);

        /** @brief Sends a key event to the active window.
        */
        void sendKeyEvent(const KeyEvent& ev);

        void invalidate();

        // TODO: this might be the same as loop().waitNext()
        void nextEvent();

        Pt::uint64_t makeId();

        const Visual* findVisual(Pt::uint64_t id) const;

        ApplicationImpl* impl();

    protected:
        void onResizeEvent(const ResizeEvent& ev);

        void onMouseEvent(const MouseEvent& ev);

        void onTouchEvent(const TouchEvent& ev);

        void onScrollEvent(const ScrollEvent& ev);

        void onUpdateEvent(const UpdateEvent& ev);

        void onPaintEvent(const PaintEvent& ev);

        void onMoveEvent(const MoveEvent& ev);

        void onActivateEvent(const ActivateEvent& ev);

        void onEnableEvent(const EnableEvent& ev);

        void onShowEvent(const ShowEvent& ev);

        void onKeyEvent(const KeyEvent& ev);

        void onCloseEvent(const CloseEvent& ev);

        void onEnterEvent(const EnterEvent& ev);

        void onLeaveEvent(const LeaveEvent& ev);

        void onFocusEvent(const FocusEvent& ev);

        void onWindowStateEvent(const WindowStateEvent& ev);

        void onInvalidateEvent(const InvalidateEvent& ev);

        void onLayoutEvent(const LayoutEvent& ev);

    private:
        void registerVisual(Visual& visual);

        void unregisterVisual(Visual& visual);

        void grabLast();

        void setPointerWindow(Window* w);

        void setPointerWidget(Widget* widget);

        void processTouchEvent(const TouchEvent& ev);

        void processMouseEvent(const MouseEvent& mev);

    private:
        typedef std::map<Pt::uint64_t, Visual*> VisualMap;

        ApplicationImpl*   _impl; 
        Screen*            _mainScreen;
        Pt::uint64_t       _lastId;
        VisualMap          _visuals;
        Window*            _pointerWindow;
        Widget*            _pointerWidget;
        Visual*            _pointerGrabber;
        std::vector<Visual*> _grabbers;
        Style              _style;
        StyleOptions       _styleOptions;
        DefaultInputMethod _defaultInputMethod;
        InputMethod*       _inputMethod;

        Gfx::PointF        _scrollFrom;
        bool               _onScroll;
};

} // namespace

} // namespace

#endif
