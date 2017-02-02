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

#ifndef Pt_Hmi_ListBox_H
#define Pt_Hmi_ListBox_H

#include <Pt/Hmi/Control.h>
#include <Pt/Hmi/Button.h>
#include <Pt/Hmi/ScrollView.h>
#include <Pt/Hmi/FlowLayout.h>
#include <Pt/Gfx/Color.h>
#include <Pt/Gfx/Brush.h>
#include <Pt/SmartPtr.h>

namespace Pt {

namespace Hmi {

class PT_HMI_API ListBoxItem : public Button
{
    typedef Control Button;

	  public:
        ListBoxItem();
		
        virtual ~ListBoxItem();	

    protected:
        virtual void onInvalidate();
	
        virtual void onPaint(PaintSurface& surface, const Gfx::RectF& updateRect);

        virtual void onResizeEvent(const ResizeEvent& ev);
};


class PT_HMI_API ListBoxLayout : public FlowLayout
{
    typedef FlowLayout Base;

	  public:
        ListBoxLayout();
		
        virtual ~ListBoxLayout();

    protected:
        virtual void onAddWidget(Widget& w);

        virtual void onRemoveWidget(Widget& w);

    private:
        void onContentChanged();
};


class PT_HMI_API ListBox : public Control
{
    typedef Control Base;

	  public:
        ListBox();
		
        virtual ~ListBox();

        void addItem(ListBoxItem& item);

        void removeItem(ListBoxItem& item);

    protected:
        virtual void onInvalidate();
	
        virtual void onPaint(PaintSurface& surface, const Gfx::RectF& updateRect);
    
    protected:
        virtual void onResizeEvent(const ResizeEvent& ev);
    
    private:
        ListBoxLayout _layout;
        ScrollView    _scrollView;
};

} // namespace

} // namespace

#endif
