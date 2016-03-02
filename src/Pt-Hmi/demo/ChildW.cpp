#include "ChildW.h"
#include <Pt/Hmi/Application.h>
#include <Pt/Hmi/Docking.h>
#include "Dialog1.h"

namespace Pt{

namespace Hmi{

namespace Demo{

ChildW::ChildW(const std::string& title)
{
    setTitle(title);
    setPosition( Gfx::PointF(5,5) );
    setSize( Gfx::SizeF(520, 380) );         

    //Text    
    _textLabel.setAutoSize(true);
    _textLabel.setSize( Gfx::SizeF(100,40) );
    _textLabel.setName("TextLabel");
    _textLabel.setText("Pla&tinum C++ Framework");  
    _textLabel.setPosition( Gfx::PointF(60,60) );
    _textLabel.setForegroundColor( Gfx::Color(1,0,0,0) );    
    _textLabel.setBackgroundColor( Gfx::Color(1,1,1,0) );
    _textLabel.setMnemonicWidget(&_toggleButton);
    _textLabel.setCursor( Hmi::Cursor::waitCursor() );
    _textLabel.docking().setType(Docking::Fill);  
    _textLabel.update();

    //Toggle button
    _toggleButton.setName("ToggleButton");
    _toggleButton.setText("Toggle Me [CTRL+I]" );
    
    Pt::Hmi::Key key(Pt::Hmi::Key::Control, Pt::Hmi::Key::I);
    _toggleButton.setShortcut( &key );
    _toggleButton.setPosition( Gfx::PointF(20,30) );
    _toggleButton.setSize( Gfx::SizeF(130,30) ); 
    _toggleButton.margin().setAll(5);
    _toggleButton.padding().setAll(5);
    _toggleButton.docking().setType( Docking::Bottom );
    _toggleButton.update(); 

    //Dialog button   
    _dialogButton.setName("DialogButton");  
    _dialogButton.setText("&&Dia&log [CTRL+D]&");
    Pt::Hmi::Key dKey(Pt::Hmi::Key::Control, Pt::Hmi::Key::D);
    _dialogButton.setShortcut( &dKey );
    _dialogButton.setPosition( Gfx::PointF(20,100));
    _dialogButton.setSize( Gfx::SizeF(130,30) );
    _dialogButton.margin().setAll(5);
    _dialogButton.padding().setAll(5);
    _dialogButton.docking().setType( Docking::Bottom );
    _dialogButton.clicked() += Pt::slot(*this, &ChildW::onShowDialog);
    _dialogButton.update(); 
    
    //Close button
    _closeButton.setName("CloseButton"); 
    _closeButton.setText("Close App [CTRL+X]");
    
    Pt::Hmi::Key xKey(Pt::Hmi::Key::Control, Pt::Hmi::Key::X);

    _closeButton.setShortcut(&xKey);
    _closeButton.setPosition( Gfx::PointF(20,200) );
    _closeButton.setSize( Gfx::SizeF(130, 30) );
    _closeButton.margin().setAll(5);
    _closeButton.padding().setAll(5);
    _closeButton.docking().setType( Docking::Bottom );
    _closeButton.clicked() += Pt::slot(*this, &ChildW::onCloseApp);
    _closeButton.update(); 

    _buttonBar.setAlignment(FlowLayout::Bottom);
    _buttonBar.setName("ButtonBar");
    _buttonBar.setSize( Gfx::SizeF(700, 180) );
    _buttonBar.padding().setAll(5);
    _buttonBar.docking().setType( Docking::Bottom );
    _buttonBar.add(_closeButton);
    _buttonBar.add(_dialogButton);
    _buttonBar.add(_toggleButton);
    _buttonBar.update(); 

    //Panel  
    _mainWidget.setSize( Gfx::SizeF(800, 600) );
    _mainWidget.setPosition( Gfx::PointF(20,20) );
    _mainWidget.setName("MainPanel");
    _mainWidget.padding().setAll(20); 
    _mainWidget.add(_textLabel);
    _mainWidget.add(_buttonBar);
    _mainWidget.update(); 

    //_childWindow2.setMainWidget(&_closeButton);          
    _childWindow2.setPosition(Gfx::PointF(5,5));    
    _childWindow2.setSize (Gfx::SizeF(240, 320));
    _childWindow2.setTitle("Child of " + title);    

    _childWindow2.setMainWidget(&_mainWidget);
    
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

void ChildW::onCloseApp(Button& button)
{
    Hmi::Application::instance().exit();
}

} // namespace

} // namespace

} // namespace
