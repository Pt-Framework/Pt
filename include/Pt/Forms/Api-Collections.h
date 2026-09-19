/* Copyright (C) 2026 Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#ifndef PT_FORMS_API_COLLECTIONS_H
#define PT_FORMS_API_COLLECTIONS_H

/** @addtogroup Pt-Forms-Collections

    @brief Lists, tabs, and scrollable content.

    Use a collection when the user should browse a list, switch among
    pages, or scroll content that does not fit.

    %ListBox is a scrollable list of caller-owned items. Items show
    text, sometimes with an icon. A selected item is highlighted.
    Optional scroll bars appear when the list does not fit.

    %ListBoxItem is an item in a list. The application creates it and
    adds it to a %ListBox. %ComboBox uses the same type. An item
    shows text and an optional icon. Selection highlights the item.

    %TabView presents several pages. A row of tabs shows the
    titles. One tab is current. Choosing a tab shows that page.

    %ScrollBar is a position in a range. A track shows the range.
    A handle shows the current position. Decrease and increase
    controls step it. A style may place those controls at the ends
    of the track.

    %ScrollView is a viewport over larger content. Optional scroll
    bars move the content.

    %TabViewItem is not a control. %TabView creates tab labels from
    %addTab titles. The view owns the titles, the current tab, tab
    geometry, and tab input. There is no tab-bar widget.
    %ListBoxLayout is an integrated subpart. Applications do not
    construct it.

    List and tab chrome are separate from repeated-element visuals.
    %ListBoxRenderer paints the list. %ListItemRenderer paints an
    item. %TabViewRenderer paints the view chrome and each tab.
    Item and tab methods take rectangles, text, icons, and item or
    tab state. They do not take the item widget, a model object, or
    the tab collection.

    A collection that only needs pane or frame chrome can use the
    panel appearance family. A dedicated container renderer is for
    chrome that differs from a panel.

    %ScrollView has no appearance family of its own. Optional bars
    are %ScrollBar objects. Offset of oversized content is a
    %ScrollLayout.

    Appearance follows the application style. On invalidate the
    control binds a family renderer through a styler. Widget-local
    brushes, pens, colors, fonts, and %setRenderer() overlay that
    style without replacing it.
*/

#endif
