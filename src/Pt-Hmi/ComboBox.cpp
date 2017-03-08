/* Copyright (C) 2017 Marc Boris Duerner
 
 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.
 
 As a special exception, you may use this file as part of a free
 software library without restriction. Specifically, if other files
 instantiate templates or use macros or inline functions from this
 file, or you compile this file and link it with other files to
 produce an executable, this file does not by itself cause the
 resulting executable to be covered by the GNU General Public
 License. This exception does not however invalidate any other
 reasons why the executable file might be covered by the GNU Library
 General Public License.
 
 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
 MA 02110-1301 USA
*/

#include <Pt/Hmi/ComboBox.h>
#include <Pt/Hmi/Painter.h>

namespace Pt {

namespace Hmi {

/////////////////////////////////////////////////////////////////////////////
// ComboBoxPopup
/////////////////////////////////////////////////////////////////////////////

ComboBoxPopup::ComboBoxPopup()
: Popup()
{
    setMainWidget(&_items);

    _items.setAutoSize(true);
    _items.selected() += Pt::slot(*this, &ComboBoxPopup::onItemSelected);
}

		
ComboBoxPopup::~ComboBoxPopup()
{
}


void ComboBoxPopup::addItem(ListBoxItem& item)
{   
    _items.addItem(item);
}


void ComboBoxPopup::removeItem(ListBoxItem& item)
{   
    _items.removeItem(item);
}


void ComboBoxPopup::onItemSelected(ListBoxItem&)
{
    show(false);
}


void ComboBoxPopup::setScrollBars(bool hasScrollBars)
{
    _items.setScrollBars(hasScrollBars);
}


Pt::Signal<ListBoxItem&>& ComboBoxPopup::selected()
{
    return _items.selected();
}


Gfx::SizeF ComboBoxPopup::preferredSize(const SizePolicy& policy) const
{
    return _items.preferredSize(policy);
}

/////////////////////////////////////////////////////////////////////////////
// ComboBox
/////////////////////////////////////////////////////////////////////////////

ComboBox::ComboBox()
: _maxHeight(500)
, _spacing(2)
, _isEditable(true)
, _hasRenderer(false)
{
    setTextInput(_isEditable);
    setFocusPolicy(Widget::NormalFocus);

    _popup.eventReady() += Pt::slot(*this, &ComboBox::processKeyEvent);
    _popup.selected() += Pt::slot(*this, &ComboBox::onItemSelected);
}


ComboBox::~ComboBox()
{
}


void ComboBox::addItem(ListBoxItem& item)
{   
    _popup.addItem(item);

    //item.setTextInput(true);
}


void ComboBox::removeItem(ListBoxItem& item)
{
    _popup.addItem(item);
}


bool ComboBox::isEditable() const
{
    return _isEditable;
}


void ComboBox::setEditable(bool e)
{
    _isEditable = e;
    setTextInput(_isEditable);
    update();
}


const Pt::String& ComboBox::text() const
{
    return _editor.text();
}


void ComboBox::setText(const Pt::String& str)
{
    _editor.setText(str);
    invalidate();
}


Adjustment ComboBox::textAdjustment() const
{
    return _editor.adjustment();
}


void ComboBox::setTextAdjustment(Adjustment a)
{
    _editor.setAdjustment(a);
    invalidate();
}


void ComboBox::setScrollBars(bool hasScrollBars)
{
    _popup.setScrollBars(hasScrollBars);
}


void ComboBox::setMaxHeight(double height)
{
    _maxHeight = height;
}


void ComboBox::showPopup()
{
    SizePolicy policy(SizePolicy::Fixed, SizePolicy::Preferred);
    policy.setSize( size() );

    Gfx::SizeF popupSize = _popup.preferredSize(policy);
    popupSize.setHeight( std::min(popupSize.height(), _maxHeight) );
    _popup.resize(popupSize);
    
    Gfx::PointF popupPos(0, size().height() );
    popupPos = this->toScreen(popupPos);
    _popup.move(popupPos);

    _popup.show();
    _popup.activate();
}


void ComboBox::hidePopup()
{
    _popup.show(false);
}


Pt::Signal<const Pt::String&>& ComboBox::textEdited()
{
    return _textEdited;
}


Pt::Signal<const Pt::String&>& ComboBox::returnPressed()
{
    return _returnPressed;
}


Pt::Signal<const Pt::String&>& ComboBox::editingFinished()
{
    return _editingFinished;
}


Pt::Signal<ListBoxItem&>& ComboBox::selected()
{
    return _popup.selected();
}

const Gfx::Brush& ComboBox::background() const
{
    return _background ? *_background
                       : Application::instance().styleOptions().textBackground();
}


void ComboBox::setBackground(const Gfx::Brush& b)
{
    _background.reset( new Gfx::Brush(b) );
    invalidate();
}


const Gfx::Brush& ComboBox::foreground() const
{
    return _foreground ? *_foreground
                       : Application::instance().styleOptions().foreground();
}


void ComboBox::setForeground(const Gfx::Brush& b)
{
    _foreground.reset( new Gfx::Brush(b) );
    invalidate();
}


const Gfx::Pen& ComboBox::contour() const
{
    return _contour ? *_contour
                    : Application::instance().styleOptions().contour();
}


void ComboBox::setContour(const Gfx::Pen& p)
{
    _contour.reset( new Gfx::Pen(p) );
    invalidate();
}


const Gfx::Color& ComboBox::textColor() const
{
    return _textColor ? *_textColor
                      : Application::instance().styleOptions().textColor();
}


void ComboBox::setTextColor(const Gfx::Color& color)
{
    _textColor.reset( new Gfx::Color(color) );
    invalidate();
}


const std::string& ComboBox::font() const
{
    return _fontName ? *_fontName
                     : Application::instance().styleOptions().font().name();
}


void ComboBox::setFont(const std::string& fontName)
{
    _fontName.reset( new std::string(fontName) );
    invalidate();
}


std::size_t ComboBox::fontSize() const
{

    return _fontSize ? *_fontSize
                     : Application::instance().styleOptions().font().size();
}


void ComboBox::setFontSize(const std::size_t s)
{
    _fontSize.reset( new std::size_t(s) );
    invalidate();
}


Gfx::Font::Style ComboBox::fontStyle() const
{
    return _fontStyle ? *_fontStyle
                      : Application::instance().styleOptions().font().style();
}


void ComboBox::setFontStyle(Gfx::Font::Style style)
{
    _fontStyle.reset( new Gfx::Font::Style(style) );
    invalidate();
}


void ComboBox::setRenderer(ComboBoxRenderer* renderer)
{
    _renderer.reset(renderer);
    _hasRenderer = renderer != 0;

    invalidate();
}


void ComboBox::onItemSelected(ListBoxItem& item)
{
    Application::instance().inputMethod().finish();
    _editor.setText( item.text() );
    invalidate();
}


void ComboBox::onInvalidate()
{
    Base::onInvalidate();

    const StyleOptions& options = Application::instance().styleOptions();
    const Style& style = Application::instance().style();

    _backgroundBrush = background();
    _foregroundBrush = foreground();
    _pen = contour();
    _textPen = textColor();
    _font = Gfx::Font(font(), fontSize(), fontStyle());

    if( ! _hasRenderer )
        _renderer.reset( style.get<ComboBoxRenderer>() );
    
    if( ! _renderer )
        return;

    _renderer->prepare(*this, options, _backgroundBrush, _foregroundBrush,
                       _pen, _font, _textPen);
    
    _editor.setFont(_font);
    _editor.layout(_line);
}


void ComboBox::onPaint(PaintSurface& surface, const Gfx::RectF& rect)
{
    const StyleOptions& options = Application::instance().styleOptions();

    if( ! _renderer)
        return;

    Painter painter(surface);
    painter.setClip(rect);

    //
    // combo box
    //

    _renderer->renderBackground( *this, options, painter, rect,
                                 _pen, _backgroundBrush );

    _renderer->renderButton( *this, options, painter, rect,
                             _pen, _foregroundBrush );

    //
    // text with cursor
    //
    
    Gfx::RectF cursorRect;

    Gfx::PointF clipPos = _editor.position();
    Gfx::SizeF clipSize = _editor.size();
    clipSize.addWidth(_spacing); // cursor

    if( _isEditable && hasFocus() )
    {
        double cursorX = _line.cursorToX( _editor.cursorPosition() );
        cursorX += _line.position().x();
        
        double cursorWidth = 1;

        cursorRect.set(Gfx::PointF( cursorX, _line.position().y() ),
                       Gfx::SizeF( cursorWidth, _line.maxHeight() ) );           
    }

    Gfx::RectF clipRect(clipPos, clipSize);
    painter.setClip( Gfx::RectF(clipPos, clipSize) );

    Gfx::PointF textPos = _line.position();
    textPos.addY( _line.ascent() );

    _renderer->renderText(*this, options, painter, rect, 
                          _editor.text(), textPos, _font, _textPen, cursorRect);
}


void ComboBox::onResizeEvent(const ResizeEvent& ev)
{
    Base::onResizeEvent(ev);
    
    if( ! _renderer )
    {
        const Style& style = Application::instance().style();
        _renderer.reset( style.get<ComboBoxRenderer>() );
    }

    if( ! _renderer )
        return;

    _renderer->prepareLayout(*this, _buttonSize);

    _spacing = ev.size().height() / 5;
    if(_spacing < 2)
        _spacing = 2;

    Gfx::PointF editPosition(_spacing, 0);
    _editor.setPosition(editPosition);

    Gfx::SizeF editSize = ev.size();
    editSize.addWidth( - _buttonSize.width() );
    editSize.addWidth(-3 * _spacing); // left, right, cursor
    _editor.setSize(editSize);

    _editor.layout(_line);
}


void ComboBox::processKeyEvent(const KeyEvent& ev)
{
    if( ! ev.isPress() || ! _isEditable )
        return;

    if( ev.key().code() == Pt::Hmi::Key::ArrowLeft )
    {
        _editor.left();
        invalidate();
    }
    else if( ev.key().code() == Pt::Hmi::Key::ArrowRight )
    {
        _editor.right();
        invalidate();
    }
    else if( ev.key().code() == Pt::Hmi::Key::Return )
    {
        invalidate();
        _returnPressed.send( _editor.text() );
    }
    else if( ev.key().code() == Pt::Hmi::Key::Delete )
    {
        _editor.del();
        invalidate();

        _textEdited.send( _editor.text() );
    }
    else if( ev.key().code() == Pt::Hmi::Key::Backspace )
    {
        _editor.backspace();
        invalidate();

        _textEdited.send( _editor.text() );
    }
    else
    {
        Pt::Char ch = ev.unicode();
        if( Pt::isprint(ch) )
        {
            _editor.insert(ch);
            invalidate();

            _textEdited.send( _editor.text() );
        }
    }
}


void ComboBox::onKeyEvent(const KeyEvent& ev)
{
    Base::onKeyEvent(ev);

    processKeyEvent(ev);
}


bool ComboBox::onMouseEvent(const MouseEvent& ev)
{    
    Base::onMouseEvent(ev);

    if( ! ev.isPress() )
        return true;

    double buttonX = size().width() - _buttonSize.width();
        
    if( ev.position().x() > buttonX )
    {
        showPopup();
    }
    else if(_isEditable)
    {
        std::size_t n = _line.xToCursor( ev.x() );
        _editor.setCursorPosition(n);
        update();
            
        Application::instance().inputMethod().begin(*this);
    }

    return true;
}


void ComboBox::onTouchEvent(const TouchEvent& ev)
{    
    Base::onTouchEvent(ev);

    if( ! ev.isPress() )
        return;

    double buttonX = size().width() - _buttonSize.width();

    if( ev.position().x() > buttonX )
    {
        showPopup();
    }
    else if(_isEditable)
    {
        std::size_t n = _line.xToCursor( ev.x() );
        _editor.setCursorPosition(n);
        update();

        Application::instance().inputMethod().begin(*this);
    }
}


void ComboBox::onFocusEvent(const FocusEvent& ev)
{
    Base::onFocusEvent(ev);

    if( ! ev.isFocused() )
    {
        //if( isAccepted() && _isTextChanged)
        //{
        //    _isTextChanged = false;
        //    _editingFinished.send(_text);
        //}
    }
    else if(_isEditable)
    {
        Application::instance().inputMethod().begin(*this);
    }
}

} // namespace

} // namespace


