/* Copyright (C) 2017 Marc Boris Duerner
 
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

#ifndef Pt_Hmi_ComboBox_H
#define Pt_Hmi_ComboBox_H

#include <Pt/Hmi/Control.h>
#include <Pt/Hmi/Window.h>
#include <Pt/Hmi/ListBox.h>
#include <Pt/Gfx/Color.h>
#include <Pt/Gfx/Brush.h>
#include <Pt/String.h>
#include <Pt/SmartPtr.h>

namespace Pt {

namespace Hmi {

class PT_HMI_API ComboBoxMenu : public Window
{
    typedef Window Base;

	  public:
        ComboBoxMenu();
		
        virtual ~ComboBoxMenu();

        void addItem(ListBoxItem& item);

        void setScrollBars(bool hasScrollBars);

    protected:
        void onPaintBackground(const Gfx::RectF& rect);

        void onShowEvent(const ShowEvent& ev);

        void onMouseEvent(const MouseEvent& ev);

        void onTouchEvent(const TouchEvent& ev);

    private:
        ListBox _items;
};


class PT_HMI_API ComboBox : public Control
{
    typedef Control Base;

	  public:
        ComboBox();
		
        virtual ~ComboBox();

        void setScrollBars(bool hasScrollBars);

    protected:
        void onInvalidate();
	
        virtual void onPaint(PaintSurface& surface, const Gfx::RectF& updateRect);

    protected:
        virtual void onKeyEvent(const KeyEvent& ev);

        virtual void onMouseEvent(const MouseEvent& ev);

        virtual void onTouchEvent(const TouchEvent& ev);

        virtual bool onScrollEvent(const ScrollEvent& ev);

    private:
        void onMenuKeyEvent(const KeyEvent& ev);

        void onOpenCombo();

    private:
        ComboBoxMenu _menu;
        Pt::String  _text;
        ListBoxItem _item1;
        ListBoxItem _item2;
        ListBoxItem _item3;
        ListBoxItem _item4;
        ListBoxItem _item5;
        ListBoxItem _item6;
        ListBoxItem _item7;
        ListBoxItem _item8;
};

} // namespace

} // namespace

#endif
