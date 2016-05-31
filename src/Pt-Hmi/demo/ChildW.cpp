#include "ChildW.h"
#include <Pt/Hmi/Application.h>
#include <Pt/Hmi/Docking.h>
#include "Dialog1.h"

namespace Pt{

namespace Hmi{

namespace Demo{

ChildW::ChildW(const std::string& title)
{
    _menu.setName("FileMenu");
    
    _item1.setText("New");
    _item1.setShortcut( &Key(Key::F3) );
    _menu.addItem(_item1);

    _item2.setText("Open");
    _menu.addItem(_item2);

    _item3.setText("Exit");
    _item3.setShortcut( &Key(Key::Control, Key::A) );
    _menu.addItem(_item3);

    _menuBar.resize( Pt::Gfx::SizeF(20, 24) );
    _menuBar.addMenu(_menu, "File");

    _mainLayout.setAlignment(FlowLayout::Top);
    _mainLayout.add(_menuBar);

    setMainWidget(&_mainLayout);

    setTitle(title);
    move( Gfx::PointF(5, 5) );
    resize( Gfx::SizeF(480, 400) );        
    
    //Text    
    _textLabel.setAutoSize(true);
    _textLabel.resize( Gfx::SizeF(100,40) );
    _textLabel.setName("TextLabel");
    _textLabel.setText("Pla&tinum C++ Framework");  
    _textLabel.move( Gfx::PointF(60,60) );
    _textLabel.setForegroundColor( Gfx::Color(1,0,0,0) );    
    _textLabel.setBackgroundColor( Gfx::Color(1,1,1,0) );
    _textLabel.setMnemonicWidget(&_toggleButton);
    _textLabel.setCursor( Hmi::Cursor::waitCursor() );
    _textLabel.setDocking(Docking::Fill);  

    //Toggle button
    _toggleButton.setName("ToggleButton");
    _toggleButton.setText("Toggle Me [CTRL+I]" );
    
    Pt::Hmi::Key key(Pt::Hmi::Key::Control, Pt::Hmi::Key::I);
    _toggleButton.setShortcut( &key );
    _toggleButton.move( Gfx::PointF(20,30) );
    _toggleButton.resize( Gfx::SizeF(130,30) ); 
    _toggleButton.setMargin(5);
    _toggleButton.setPadding(5);
    _toggleButton.setDocking( Docking::Bottom );    

    //Dialog button   
    _dialogButton.setName("DialogButton");  
    _dialogButton.setText("&&Dia&log [CTRL+D]&");
    Pt::Hmi::Key dKey(Pt::Hmi::Key::Control, Pt::Hmi::Key::D);
    _dialogButton.setShortcut( &dKey );
    _dialogButton.move( Gfx::PointF(20,100));
    _dialogButton.resize( Gfx::SizeF(130,30) );
    _dialogButton.setMargin(5);
    _dialogButton.setPadding(5);
    _dialogButton.setDocking( Docking::Bottom );
    _dialogButton.clicked() += Pt::slot(*this, &ChildW::onShowDialog);
    
    
    //Close button
    _closeButton.setName("CloseButton"); 
    _closeButton.setText("Close App [CTRL+X]");
    
    Pt::Hmi::Key xKey(Pt::Hmi::Key::Control, Pt::Hmi::Key::X);

    _closeButton.setShortcut(&xKey);
    _closeButton.move( Gfx::PointF(20,200) );
    _closeButton.resize( Gfx::SizeF(130, 30) );
    _closeButton.setMargin(5);
    _closeButton.setPadding(5);
    _closeButton.setDocking( Docking::Bottom );
    _closeButton.clicked() += Pt::slot(*this, &ChildW::onCloseApp);

    _buttonBar.setAlignment(FlowLayout::Bottom);
    _buttonBar.setName("ButtonBar");
    _buttonBar.resize( Gfx::SizeF(700, 180) );
    _buttonBar.setPadding(5);
    _buttonBar.setDocking(Docking::Bottom);
    _buttonBar.add(_closeButton);
    _buttonBar.add(_dialogButton);
    _buttonBar.add(_toggleButton);
    
    //Panel  
    _childView.resize( Gfx::SizeF(800, 600) );
    _childView.move( Gfx::PointF(20,20) );
    _childView.setName("MainPanel");
    _childView.setPadding(20); 
    _childView.add(_textLabel);
    _childView.add(_buttonBar);    

    //_childWindow2.setMainWidget(&_closeButton);          
    _childWindow2.move(Gfx::PointF(5, 40));    
    _childWindow2.resize (Gfx::SizeF(240, 320));
    _childWindow2.setTitle("Child of " + title);    
    _childWindow2.setMainWidget(&_childView);
    add( _childWindow2 );

    _childWindow2.show(true); // Child A/B
}


ChildW::~ChildW()
{
}


void ChildW::onShowDialog(Button& button)
{
    std::clog << "----------------" << std::endl;
    _closeButton.setText("AAA");
    _closeButton.setText("BBB");
    _closeButton.setText("CCC");    

    //Gfx::SizeF size(400,260);
    //_childWindow2.resize(Gfx::SizeF(400,260));

    //Gfx::PointF pos(0,0);
    //_childWindow2.move(pos);

    Dialog1 d;    
    d.showModal();
}

void ChildW::onCloseApp(Button& button)
{
    parent()->close();
}

} // namespace

} // namespace

} // namespace
