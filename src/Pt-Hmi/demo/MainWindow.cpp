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
#include <Pt/Hmi/Painter.h>
#include <Pt/Gfx/PngReader.h>
#include <Pt/Gfx/JpegReader.h>
#include <Pt/Gfx/Image.h>
#include <Pt/Gfx/ImagePainter.h>
#include <Pt/Gfx/ImagePainter2.h>
#include <Pt/Gfx/Argb32Format.h>
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
    resize( Gfx::SizeF(700, 900) ); 
    
    //_child2.setTopMost(true);
    _child2.resize( Gfx::SizeF(550, 600) );
    add( _child2 );
    
    _child2.setTitle("Child 2");
    _child2.move( Gfx::PointF(10, 10) );
    
    for(int n = 0; n < 10; ++n)
    {
        SizePolicy policy(SizePolicy::Any, SizePolicy::Fixed);
        policy.setHeight(40);
        _btns[n].setSizePolicy(policy);

        _btns[n].setPadding(5);
        _btns[n].setMargin(5);

        _btns[n].setText("Hallo");
        _btns[n].clicked() += Pt::slot(*this, &MainWindow::onButton);
        
        _scrollContainer.addItem(_btns[n] );
    }
    
    _bt2.setText("Ende");
    _bt2.setMinimumHeight(50);
    _bt2.setPadding(5);
    _bt2.setMargin(5);
    _scrollContainer.addItem(_bt2);
    
    _scrollContainer.setPadding(5);
    _scrollContainer.setMargin(5);

  
    SizePolicy policy(SizePolicy::Fixed, SizePolicy::Fixed);
    policy.setSize( Gfx::SizeF(500, 1100) );
    _scrollContainer.setSizePolicy(policy);

    _scrollView.setContent(_scrollContainer);
    
    _child2.setContent(&_scrollView);
    _child2.show(true);

    add( _child1 );

    _tabLabel1.setText("Tab Label 1");
    _tabLabel1.setAlignment(Alignment::Center);
    _tabLabel1.setBackground( Gfx::Color::fromRgb8(200, 120, 100) );
    _tabLabel1.setContour( Gfx::Color::fromRgb8(120, 50, 50) );
    _tabLabel1.setMargin(5);
    
    _tabLabel2.setText("Tab Label 2");
    _tabLabel2.setAlignment(Alignment::Center);
    _tabLabel2.setBackground( Gfx::Color::fromRgb8(100, 200, 120) );
    
    _tabLabel3.setText("Tab Label 3");
    _tabLabel3.setAlignment(Alignment::Center);
    _tabLabel3.setBackground( Gfx::Color::fromRgb8(100, 120, 200) );

    _tabView.addTab(_tabLabel1, "Tab 1");
    _tabView.addTab(_tabLabel2, "Tab 2");
    _tabView.addTab(_tabLabel3, "Tab 3");

    _tabView.setCurrent(0);

    _child1.setContent(&_tabView);
    
    //_child1.setTopMost(true);
    _child1.move( Gfx::PointF(30,30));
    _child1.resize( Gfx::SizeF(300, 600) );
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

    Gfx::ImagePainter::setDefaultFont("DejaVu Sans");

    Gfx::Image image( painter.format(), Gfx::Size(800, 800) );
    Gfx::ImagePainter2 imagePainter(image);

    Pt::Gfx::Transform trans;
    trans.translate(-125, -150);     
    trans.rotateDeg(45);
    trans.translate(125, 150);
    

    Pt::Gfx::Path path;    
    path.moveTo( Pt::Gfx::PointF(100, 100) );
    //path.addEllipse( Pt::Gfx::SizeF(50, 100) );
    path.addRoundRect(Pt::Gfx::SizeF(50,50), 10);
    //path.transform(trans);

    imagePainter.setPen(Gfx::Color::fromRgb8(255, 0, 0)  );
    imagePainter.setBrush( Gfx::Color::fromRgb8(255, 0, 0)  );
    imagePainter.fillPath(path);

    imagePainter.fillRoundedRect( Pt::Gfx::RectF(Pt::Gfx::PointF(150, 150), Pt::Gfx::SizeF(50,50)), 10);
      
    //imagePainter.setPen( Gfx::Color::fromRgb8(150, 150, 255) );
    //imagePainter.drawEllipse(Pt::Gfx::PointF(100, 100), Pt::Gfx::SizeF(50, 100));

//    imagePainter.setFont( Pt::Gfx::Font("", 24) );
//    imagePainter.drawText(Pt::Gfx::PointF(115, 120), "Hello");

    //painter.drawImage(Gfx::PointF(0, 0), image);
}


void MainWindow::onButton()
{
    _btns[2].setMargin(10);
}


bool MainWindow::onMouseEvent(const MouseEvent& ev)
{
    Window::onMouseEvent(ev);

    if( ev.isRelease(MouseEvent::Right) )
    {
        Gfx::PointF menuPos = this->toScreen( ev.position() );

        _menu.show(menuPos);
    }

    return true;
}


void MainWindow::onCloseEvent(const CloseEvent& ev)
{
   Window::onCloseEvent(ev);
   Application::instance().exit();
}

} // namespace

} // namespace

} // namespace
