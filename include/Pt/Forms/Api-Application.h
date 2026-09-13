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

#ifndef PT_FORMS_API_APPLICATION_H
#define PT_FORMS_API_APPLICATION_H

/** @defgroup Pt-Forms-Application Forms Applications

    @brief The Application lifecycle and Structure.

    A Forms application creates windows and controls, shows the visual
    hierarchy, and runs the event loop. The inherited event loop also supports
    I/O, timers, and asynchronous operations.

    %Application is the starting point and must be constructed before forms,
    windows, or controls. It is not a %Widget. It provides the primary
    %Screen and %WindowManager, dispatches platform events, and supplies
    shared services such as scaling, styles, fonts, and input methods.

    Forms distinguishes the class hierarchy from the visual hierarchy. The
    following diagram shows the C++ inheritance relationship. It does not show
    which objects display or contain other objects.

    @code
    Widget
        Screen
        WindowManager
        View
            Form
                Window
            Control
    @endcode

    The visual hierarchy describes how Forms objects appear together. The
    same non-owning host pattern repeats at each level: %Application provides
    the %Screen, a %WindowManager attaches %Window objects, a %Form attaches
    one content %Control, and a %Control attaches child controls, including
    layouts. A %Workspace follows the same principle inside an ordinary
    control: it contains a window manager that presents several windows in
    its bounds.

    @code
    Application -> Screen -> WindowManager -> Window (Form) -> content Control -> child Controls
    @endcode

    Forms does not take ownership of a window or control when it becomes part
    of the visual hierarchy. The code that creates an object keeps it alive
    while the hierarchy uses it. A window or control removes itself from its
    parent when it is destroyed.

    The following example creates the smallest useful visual hierarchy: a
    window, its content layout, and a control displayed by that layout.

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

#endif