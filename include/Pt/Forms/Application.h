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

#ifndef Pt_Forms_Application_h
#define Pt_Forms_Application_h

#include <Pt/Forms/Api.h>
#include <Pt/Forms/Screen.h>
#include <Pt/Forms/Style.h>
#include <Pt/Forms/StyleOptions.h>
#include <Pt/Forms/GraphicsBackend.h>
#include <Pt/Forms/PlatinumStyle.h>
#include <Pt/Forms/InputMethod.h>
#include <Pt/Forms/Icon.h>
#include <Pt/Gfx/Font.h>
#include <Pt/Gfx/FontFace.h>
#include <Pt/Gfx/PngReader.h>
#include <Pt/System/Application.h>
#include <Pt/System/Path.h>

#include <list>
#include <vector>

namespace Pt {

namespace Forms {

class Cursor;
class Popup;

/** @brief Runtime root of a Forms user interface.

    %Application is the Forms runtime. It is not a %Widget. Construct one
    before any widget, then show the visual hierarchy and enter the inherited
    event loop with %Pt::System::Application::run().

    The application owns its platform implementation, graphics backend, and
    primary %Screen. It does not own the windows or controls that use them.
    Widgets remove themselves from their visual parent when they are
    destroyed. Each widget receives an ID while the application exists.
    %findWidget() can resolve that ID only while the widget remains alive.

    Platform events first enter the inherited event loop and are then routed
    through the Forms event dispatcher to their target widget or the primary
    screen. State changes such as layout, invalidation, repainting, scaling,
    and window changes use the same dispatcher, which permits the hierarchy
    to coalesce deferred work. Applications normally request changes through
    widget APIs rather than calling %processEvent() directly. The latter is
    useful only for code that deliberately supplies Forms events.

    The application provides shared services such as styles, style options,
    input methods, fonts, and scaling. %setStyle() and %setStyleOptions()
    reset the shared style and invalidate the registered widgets so their
    visual state is rebuilt. %setScaleFactor() sends a rescale event through
    the primary screen. Configure these services before showing the user
    interface when possible. %impl() and %graphicsBackend() support Forms
    backend implementations; ordinary applications use the higher-level
    Forms APIs.

    @ingroup Pt-Forms
*/
class PT_FORMS_API Application : public Pt::System::Application
{
    friend class Widget;
    friend class Popup;
    friend class Screen;

    public:
        /** @brief Creates the Forms runtime and its primary screen.
        */
        Application(int argc = 0, char** argv = 0);

        /** @brief Destroys the Forms runtime and the services it owns.
        */
        virtual ~Application();

        /** @brief Returns the platform-specific Forms implementation.

            This is an implementation API for Forms backends. The application
            owns the returned object.
        */
        ApplicationImpl* impl();

        /** @brief Returns the graphics backend used by Forms.

            This is an implementation API for Forms backends. The application
            owns the returned backend.
        */
        GraphicsBackend& graphicsBackend();

        /** @brief Returns the graphics backend used by Forms.

            This is an implementation API for Forms backends. The application
            owns the returned backend.
        */
        const GraphicsBackend& graphicsBackend() const;

        /** @brief Returns the Forms application instance.
        */
        static Application& instance();

        /** @brief Returns the primary screen.

            The application owns the returned screen. Use its window manager
            for top-level windows.
        */
        const Screen& screen() const;

        /** @brief Returns the primary screen.

            The application owns the returned screen. Use its window manager
            for top-level windows.
        */
        Screen& screen();

        /** @brief Returns the time since the last platform user activity.
        */
        Pt::Timespan inactivityTime() const;

        /** @brief Sets the platform cursor.

            Pass 0 to restore the platform default cursor. The caller retains
            ownership of @a cursor.
        */
        void setCursor(const Cursor* cursor = 0);

        /** @brief Returns the current style.
        */
        const Style& style() const;

        /** @brief Sets the global style for all widgets.

            Resets the style with the current %StyleOptions and invalidates all
            registered widgets.
        */
        void setStyle(const Style& s);

        /** @brief Returns the current style options.
        */
        const StyleOptions& styleOptions() const;

        /** @brief Replaces the style options for all widgets.

            Resets the current style and invalidates all registered widgets.
        */
        void setStyleOptions(const StyleOptions& options);

        /** @brief Loads a PNG file into @a image.

            Resets @a image when @a path does not exist.
        */
        void loadImage(const System::Path& path, Gfx::Image& image);

        /** @brief Sets the Forms scaling factor.

            Immediately sends a %RescaleEvent through the primary screen.
        */
        void setScaleFactor(double scale);

        /** @brief Returns the current Forms scaling factor.
        */
        double scaleFactor() const;

        /** @brief Returns the active input method.

            The application always provides its default input method when no
            custom method is installed.
        */
        InputMethod& inputMethod();

        /** @brief Installs @a im as the active input method.

            Removes the previously active method and registers @a im with this
            application. The caller retains ownership of @a im and must keep
            it alive until it is removed.
        */
        void setInputMethod(InputMethod& im);

        /** @brief Removes @a im when it is the active input method.

            Restores the application-owned default input method.
        */
        void removeInputMethod(InputMethod& im);

        /** @brief Creates an ID for an internal Forms widget.

            The ID is unique within this application while the application is
            alive.
        */
        Pt::uint64_t makeId();

        /** @brief Finds the live widget with @a id.

            Returns 0 when no registered widget has @a id. The returned widget
            is not owned by the application and remains valid only while the
            widget remains alive.
        */
        Widget* findWidget(Pt::uint64_t id);

        /** @brief Processes the next platform event.

            Use this only when implementing a manual Forms event loop.

            TODO: this might be the same as loop().waitNext()
        */
        void nextEvent();

        /** @brief Queues @a ev on the application event loop.
        */
        void commitEvent(const Event& ev);

        /** @brief Processes @a ev immediately through Forms event dispatch.

            This supports Forms backends, tests, and deliberate event
            injection. Applications normally request state changes through
            widget APIs instead.
        */
        void processEvent(const Event& ev);

        /** @brief Notifies of events passed to %processEvent().
        */
        Pt::Signal<const Pt::Event&>& eventReceived();

        /** @brief Invalidates every registered widget.

            Use this for application-wide visual changes such as a style
            update.
        */
        void invalidate();

        /** @brief Emulates a platform key event.

            This supports tests and automation, not ordinary widget event
            delivery.
        */
        void sendKeyEvent(const KeyEvent& ev);

        /** @brief Emulates a platform mouse event.

            This supports tests and automation, not ordinary widget event
            delivery.
        */
        void sendMouseEvent(const MouseEvent& ev);

        /** @brief Returns the active pointer-capture target.

            Returns the most recently capturing widget, otherwise the most
            recently shown transient popup, or 0 when neither exists. The
            returned widget is not owned by the application.
        */
        Widget* capture() const;

    public:
        /** @brief Adds the font files found in @a dir.
        */
        void addFonts(const Pt::System::Path& dir);

        /** @brief Adds the font at @a path.

            Returns true when the font was added.
        */
        bool addFont(const Pt::System::Path& path);

        /** @brief Removes the font at @a path.

            Returns true when the font was removed.
        */
        bool removeFont(const Pt::System::Path& path);

        /** @brief Returns the paths of the registered font files.

            The returned collection is owned by the font subsystem.
        */
        const std::vector<Pt::System::Path>& fontFiles() const;

        /** @brief Returns the default font family used by the graphics backend.
        */
        std::string defaultFont() const;

        /** @brief Sets the default font family used by the graphics backend.
        */
        void setDefaultFont(const std::string& family);

        /** @brief Returns the available font family names.
        */
        std::vector<std::string> fontFamilies() const;

        /** @brief Returns the available faces in @a family.
        */
        std::vector<Gfx::FontFace> fontFaces(const std::string& family) const;

    protected:
        void onSetPointer(Widget& widget, bool isPointer);

        void onRequestCapture(Widget& target, bool capture);

        void onShowPopup(Popup& w, bool transient);

        bool isAnchoredTo(Popup& w, Window& top) const;

        bool isPopupOf(Popup& w, Window& top) const;

        void onClosePopups(const Gfx::PointF& screenPos);

    private:
        void registerWidget(Widget& widget);

        void unregisterWidget(Widget& widget);

    private:
        void onDispatchMouseEvent(const MouseEvent& ev);

        void onProcessMouseEvent(const MouseEvent& ev);


        void onDispatchTouchEvent(const TouchEvent& ev);

        void onProcessTouchEvent(const TouchEvent& ev);


        void onDetectScroll(Widget* widget, const Gfx::PointF& screenPos,
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
        typedef std::map<Pt::uint64_t, Widget*> WidgetMap;

        ApplicationImpl*             _impl;
        GraphicsBackend*             _graphicsBackend;
        Pt::Signal<const Pt::Event&> _eventReceived;
        Screen*                      _mainScreen;
        Pt::uint64_t                 _lastId;
        WidgetMap                    _widgets;

        Style                        _style;
        StyleOptions                 _styleOptions;

        DefaultInputMethod*          _defaultInputMethod;
        InputMethod*                 _inputMethod;

        std::list<Popup*>            _popups;
        std::list<Widget*>           _capture;

        Gfx::PointF                  _scrollFrom;
        bool                         _onScroll;
        Gfx::PngReader               _iconReader;
        double                       _scaling;
};

} // namespace

} // namespace

#endif
