#include "ChildW.h"
#include <Pt/Hmi/Application.h>

namespace Pt{
namespace Hmi{
namespace Demo{


ChildW::ChildW()
{
	Position = Gfx::PointF(10,10);
	Size =Gfx::SizeF(520,350);
	Visible = true;
  Caption = "Child 1";
  Icon = Icon;

	//Panel  
	_mainPanel.Size =Gfx::SizeF(800,600);
	_mainPanel.Position =Gfx::PointF(20,20);
	_mainPanel.Caption = "MainPanel";
	_mainPanel.PanelBorderStyle = Pt::Hmi::BorderStyle::Single;  
	_mainPanel.Dock = Docking::Fill;	
	_mainPanel.BackColor = Gfx::Color(1,0,0 );
	_mainPanel.BackgroundImageLayout = Pt::Hmi::ImageLayout::Strech;    	

	//Text	
	_textLabel.AutoSize = true;
	_textLabel.Margin = Hmi::Margin(10);
	_textLabel.Size =Gfx::SizeF(50,40);
	_textLabel.Caption = std::string("T&his is a Platinum C++");  
	_textLabel.Position =Gfx::PointF(20,20);
	_textLabel.ForeColor =Gfx::Color(1,0,0,0);
	_textLabel.UseMnemonic = true;	
	_textLabel.BackColor =Gfx::Color(1,1,1,0);
  _textLabel.bindMnemonicToWidget(_toggleButton);
  _textLabel.Cursor = Hmi::Cursor::waitCursor();
	_mainPanel.addChild(&_textLabel);
	
	//Toggle button
	_toggleButton.ButtonType = Hmi::ButtonType::Toggle;
	_toggleButton.Caption = std::string("Toggle Me [CTRL+I]");
	_toggleButton.ShortcutKey = std::string("C//i");
	_toggleButton.Position =Gfx::PointF(20,60);
	_toggleButton.Size =Gfx::SizeF(150,25);		
	_mainPanel.addChild(&_toggleButton);

	//Dialog button  
	_dialogButton.ButtonType = Hmi::ButtonType::Press;
	_dialogButton.Caption = std::string("&&Dia&log [CTRL+D]&");
	_dialogButton.ShortcutKey = std::string("C//d");
	_dialogButton.Position =Gfx::PointF(20,100);
	_dialogButton.Size =Gfx::SizeF(150,25);	
	_dialogButton.UseMnemonic = true;
	_dialogButton.Clicked  += Pt::slot(*this, &ChildW::onShowDialog);
	
	_mainPanel.addChild(&_dialogButton);
	
	//Close button
	_closeButton.ButtonType = Hmi::ButtonType::Press;
	_closeButton.Caption = std::string("Close App [CTRL+X]");
	_closeButton.ShortcutKey = std::string("C//x");
	_closeButton.Position =Gfx::PointF(590,525);
	_closeButton.Size =Gfx::SizeF(150,25);
	_closeButton.Clicked += Pt::slot(*this, &ChildW::onCloseApp);
	_closeButton.Size =Gfx::SizeF(20, 40);
	_closeButton.Dock = Docking::Bottom;
	_closeButton.Margin = Hmi::Margin(5);

	_childWindow2.addChild(&_closeButton);	  	
	_childWindow2.Position =Gfx::PointF(10,10);	
	_childWindow2.Size =Gfx::SizeF(420,300);
  _childWindow2.Caption = "Child A";	
	_childWindow2.addChild(&_mainPanel);
	_childWindow2.Visible = true;

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