#include "ChildW.h"
#include <Pt/Hmi/Application.h>
#include <Pt/Hmi/Docking.h>
#include "Dialog1.h"

namespace Pt{
namespace Hmi{
namespace Demo{


static int a = 0;

ChildW::ChildW()
{
    setPosition( Gfx::PointF(10,10) );
    setSize( Gfx::SizeF(800, 600) );       

    //Panel  
    _mainPanel.setSize( Gfx::SizeF(800, 600) );
    _mainPanel.setPosition( Gfx::PointF(20,20) );
    _mainPanel.setCaption( "MainPanel" );
    _mainPanel.setBorderStyle( Panel::Single );      
    _mainPanel.setBackgroundColor( Gfx::Color(1,0,0 ) ); 
    _mainPanel.layout().setType(Layout::LeftToRight);
    _mainPanel.layout().padding().setAll(20);
    _mainPanel.update();   

    //Text    
    _textLabel.setAutoSize(true);
    _textLabel.setSize( Gfx::SizeF(100,50) );
    _textLabel.setCaption( std::string("T&his is a Platinum C++") );  
    _textLabel.setPosition( Gfx::PointF(60,60) );
    _textLabel.setForegroundColor( Gfx::Color(1,0,0,0) );    
    _textLabel.setBackgroundColor( Gfx::Color(1,1,1,0) );
    _textLabel.setMnemonicWidget(&_toggleButton);
    _textLabel.setCursor( Hmi::Cursor::waitCursor() );
    _textLabel.update();
    _textLabel.docking().setType( Docking::Right);    
    _mainPanel.add(_textLabel);
    
    //Toggle button
    _toggleButton.setCaption( std::string("Toggle Me [CTRL+I]") );
    _toggleButton.setShortcut( &Pt::Hmi::Key(Pt::Hmi::Key::Control, Pt::Hmi::Key::I) );
    _toggleButton.setPosition( Gfx::PointF(20,60) );
    _toggleButton.setSize( Gfx::SizeF(150,25) );    
    _toggleButton.docking().setType(Docking::Top); 
    _toggleButton.update(); 
    _mainPanel.add(_toggleButton);

    //Dialog button     
    _dialogButton.setCaption(std::string("&&Dia&log [CTRL+D]&") );
    _dialogButton.setShortcut( &Pt::Hmi::Key(Pt::Hmi::Key::Control, Pt::Hmi::Key::D) );
    _dialogButton.setPosition( Gfx::PointF(20,100));
    _dialogButton.setSize( Gfx::SizeF(150,25) );    
    _dialogButton.docking().setType( Docking::Top);
    _dialogButton.margin().setTop(10);
    _dialogButton.margin().setBottom(3);
    _dialogButton.margin().setLeft(20);
    _dialogButton.margin().setRight(50);
    _dialogButton.clicked() += Pt::slot(*this, &ChildW::onShowDialog);
    _dialogButton.update(); 
    _mainPanel.add(_dialogButton);
    
    //Close button
    _closeButton.setCaption(std::string("Close App [CTRL+X]"));
    _closeButton.setShortcut(&Pt::Hmi::Key(Pt::Hmi::Key::Control, Pt::Hmi::Key::X));
    _closeButton.setPosition(Gfx::PointF(20,200));
    _closeButton.setSize(Gfx::SizeF(200, 40));
    _closeButton.docking().setType( Docking::Bottom);
    _closeButton.update(); 
    _mainPanel.add(_closeButton);

    //_childWindow2.setMainWidget(&_closeButton);          
    _childWindow2.setPosition(Gfx::PointF(10,10));    
    _childWindow2.setSize (Gfx::SizeF(750,500));
    if( a == 0)
    {
        _childWindow2.setTitle ("Child A");    
        ++a;
    }
    else
    {
        _childWindow2.setTitle ("Child B");    
    }
   
    _childWindow2.setMainWidget(&_mainPanel);
    
    add( _childWindow2 );
    _childWindow2.setVisible(true); // Child A/B
}


ChildW::~ChildW()
{

}


void ChildW::onShowDialog(Button& button)
{
    Dialog1 d;
    d.runModal();
}

void ChildW::onCloseApp()
{
    Hmi::Application::instance().exit();
}

}}}
