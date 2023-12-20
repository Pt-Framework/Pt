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
#ifndef PT_HMI_MENUBAR_H
#define PT_HMI_MENUBAR_H

#include <Pt/Hmi/Api.h>
#include <Pt/Hmi/MenuBase.h>
#include <Pt/Hmi/MenuBarItem.h>
#include <Pt/Hmi/Button.h>
#include <Pt/Hmi/Control.h>
#include <Pt/Hmi/FlowLayout.h>
#include <Pt/SmartPtr.h>
#include <vector>

namespace Pt {
namespace Hmi {

class Menu;
class MenuBar;


class PT_HMI_API MenuBar : public Pt::Hmi::Control, protected MenuBase
{ 
    public:
        MenuBar();
    
        virtual ~MenuBar();

        void addItem(MenuBarItem& item);

        void removeItem(MenuBarItem& item);

        const Pt::Gfx::Brush& background() const;

        void setBackground(const Pt::Gfx::Brush& b);

        const Pt::Gfx::Pen& contour() const;

        void setContour(const Pt::Gfx::Pen& p);


    protected:

        virtual Pt::Hmi::Visual* onFindMenu(const Pt::Gfx::PointF& screenPos);

        virtual void onAddMenu(MenuMenuItem& item);

        virtual void onRemoveMenu(MenuMenuItem& item);

        virtual void onOpenMenu(MenuMenuItem& item);

        virtual void onCloseMenu(MenuMenuItem& item);

        virtual void onCancel();
       
        virtual void onInvalidate();

        virtual Pt::Gfx::SizeF onMeasure(const Pt::Hmi::SizePolicy& policy);

        virtual void onLayout(const Pt::Gfx::RectF& rect);

        virtual void onPaint(Pt::Gfx::PaintSurface& surface, const Pt::Gfx::RectF& rect);
        
        virtual bool onMouseEvent(const Pt::Hmi::MouseEvent& ev);

        virtual bool onTouchEvent(const Pt::Hmi::TouchEvent& ev);

        void onItemClicked(MenuBaseItem& item);

        void onProcessMouseEvent(const Pt::Hmi::MouseEvent& ev);

    private:
        Pt::Hmi::FlowLayout         _layout;
        MenuMenuItem*               _currentItem;
        Pt::AutoPtr<Pt::Gfx::Brush> _background;
        Pt::AutoPtr<Pt::Gfx::Pen>   _contour;
        Pt::Gfx::Brush               _brush;
        Pt::Gfx::Pen                 _pen;        
};

}}

#endif
