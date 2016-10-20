#include "ChildW.h"
#include <Pt/Hmi/Application.h>
#include <Pt/Hmi/Docking.h>
#include "Dialog1.h"

namespace Pt{

namespace Hmi{

namespace Demo{

ChildW::ChildW(const std::string& title)
: _vscroll(ScrollBar::Vertical)
{
    _fileMenu.setName("FileMenu");
    
    _item1.setText("New");
    Key f3(Key::F3); 
    _item1.setShortcut(&f3);
    _fileMenu.addItem(_item1);

    _item2.setText("Open");
    _fileMenu.addItem(_item2);

    _item3.setText("Exit");
    Key ctrlA(Key::Control, Key::A);
    _item3.setShortcut(&ctrlA);
    _fileMenu.addItem(_item3);

    _editMenu.setName("FileMenu");
    
    _edit1.setText("Cut");
    _editMenu.addItem(_edit1);

    _edit2.setText("Copy");

    Key ctrlC(Key::Control, Key::C);
    _edit2.setShortcut(&ctrlC);
    _editMenu.addItem(_edit2);

    _edit3.setText("Paste");
    _editMenu.addItem(_edit3);

    _menuBar.resize( Pt::Gfx::SizeF(100, 20) );
    _menuBar.addMenu(_fileMenu, "File");
    _menuBar.addMenu(_editMenu, "Edit");

    _mainLayout.setDirection(FlowLayout::Top);
    _mainLayout.add(_menuBar);

    setMainWidget(&_mainLayout);
    
    
    setTitle(title);
    
    //Text    
    _textLabel.setAutoSize(true);
    _textLabel.resize( Gfx::SizeF(100,40) );
    _textLabel.setName("TextLabel");
    _textLabel.setText("Pla&tinum C++ Framework");  
    _textLabel.move( Gfx::PointF(60,60) );
    _textLabel.setForegroundPen( Gfx::Pen(Gfx::Color(0, 0, 0), 1) );
    _textLabel.setBackgroundBrush( Gfx::Color(65535,65535,65535,0) );
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
    _dialogButton.setDocking(Docking::Top); 
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

    _closeButton.setDocking(Docking::Bottom);
    _closeButton.setShortcut(&xKey);
    _closeButton.move( Gfx::PointF(20,200) );
    _closeButton.resize( Gfx::SizeF(130, 30) );
    _closeButton.setMargin(5);
    _closeButton.setPadding(5);
    _closeButton.setDocking( Docking::Bottom );
    _closeButton.clicked() += Pt::slot(*this, &ChildW::onCloseApp);

    _buttonBar.setName("ButtonBar");
    _buttonBar.resize( Gfx::SizeF(700, 180) );
    _buttonBar.setPadding(5);
    _buttonBar.setDocking(Docking::Bottom);
    _buttonBar.add(_closeButton);
    _buttonBar.add(_dialogButton);
    _buttonBar.add(_toggleButton);
    
    _vscroll.resize( Gfx::SizeF(24, 24) );
    _vscroll.setDocking(Docking::Right);

    //Panel  
    _childView.resize( Gfx::SizeF(300, 600) );
    _childView.move( Gfx::PointF(1,1) );
    _childView.setName("MainPanel");
    _childView.setPadding(20); 
    _childView.add(_textLabel);
    _childView.add(_buttonBar);   
    _childView.add(_vscroll);
     

    //_childWindow2.setMainWidget(&_closeButton);          
    _childWindow2.move(Gfx::PointF(5, 40));    
    _childWindow2.resize( Gfx::SizeF(240, 320) );
    _childWindow2.setTitle("Child of " + title);    
    _childWindow2.setMainWidget(&_childView);
    add( _childWindow2 );

    _childWindow2.show(true); // Child A/B
}


ChildW::~ChildW()
{
}


void ChildW::onShowDialog(Pt::Hmi::Button&)
{
    //std::clog << "----------------" << std::endl;
    //_closeButton.setText("AAA");
    //_closeButton.setText("BBB");
    //_closeButton.setText("CCC");    
    //_closeButton.setMargin(50);
    

    //Gfx::SizeF size(400,260);
    //_childWindow2.resize(Gfx::SizeF(400,260));

    //Gfx::PointF pos(0,0);
    //_childWindow2.move(pos);

    Dialog1 d;    
    d.showModal();
    
    //enable(false);
}

void ChildW::onCloseApp(Pt::Hmi::Button&)
{
    mainWindow().close();
}

} // namespace

} // namespace

} // namespace
