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
#include <Pt/Gfx/JpegReader.h>
#include <sstream>
#include <fstream>

namespace Pt {

namespace Hmi {

namespace Demo {

void loadIcon(Gfx::Image& icon)
{
    const char* iconData = reinterpret_cast<const char*>(atesionIcon);
    std::streamsize iconSize = sizeof(atesionIcon);

    std::stringstream ss(std::ios::binary|std::ios::in|std::ios::out);
    ss.write(iconData, iconSize);
    ss.clear();
    ss.seekg(0);

    std::istream* is = &ss;
    Gfx::PngReader reader(*is, icon);
    //std::istream* is = &ifs;
    //Gfx::JpegReader reader(*is, icon);

    do
    {
        is->peek();
    } 
    while( ! reader.advance() );

    icon = icon.convert( Pt::Gfx::ImageFormat::argb8888() );

    for(size_t w = 0; w < icon.width(); ++ w )
    {
        for(size_t h = 0; h < icon.height(); ++h )
        {
            Gfx::Color color = icon.color(w,h);

			if( color.red() > 0.99  &&  color.green() >0.99 && color.blue() > 0.99 )				
				color.setAlpha(0);
			else
				color.setAlpha(1);
                
            icon.setColor( w,h, color);
        }
    }
}

MainWindow::MainWindow()
: _child1("Child 1")
{
    loadIcon(_icon);

    _picture.set( _icon);
    setTitle("Main 1");
    move( Gfx::PointF(60, 60) );
    resize( Gfx::SizeF(600, 480) ); 
    
//    _child2.resize( Gfx::SizeF(200, 100) );

    //add( _child2 );



//    _child2.show(true);

    

   
    
/*
    _child2.setMinimumSize( Gfx::SizeF(150, 50) );
    _child2.setMaximumSize( Gfx::SizeF(300, 300) );
    _child2.setTitle("Child 2");
    _child2.move( Gfx::PointF(10, 10) );
    */
    
    add( _child1 );


    _child1.move( Gfx::PointF(30,30));
    _child1.resize( Gfx::SizeF(300, 500) );

    _child1.show(true);
    

    
    
            
    // context menu
    _menu.setName("All Music");
    
    _item1.setText("Heavy Metal");
    _item1.setIcon(_icon);

    Key f3(Key::F3);
    _item1.setShortcut( &f3 );
    _menu.addItem(_item1);

    _item2.setText("Classic Rock");
    _menu.addItem(_item2);

    _item3.setText("Folk Music");
    
    Key ctrlA(Key::Control, Key::A);
    _item3.setShortcut( &ctrlA );
    _menu.addItem(_item3);

    // context sub menu
    _subMenu.setName("Other Music");
    
    _subItem1.setText("Vegetarian Progressive Grindcore");
    _subMenu.addItem(_subItem1);
    
    _subItem2.setText("Super Black Metal");
    _subMenu.addItem(_subItem2);

    _subItem3.setText("Romanian Polka");
    _subMenu.addItem(_subItem3);
    
    _menu.addMenu(_subMenu, "Other Music");
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

        _menu.show(menuPos);
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
