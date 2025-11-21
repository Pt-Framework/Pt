#include "ChildW.h"
#include "Dialog1.h"
#include <Pt/Forms/Application.h>
#include <Pt/Gfx/Algorithm.h>

#include <Pt/Gfx/PngWriter.h>
#include <fstream>

namespace Pt {

namespace Forms {

namespace Demo {

void ChildW::checkInput(const Pt::String& s)
{
    _lineEdit.setAccepted(s.size() == 3);

    if( ! _lineEdit.isAccepted() )
        _lineEdit.setTextColor( Gfx::Color::fromRgb8(255,0,0));
    else
        _lineEdit.setTextColor( Gfx::Color::fromRgb8(0,0,0));
}


void ChildW::checkSpinBox(int value)
{
    _spinBox.setAccepted( _spinBox.value() % 2 == 0);

    if( ! _spinBox.isAccepted() )
        _spinBox.setTextColor( Gfx::Color::fromRgb8(255,0,0));
    else
        _spinBox.setTextColor( Gfx::Color::fromRgb8(0,0,0));
}


ChildW::ChildW(const std::string& title)
{
    _fileMenu.setName("FileMenu");

    _item1.setText("New");
    _item1.setName("NewItem");
    Key f3(Key::F3); 
    _item1.setShortcut(&f3);
    _fileMenu.addItem(_item1);

    _item2.setText("Op&en");
    _item2.setName("OpenItem");
    _item2.setSeperator(true);
    _item2.setContour(Gfx::Pen(Pt::Gfx::Color::fromRgb8(0,0,0), 2));
    _fileMenu.addItem(_item2);

    _item3.triggered() += Pt::slot(*this, &ChildW::onMenuExit);
    _item3.setText("Exit");    
    _item3.setName("ExitItem");
    Key ctrlA(Key::Control, Key::A);
    _item3.setShortcut(&ctrlA);
    _fileMenu.addItem(_item3);

    _editMenu.setName("EditMenu");
    
    _edit1.setText("Cut");
    _editMenu.addItem(_edit1);

    _edit2.setText("Copy");

    Key ctrlC(Key::Control, Key::C);
    _edit2.setShortcut(&ctrlC);
    _editMenu.addItem(_edit2);

    _edit3.setText("Paste");
    _editMenu.addItem(_edit3);
    
    _fileMenuItem.setMnemonic("F&ile");
    _fileMenuItem.setText("File");
    _fileMenuItem.setMenu(&_fileMenu);

    _menuBar.addItem(_fileMenuItem);

    _editMenuItem.setText("Edit");
    _editMenuItem.setMenu(&_editMenu);

    _menuBar.addItem(_editMenuItem);

    _mainLayout.addItem(_menuBar, DockingLayout::Top);

    _workspace.setContent(&_mainLayout);

    setContent(&_workspace);
    
    setTitle(title);
    
    //Icon label
    System::Path appdir( Application::instance().argv()[0] );
    appdir = appdir.dirName();

    Icon icon;
    icon.addImage(16, 16, appdir / "test-16x16.png");
    icon.addImage(24, 24, appdir / "test-24x24.png");
    icon.addImage(32, 32, appdir / "test-32x32.png");
    icon.addImage(48, 48, appdir / "test-48x48.png");
    icon.addImage(64, 64, appdir / "test-64x64.png");

    _iconLabel.setIcon(icon, icon.minimumSize());
    _iconLabel.setAlignment(Alignment::Bottom);
    _iconLabel.setPadding(10);

    //Text    
    _textLabel.setName("TextLabel");
    _textLabel.setText("Platinum C++     Framework");
    _textLabel.setAlignment(Alignment::Bottom);
    _textLabel.setPadding(10);
    _textLabel.setBackground( Gfx::Color::fromRgb8(240, 220, 70) );
    _textLabel.setMnemonicControl(&_toggleButton);
    _textLabel.setCursor( &Forms::Cursor::waitCursor() ); 
    _textLabel.setContour(Gfx::Color::fromRgb8(200, 190, 60));

    //Toggle button
    Pt::Forms::Key key(Pt::Forms::Key::Control, Pt::Forms::Key::I);
    Pt::Gfx::Image toggleImage(Pt::Gfx::ImageFormat::argb32(), 10, 10);
    Gfx::fill(toggleImage.begin(), toggleImage.end(), Gfx::Color());
    
    _toggleButton.setName("toggle");
    _toggleButton.setText("&Toggle Me [CTRL+I]" );
    _toggleButton.setToggle(true);  
    _toggleButton.setShortcut( &key );
    _toggleButton.setMargin(5);
    _toggleButton.setPadding(5);
    _toggleButton.setIcon(icon, icon.minimumSize());

    //Dialog button
    Pt::Forms::Key dKey(Pt::Forms::Key::Control, Pt::Forms::Key::D);
    
    _dialogButton.setName("dialog");  
    _dialogButton.setText("&&Dia&log [CTRL+D]&");
    _dialogButton.setShortcut( &dKey );
    _dialogButton.setMargin(5);
    _dialogButton.setPadding(5);
    _dialogButton.clicked() += Pt::slot(*this, &ChildW::onShowDialog);
    
    //Close button    
    Pt::Forms::Key xKey(Pt::Forms::Key::Control, Pt::Forms::Key::X);

    _closeButton.setContour( Gfx::Color::fromRgb8(150, 30, 30) );
    _closeButton.setForeground( Gfx::Color::fromRgb8(220, 80, 80) );
    _closeButton.setAccentColor( Gfx::Color::fromRgb8(190, 30, 30) );
    _closeButton.setHighlightColor( Gfx::Color::fromRgb8(200, 70, 70) );
    _closeButton.setMinimumHeight(40);

    _closeButton.setName("close"); 
    _closeButton.setText("Close App [CTRL+X]");
    _closeButton.setShortcut(&xKey);
    _closeButton.setMargin(5);
    _closeButton.setPadding(5);
      
    _checkBox.setState(CheckBox::Unspecified);
    _checkBox.setName("CheckBox"); 
    _checkBox.setText("Ch&eck Me");

    _checkBox.setMargin(5);
    _checkBox.setPadding(5);
    _checkBox.clicked() += Pt::slot(*this, &ChildW::onCheckBox);

    _lineEdit.setName("LineEdit");
    _lineEdit.setAccepted(true); 
    //_lineEdit.setText("Hello World!");
    _lineEdit.setPlaceholderText("placeholder text");
    _lineEdit.setMargin(5);
    _lineEdit.setTextAdjustment(Adjustment::Center);
    //_lineEdit.setEchoMode(Pt::Forms::LineEdit::Masked);
    _lineEdit.editingFinished() += Pt::slot(_textLabel, &Pt::Forms::Label::setText);
    _lineEdit.textEdited() += Pt::slot(*this, &ChildW::checkInput);

    _comboBox.setName("ComboBox");
    _comboBox.setMargin(5);
    _comboBox.setMaxHeight(150);
    _comboBox.selected() += Pt::slot(*this, &ChildW::onComboSelected);

    for(unsigned n = 0; n < 9; ++n)
    {
        ListBoxItem* item = new ListBoxItem;
        
        std::ostringstream oss;
        oss << "Item " << n;
        item->setText(oss.str().c_str());
        item->setIcon(icon, icon.minimumSize());

        _comboBox.addItem(*item);
        _comboItems.push_back(item);
    }

    _progressBar.setName("ProgressBar"); 
    _progressBar.setRange(0, 100);
    _progressBar.setMargin(5);
    _progressBar.setPadding(5);

    _slider.setName("Slider"); 
    _slider.setMargin(5);
    _slider.setPadding(5);
    _slider.setRange(0, 100);
    _slider.setPosition(100);
    _slider.positionChanged() += Pt::slot(_progressBar, &ProgressBar::setValue);

    _spinBox.setName("SpinBox"); 
    _spinBox.setMargin(5);
    _spinBox.valueEdited() += Pt::slot(*this, &ChildW::checkSpinBox);

    _buttonBar.setName("ButtonBar");
    _buttonBar.setPadding(5);
    _buttonBar.addItem(_comboBox, DockingLayout::Top);
    _buttonBar.addItem(_lineEdit, DockingLayout::Top);
    _buttonBar.addItem(_checkBox, DockingLayout::Top);
    _buttonBar.addItem(_closeButton, DockingLayout::Top);
    _buttonBar.addItem(_dialogButton, DockingLayout::Top); 
    _buttonBar.addItem(_toggleButton, DockingLayout::Top);
    _buttonBar.addItem(_progressBar, DockingLayout::Top);
    _buttonBar.addItem(_slider, DockingLayout::Top);
    _buttonBar.addItem(_spinBox, DockingLayout::Top);
 
    _childView.setName("MainPanel");
    _childView.setPadding(20); 
    _childView.addItem(_textLabel, DockingLayout::Fill);
    _childView.addItem(_iconLabel, DockingLayout::Bottom);
    _childView.addItem(_buttonBar, DockingLayout::Bottom);
     
    //_childWindow2.setMainWidget(&_closeButton);
    _childWindow2.move(Gfx::PointF(5, 40));
    _childWindow2.resize( Gfx::SizeF(250, 500) );
    _childWindow2.setTitle("Child of " + title);
    _childWindow2.setContent(&_childView);
    _workspace.addWindow( _childWindow2 );

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


void ChildW::onMenuExit(MenuBaseItem& item)
{
    Pt::Forms::Application::instance().exit();
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

    _buttonBar.removeItem(_closeButton);
    _buttonBar.addItem(_closeButton, DockingLayout::Bottom);

    _dialogButton.setText("Hallo");

    //std::ofstream ofs("child-window.png", std::ios::binary);
    //Gfx::PngWriter pw(ofs);
    //pw.write( _childWindow2.getImage() );
}


void ChildW::onCheckBox()
{
}


void ChildW::onComboSelected(Pt::Forms::ListBoxItem& item)
{
    _textLabel.setText( item.text() );
}


void ChildW::onTextEntered(const Pt::String& text)
{
    std::clog << "TEXT ENTERED: " << text.narrow() << std::endl;
    _lineEdit.focus();
}


} // namespace

} // namespace

} // namespace
