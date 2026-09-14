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
    %addTab titles. %ListBoxLayout is an integrated subpart.
    Applications do not construct it.

    %ScrollView has no appearance family of its own. Optional bars
    are %ScrollBar objects. Offset of oversized content is a
    %ScrollLayout.

    Appearance follows the application style. On invalidate the
    control binds a family renderer through a styler. Widget-local
    brushes, pens, colors, fonts, and %setRenderer() overlay that
    style without replacing it.
*/

#endif
