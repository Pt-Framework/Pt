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
#ifndef PT_FORMS_MENU_H
#define PT_FORMS_MENU_H

#include <Pt/Forms/Api.h>
#include <Pt/Forms/MenuBase.h>
#include <Pt/Forms/Popup.h>
#include <Pt/Forms/FlowLayout.h>
#include <Pt/Forms/StyleOptions.h>
#include <Pt/Gfx/Brush.h>
#include <Pt/Gfx/Pen.h>
#include <Pt/SmartPtr.h>

namespace Pt {
namespace Forms {

class MenuItem;
class MenuBarItem;
class MenuSubItem;
class MenuMenuItem;
class MenuBaseItem;

class PT_FORMS_API Menu : public Pt::Forms::Popup, protected MenuBase
{
    public:
        friend class MenuBar;
        friend class MenuMenuItem;

    public:
        Menu();
    
        virtual ~Menu();
        
        double iconWidth() const;

        const Pt::Gfx::Brush& background() const;

        void setBackground(const Pt::Gfx::Brush& b);

        const Pt::Gfx::Pen& contour() const;

        void setContour(const Pt::Gfx::Pen& p);     

        void addItem(MenuItem& item);

        void addItem(MenuSubItem& item);

        void removeItem(MenuItem& item);

        void removeItem(MenuSubItem& item);
        
    protected:
        //Pt::Forms::Popup
        virtual void onInvalidate();

        virtual void onPaintEvent(const Pt::Forms::PaintEvent& ev);

        virtual void onCloseEvent(const Pt::Forms::CloseEvent& ev);

        virtual void onShowEvent(const Pt::Forms::ShowEvent& ev);

        virtual bool onMouseEvent(const Pt::Forms::MouseEvent& ev);

        virtual bool onEnterEvent(const Pt::Forms::EnterEvent& ev);

        virtual bool onLeaveEvent(const Pt::Forms::LeaveEvent& ev);

        virtual void onRenderBackground( const Pt::Forms::StyleOptions& options, Pt::Forms::Painter& painter,
                                         const Pt::Gfx::RectF& rect) const;
       
       //MenuBase
        virtual void onAddMenu(MenuMenuItem& item);

        virtual void onRemoveMenu(MenuMenuItem& item);

        virtual void onCloseMenu(MenuMenuItem& item);

        virtual void onOpenMenu(MenuMenuItem& item);

        virtual void onCancel();

        virtual Pt::Forms::Widget* onFindMenu(const Pt::Gfx::PointF& screenPos);

    protected:
        void onProcessMouseEvent(const Pt::Forms::MouseEvent& ev);

    private:
        void onItemTriggered(MenuBaseItem& m);

        void drawBorder(Pt::Forms::Painter& painter, const Pt::Gfx::RectF& borderRect) const;

    private:
        MenuBaseItem*          _currentItem;
        Pt::Forms::FlowLayout    _layout;
        Pt::ssize_t            _iconWidth;
        Pt::AutoPtr<Pt::Gfx::Brush>  _background;
        Pt::AutoPtr<Pt::Gfx::Pen>   _contour;
        Pt::Gfx::Brush             _brush;
        Pt::Gfx::Pen               _pen;
        bool                     _isOpen;
};

}}

#endif
