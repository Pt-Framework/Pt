/* Copyright (C) 2016 Marc Boris Duerner 
   Copyright (C) 2016 Laurentiu-Gheorghe Crisan

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

#ifndef Pt_Hmi_MenuBar_H
#define Pt_Hmi_MenuBar_H

#include <Pt/Hmi/MenuShell.h>
#include <Pt/Hmi/Panel.h>
#include <Pt/Hmi/Button.h>
#include <Pt/Hmi/FlowLayout.h>
#include <vector>

namespace Pt {

namespace Hmi {

class Menu;

class MenuBarItem : public Button
{
    public:
        MenuBarItem(Menu& menu, const Pt::String& text);

        ~MenuBarItem();

    protected:
        void onClicked(const Gfx::PointF& pos);

        void onPaint(PaintSurface& surface, const Gfx::RectF& updateRect);

    private:
        Menu& _menu;
};


class PT_HMI_API MenuBar : public MenuShell
                         , public Panel
{
    typedef Panel WidgetBaseType;

    public:
        MenuBar();
    
        virtual ~MenuBar();

    protected:
        virtual void onAddMenu(Menu& menu, const Pt::String& text);

        virtual void onRemoveMenu(Menu& menu);

        virtual void onOpenMenu(Menu& menu)
        {}

        virtual void onCloseMenu(Menu& menu)
        {}

        virtual void onCancel()
        {}

        virtual MenuShell* onFindMenu(const Gfx::PointF& screenPos);

    protected:
        virtual void onResizeEvent(const ResizeEvent& ev);

    private:
        FlowLayout                _layout;
        std::vector<MenuBarItem*> _menus;
};

} // namespace

} // namespace

#endif
