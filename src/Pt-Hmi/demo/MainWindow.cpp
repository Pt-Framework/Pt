/* Copyright (C) 2013 Laurentiu-Gheorghe Crisan
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
 * MA  02110-1301  USA 
*/

#include "MainWindow.h"
#include "AtesionIcon.h"
#include <Pt/Hmi/Application.h>
#include <Pt/Gfx/PngReader.h>
#include <sstream>
#include <fstream>

namespace Pt {

namespace Hmi {

namespace Demo {

MainWindow::MainWindow()
: _child1("Child 1")
//, _child2("Child 2")
{
    const char* iconData = reinterpret_cast<const char*>(atesionIcon);
    std::streamsize iconSize = sizeof(atesionIcon);

    std::stringstream ss(std::ios::binary|std::ios::in|std::ios::out);
    ss.write(iconData, iconSize);
    ss.clear();
    ss.seekg(0);

    Gfx::Image icon;
    Gfx::PngReader reader(ss, icon);

    while( ! reader.advance() )
    {
        ss.peek();
        reader.advance();
    }

    setTitle("Main 1");
    move( Gfx::PointF(60, 30) );
    resize( Gfx::SizeF(600, 480) ); 
    
    add( _child1 );
    //add( _child2 );

    // context menu
    _menu.setName("All Music");
    
    _item1.setText("Heavy Metal");
    _item1.setIcon(icon);
    _item1.setShortcut( &Key(Key::F3) );
    _menu.addItem(_item1);

    _item2.setText("Classic Rock");
    _menu.addItem(_item2);

    _item3.setText("Folk Music");
    _item3.setShortcut( &Key(Key::Control, Key::A) );
    _menu.addItem(_item3);

    // context sub menu
    _subMenu.setName("Other Music");
    _subItem1.setText("Vegetarian Progressive Grindcore");
    _subMenu.addItem(_subItem1);
    
    _subItem2.setText("Rhythm and Blues");
    _subMenu.addItem(_subItem2);

    _subItem3.setText("Romanian Polka");
    _subMenu.addItem(_subItem3);
    
    _menu.addMenu(_subMenu, "Other Music");
    _child1.show(true);
}


MainWindow::~MainWindow()
{
}


void MainWindow::onMouseEvent(const MouseEvent& ev)
{
    Window::onMouseEvent(ev);

    if( ev.isRelease(MouseEvent::Right) )
    {
        Gfx::PointF menuPos = this->toScreen( ev.position() );

        _menu.move(menuPos);
        _menu.show();
    }
}


void MainWindow::onCloseEvent(const CloseEvent& ev)
{
   Window::onCloseEvent(ev);
   Application::instance().exit();
}

} // namespace

} // namespace

} // namespace
