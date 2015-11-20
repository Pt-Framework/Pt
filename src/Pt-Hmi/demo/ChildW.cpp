#include "ChildW.h"
#include <Pt/Hmi/Application.h>

namespace Pt{
namespace Hmi{
namespace Demo{


ChildW::ChildW()
{
	setPosition( Gfx::PointF(10,10) );
	setSize( Gfx::SizeF(520,350) );
	setVisible( true) ;
  setCaption(  "Child 1" );

	//Panel  
	_mainPanel.setSize( Gfx::SizeF(800,600) );
	_mainPanel.setPosition( Gfx::PointF(20,20) );
	_mainPanel.setCaption( "MainPanel" );
	_mainPanel.setPanelBorderStyle( Pt::Hmi::BorderStyle::Single );  
	_mainPanel.setDock( Docking::Fill );	
	_mainPanel.setBackgroundColor(  Gfx::Color(1,0,0 ) );	

	//Text	
	_textLabel.setAutoSize( true );
	_textLabel.setMargin( Hmi::Margin(10) );
	_textLabel.setSize( Gfx::SizeF(50,40) );
	_textLabel.setCaption(  std::string("T&his is a Platinum C++") );  
	_textLabel.setPosition( Gfx::PointF(20,20) );
	_textLabel.setForegroundColor( Gfx::Color(1,0,0,0) );
	_textLabel.setUseMnemonic( true );	
	_textLabel.setBackgroundColor( Gfx::Color(1,1,1,0) );
  _textLabel.bindMnemonicToWidget(_toggleButton);
  _textLabel.setCursor(  Hmi::Cursor::waitCursor() );
	_mainPanel.addChild(&_textLabel);
	
	//Toggle button
	_toggleButton.setButtonType(  Hmi::ButtonType::Toggle );
	_toggleButton.setCaption( std::string("Toggle Me [CTRL+I]") );
	_toggleButton.setShortcutKey( std::string("C//i") );
	_toggleButton.setPosition( Gfx::PointF(20,60) );
	_toggleButton.setSize( Gfx::SizeF(150,25) );		
	_mainPanel.addChild(&_toggleButton);

	//Dialog button  
	_dialogButton.setButtonType( Hmi::ButtonType::Press );
	_dialogButton.setCaption(std::string("&&Dia&log [CTRL+D]&") );
	_dialogButton.setShortcutKey( std::string("C//d") );
	_dialogButton.setPosition( Gfx::PointF(20,100));
	_dialogButton.setSize( Gfx::SizeF(150,25) );	
	_dialogButton.setUseMnemonic( true );
	
	_mainPanel.addChild(&_dialogButton);
	
	//Close button
	_closeButton.setButtonType(Hmi::ButtonType::Press);
	_closeButton.setCaption(std::string("Close App [CTRL+X]"));
	_closeButton.setShortcutKey(std::string("C//x"));
	_closeButton.setPosition(Gfx::PointF(590,525));
	_closeButton.setSize(Gfx::SizeF(150,25));
	_closeButton.setSize(Gfx::SizeF(20, 40));
	_closeButton.setDock(Docking::Bottom);
	_closeButton.setMargin(Hmi::Margin(5));

	_childWindow2.addChild(&_closeButton);	  	
	_childWindow2.setPosition(Gfx::PointF(10,10));	
	_childWindow2.setSize (Gfx::SizeF(420,300));
  _childWindow2.setCaption ( "Child A");	
	_childWindow2.addChild(&_mainPanel);
	_childWindow2.setVisible(true);

	addChildWindow( _childWindow2 );	
}

ChildW::~ChildW()
{

}


void ChildW::onShowDialog()
{

}

void ChildW::onCloseApp()
{
	Hmi::Application::instance().exit();
}

}}}