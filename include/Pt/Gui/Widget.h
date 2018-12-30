/*
 * Copyright (C) 2006 PTV AG
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef PT_GUI_WIDGET_H
#define PT_GUI_WIDGET_H

#include <Pt/Signal.h>
#include <Pt/Connectable.h>
#include <Pt/NonCopyable.h>
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Rect.h>
#include <Pt/String.h>
#include <Pt/Gfx/ARgbColor.h>
#include <Pt/Gui/Api.h>
#include <Pt/Gui/Insets.h>
#include <cstddef>
#include <memory>


namespace Pt {

    class Event;

namespace Gui {

    class Event;
    class CloseEvent;
    class MouseEvent;
    class MouseMoveEvent;
    class MoveEvent;
    class Painter;
    class PaintEvent;
    class ResizeEvent;
    class KeyEvent;
    class Layout;
    class LayoutData;

    /**
     * @brief A generic widget containing all basic widget operations including parent/child control.
     *
     * This generic widget is the base class for all specific widgets and also can function as
     * a top-level widget (aka window).
     *
     * !General presentation properties
     * Each widget has some general presentation properties:
     * - Foreground color - The color in which the data of the foreground is drawn. The usage of the
     * foreground color is widget-dependent. For a button or label this may be the text color, for a
     * checkbox this may be the color of the marker and for other widget it may have no meaning at
     * all.
     * - Background color - The color which is used to paint the background. The usage of the background
     * color is widget-dependent. For a button this may be the color of the button area, for a label
     * this may be the background where there is no text, for a checkbox this may be the color inside
     * the marker-box. For other widgets the background color may have no meaning at all.
     * - Title - The title of the widget. This is only used for top-level windows as window title.
     * - Insets - The insets are the inner spacing between the actual content of the widget and its
     * border. The usage of insets is widget-dependent. For a button this may be the spacing between
     * the button's text and its painted border, for a checkbox this may be the distance between the
     * marker-box and its text. When a widget contains other widgets (parent-child-relationsip) the
     * insets are the spacing between the child widgets and the border of the parent widget.
     *
     * Each widget has a position and size. For top-level widgets this position is relative to top-left
     * corner of the desktop. For child widgets this position is relative to the top-left corner of
     * the parent's widget.
     *
     * A widget also has a minimum and preferred size, which may depend on its content. These sizes are
     * relevant for layouting and are described below.
     *
     * Widgets can be shown an hidden. When hidden a widget will still be layouted as if it was visible.
     * Use show() and hide() to show or hide a widget.
     *
     * !Parent - child relationship
     * In principle any widget can act as a container which can contain child widgets. This widget base
     * class provides operations for this matter. To add a widget to a parent widget, the parent widget
     * can be passed as first argument to the widget class' constructor. This will add the widget to the
     * child list of the parent and set the parent of the newly created widget to the given widget.
     * Adding children to parent widget will create a physical tree structure, for example to create
     * a logical dependency structure.
     *
     * Nesting widgets by adding a container to a container makes it possible to nest LayoutManagers (see
     * below) which allows the creation of complex layouts using only simple LayoutManagers.
     *
     * By hiding a parent, all direct or indirect child widgets will be hidden, too.
     *
     * A widget can be removed from a parent by calling unparent(). To set another widget as new parent
     * reparent() can be used.
     *
     * The position of a widget is always relative to its parent top-left corner. When a widget is partly
     * or completely outside of the parents bounds it will be clipped.
     *
     * To access the children of the widget childWidgets() can be called. To access the parent of a
     * widget parent() can be called.
     *
     * Not every widget does support to contain child widgets. (A button, for example, will possible not
     * have children). In this case the more specific widget class (e.g. Button) may block the attempt
     * to being used as a parent widget during widget creation.
     *
     * !Event-Handling and signals
     * For a widget a set of GUI events can occur. These events are generated as platform-dependent events
     * and transformed to GUI events before delivered to one of the event-handling method of the widget.
     * All events are delivered to the event()-method from which they are dispatched to one of the XXXevent()
     * methods according to their type. At this point the events are processed internally or may be
     * further delivered to one of the signals the widget provides. The closed-signal will for example
     * be sent as result of a received CloseEvent.
     *
     * An application can register a slot to one or more of the signals the widget or one of its sub-classes
     * provides by using the connect()-methods.
     *
     * Sub-classes of Widget can add specific event handling functionality to the existing event-handling
     * methods by overriding the protected event-handling methods which names start with an underscore (_).
     * The public event handling methods contain house-keeping code and call the underscore-methods.
     *
     * To add support for other events in a sub-class of Widget, the sub-class must override _event(), call
     * it's direct base-class' _event() method (so all other events are still handled correctly) and add
     * the event handling code for the new event.
     *
     * !Layout-Manager
     * A LayoutManager can be set for every widget. It will layout the child widgets of the widget in a
     * defined way whenever updateLayout() is called. To set the LayoutManager for a widget call the
     * static create()-method of the concrete LayoutManager-class and specify the widget for which
     * the LayoutManager is created.
     *
     * To help the layouting two methods - minimumSize() and preferredSize() - exist which return the
     * minimum size and preferred size of the widget. The preferred size is used by several LayoutManagers
     * to calculate parts of the positioning and sizing of the involved widgets. The minimum size is
     * used to describe the minimum size a widget should be set to by a layout manager and should be
     * respected by the layouting process. The preferred size usually depends on the content of the
     * widget. The preferred size of the Button, for example, depends on the length of the button's
     * text, its font size, the spacing of the text to the border (=insets) and the border width itself.
     * The optimal presentation concerning all those attributes contribute to the preferred size.
     *
     * By calling the method pack() the widget will calculate the preferred size of the widget by using
     * the preferred size of all child widgets (and their child widgets) and then resize the top-level
     * widget to this preferred size. This does only work for top-level widgets (=windows and dialogs).
     *
     * @see Insets
     * @see LayoutManager
     */
    class PT_GUI_API Widget : public Connectable, public NonCopyable
    {
        private:
            //! @brief A pointer to the platform-specific Widget implementation.
            class WidgetImpl* _impl;

        public:
            /**
             * @brief Constructs a new widget using the given point as position and size as size.
             *
             * A widget is created. The given parent is set as parent of this widget and the
             * widget is added to the parent's children list. The widget is positioned at the
             * given location using the given size. If the widget is a child widget the position
             * is relative to the parent's top-left corner of the parent. If it is a top-level
             * widget the position is relative to the top-left corner of the desktop.
             *
             * If the widget is a top-level widget (parent == 0), it is hidden from start. Use
             * show() to show it. Child widgets are visible from start.
             *
             * @param parent The parent widget for this widget. The widget will become the child of
             * this parent and be shown inside of it. To create a top-level widget 0 can be passed
             * as an argument.
             * @param at The position of this widget inside its parent relative to the parent's top-left corner.
             * @param size The size of this widget. The size must be >0 for width and height.
             */
            Widget(Widget& parent, const Gfx::Point& at, const Gfx::Size& size);

            /**
             * @brief Constructs a new widget using the platform's default position and size.
             *
             * A widget is created. The given parent is set as parent of this widget and the
             * widget is added to the parent's children list. The widget is positioned at the
             * default position of the platform. As widget size the default widget size of the
             * platform is used. This is especially useful for top-level widgets when using mobile
             * platforms as they position and size top-level widgets in a way they occupy all of
             * the screen.
             *
             * If the widget is a top-level widget (parent == 0), it is hidden from start. Use
             * show() to show it. Child widgets are visible from start.
             *
             * @param parent The parent widget for this widget. The widget will become the child of
             * this parent and be shown inside of it. To create a top-level widget 0 can be passed
             * as an argument.
             */
            Widget(Widget& parent);

            /**
             * @brief Constructs a new top-level widget using the given point as position and size
             * as size.
             *
             * A widget is created. The widget is positioned relative to the top-left corner
             * of the desktop using the given size.
             *
             * As the widget is a top-level widget it is hidden from start. Use show() to show it.
             *
             * @param at The position of this widget inside its parent relative to the parent's top-left corner.
             * @param size The size of this widget. The size must be >0 for width and height.
             */
            Widget(const Gfx::Point& at, const Gfx::Size& size);

            /**
             * @brief Constructs a new top-level widget using the platform's default position and
             * size.
             *
             * A widget is created. The widget is positioned at the default position of the platform.
             * As widget size the default widget size of the platform is used. This is especially
             * useful for top-level widgets when using mobile platforms as they position and size
             * top-level widgets in a way they occupy all of the screen.
             *
             * As the widget is a top-level widget it is hidden from start. Use show() to show it.
             */
            Widget();

            /**
             * @brief Destructs the widget and frees all resources which are currently used by
             * the widget.
             */
            virtual ~Widget();

            /**
             * @brief Sets the title of this widget.
             *
             * Sets the title of this widget. In case the widget is a top-level widget and the
             * platform show's the window's title, the newly set title will be visible immediately.
             * In case this widget is a child widget the call will not have a visible effect as
             * child widgets usually don't have a visible title. In case the widget becomes a
             * top-level widget at a later time the set title will be visible, though.
             *
             * @param text The new title for this widget.
             * @see title()
             */
            void setTitle(const Pt::String& text);

            /**
             * @brief Returns the title of this widget.
             * @return The title of this widget.
             * @see setTitle()
             */
            Pt::String title();

            /**
             * @brief Returns a reference to the color which is currently set as background color
             * for this widget.
             * @return The background color of this widget.
             * @see setBackgroundColor()
             */
            const Gfx::ARgbColor& backgroundColor() const;

            /**
             * @brief Sets the background color of this widget to the given color.
             *
             * The background color is used to paint the background. The usage of the background
             * color is widget-dependent. For a button this may be the color of the button area,
             * for a label this may be the background where there is no text, for a checkbox this
             * may be the color inside the marker-box. For other widgets the background color may
             * have no meaning at all.
             *
             * @param color The new background color for this widget.
             * @see backgroundColor()
             */
            void setBackgroundColor(const Gfx::ARgbColor& color);

            /**
             * @brief Returns a reference to the color which is currently set as foreground color
             * for this widget.
             * @return The foreground color of this widget.
             * @see setForegroundColor()
             */
            const Gfx::ARgbColor& foregroundColor() const;

            /**
             * @brief Sets the foreground color of this widget to the given color.
             *
             * The foreground color is the color which is usually used to paint the content of the
             * widget. The usage of the foreground color is widget-dependent. For a button or label
             * this may be the text color, for a checkbox this may be the color of the marker and
             * or other widget it may have no meaning at all.
             *
             * @param color The new foreground color for this widget.
             * @see foregroundColor()
             */
            void setForegroundColor(const Gfx::ARgbColor& color);

            /**
             * @brief Sets the insets of this widget.
             *
             * The insets are the inner spacing between the actual content of the widget and its
             * border. The usage of insets is widget-dependent. For a button this may be the
             * spacing between the button's text and its painted border, for a checkbox this may
             * be the distance between the marker-box and its text.
             *
             * When a widget contains other widgets (parent-child-relationship) the insets are the
             * spacing between the child widgets and the border of the parent widget.
             *
             * @param insets The new insets for this widget.
             * @see insets()
             */
            void setInsets(const Insets& insets);

            /**
             * @brief Returns the insets of this widget.
             * @return The insets of the widget.
             * @see setInsets()
             */
            const Insets& insets() const;

            /**
             * @brief Returns this widget's current bounding box (= position + size).
             *
             * A Region object is returned which describes the widget's current bounds; the widget's
             * position and size. For top-level widgets the position is relative to the top-left
             * corner of the desktop. For child widgets the position is relative to the top-left
             * corner of the parent widget.
             *
             * @return The widget's current bounding box (= position + size).
             * @see resize()
             * @see size()
             */
            const Pt::Gfx::Region& region() const;

            /**
             * @brief Returns this widget's current size.
             *
             * The current size of this widget is returned.
             *
             * @return The current size of this widget.
             * @see resize()
             * @see rect()
             */
            const Gfx::Size& size() const;

            /**
             * @brief Moves the widget to the given position (x, y).
             *
             * The widget is moved to the specified position. For top-level widgets this position
             * is relative to the top-left corner of the desktop. For child widgets this position
             * is relative to the top-left corner of the parent widget.
             *
             * Moving a widget will result in a MoveEvent.
             *
             * @param x The new x-position for this widget.
             * @param y The new y-position for this widget.
             * @see resize()
             * @see rect()
             */
            virtual void move(ssize_t x, ssize_t y);

            /**
             * @brief Returns the x-position of this widget relative to its parent widget.
             *
             * @return The x-position of this widget relative to its parent widget.
             */
            ssize_t x() const;

            /**
             * @brief Returns the y-position of this widget relative to its parent widget.
             *
             * @return The y-position of this widget relative to its parent widget.
             */
            ssize_t y() const;

            /**
             * @brief Returns the width of this widget.
             *
             * @return The width of this widget.
             */
            size_t width() const;

            /**
             * @brief Returns the height of this widget.
             *
             * @return The height of this widget.
             */
            size_t height() const;

            /**
             * @brief Resizes this widget to the given width and height.
             *
             * Resizing a widget will result in a ResizeEvent.
             *
             * @param width The new width for this widget.
             * @param height The new height for this widget.
             * @see move()
             * @see size()
             * @see resize(Gfx::Size&)
             */
            virtual void resize(size_t width, size_t height);

            /**
            * @brief Resizes this widget to the given size.
            *
            * Resizing a widget will result in a ResizeEvent.
            *
            * @param size The new size for this widget.
            * @see move()
            * @see size()
            * @see resize(size_t, size_t)
            */
            virtual void resize(const Gfx::Size& newSize);

            /**
             * @brief Makes this widget visible.
             *
             * The widget will be shown after calling this method. If the widget was invisible
             * before calling this method a PaintEvent is triggered. If the widget already was
             * visible, nothing happens.
             *
             * @see hide()
             */
            virtual void show();

            /**
             * @brief Makes this widget invisible.
             *
             * The widget will be hidden (not being shown) after calling this method. If the widget
             * already was invisible, nothing happens.
             *
             * Note that hidden widgets are still layouted by the LayoutManager as if they were
             * visible.
             *
             * @see show()
             */
            virtual void hide();

            /**
             * @brief Calculates and returns the minimum size of this widget.
             *
             * When resizing a widget the new size should not be smaller than the minimum size
             * returned by this method. The minimum size specifies the size at which the widget can
             * still be shown normally without major graphical glitches.
             *
             * A call to resize() does no check against the minimum size. The widget can be
             * resized to a smaller size than is returned by minimumSize().
             *
             * @return The minimum size of this widget.
             * @see preferredSize()
             */
            virtual Gfx::Size minimumSize();

            /**
             * @brief Calculates and returns the preferred size of this widget.
             *
             * The preferred size specifies the size at which the presentation of the widget is
             * optimal. The preferred size depends on the widget, on its content, its insets and its
             * presentation. The preferred size of the Button, for example, depends on the length of
             * the button's text, its font size, the spacing of the text to the border (=insets)
             * and the border width itself.
             *
             * The optimal presentation concerning all those attributes contribute to the preferred
             * size.
             *
             * @return The preferred size of this widget.
             * @see minimumSize()
             */
            virtual Gfx::Size preferredSize();

            /**
             * @brief Updates the layout of this widget by doing a re-layout of all child widgets.
             *
             * This methods lays out the child widgets of this widget by initiating the layouting
             * process of the associated LayoutManager.
             *
             * @see LayoutManager::update();
             */
            virtual void updateLayout();

            /**
             * @brief Resizes this top-level widget to its preferred size and layouts its child
             * widgets accordingly.
             *
             * The widget will calculate the preferred size by considering the preferred sizes of
             * all child widgets (and their child widgets) and then resize this widget so all
             * child widgets can be layouted using their preferred sizes.
             *
             * This method only has an effect if the widget is a top-level widget (=window or
             * dialog). For child widgets the method does nothing.
             *
             * Packing a widget will usually result in a ResizeEvent and several PaintEvents
             * as the widget is resized and contained widgets are re-positioned and resized.
             */
            virtual void pack();

            /**
             * @brief Returns the LayoutData object of this widget.
             *
             * The LayoutData object contains LayoutManager-specific information on how this
             * widget is supposed to be layouted in the LayoutManager's context.
             * TODO Does this method really exist?
             */
            const LayoutData& layoutData() const;

            /**
             * @brief Returns the LayoutManager which is set for this widget.
             *
             * To set the LayoutManager for a widget use the Factory-method of the specific
             * LayoutManager which should be set. The the documentation of the LayoutManager
             * for further details.
             *
             * The LayoutManager object returned by this method is of the generic base class for
             * all LayoutManager objects. To access specific functionality of the actual
             * LayoutManager which is set for this widget, the returned object has to be cast.
             *
             * @return The LayoutManager which is currently set for this widget.
             * @see LayoutMananger
             */
            Layout& layout() const;

            /**
             * @brief Returns a list containing the child widgets of this widget.
             *
             * Widgets can be added as child to a widget using the Widget-constructor or by using
             * reparent(). To remove a children from a widget use unparent() or reparent().
             *
             * @return A list containing the child widgets of this widget.
             * @see Widget()
             * @see reparent()
             */
            const std::list<Widget*>& childWidgets();

            /**
             * @brief Returns a list containing the child widgets of this widget.
             *
             * Widgets can be added as child to a widget using the Widget-constructor or by using
             * reparent(). To remove a children from a widget use unparent() or reparent().
             *
             * @return A list containing the child widgets of this widget.
             * @see Widget()
             * @see reparent()
             */
            const std::list<Widget*>& childWidgets() const;

            /**
             * @brief Removes this widget from the child list of its parent and sets the parent to 0.
             *
             * If the widget has no parent nothing happens.
             *
             * The widget becomes a top-level widget when it is removed from its parent. To
             * set another widget as parent reparent() can be used.
             *
             * @see unparent()
             * @see parent()
             */
            void unparent();

            /**
             * @brief Sets a new parent for this widget.
             *
             * If the parent had a parent before this method is called, it is removed from this
             * parent before the given widget is set a parent widget. If the widget had no parent
             * the given widget is set as parent directly.
             *
             * 0 can be passed as parent pointer. In this case this method does the same as
             * unparent().
             *
             * @see unparent()
             * @see parent()
             */
            void reparent(Widget* parent);

            /**
             * @brief Returns a pointer to the parent widget of this widget.
             *
             * If there is no parent set for this widget, 0 is returned.
             *
             * @return A pointer to the parent widget.
             */
            Widget* parent() const;

            /**
            * @brief Enables this widget. This is the same as calling \c setEnabled(true);
            * @see setEnabled(bool)
            */
            void enable();

            /**
             * @brief Disabled this widget. This is the same as calling \c setEnabled(false);
             * @see setEnabled(bool)
             */
            void disable();

            /**
             * @brief Enables or disables this widget.
             *
             * A disabled widget does not receive input events like mouse events or key events.
             * It is usually also displayed as being disabled, for example by showing the text
             * or background in a specific color.
             *
             * Widgets are enabled by default.
             *
             * Currently the disable state is not propagated to the child widgets. If all children
             * of a widget also need to be disabled, this has currently to be done manually by
             * disabling/enabling all child widgets.
             *
             * @brief newEnabledState New enabled state: \c true for enabled and \c false for disabled.
             */
            void setEnabled(bool newEnabledState);

            /**
             * @brief Returns the current enabled/disabled state of this widget.
             *
             * If the widget is currently enabled \c true is returned. If the widget is disabled
             * \c false is returned.
             *
             * See setEnabled(bool) for more details about the enabled/disabled state of a widget.
             *
             * @return \c true if this widget is enabled; \c false if it is disabled.
             */
            bool isEnabled() const;

            /**
             * @brief Does a complete repaint of this widget's presentation.
             *
             * Does a complete repaint of this widget. If the widget is backbuffered the backbuffer
             * is drawn on the widget's surface to avoid the possibly complex repaint of the
             * widget's graphical representation.
             *
             * This method must be overwritten in sub-classes to draw the specific presentation of
             * the concrete widget.
             */
            virtual void update()
            {}

            /**
             * @brief Returns a Painter object to draw on this widget's surface.
             *
             * The Painter which is returned by this widget can be used to draw on this widget
             * at every time. Note that the paint operations only affect the widget's surface but
             * not its backbuffer. The backbuffer has to be drawn separately by getting a Painter
             * object for the backbuffer.
             *
             * The returned Painter can be copied as often as necessary without much overhead.
             * The Painter object should not be stored for a longer time, for example inside
             * a member variable! It should only be held as long as it is needed to finish the
             * current paint process. After this, it should be freed.
             *
             * Requesting the Painter object may be expensive in resources, so do not request
             * Painter objects multiple times during a paint process, except it is absolutely
             * necessary.
             *
             * @return A Painter object to draw on this widget's surface.
             */
            Painter painter();

            /**
             * @brief Receives all events for this widgets and dispatches them to specific methods.
             *
             * This method only calls _event() and does the house-keeping. For a precise description
             * of the underlying mechanism see _event().
             *
             * To add event dispatching for new events don't override this method, but override
             * _event() directly.
             *
             * @param event The event which should be dispatched.
             * @see _event()
             */
            void event(const Event& event);

            /**
             * @brief Receives and processes a CloseEvent.
             *
             * This method only does the house-keeping and then calls _closeEvent(). For a
             * precise description see there.
             *
             * @param event The CloseEvent which is supposed to be processed.
             * @see _closeEvent()
             */
            void closeEvent(const CloseEvent& event);

            /**
             * @brief Receives and processes a MouseEvent.
             *
             * This method only does the house-keeping and then calls _mouseEvent(). For a
             * precise description see there.
             *
             * @param event The MouseEvent which is supposed to be processed.
             * @see _mouseEvent()
             */
            void mouseEvent(const MouseEvent& event);

            /**
             * @brief Receives and processes a MouseMoveEvent.
             *
             * This method only does the house-keeping and then calls _mouseMoveEvent(). For a
             * precise description see there.
             *
             * @param event The MouseMoveEvent which is supposed to be processed.
             * @see _mouseMoveEvent()
             */
            void mouseMoveEvent(const MouseMoveEvent& event);

            /**
             * @brief Receives and processes a MoveEvent.
             *
             * This method only does the house-keeping and then calls _moveEvent(). For a
             * precise description see there.
             *
             * This method updates the internally stored position of the widget.
             *
             * @param event The MoveEvent which is supposed to be processed.
             * @see _moveEvent()
             */
            void moveEvent(const MoveEvent& event);

            /**
             * @brief Receives and processes a PaintEvent.
             *
             * This method only does the house-keeping and then calls _paintEvent(). For a
             * precise description see there.
             *
             * @param event The PaintEvent which is supposed to be processed.
             * @see _paintEvent()
             */
            void paintEvent(const PaintEvent& event);

            /**
             * @brief Receives and processes a ResizeEvent.
             *
             * This method only does the house-keeping and then calls _resizeEvent(). For a
             * precise description see there.
             *
             * This method updates the internally stored size of the widget.
             *
             * @param event The ResizeEvent which is supposed to be processed.
             * @see _resizeEvent()
             */
            void resizeEvent(const ResizeEvent& event);

            /**
             * @brief Receives and processes a KeyEvent.
             *
             * This method only does the house-keeping and then calls _keyEvent(). For a
             * precise description see there.
             *
             * @param event The KeyEvent which is supposed to be processed.
             * @see _keyEvent()
             */
            void keyEvent(const KeyEvent& event);

        public:
            //! @brief Signal which is sent when the widget is closed by the underlying platform.
            //! To get informed about signals use one of the connect()-methods.
            Signal<> closed;

            //! @brief Signal which is sent when the widget object is destroyed (-> destructor).
            //! To get informed about signals use one of the connect()-methods.
            Signal<Widget&> destroyed;

        protected:
            /**
             * @brief Receives all events for this widgets and dispatches them to specific methods.
             *
             * This method receives all events for this widget and dispatches them to one of the
             * xxxEvent-methods. A CloseEvent is passed to closeEvent(), a MouseEvent is passed
             * to mouseEvent() and so on. These methods forward them to the corresponding method
             * starting with an underscore, for example _closeEvent() and _mouseEvent() after they
             * have finished their house-keeping code.
             *
             * If the received event is unknown and so can not be dispatched, it is discarded.
             *
             * To add new events to the dispatch processing this method must be overridden. (Do
             * not override event()!) To allow the _event()-method to dispatch all events which
             * already were know before, the _event()-method of the super-class should be called.
             *
             * @param event The event which should be dispatched.
             */
            virtual void _event(const Event& event);

            /**
             * @brief Receives and processes a CloseEvent.
             *
             * A CloseEvent is generated when a widget is closed, which means that it was destroyed.
             *
             * When a CloseEvent is received by this method the 'closed' signal is sent to all
             * registered slots.
             *
             * Override this method to add a more specific CloseEvent-handling.
             *
             * @param event The CloseEvent which is processed.
             */
            virtual void _closeEvent(const CloseEvent& event);

            /**
             * @brief Receives and processes a MouseEvent.
             *
             * A MouseEvent is generated when the widget was clicked.
             *
             * Override this method to add a more specific MouseEvent-handling.
             *
             * @param event The MouseEvent which is processed.
             */
            virtual void _mouseEvent(const MouseEvent& event);

            /**
             * @brief Receives and processes a MoveEvent.
             *
             * A MoveEvent is generated when the widget was moved by the user or when
             * Widget::move() was called.
             *
             * Override this method to add a more specific MoveEvent-handling.
             *
             * @param event The MoveEvent which is processed.
             */
            virtual void _moveEvent(const MoveEvent& event);

            /**
             * @brief Receives and processes a MouseMoveEvent.
             *
             * A MouseMoveEvent is generated when the mouse cursor is moved over the widget or
             * when the mouse cursor enters or leaves the widget.
             *
             * Override this method to add a more specific MouseMoveEvent-handling.
             *
             * @param event The MouseMouseEvent which is processed.
             */
            virtual void _mouseMoveEvent(const MouseMoveEvent& event);

            /**
             * @brief Receives and processes a PaintEvent by doing a repaint of the widget.
             *
             * A PaintEvent is usually generated when parts of or all of the widget is disclosed or
             * when the widget becomes visible for other reasons.
             *
             * Override this method to paint the widget whenever it is necessary.
             *
             * @param event The PaintEvent which is processed.
             */
            virtual void _paintEvent(const PaintEvent& event);

            /**
             * @brief Receives and processes a ResizeEvent.
             *
             * A ResizeEvent is generated when the widget is resized by the user or by a call
             * to Widget::resize().
             *
             * Override this method to add a more specific ResizeEvent-handling.
             *
             * @param event The ResizeEvent which is processed.
             */
            virtual void _resizeEvent(const ResizeEvent& event);

            /**
             * @brief Receives and processes a KeyEvent.
             *
             * A KeyEvent is generated when a key is pressed while the widget has the focus.
             *
             * Override this method to handle keys which are pressed for a widget.
             *
             * @param event The KeyEvent which is processed.
             */
            virtual void _keyEvent(const KeyEvent& event);

        public:
            /**
             * @brief Returns the platform-dependent implementation of this Widget.
             * @return The implementation of this Widget.
             */
            WidgetImpl& impl()
            { return *_impl; }

            /**
             * @brief Returns the platform-dependent implementation of this Widget.
             * @return The implementation of this Widget.
             */
            const WidgetImpl& impl() const
            { return *_impl; }

        protected:
            friend class WidgetImpl;

            /**
             * @brief Add the given widget to the child list of this widget.
             *
             * Note that this method does not update any parent-information. It only does add
             * the widget to the child list without changing any real parent/child-relationship.
             *
             * Also note that a widget may be added multiple times to this list. No checking is
             * done to prevent this.
             *
             * @param widget This widget is added to the child list of this widget.
             */
            void addChild(Widget& widget)
            {
                _childWidgets.push_back(&widget);
            }

            /**
             * @brief Removes the given widget from the child list of this widget.
             *
             * Note that this method does not update any parent-information. It only does remove
             * the widget from the child list without changing any real parent/child-relationship.
             *
             * If the given widget can not be found in the child list, nothing happens.
             *
             * @param widget This widget is removed from the child list of this widget.
             */
            void removeChild(Widget& widget)
            {
                _childWidgets.remove(&widget);
            }

        private:
            friend class Layout;

            /**
             * @brief Sets the given LayoutManger as new LayoutManager for this widget.
             *
             * This method is only called by the LayoutManager's factory method and is used
             * only internally. A LayoutManager can only be set for a widget by calling this
             * factory method.
             *
             * @param layout The new LayoutManager which is set for this widget.
             */
            void setLayout(Layout* layout);

        private:
            Widget* _parent;
            Pt::Gfx::Region _region;
            Gfx::ARgbColor _foregroundColor;
            Gfx::ARgbColor _backgroundColor;
            std::list<Widget*> _childWidgets;
            Insets _insets;
            std::auto_ptr<Layout> _layout;
            bool _enabled;
    };

} // namespace Gui

} // namespace Pt

#endif
