/*
  Copyright (C) 2026 Marc Boris Duerner

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
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the:
  Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
  Boston, MA 02110-1301 USA
*/

#ifndef PT_FORMS_API_ARCHITECTURE_H
#define PT_FORMS_API_ARCHITECTURE_H

/** @defgroup Pt-Forms-Architecture Forms Architecture

    @brief The Forms object model and lifecycle.

    A Forms application creates windows and controls, processes platform
    events, and runs the event loop. The inherited event loop also supports
    I/O, timers, and asynchronous operations.

    %Application is the runtime root. It provides the main %Screen, which
    represents a display and owns its top-level %WindowManager. A
    %WindowManager can also be embedded in a %Workspace. Windows attach to a
    window manager; each window is a %Form that acts as the root of one
    control tree.

    The following diagram shows the class hierarchy. It does not describe the
    runtime parent relationship between controls.

    @code
    Widget
        View
            Form
                Window
            Control
            Layout
    @endcode

    The runtime containment model is separate from that hierarchy:

    @code
    Application -> Screen -> Window -> Form -> content Control -> child Controls
    @endcode

    %Screen and %View are %Widget types. A widget supplies the common geometry,
    visibility, repainting, scaling, screen-connection, and event-processing
    behavior. The platform backend submits events to the application event
    loop, which dispatches them to widgets. Every widget is also a %Responder.
    This lets input events travel through the responder chain.

    %Form and %Control are %View types. A form has one main content control; a
    control can have several child controls. Together, these controls form the
    visible user interface. A form is the layout root for its content tree,
    while each control is responsible for the geometry of its direct children.
    Layouting happens in two passes. The first pass determines the space each
    control needs. The second pass arranges the controls within the available
    space.

    A view provides a %PaintSurface for its attached controls and translates
    coordinates at the boundary between the view and each control. A window is
    connected to the paint surface supplied by its internal %WindowFrame. The
    frame uses a pixmap surface that the display presents after painting.

    Changes to visual state or layout normally request a repaint. The repaint
    request travels upward through the views to the window and its window frame.
    Painting then travels downward again: the window paints its content, and
    forms and controls paint their visible children.
*/

#endif