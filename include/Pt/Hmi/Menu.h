/* Copyright (C) 2013 Marc Boris Duerner 
   Copyright (C) 2013 Laurentiu-Gheorghe Crisan

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

#ifndef Pt_Hmi_Menu_H
#define Pt_Hmi_Menu_H

#include <Pt/Hmi/Window.h>
#include <Pt/Hmi/Panel.h>
#include <Pt/Hmi/FlowLayout.h>

namespace Pt {

namespace Hmi {

class MenuItem;

class PT_HMI_API Menu : public Window
{
    typedef Window BaseType;
    friend class SubMenu;

    public:
        Menu();
    
        virtual ~Menu();

        void addItem(MenuItem& item);

        void removeItem(MenuItem& item);

        void setParentMenu(Menu* menu);

        // TODO: move this to window and widget?
        Signal<>& closed()
        {
            return _closed;
        }

    protected:
        virtual void onPaintEvent(const PaintEvent& ev);
        
        virtual void onActivateEvent(const ActivateEvent& ev);

        virtual void onCloseEvent(const CloseEvent& ev);

        virtual void onMoveEvent(const MoveEvent& ev);

        virtual void onResizeEvent(const ResizeEvent& ev);

    protected:
        void onMenuEnter(Menu* m);

        void onMenuLeave();

    private:
        void onItemTriggered();
        
        void onContentChanged();

    private:        
        Signal<>    _closed;
        Menu*       _submenu;
        FlowLayout  _layout;
        std::size_t _iconWidth;
};

} // namespace

} // namespace

#endif
