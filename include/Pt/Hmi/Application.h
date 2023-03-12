/* Copyright (C) 2015 Marc Boris Duerner 
   Copyright (C) 2015 Laurentiu-Gheorghe Crisan
 
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
#include <Pt/Hmi/Style.h>
#include <Pt/Hmi/StyleOptions.h>
#include <Pt/Hmi/PlatinumStyle.h>
#include <Pt/Hmi/InputMethod.h>
#include <Pt/Hmi/Icon.h>
#include <Pt/Gfx/Font.h>
#include <Pt/Gfx/PngReader.h>
#include <Pt/System/Application.h>
#include <Pt/System/Path.h>

#include <list>

namespace Pt {

namespace Hmi {

class Cursor;
class Popup;

class PT_HMI_API Application : public Pt::System::Application
{
    friend class Visual;
    friend class Popup;
    friend class Screen;

    public:
        Application(int argc = 0, char** argv = 0);

        virtual ~Application();

        ApplicationImpl* impl();

        static Application& instance();

        const Screen& screen() const;

        Screen& screen();

        Pt::Timespan inactivityTime() const;

        void setCursor(const Cursor* cursor = 0);

        const Style& style() const;

        void setStyle(const Style& s);

        const StyleOptions& styleOptions() const;

        StyleOptions& styleOptions();

        void setFontDir(const Pt::System::Path& dir);

        void setDefaultFont(const std::string& fontName);

        void loadImage(const System::Path& path, Gfx::Image& image);

        void setScaleFactor(double scale);

        double scaleFactor() const;

        InputMethod& inputMethod();

        void setInputMethod(InputMethod& im);

        void removeInputMethod(InputMethod& im);

        Pt::uint64_t makeId();

        Visual* findVisual(Pt::uint64_t id);

        // TODO: this might be the same as loop().waitNext()
        void nextEvent();

        void commitEvent(const Event& ev);

        void processEvent(const Event& ev);

        Pt::Signal<const Pt::Event&>& eventReceived();

        void invalidate();

        /** @brief Emulates a key event.
        */
        void sendKeyEvent(const KeyEvent& ev);

        /** @brief Emulates a mouse event.
        */
        void sendMouseEvent(const MouseEvent& ev);

        Visual* capture() const;

    protected:
        void onSetPointer(Visual& v, bool isPointer);

        void onRequestCapture(Visual& target, bool capture);

        void onShowPopup(Popup& w, bool transient);

        bool isAnchoredTo(Popup& w, Window& top) const;

        bool isPopupOf(Popup& w, Window& top) const;

        void onClosePopups(const Gfx::PointF& screenPos);

    private:
        void registerVisual(Visual& visual);

        void unregisterVisual(Visual& visual);

    private:
        void onDispatchMouseEvent(const MouseEvent& ev);

        void onProcessMouseEvent(const MouseEvent& ev);


        void onDispatchTouchEvent(const TouchEvent& ev);

        void onProcessTouchEvent(const TouchEvent& ev);


        void onDetectScroll(Visual* visual, const Gfx::PointF& screenPos,
                            bool isPress, bool isPressed);


        void onDispatchScrollEvent(const ScrollEvent& ev);

        void onProcessScrollEvent(const ScrollEvent& ev);


        void onDispatchEnterEvent(const EnterEvent& ev);

        void onProcessEnterEvent(const EnterEvent& ev);


        void onDispatchLeaveEvent(const LeaveEvent& ev);

        void onProcessLeaveEvent(const LeaveEvent& ev);


        void onDispatchKeyEvent(const KeyEvent& ev);

        void onProcessKeyEvent(const KeyEvent& ev);


        void onDispatchInvalidateEvent(const InvalidateEvent& ev);

        void onProcessInvalidateEvent(const InvalidateEvent& ev);


        void onDispatchLayoutEvent(const LayoutEvent& ev);
        
        void onProcessLayoutEvent(const LayoutEvent& ev);


        void onDispatchRescaleEvent(const RescaleEvent& ev);

        void onProcessRescaleEvent(const RescaleEvent& ev);


        void onDispatchPaintEvent(const PaintEvent& ev);

        void onProcessPaintEvent(const PaintEvent& ev);


        void onDispatchMoveEvent(const MoveEvent& ev);

        void onProcessMoveEvent(const MoveEvent& ev);


        void onDispatchResizeEvent(const ResizeEvent& ev);

        void onProcessResizeEvent(const ResizeEvent& ev);


        void onDispatchActivateEvent(const ActivateEvent& ev);

        void onProcessActivateEvent(const ActivateEvent& ev);


        void onDispatchEnableEvent(const EnableEvent& ev);

        void onProcessEnableEvent(const EnableEvent& ev);


        void onDispatchShowEvent(const ShowEvent& ev);

        void onProcessShowEvent(const ShowEvent& ev);


        void onDispatchCloseEvent(const CloseEvent& ev);

        void onProcessCloseEvent(const CloseEvent& ev);


        void onDispatchFocusEvent(const FocusEvent& ev);

        void onProcessFocusEvent(const FocusEvent& ev);


        void onDispatchWindowStateEvent(const WindowStateEvent& ev);

        void onProcessWindowStateEvent(const WindowStateEvent& ev);

    private:
        typedef std::map<Pt::uint64_t, Visual*> VisualMap;

        ApplicationImpl*             _impl;
        Pt::Signal<const Pt::Event&> _eventReceived;
        Screen*                      _mainScreen;
        Pt::uint64_t                 _lastId;
        VisualMap                    _visuals;

        Style                        _style;
        StyleOptions                 _styleOptions;
        
        DefaultInputMethod*          _defaultInputMethod;
        InputMethod*                 _inputMethod;
        
        std::list<Popup*>            _popups;
        std::list<Visual*>           _capture;
                                     
        Gfx::PointF                  _scrollFrom;
        bool                         _onScroll;
        Gfx::PngReader               _iconReader;
        double                       _scaling;
};

} // namespace

} // namespace

#endif
