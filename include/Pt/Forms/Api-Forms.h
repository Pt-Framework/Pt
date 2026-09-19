/* Copyright (C) 2026 Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
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

/** @defgroup Pt-Forms-Collections Collections

    @ingroup Pt-Forms
*/

/** @defgroup Pt-Forms-Menus Menus

    @ingroup Pt-Forms
*/

/** @defgroup Pt-Forms-Icons Icons and Text

    @ingroup Pt-Forms
*/

/** @defgroup Pt-Forms-Styling Styles and Renderers

    @ingroup Pt-Forms
*/

#endif