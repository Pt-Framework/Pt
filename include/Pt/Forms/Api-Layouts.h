/* Copyright (C) 2026 Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#ifndef PT_FORMS_API_LAYOUTS_H
#define PT_FORMS_API_LAYOUTS_H

/** @addtogroup Pt-Forms-Layouts

    @brief Arrangement of child controls.

    A layout control derives the geometry of its children from a placement
    policy. It remains in the visual hierarchy: a form's content is often a
    layout, and a layout can nest other layouts. Attachment is non-owning.
    %addItem() records the layout-specific placement and attaches the child;
    %removeItem() detaches it. %Control::add() attaches without that policy.
    Keep each child alive until it is detached.

    Layouting follows the cycle in @ref Pt-Forms-Page-Updating. Measure runs
    inside-out under a %SizePolicy. Layout runs outside-in and assigns child
    geometry with %move() and %resize(). A layout implements those passes for
    its policy. %Control::padding() is the inner inset of the layout. A
    child's %Control::margin() is the outer gap around that child. Invisible
    children are skipped.

    Choose a layout by the arrangement it produces.

    @code
    FlowLayout Left              DockingLayout
    +------------------+         +------------------+
    | [A] [B] [C]      |         | Top              |
    +------------------+         +----+--------+----+
                                 |Left| Fill   |Right|
    GridLayout Vertical, 3       +----+--------+----+
    +---+---+---+                | Bottom           |
    | A | B | C |                +------------------+
    +---+---+---+
    | D | E |   |

    TableLayout2                 StackLayout
    +--------+--------+          +------------------+
    | 0,0    | 0,1    |          | current          |
    +--------+--------+          | (others hidden)  |
    | 1,0    | 1,1    |          +------------------+
    +--------+--------+

    CanvasLayout                 ScrollLayout
    +------------------+         +------------------+
    | A@(x,y)          |         | content > view   |
    |        B@(x,y)   |         |            [====]|
    +------------------+         +------------------+
    @endcode

    A flow places children on one axis. Docking consumes edges in add order
    and gives leftover space to Fill. A grid uses uniform cells and wrapping.
    A table assigns children to row and column tracks that can prefer, fix, or
    fill size. A stack shows one child at a time. A canvas uses stored
    positions. A scroll layout offsets children when content exceeds the
    viewport; it is not a scroll view with bars.

    Layout attributes refine placement without being layout controls.
    %Spacing is the four-sided inset used as a control's margin or padding.
    %Direction is the axis of a flow. %Alignment places content in a
    rectangle. %Adjustment aligns along one axis, typically a line of text.

    @code
    Spacing                         Alignment
    +------------------+            +---+---+---+
    |      top         |            |TL | T |TR |
    | left  [  ] right |            | L | C | R |
    |     bottom       |            |BL | B |BR |
    +------------------+            +---+---+---+

    Direction                       Adjustment
    Left -->   Top |                Left   |text    |
    <-- Right      v                Center |  text  |
                   Bottom           Right  |    text|
    @endcode
*/

#endif
