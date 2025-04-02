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
   MA  02110-1301  USA
*/

#ifndef Pt_Forms_MenuImpl_H
#define Pt_Forms_MenuImpl_H

#include <Pt/Forms/Window.h>
#include <Pt/Forms/FlowLayout.h>
#include <Pt/Forms/MenuItem.h>
#include <Pt/Forms/MenuShell.h>
#include <vector>

namespace Pt {

namespace Forms {

class Menu;

class SubMenuItem : public MenuItem
{
    typedef MenuItem BaseType;

    public:
        explicit SubMenuItem(Menu& menu, const Pt::String& text);
    
        virtual ~SubMenuItem();

        Menu& menu()
        { return _menu; }

    protected:
        virtual Gfx::SizeF onAutoSize() const;

        virtual void onPaintShortcut(PaintSurface& surface, 
                                     const Gfx::RectF& updateRect);

    private:
        Menu& _menu;
};


class PT_FORMS_API MenuImpl : public Window 
{
    typedef Window BaseType;

    public:
        MenuImpl(Menu& self);
    
        virtual ~MenuImpl();

        void addItem(MenuItem& item);

        void removeItem(MenuItem& item);

        void onAddMenu(Menu& menu, const Pt::String& text);

        void onRemoveMenu(Menu& menu);

        MenuShell* onFindMenu(const Gfx::PointF& screenPos);

        void onOpenMenu(Menu& menu);

        void onCloseMenu(Menu& menu);

        void onCancel();

    protected:
        void onItemTriggered(MenuItem& m);

        void onItemRemoved(MenuItem& m);

        void onMenuTriggered(MenuItem& m);

        // TODO: need a common way to react to widget content changes
        void onContentChanged();
    
    protected:
        virtual void onPaintEvent(const PaintEvent& ev);

        virtual void onPaintBackground(const Gfx::RectF& rect);
        
        virtual void onCloseEvent(const CloseEvent& ev);

        virtual void onResizeEvent(const ResizeEvent& ev);

        virtual void onShowEvent(const ShowEvent& ev);

        virtual void onMouseEvent(const MouseEvent& ev);

        virtual void onEnterEvent(const EnterEvent& ev);

        virtual void onLeaveEvent(const LeaveEvent& ev);
 
    private:
        Menu&                     _self;
        std::vector<SubMenuItem*> _subMenus;
        Menu*                     _currentMenu;
        FlowLayout                _layout;
        std::size_t               _iconWidth;
};

} // namespace

} // namespace

#endif
