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
#ifndef PT_FORMS_MENUBAR_H
#define PT_FORMS_MENUBAR_H

#include <Pt/Forms/Api.h>
#include <Pt/Forms/MenuBase.h>
#include <Pt/Forms/MenuBarItem.h>
#include <Pt/Forms/Button.h>
#include <Pt/Forms/Control.h>
#include <Pt/Forms/FlowLayout.h>
#include <Pt/SmartPtr.h>
#include <vector>

namespace Pt {

namespace Forms {

class Menu;
class MenuBar;

/** @brief Represents a horizontal bar of top-level menus.

    Use a %MenuBar to present a stable row of top-level command categories.
    Add %MenuBarItem objects and associate each item with a %Menu. The bar
    opens the selected menu below its item and closes the previously active
    menu. It does not own its items; remove an item before destroying it.

    A menu bar derives its default background and contour from the application
    style during invalidation. %setBackground() and %setContour() supply local
    overrides for this menu bar.

    @code
    Pt::Forms::MenuBarItem fileItem;
    fileItem.setText("File");

    Pt::Forms::MenuBarItem editItem;
    editItem.setText("Edit");

    Pt::Forms::Menu fileMenu;
    fileMenu.addItem(&fileItem);
    fileMenu.addItem(&editItem);

    Pt::Forms::MenuBar menuBar;
    menuBar.addItem(fileItem);
    @endcode

    @ingroup Pt-Forms-Menus
*/
class PT_FORMS_API MenuBar : public Control
                         , protected MenuBase
{
    typedef Control Base;

    public:
        /** @brief Creates an empty menu bar.
        */
        MenuBar();

        /** @brief Destroys the menu bar.
        */
        virtual ~MenuBar();

        /** @brief Adds top-level entry @a item to this menu bar.

            The menu bar does not own @a item. Keep it alive until it is
            removed. Keep an associated menu alive while the item uses it.
        */
        void addItem(MenuBarItem& item);

        /** @brief Removes top-level entry @a item from this menu bar.
        */
        void removeItem(MenuBarItem& item);

        /** @brief Returns the background brush.

            Returns the local override when one was set; otherwise returns the
            application style background brush.
        */
        const Pt::Gfx::Brush& background() const;

        /** @brief Sets a local background brush.
        */
        void setBackground(const Pt::Gfx::Brush& b);

        /** @brief Returns the contour pen.

            Returns the local override when one was set; otherwise returns the
            application style contour pen.
        */
        const Pt::Gfx::Pen& contour() const;

        /** @brief Sets a local contour pen.
        */
        void setContour(const Pt::Gfx::Pen& p);

    protected:

        virtual Pt::Forms::Widget* onFindMenu(const Pt::Gfx::PointF& screenPos);

        virtual void onAddMenu(MenuSubItem& item);

        virtual void onRemoveMenu(MenuSubItem& item);

        virtual void onOpenMenu(MenuSubItem& item);

        virtual void onCloseMenu(MenuSubItem& item);

        virtual void onCancel();

        virtual void onInvalidate();

        virtual Pt::Gfx::SizeF onMeasure(const Pt::Forms::SizePolicy& policy);

        virtual void onLayout(const Pt::Gfx::RectF& rect);

        virtual void onPaint(PaintContext& context, const Pt::Gfx::RectF& rect);

        virtual bool onMouseEvent(const Pt::Forms::MouseEvent& ev);

        virtual bool onTouchEvent(const Pt::Forms::TouchEvent& ev);

        void onItemClicked(MenuItemBase& item);

        void onProcessMouseEvent(const Pt::Forms::MouseEvent& ev);

    private:
        Pt::Forms::FlowLayout         _layout;
        MenuSubItem*               _currentItem;
        Pt::AutoPtr<Pt::Gfx::Brush> _background;
        Pt::AutoPtr<Pt::Gfx::Pen>   _contour;
        Pt::Gfx::Brush               _brush;
        Pt::Gfx::Pen                 _pen;
};

}}

#endif
