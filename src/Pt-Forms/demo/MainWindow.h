/* Copyright (C) 2013 Laurentiu-Gheorghe Crisan
   
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

#ifndef Pt_Forms_Demo_MainWindow_h
#define Pt_Forms_Demo_MainWindow_h

#include <Pt/Forms/Window.h>
#include <Pt/Forms/Menu.h>
#include <Pt/Forms/MenuItem.h>
#include <Pt/Forms/MenuSubItem.h>
#include <Pt/Forms/ScrollView.h>
#include <Pt/Forms/TabView.h>
#include <Pt/Forms/Shell.h>
#include "ChildW.h"

namespace Pt {

namespace Forms {

namespace Demo {

class MainWindow : public Pt::Forms::Window
{
    public:
        MainWindow();

      virtual ~MainWindow();

    protected:
        void onZoom(MenuBaseItem&);

    protected:
        virtual void onPaintEvent(const PaintEvent& ev);

        virtual bool onMouseEvent(const MouseEvent& ev);

        virtual void onCloseEvent(const CloseEvent& ev);

    private:
        Menu     _menu;
        MenuItem _item1;
        MenuItem _item2;
        MenuItem _item3;

        Menu     _subMenu;
        MenuSubItem _subMenuItem;
        MenuItem _subItem1;
        MenuItem _subItem2;
        MenuItem _subItem3;

        MenuSubItem _zoomSubMenuItem;
        Menu     _zoomMenu;
        MenuItem _zoomItem1;
        MenuItem _zoomItem2;
        MenuItem _zoomItem3;
            
        Shell _shell;

        ChildW _child1;
        Window _child2;

        ScrollView _scrollView;
        FlowLayout _scrollContainer;
        
        ScrollView _scrollView2;
        FlowLayout _scrollContainer2;
        PushButton _btns[20];
        PushButton _btns2[20];
        PushButton _bt2;
        Gfx::Image _image;

        TabView _tabView;
        Label _tabLabel1;
        Label _tabLabel2;
        Label _tabLabel3;
};

} // namespace

} // namespace

} // namespace

#endif
