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

#ifndef PT_FORMS_GALLERY_GALLERYPAGE_H
#define PT_FORMS_GALLERY_GALLERYPAGE_H

#include <Pt/Forms/Panel.h>
#include <Pt/Forms/DockingLayout.h>
#include <Pt/Forms/FlowLayout.h>
#include <Pt/Forms/Label.h>
#include <Pt/Forms/ScrollView.h>
#include <Pt/Forms/ListBox.h>
#include <Pt/Forms/Icon.h>
#include <Pt/String.h>

namespace Pt {

namespace Forms {

namespace Gallery {

Icon loadGalleryIcon();

class GalleryPage : public Panel
{
    public:
        GalleryPage();

        virtual ~GalleryPage();

        ListBoxItem& navItem();

        const String& pageTitle() const;

        void setPageTitle(const String& title);

        void setPageDescription(const String& text);

        void setBody(Control& body, bool scroll = true);

        void log(const String& text);

        void log(const char* text);

    private:
        DockingLayout _pageLayout;
        FlowLayout    _header;
        Label         _title;
        Label         _description;
        ScrollView    _scroll;
        Label         _log;
        ListBoxItem   _navItem;
};

} // namespace

} // namespace

} // namespace

#endif
