/* Copyright (C) 2015 Marc Boris Duerner
   Copyright (C) 2015 Laurentiu-Gheorghe Crisan

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
#ifndef PT_FORMS_MENUBASE_H
#define PT_FORMS_MENUBASE_H

#include <Pt/Forms/Api.h>
#include <Pt/Forms/Widget.h>

namespace Pt {
namespace Forms {

class MenuSubItem;
class MenuItemBase;

class PT_FORMS_API MenuBase
{
    public:
        MenuBase();

        virtual ~MenuBase();

        void cancel()
        {
            onCancel();
        }

        Pt::Forms::Widget* findMenu(const Pt::Gfx::PointF& screenPos)
        {
            return onFindMenu(screenPos);
        }

        void closeMenu(MenuSubItem& item)
        {
            onCloseMenu(item);
        }

        void openMenu(MenuSubItem& item)
        {
            onOpenMenu(item);
        }

        const MenuItemBase* parentItem() const
        {
            return _parentItem;
        }

        MenuItemBase* parentItem()
        {
            return _parentItem;
        }

        void setParentItem(MenuItemBase* item)
        {
            _parentItem = item;
        }

        void addMenu(MenuSubItem& item)
        {
            onAddMenu(item);
        }

        void removeMenu(MenuSubItem& item)
        {
            onRemoveMenu(item);
        }

    protected:

        virtual void onCloseMenu(MenuSubItem& item) = 0;

        virtual void onOpenMenu(MenuSubItem& item) = 0;

        virtual void onAddMenu(MenuSubItem& item) = 0;

        virtual void onRemoveMenu(MenuSubItem& item) = 0;

        virtual void onCancel() = 0;

        virtual Pt::Forms::Widget* onFindMenu(const Pt::Gfx::PointF& screenPos) = 0;

    private:
        MenuItemBase* _parentItem;
};

}}

#endif