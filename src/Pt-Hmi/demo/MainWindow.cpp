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
#include <Pt/Gfx/Image.h>
#include <Pt/Gfx/ImagePainter.h>
#include <Pt/Gfx/Algorithm.h>
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
    Gfx::Image image;
    Gfx::PngReader reader(*is, image);
    //std::istream* is = &ifs;
    //Gfx::JpegReader reader(*is, image);
    
    reader.get();

    icon.reset(Pt::Gfx::ImageFormat::argb32(), image.size() );
    Gfx::copy(image.begin(), image.end(), icon.begin() );
    
    for(size_t w = 0; w < icon.width(); ++ w )
    {
        for(size_t h = 0; h < icon.height(); ++h )
        {
            Gfx::Pixel pixel(icon.view(), w, h);

            Gfx::Color color = icon.format().getColor(pixel);
         
            if( color.red() >= 65535 && color.green() >= 65535 && color.blue() >= 65535 )				
                color.setAlpha(0);
            else
                color.setAlpha(65535);
                
            icon.format().setPixel(pixel, color, Gfx::CompositionMode::SourceCopy);
        }
    }
}


MainWindow::MainWindow()
: _child1("Child 1")
, _scrollContainer(Hmi::FlowLayout::Top)
{
    loadIcon(_icon);
    _picture.set(_icon);
    
    setTitle("Main 1");
    move( Gfx::PointF(60, 60) );
    resize( Gfx::SizeF(1000, 1000) ); 
    
    _child2.resize( Gfx::SizeF(600, 800) );
    add( _child2 );
    

    _child2.setTitle("Child 2");
    _child2.move( Gfx::PointF(10, 10) );
    

    for(int n = 0; n < 20; ++n)
    {
        _btns[n].resize(Pt::Gfx::SizeF(100, 40));

        _btns[n].setPadding(5);
        _btns[n].setMargin(5);

        _btns[n].setText("Hallo");
        
        _scrollContainer.add(_btns[n] );
    }
    
    _bt2.resize(Pt::Gfx::SizeF(100, 40));
    _bt2.setText("Ende");
    _bt2.setPadding(5);
    _bt2.setMargin(5);

    _scrollContainer.add(_bt2);

    _scrollContainer.setPadding(5);
    _scrollContainer.setMargin(5);

    _scrollContainer.resize( Gfx::SizeF(500, 1000) );

    _scrollView.setWidget(_scrollContainer);
    
    _child2.setMainWidget(&_scrollView);
    _child2.show(true);

    add( _child1 );
    
    _child1.move( Gfx::PointF(30,30));
    _child1.resize( Gfx::SizeF(300, 400) );
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


void MainWindow::onPaintBackground(const Gfx::RectF& rect)
{
    Window::onPaintBackground(rect);

    Painter painter( surface() );

    Gfx::Image image( painter.format(), Gfx::Size(200, 30) );
    Gfx::ImagePainter imagePainter(image);

    imagePainter.setBrush( Gfx::Brush( Gfx::Color::fromRgb8(200,200,200) ) );
    imagePainter.fillRect(Gfx::RectF(Gfx::PointF(0,0), Gfx::SizeF(200, 30) ) );
    
    Gfx::FontMetrics fm = imagePainter.fontMetrics("Hello");

    imagePainter.setPen( Gfx::Pen( Gfx::Color::fromRgb8(0,0,0) ) );
    imagePainter.drawText( Gfx::PointF(30, 20), "Hello World!");

    painter.drawImage(Gfx::PointF(0, 0), image);
    painter.drawPicture(Gfx::PointF(0,0), _picture);
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
