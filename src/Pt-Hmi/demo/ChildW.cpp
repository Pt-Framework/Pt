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
	setSize( Gfx::SizeF(520,350) );	   

	//Panel  
	_mainPanel.setSize( Gfx::SizeF(800,600) );
	_mainPanel.setPosition( Gfx::PointF(20,20) );
	_mainPanel.setCaption( "MainPanel" );
	_mainPanel.setPanelBorderStyle( Panel::Single );      
	_mainPanel.setBackgroundColor(  Gfx::Color(1,0,0 ) );	

	//Text	
	_textLabel.setAutoSize( false );
	_textLabel.setSize( Gfx::SizeF(50,40) );
	_textLabel.setCaption(  std::string("T&his is a Platinum C++") );  
	_textLabel.setPosition( Gfx::PointF(20,20) );
	_textLabel.setForegroundColor( Gfx::Color(1,0,0,0) );	
	_textLabel.setBackgroundColor( Gfx::Color(1,1,1,0) );
    _textLabel.setMnemonicWidget(&_toggleButton);
    _textLabel.setCursor(  Hmi::Cursor::waitCursor() );
    //_textLabel.docking().setType( Docking::Fill);	
	_mainPanel.add(_textLabel);
	
	//Toggle button
	_toggleButton.setCaption( std::string("Toggle Me [CTRL+I]") );
	_toggleButton.setShortcut( &Pt::Hmi::Key(Pt::Hmi::Key::Control, Pt::Hmi::Key::I) );
	_toggleButton.setPosition( Gfx::PointF(20,60) );
	_toggleButton.setSize( Gfx::SizeF(150,25) );	
    _toggleButton.docking().setType( Docking::Top);	
	_mainPanel.add(_toggleButton);

	//Dialog button     
	_dialogButton.setCaption(std::string("&&Dia&log [CTRL+D]&") );
	_dialogButton.setShortcut( &Pt::Hmi::Key(Pt::Hmi::Key::Control, Pt::Hmi::Key::D) );
	_dialogButton.setPosition( Gfx::PointF(20,100));
	_dialogButton.setSize( Gfx::SizeF(150,25) );	
    _dialogButton.docking().setType( Docking::Top);
	_dialogButton.clicked() += Pt::slot(*this, &ChildW::onShowDialog);

	_mainPanel.add(_dialogButton);
	
	//Close button
	_closeButton.setCaption(std::string("Close App [CTRL+X]"));
	_closeButton.setShortcut(&Pt::Hmi::Key(Pt::Hmi::Key::Control, Pt::Hmi::Key::X));
	_closeButton.setPosition(Gfx::PointF(20,200));
	_closeButton.setSize(Gfx::SizeF(20, 40));
	_closeButton.docking().setType( Docking::Bottom);
	
    _mainPanel.add(_closeButton);

	//_childWindow2.setMainWidget(&_closeButton);	  	
	_childWindow2.setPosition(Gfx::PointF(10,10));	
	_childWindow2.setSize (Gfx::SizeF(420,300));
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
