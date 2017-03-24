#include "ChildW.h"
#include "Dialog1.h"
#include <Pt/Hmi/Application.h>
#include <Pt/Gfx/Algorithm.h>

namespace Pt {

namespace Hmi {

namespace Demo {

void ChildW::checkInput(const Pt::String& s)
{
    _lineEdit.setAccepted(s.size() == 3);

    if( ! _lineEdit.isAccepted() )
        _lineEdit.setTextColor( Gfx::Color::fromRgb8(255,0,0));
    else
        _lineEdit.setTextColor( Gfx::Color::fromRgb8(0,0,0));
}


ChildW::ChildW(const std::string& title)
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
    //_textLabel.setAutoSize(true);
    _textLabel.resize( Gfx::SizeF(100,40) );
    _textLabel.setName("TextLabel");
    _textLabel.setText("Platinum C++     Framework");
    _textLabel.setAlignment(Alignment::Bottom);
    _textLabel.setPadding(10);
    _textLabel.move( Gfx::PointF(60,60) );
    _textLabel.setBackground( Gfx::Color::fromRgb8(240, 220, 70) );
    _textLabel.setMnemonicWidget(&_toggleButton);
    _textLabel.setCursor( Hmi::Cursor::waitCursor() ); 
    _textLabel.setContour(Gfx::Color::fromRgb8(200, 190, 60));

    //Toggle button
    Pt::Hmi::Key key(Pt::Hmi::Key::Control, Pt::Hmi::Key::I);
    Pt::Gfx::Image toggleImage(Pt::Gfx::ImageFormat::argb32(), Pt::Gfx::Size(10,10));
    Gfx::fill(toggleImage.begin(), toggleImage.end(), Gfx::Color());
    
    _toggleButton.setName("ToggleButton");
    _toggleButton.setText("&Toggle Me [CTRL+I]" );
    _toggleButton.setToggle(true);  
    _toggleButton.setShortcut( &key );
    _toggleButton.move( Gfx::PointF(20,30) );
    _toggleButton.resize( Gfx::SizeF(130,30) ); 
    _toggleButton.setMargin(5);
    _toggleButton.setPadding(5);    
    _toggleButton.setIcon(toggleImage);

    //Dialog button
    Pt::Hmi::Key dKey(Pt::Hmi::Key::Control, Pt::Hmi::Key::D);
    
    _dialogButton.setName("DialogButton");  
    _dialogButton.setText("&&Dia&log [CTRL+D]&");
    _dialogButton.setShortcut( &dKey );
    _dialogButton.move( Gfx::PointF(20,100));
    _dialogButton.resize( Gfx::SizeF(130,30) );
    _dialogButton.setMargin(5);
    _dialogButton.setPadding(5);
    _dialogButton.clicked() += Pt::slot(*this, &ChildW::onShowDialog);
    
    //Close button    
    Pt::Hmi::Key xKey(Pt::Hmi::Key::Control, Pt::Hmi::Key::X);

    _closeButton.setContour( Gfx::Color::fromRgb8(140, 30, 30) );
    _closeButton.setForeground( Gfx::Color::fromRgb8(220, 80, 80) );
    _closeButton.setName("CloseButton"); 
    _closeButton.setText("Close App [CTRL+X]");
    _closeButton.setShortcut(&xKey);
    _closeButton.move( Gfx::PointF(20,200) );
    _closeButton.resize( Gfx::SizeF(130, 30) );
    _closeButton.setMargin(5);
    _closeButton.setPadding(5);
    _closeButton.clicked() += Pt::slot(*this, &ChildW::onCloseApp);
      
    _checkBox.setState(CheckBox::Unspecified);
    _checkBox.setName("CheckBox"); 
    _checkBox.setText("Ch&eck Me");
    _checkBox.move( Gfx::PointF(0, 0) );
    _checkBox.resize( Gfx::SizeF(130, 30) );
    _checkBox.setMargin(5);
    _checkBox.setPadding(5);
    _checkBox.clicked() += Pt::slot(*this, &ChildW::onCheckBox);

    _lineEdit.setName("LineEdit");
    _lineEdit.setAccepted(true); 
    //_lineEdit.setText("Hello World!");
    _lineEdit.setPlaceholderText("placeholder text");
    _lineEdit.resize( Gfx::SizeF(130, 26) );
    _lineEdit.setMargin(5);
    _lineEdit.setTextAdjustment(Adjustment::Center);
    //_lineEdit.setEchoMode(Pt::Hmi::LineEdit::Masked);
    _lineEdit.editingFinished() += Pt::slot(_textLabel, &Pt::Hmi::Label::setText);
    _lineEdit.textEdited() += Pt::slot(*this, &ChildW::checkInput);

    _comboBox.setName("ComboBox");
    _comboBox.setMargin(5);
    _comboBox.resize( Gfx::SizeF(66, 26) );
    _comboBox.setMaxHeight(100);
    _comboBox.selected() += Pt::slot(*this, &ChildW::onComboSelected);

    for(unsigned n = 0; n < 9; ++n)
    {
        ListBoxItem* item = new ListBoxItem;
        //item->resize( Gfx::SizeF(20, 30) );
        item->setAutoSize(true);
        
        std::ostringstream oss;
        oss << "Item " << n;
        item->setText(oss.str().c_str());

        _comboBox.addItem(*item);
        _comboItems.push_back(item);
    }

    _progressBar.setName("ProgressBar"); 
    _progressBar.move( Gfx::PointF(0, 0) );
    _progressBar.resize( Gfx::SizeF(130, 30) );
    _progressBar.setMargin(5);
    _progressBar.setPadding(5);

    _slider.setName("Slider"); 
    _slider.move( Gfx::PointF(0, 0) );
    _slider.resize( Gfx::SizeF(100, 30) );
    _slider.setMargin(5);
    _slider.setPadding(5);
    _slider.setRange(0, 100);
    _slider.setPosition(100);

    _buttonBar.setName("ButtonBar");
    _buttonBar.resize( Gfx::SizeF(700, 290) );
    _buttonBar.setPadding(5);
    _buttonBar.dock(_comboBox, DockingLayout::Bottom);
    _buttonBar.dock(_lineEdit, DockingLayout::Bottom);
    _buttonBar.dock(_checkBox, DockingLayout::Bottom);
    _buttonBar.dock(_closeButton, DockingLayout::Bottom);
    _buttonBar.dock(_dialogButton, DockingLayout::Bottom); 
    _buttonBar.dock(_toggleButton, DockingLayout::Bottom);
    _buttonBar.dock(_progressBar, DockingLayout::Bottom);
    _buttonBar.dock(_slider, DockingLayout::Bottom);
 
    _childView.resize( Gfx::SizeF(300, 620) );
    _childView.move( Gfx::PointF(1,1) );
    _childView.setName("MainPanel");
    _childView.setPadding(20); 
    _childView.dock(_textLabel, DockingLayout::Fill);
    _childView.dock(_buttonBar, DockingLayout::Bottom);
     
    //_childWindow2.setMainWidget(&_closeButton);
    _childWindow2.move(Gfx::PointF(5, 40));
    _childWindow2.resize( Gfx::SizeF(250, 500) );
    _childWindow2.setTitle("Child of " + title);
    _childWindow2.setMainWidget(&_childView);
    add( _childWindow2 );

    _childWindow2.show(true); // Child A/B
}


ChildW::~ChildW()
{
    std::vector<ListBoxItem*>::iterator it;
    for(it = _comboItems.begin(); it != _comboItems.end(); ++it)
    {
        delete *it;
    }
}


void ChildW::onShowDialog()
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

    //Dialog1 d;    
    //d.showModal();
    
    //enable(false);

    _buttonBar.remove(_closeButton);
    _buttonBar.dock(_closeButton, DockingLayout::Bottom);

    _dialogButton.setText("Hallo");
}


void ChildW::onCheckBox()
{
    std::clog << "CHECKBOX CLICKED" << std::endl;
}


void ChildW::onComboSelected(Pt::Hmi::ListBoxItem& item)
{
    _textLabel.setText( item.text() );
}


void ChildW::onTextEntered(const Pt::String& text)
{
    std::clog << "TEXT ENTERED: " << text.narrow() << std::endl;
    _lineEdit.focus();
}


void ChildW::onCloseApp()
{
    mainWindow().close();
}

} // namespace

} // namespace

} // namespace
