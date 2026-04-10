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
#include <Pt/Forms/Application.h>
#include <Pt/Gfx/Painter.h>
#include <Pt/Gfx/PaintContext.h>
#include <Pt/Gfx/PngReader.h>
#include <Pt/Gfx/JpegReader.h>
#include <Pt/Gfx/Image.h>
#include <Pt/Gfx/Bitmap.h>
//#include <Pt/Gfx/ImagePainter2.h>
#include <Pt/Gfx/Transform.h>
#include <sstream>
#include <fstream>

//#define DEMO_WITH_SKIA 1

#ifdef DEMO_WITH_SKIA
#include <SkSurface.h>
#include <SkImage.h>
#include <SkCanvas.h>
#endif

namespace Pt {

namespace Forms {

namespace Demo {

MainWindow::MainWindow()
: _child1("Child 1")
, _scrollContainer(Forms::FlowLayout::Top)
, _scrollContainer2(Forms::FlowLayout::Top)
{
    setContent(&_workspace);
    
    Pt::Gfx::Image windowIcon;

    std::stringstream ss;
    ss.write((const char*)atesionIcon, atesionIconSize);

    Pt::Gfx::PngReader reader(ss, windowIcon);
    
    setTitle("Main 1");
    move( Gfx::PointF(80, 80) );
    resize( Gfx::SizeF(900, 750) );

    windowIcon = reader.get();    
    setIcon(windowIcon);

    //_child2.setTopMost(true);
    _child2.resize( Gfx::SizeF(550, 600) );
    //_workspace.addWindow( _child2 );

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

        _scrollContainer.addItem(_btns[n] );
    }

    for(int n = 0; n < 20; ++n)
    {
        SizePolicy policy(SizePolicy::Fixed, SizePolicy::Fixed);
        policy.setWidth(100);
        policy.setHeight(40);
        _btns2[n].setSizePolicy(policy);

        _btns2[n].setPadding(5);
        _btns2[n].setMargin(5);

        std::stringstream ss;
        ss << "Scroll2 "<< (n+1);

        _btns2[n].setText(ss.str().c_str());

        _scrollContainer2.addItem(_btns2[n] );
    }

    _scrollView2.setScrollBars(true);
    Pt::Forms::SizePolicy spp(  SizePolicy::Fixed, SizePolicy::Fixed );

     spp.setSize(200,400);
    _scrollView2.setSizePolicy(spp);

    _scrollContainer.addItem(_scrollView2);

    _bt2.setText("Ende");
    _bt2.setName("endbut");
    _bt2.setMinimumHeight(50);
    _bt2.setPadding(5);
    _bt2.setMargin(5);
    _bt2.clicked() += Pt::slot(Application::instance(), &Application::exit);

    _scrollContainer.addItem(_bt2);
    _scrollContainer.setPadding(5);
    _scrollContainer.setMargin(5);
    _scrollContainer2.setPadding(5);
    _scrollContainer2.setMargin(5);

    SizePolicy policy(SizePolicy::Fixed, SizePolicy::Fixed);
    policy.setSize( Gfx::SizeF(500, 1100) );

    _scrollContainer.setSizePolicy(policy);

    SizePolicy policy2(SizePolicy::Fixed, SizePolicy::Fixed);
    policy2.setSize( Gfx::SizeF(500, 600) );

    _scrollContainer2.setSizePolicy(policy2);

    _scrollView2.setContent(_scrollContainer2);
    _scrollView.setContent(_scrollContainer);

    _child2.setContent(&_scrollView);
    //_child2.show(true);  // SHOW DEMO WINDOW 2

    _workspace.addWindow( _child1 );

    _tabLabel1.setText("Tab Label 1");
    _tabLabel1.setAlignment(Alignment::Center);
    _tabLabel1.setBackground( Gfx::ColorF::fromRgb8(200, 120, 100) );
    _tabLabel1.setContour( Gfx::ColorF::fromRgb8(120, 50, 50) );
    _tabLabel1.setMargin(5);

    _tabLabel2.setText("Tab Label 2");
    _tabLabel2.setAlignment(Alignment::Center);
    _tabLabel2.setBackground( Gfx::ColorF::fromRgb8(100, 200, 120) );

    _tabLabel3.setText("Tab Label 3");
    _tabLabel3.setAlignment(Alignment::Center);
    _tabLabel3.setBackground( Gfx::ColorF::fromRgb8(100, 120, 200) );

    _tabView.addTab(_tabLabel1, "Tab 1");
    _tabView.addTab(_tabLabel2, "Tab 2");
    _tabView.addTab(_tabLabel3, "Tab 3");

    _tabView.setCurrent(0);

    _child1.layout().addItem(_tabView, DockingLayout::Fill);
    _child1.closeRequested() += Pt::slot(*this, &MainWindow::close);

    //_child1.setTopMost(true);
    _child1.move( Gfx::PointF(500, 30));
    _child1.resize( Gfx::SizeF(280, 600) );
    _child1.show(true); // SHOW DEMO WINDOW 1

    // context menu
    _menu.setName("All Music");

    _item1.setText("Heavy Metal");
    
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

    _subMenuItem.setText("Other Music");
    _subMenuItem.setMenu(&_subMenu);
    _menu.addItem(_subMenuItem);

    // zoom sub menu
    _zoomMenu.setName("Zoom");

    _zoomItem1.setText("100%");
    _zoomItem1.triggered() += Pt::slot(*this, &MainWindow::onZoom);
    _zoomMenu.addItem(_zoomItem1);

    _zoomItem2.setText("125%");
    _zoomItem2.triggered() += Pt::slot(*this, &MainWindow::onZoom);
    _zoomMenu.addItem(_zoomItem2);

    _zoomItem3.setText("133%");
    _zoomItem3.triggered() += Pt::slot(*this, &MainWindow::onZoom);
    _zoomMenu.addItem(_zoomItem3);

    _zoomSubMenuItem.setText("Zoom");
    _zoomSubMenuItem.setMenu(&_zoomMenu);
    _menu.addItem(_zoomSubMenuItem);
}


MainWindow::~MainWindow()
{
}

void MainWindow::onZoom(MenuBaseItem& item)
{
    if(item.text() == "100%")
        Application::instance().setScaleFactor(1.0);
    else if(item.text() == "125%")
        Application::instance().setScaleFactor(1.25);
    else if(item.text() == "133%")
        Application::instance().setScaleFactor(1.33);
}


void MainWindow::onPaintEvent(const PaintEvent& ev)
{
    //{
    //Gfx::Painter p;
    //p.setPen( Pt::Gfx::ColorF::fromRgb8(0,0,0));
    //PixmapSurface pm;
    //PixmapSurface pm2;
    //p.begin(pm);
    //p.begin(pm2);

    //Gfx::Bitmap image( Gfx::SizeF(320, 240) );
    //p.begin(image);
    //}
    
    Window::onPaintEvent(ev);
    //return;
    
    const Gfx::RectF& rect = ev.rect();
 
    Gfx::Painter painter;

    Gfx::PaintContext context( surface() );
    
    painter.begin(context);
    painter.setClip(rect);

    Pt::String text = "MgWjOy";

    painter.setFont( Pt::Gfx::Font("", 10) );
    painter.textMetrics(text);
    painter.drawText( Pt::Gfx::PointF(10, 15), text);

    Gfx::TextMetrics fm = painter.textMetrics(text);

    painter.setPen( Gfx::ColorF::fromRgb8(255, 0, 0, 150) );
    painter.drawLine(Pt::Gfx::PointF(10, 15), Pt::Gfx::PointF(10 + fm.width(), 15));

    //
    // Path
    //
    painter.setPen( Pt::Gfx::Pen(Gfx::ColorF(10000, 0, 10000), 2, Gfx::Pen::Dash) );
    painter.setBrush(Gfx::ColorF(65535, 65535, 0, 0));

    Pt::Gfx::Path pathW;
    pathW.moveTo( Pt::Gfx::PointF(10, size().height() - 210) );
    pathW.addRoundedRect(Pt::Gfx::SizeF(100, 100), 10);

    pathW.moveTo( Pt::Gfx::PointF(40, size().height() - 180) );
    pathW.addRoundedRect(Pt::Gfx::SizeF(100, 100), 10);

    painter.setPath(pathW);
    painter.fillPath();
    painter.drawPath();

    //painter.fillPath(pathW);
    //painter.drawPath(pathW);

    Pt::Gfx::PointF star2[6];
    star2[0] = Pt::Gfx::PointF(100+320, 20);
    star2[1] = Pt::Gfx::PointF(100+500, 200);
    star2[2] = Pt::Gfx::PointF(100+400, 20);
    star2[3] = Pt::Gfx::PointF(100+320, 200);
    star2[4] = Pt::Gfx::PointF(100+500, 20);
    star2[5] = Pt::Gfx::PointF(100+320, 20);
    //painter.fillPolygon(star2, 6);
    //painter.drawPolyline(star2, 6);
    //return;

#ifdef DEMO_WITH_SKIA
    Gfx::Image img(Gfx::ImageFormat::argb32(), Gfx::Size(800, 400));

    SkImageInfo info = SkImageInfo::Make(img.width(), img.height(),
                                         kBGRA_8888_SkColorType, kPremul_SkAlphaType, 0);
    size_t rowBytes = info.minRowBytes();

    sk_sp<SkSurface> surface = SkSurface::MakeRasterDirect( info, img.data(), rowBytes);
    SkCanvas* canvas = surface->getCanvas();
    
    canvas->clear(SK_ColorWHITE);

    SkPaint paint;
    paint.setColor(SK_ColorRED);
    paint.setAntiAlias(true);
    paint.setStrokeWidth(10);
    SkPoint p1;
    p1.fX = 20;
    p1.fY = 20;

    SkPoint p2;
    p2.fX = 170;
    p2.fY = 320;

    Pt::System::Clock clock;

    // Skia
    clock.start();

    size_t count = 10000;

    for(size_t i = 0; i < count; ++i)
        canvas->drawLine(p1, p2, paint);

    Pt::Timespan span= clock.stop();
    std::cout << "Skia: " << span.toUSecs() / 1000.0 << " ms" << std::endl;

    // Image painter 2
    Pt::Gfx::ImagePainter2  painter2(img);
    
    painter2.setPen(Gfx::Pen(Gfx::ColorF::fromRgb8(0, 255, 00), 10));

    Gfx::PointF p3(40, 20);
    Gfx::PointF p4(190, 320);

    clock.start();

    for (size_t i = 0; i < count; ++i)
        painter2.drawLine(p3, p4);

    span = clock.stop();
    std::cout << "IP2: " << span.toUSecs() / 1000.0 << " ms" << std::endl;

        painter.drawImage(Gfx::PointF(0, 0), img);

    // Native
    Gfx::PointF p5(60, 20);
    Gfx::PointF p6(210, 320);

    painter.setPen(Gfx::Pen(Gfx::ColorF::fromRgb8(0, 0, 255), 10));

    clock.start();

    for (size_t i = 0; i < count; ++i)
        painter.drawLine(p5, p6);

    span = clock.stop();
    std::cout << "Native: " << span.toUSecs() / 1000.0 << " ms" << std::endl;
    std::cout << "----------------------------" << std::endl;

    return;
#endif

    Gfx::Painter imagePainter;
    Gfx::Bitmap imageSurface( Gfx::SizeF(600, 600) );
    imagePainter.begin(imageSurface);

    Gfx::Painter imagePainter2(imageSurface);
    imagePainter2.begin(imageSurface);

    imagePainter2.setBrush(Gfx::ColorF(65535, 0, 0, 0));
    imagePainter2.fillRect( Gfx::RectF(Gfx::SizeF(600, 600)) );
    //
    //imagePainter.setAntiAliasing(true);
    //imagePainter.setAntiAliasing(false);

//#define TEST_POLYGON_RASTERIZER

#ifdef TEST_POLYGON_RASTERIZER

    // this line begins at the same position as the next one, but is just longer
    imagePainter.setPen(Pt::Gfx::Pen(Gfx::ColorF(65535, 65535, 65535), 2,
                                     Gfx::Pen::Solid, Gfx::Pen::FlatCap));
    imagePainter.drawLine(Gfx::PointF(1, 1), Gfx::PointF(9, 9));

    // this line begins at the same position as the previous one, but is just shorter
    imagePainter.setPen(Pt::Gfx::Pen(Gfx::ColorF(32767, 65535, 0, 0), 2,
                                     Gfx::Pen::Solid, Gfx::Pen::FlatCap));
    imagePainter.setCompositionMode(Gfx::CompositionMode::SourceOver);
    imagePainter.drawLine(Gfx::PointF(1, 1), Gfx::PointF(6, 6));
    imagePainter.setCompositionMode(Gfx::CompositionMode::SourceCopy);

/*
    //
    // a polygon like a short flat capped line of width 2 from 520,520 to 578,578
    //
    Pt::Gfx::PointF linePolygon[5];
    linePolygon[0] = Pt::Gfx::PointF(520.35357666015625, 569.64642333984375);
    linePolygon[1] = Pt::Gfx::PointF(519.64642333984375, 570.35357666015625);
    linePolygon[2] = Pt::Gfx::PointF(527.64642333984375, 578.35357666015625);
    linePolygon[3] = Pt::Gfx::PointF(528.35357666015625, 577.64642333984375);
    linePolygon[4] = Pt::Gfx::PointF(520.35357666015625, 569.64642333984375);

    imagePainter.setBrush(Gfx::ColorF(65535, 65535, 65535));
    imagePainter.fillPolygon(linePolygon, 5);

    // NOTE: like X from window-close button/
    imagePainter.setPen(Pt::Gfx::Pen(Gfx::ColorF(65535, 65535, 65535), 2, Gfx::Pen::Solid, Gfx::Pen::FlatCap));
    imagePainter.drawLine(Gfx::PointF(520, 520), Gfx::PointF(528, 528));
    imagePainter.drawLine(Gfx::PointF(528, 520), Gfx::PointF(520, 528));

    imagePainter.setPen(Pt::Gfx::Pen(Gfx::ColorF(65535, 65535, 65535), 2, Gfx::Pen::Solid, Gfx::Pen::SquareCap));
    imagePainter.drawLine(Gfx::PointF(520 + 20, 520), Gfx::PointF(528 + 20, 528));
    imagePainter.drawLine(Gfx::PointF(528 + 20, 520), Gfx::PointF(520 + 20, 528));

    imagePainter.setPen(Pt::Gfx::Pen(Gfx::ColorF(65535, 65535, 65535), 2, Gfx::Pen::Solid, Gfx::Pen::RoundCap));
    imagePainter.drawLine(Gfx::PointF(520 + 40, 520), Gfx::PointF(528 + 40, 528));
    imagePainter.drawLine(Gfx::PointF(528 + 40, 520), Gfx::PointF(520 + 40, 528));
//*/

#else
    //Gfx::ImagePainter2::setDefaultFont("DejaVu Sans");

    //
    // lines of various sizes
    //

    Gfx::Pen pen1( Gfx::ColorF::fromRgb8(255, 200, 100), 1);
    imagePainter.setPen(pen1);

    imagePainter.begin(imageSurface);

    imagePainter.drawLine( Pt::Gfx::PointF(300, 300),
                           Pt::Gfx::PointF(450, 300) );
    imagePainter.drawLine( Pt::Gfx::PointF(500, 300),
                           Pt::Gfx::PointF(500, 450) );
    imagePainter.drawLine( Pt::Gfx::PointF(300, 450),
                           Pt::Gfx::PointF(400, 550) );

    Gfx::Pen penb( Gfx::ColorF::fromRgb8(255, 255, 255), 1);
    imagePainter.setPen(penb);
    imagePainter.drawLine( Pt::Gfx::PointF(250, 450),
                           Pt::Gfx::PointF(380, 550) );

    Gfx::Pen pen2( Gfx::ColorF::fromRgb8(255, 200, 100), 2);
    imagePainter.setPen(pen2);
    imagePainter.drawLine( Pt::Gfx::PointF(300, 320),
                           Pt::Gfx::PointF(450, 320) );
    imagePainter.drawLine( Pt::Gfx::PointF(520, 300),
                           Pt::Gfx::PointF(520, 450) );
    imagePainter.drawLine( Pt::Gfx::PointF(320, 450),
                           Pt::Gfx::PointF(420, 550) );

    Gfx::Pen pen3( Gfx::ColorF::fromRgb8(255, 200, 100), 3);
    imagePainter.setPen(pen3);
    imagePainter.drawLine( Pt::Gfx::PointF(300, 340),
                           Pt::Gfx::PointF(450, 340) );
    imagePainter.drawLine( Pt::Gfx::PointF(540, 300),
                           Pt::Gfx::PointF(540, 450) );
    imagePainter.drawLine( Pt::Gfx::PointF(340, 450),
                           Pt::Gfx::PointF(440, 550) );

    Gfx::Pen pen4( Gfx::ColorF::fromRgb8(255, 200, 100), 4);
    imagePainter.setPen(pen4);
    imagePainter.drawLine( Pt::Gfx::PointF(300, 360),
                           Pt::Gfx::PointF(450, 360) );
    imagePainter.drawLine( Pt::Gfx::PointF(560, 300),
                           Pt::Gfx::PointF(560, 450) );
    imagePainter.drawLine( Pt::Gfx::PointF(360, 450),
                           Pt::Gfx::PointF(460, 550) );

    Gfx::Pen pen5( Gfx::ColorF::fromRgb8(255, 200, 100), 5);
    imagePainter.setPen(pen5);
    imagePainter.drawLine( Pt::Gfx::PointF(300, 380),
                           Pt::Gfx::PointF(450, 380) );
    imagePainter.drawLine( Pt::Gfx::PointF(580, 300),
                           Pt::Gfx::PointF(580, 450) );
    imagePainter.drawLine( Pt::Gfx::PointF(380, 450),
                           Pt::Gfx::PointF(480, 550) );

    // NOTE: RoundCap tests
    imagePainter.setPen(Gfx::Pen( Gfx::ColorF::fromRgb8(255, 200, 100), 1, Gfx::Pen::Solid, Gfx::Pen::RoundCap));
    imagePainter.drawLine( Pt::Gfx::PointF(300, 300 - 200), Pt::Gfx::PointF(450, 300 - 200) );
    imagePainter.drawLine( Pt::Gfx::PointF(300 - 200, 450), Pt::Gfx::PointF(400 - 200, 550) );
    imagePainter.setPen(Gfx::Pen( Gfx::ColorF::fromRgb8(255, 200, 100), 2, Gfx::Pen::Solid, Gfx::Pen::RoundCap));
    imagePainter.drawLine( Pt::Gfx::PointF(300, 320 - 200), Pt::Gfx::PointF(450, 320 - 200) );
    imagePainter.drawLine( Pt::Gfx::PointF(320 - 200, 450), Pt::Gfx::PointF(420 - 200, 550) );
    imagePainter.setPen(Gfx::Pen( Gfx::ColorF::fromRgb8(255, 200, 100), 3, Gfx::Pen::Solid, Gfx::Pen::RoundCap));
    imagePainter.drawLine( Pt::Gfx::PointF(300, 340 - 200), Pt::Gfx::PointF(450, 340 - 200) );
    imagePainter.drawLine( Pt::Gfx::PointF(340 - 200, 450), Pt::Gfx::PointF(440 - 200, 550) );
    imagePainter.setPen(Gfx::Pen( Gfx::ColorF::fromRgb8(255, 200, 100), 4, Gfx::Pen::Solid, Gfx::Pen::RoundCap));
    imagePainter.drawLine( Pt::Gfx::PointF(300, 360 - 200), Pt::Gfx::PointF(450, 360 - 200) );
    imagePainter.drawLine( Pt::Gfx::PointF(360 - 200, 450), Pt::Gfx::PointF(460 - 200, 550) );
    imagePainter.setPen(Gfx::Pen( Gfx::ColorF::fromRgb8(255, 200, 100), 5, Gfx::Pen::Solid, Gfx::Pen::RoundCap));
    imagePainter.drawLine( Pt::Gfx::PointF(300, 380 - 200), Pt::Gfx::PointF(450, 380 - 200) );
    imagePainter.drawLine( Pt::Gfx::PointF(380 - 200, 450), Pt::Gfx::PointF(480 - 200, 550) );

    imagePainter.setCompositionMode(Gfx::CompositionMode::SourceOver);
    imagePainter.setPen(Gfx::Pen( Gfx::ColorF(32767, 65535, 65535, 65535), 5, Gfx::Pen::Solid, Gfx::Pen::RoundCap));
    imagePainter.drawLine( Pt::Gfx::PointF(300, 380 - 200 + 20), Pt::Gfx::PointF(450, 380 - 200 + 20) );
    imagePainter.drawLine( Pt::Gfx::PointF(380 - 200 + 20, 450), Pt::Gfx::PointF(480 - 200 + 20, 550) );
    imagePainter.setCompositionMode(Gfx::CompositionMode::SourceCopy);

    // NOTE: Like X from window-close button
    imagePainter.setPen(Pt::Gfx::Pen(Gfx::ColorF(65535, 65535, 65535), 2, Gfx::Pen::Solid, Gfx::Pen::FlatCap));
    imagePainter.drawLine(Gfx::PointF(520, 520), Gfx::PointF(528, 528));
    imagePainter.drawLine(Gfx::PointF(528, 520), Gfx::PointF(520, 528));

    imagePainter.setPen(Pt::Gfx::Pen(Gfx::ColorF(65535, 65535, 65535), 2, Gfx::Pen::Solid, Gfx::Pen::SquareCap));
    imagePainter.drawLine(Gfx::PointF(520 + 20, 520), Gfx::PointF(528 + 20, 528));
    imagePainter.drawLine(Gfx::PointF(528 + 20, 520), Gfx::PointF(520 + 20, 528));

    imagePainter.setPen(Pt::Gfx::Pen(Gfx::ColorF(65535, 65535, 65535), 2, Gfx::Pen::Solid, Gfx::Pen::RoundCap));
    imagePainter.drawLine(Gfx::PointF(520 + 40, 520), Gfx::PointF(528 + 40, 528));
    imagePainter.drawLine(Gfx::PointF(528 + 40, 520), Gfx::PointF(520 + 40, 528));

    //
    // Gradient filled path
    //
    Gfx::ColorStops stops;
    stops.add(0, Gfx::ColorF::fromRgb8(255, 0, 0));
    stops.add(1.0, Gfx::ColorF::fromRgb8(0, 255, 0));

    //Gfx::Brush brush = Gfx::Brush::horizontalGradient(stops);
    Gfx::Brush brush = Gfx::Brush::verticalGradient(stops);
    //Gfx::Brush brush = Gfx::Brush::linearGradient(0.0, 0.5, 1.0, 0.5, stops);
    //Gfx::Brush brush = Gfx::Brush::radialGradient(0.25, 0.25, 0, 0.5f, 0.5f, 0.5, stops);
    //imagePainter.setBrush(brush);
    imagePainter.setBrush(Gfx::ColorF(65535, 0, 0));

    Pt::Gfx::Path path;
    path.moveTo( Pt::Gfx::PointF(20, 20) );
    path.addRoundedRect(Pt::Gfx::SizeF(100, 100), 10);
    
    path.moveTo( Pt::Gfx::PointF(50, 50) );
    path.addRoundedRect(Pt::Gfx::SizeF(100, 100), 10);

    //imagePainter.fillPath(path);
    //imagePainter.drawPath(path);

    imagePainter.setPath(path);
    imagePainter.fillPath();
    imagePainter.drawPath();

    Pt::Gfx::PointF star[6];
    star[0] = Pt::Gfx::PointF(20, 20);
    star[1] = Pt::Gfx::PointF(200, 200);
    star[2] = Pt::Gfx::PointF(100, 20);
    star[3] = Pt::Gfx::PointF(20, 200);
    star[4] = Pt::Gfx::PointF(200, 20);
    star[5] = Pt::Gfx::PointF(20, 20);
    //imagePainter.fillPolygon(star, 6);
    //imagePainter.drawPolyline(star, 6);

    imagePainter.setFont( Pt::Gfx::Font("", 24) );
    Pt::String str = "Hello World";

    Pt::Gfx::Transform trans;
    trans.rotateDeg(-45);
    imagePainter.drawText(Pt::Gfx::PointF(20, 260), str, trans);

    Pt::Gfx::PointF triangle[4];
    triangle[0] = Pt::Gfx::PointF(500, 500);
    triangle[1] = Pt::Gfx::PointF(510, 500);
    triangle[2] = Pt::Gfx::PointF(505, 505);
    triangle[3] = Pt::Gfx::PointF(500, 500);

    //
    // Ellipse, circle, pie and chord
    //
    //imagePainter.setCompositionMode(Gfx::CompositionMode::SourceOver);
    //imagePainter.setAntiAliasing(false);

    imagePainter.setBrush(Gfx::ColorF(32767, 65535, 0, 0));
    imagePainter.fillPolygon(triangle, 4);

    imagePainter.setBrush(Gfx::ColorF(32767, 65535, 65535, 65535));
    imagePainter.fillRect( Pt::Gfx::RectF( Pt::Gfx::PointF(220, 250), Pt::Gfx::SizeF(25.0, 6.0) ) );
    imagePainter.fillCircle( Pt::Gfx::PointF(250, 250), 6.0);

    imagePainter.setBrush(Gfx::ColorF(32767, 65535, 0, 0));
    imagePainter.fillEllipse(Pt::Gfx::PointF(200, 300), Pt::Gfx::SizeF(50, 100));
    imagePainter.setBrush(Gfx::ColorF(32767, 65535, 65535, 0));
    imagePainter.fillPie(Pt::Gfx::PointF(200, 300), Pt::Gfx::SizeF(50, 100), 0, 90);
    imagePainter.setBrush(Gfx::ColorF(32767, 0, 65535, 0));
    imagePainter.fillChord(Pt::Gfx::PointF(200, 300), Pt::Gfx::SizeF(50, 100), 180, 270);

    imagePainter.setPen( Gfx::ColorF(50000, 65535, 40000, 20000) );
    imagePainter.drawEllipse(Pt::Gfx::PointF(200 + 300, 300 - 200), Pt::Gfx::SizeF(50, 100));

    //fprintf(stderr, "AAAAA\n");
    imagePainter.setPen(Pt::Gfx::Pen(Gfx::ColorF(32757, 65535, 65535, 65535), 1) );
    imagePainter.drawCircle( Pt::Gfx::PointF(260, 250), 6.0);
    imagePainter.drawCircle( Pt::Gfx::PointF(270, 250), 7.0);
    imagePainter.drawCircle( Pt::Gfx::PointF(280, 250), 8.0);
    imagePainter.drawCircle( Pt::Gfx::PointF(290, 250), 9.0);

#endif

    //painter.setCompositionMode(Pt::Gfx::CompositionMode::SourceOver);
    
    painter.drawImage( Gfx::PointF(10, 30), imageSurface.image() );
}


bool MainWindow::onMouseEvent(const MouseEvent& ev)
{
    Window::onMouseEvent(ev);

    if( ev.isRelease(MouseEvent::Right) )
    {
        Gfx::PointF menuPos = this->toGlobal( ev.position() );
        
        _menu.autoSize();
        _menu.move(menuPos);
        _menu.setAbove(true);
        _menu.show();
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