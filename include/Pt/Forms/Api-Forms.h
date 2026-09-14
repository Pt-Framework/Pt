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

#ifndef PT_FORMS_API_FORMS_H
#define PT_FORMS_API_FORMS_H

/** @defgroup Pt-Forms Graphical User Interfaces

    @brief Application lifecycle and structure.

    A Forms application constructs its visual hierarchy, shows its windows,
    and runs the event loop. The inherited event loop also supports I/O,
    timers, and asynchronous operations.

    %Application is the runtime root. Construct it before forms, windows, or
    controls. It is not a %Widget. The application provides the primary
    %Screen and its %WindowManager, dispatches platform events, and owns the
    shared scaling, style, font, and input-method services.

    The C++ class hierarchy and the visual hierarchy describe different
    relationships. The following diagram shows inheritance only. It does not
    describe which objects display or contain other objects. In particular,
    %Window derives from %Form; it is not a direct child of %View.

    @code
    Widget
        Screen
        WindowManager
        View
            Form
                Window
            Control
    @endcode

    The visual hierarchy describes how Forms objects appear together. Each
    level follows the same non-owning host pattern: %Application provides the
    %Screen, a %WindowManager attaches %Window objects, a %Form attaches one
    content %Control, and a %Control attaches child controls, including
    layouts. A %Workspace applies the same pattern inside an ordinary control:
    its window manager presents several windows within the workspace bounds.

    @code
    Application -> Screen -> WindowManager -> Window (Form) -> content Control -> child Controls
    @endcode

    Attaching a window or control does not transfer ownership. The code that
    creates an object keeps it alive while the hierarchy uses it. A window or
    control removes itself from its parent when it is destroyed.

    The following example creates a visual hierarchy with a window, its
    content layout, and a control displayed by that layout.

    @code
    int main(int argc, char** argv)
    {
        Pt::Forms::Application application(argc, argv);

        Pt::Forms::Label label;
        label.setText("Hello, Forms");

        Pt::Forms::FlowLayout content;
        content.addItem(label);

        Pt::Forms::Window window;
        window.setTitle("Example");
        window.setContent(&content);
        window.show();

        application.run();
        return 0;
    }
    @endcode
*/

/** @defgroup Pt-Forms-Widgets Widgets and Views

    @ingroup Pt-Forms
*/

/** @defgroup Pt-Forms-Windows Windows and Workspaces

    @ingroup Pt-Forms
*/

/** @defgroup Pt-Forms-Updating Layouting and Painting

    @ingroup Pt-Forms
*/

/** @defgroup Pt-Forms-Input Pointer and Keyboard Input

    @ingroup Pt-Forms
*/

/** @defgroup Pt-Forms-Layouts Layouts

    @ingroup Pt-Forms
*/

/** @defgroup Pt-Forms-Buttons Buttons

    @ingroup Pt-Forms
*/

/** @defgroup Pt-Forms-Displays Displays

    @ingroup Pt-Forms
*/

/** @defgroup Pt-Forms-Editors Editors and Selectors

    @ingroup Pt-Forms
*/

/** @defgroup Pt-Forms-Icons Icons and Text

    @ingroup Pt-Forms
*/

/** @defgroup Pt-Forms-Styling Styles and Renderers

    @ingroup Pt-Forms
*/

#endif