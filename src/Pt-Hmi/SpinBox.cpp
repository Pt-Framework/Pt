/* Copyright (C) 2015 Marc Boris Duerner 
   Copyright (C) 2015 Laurentiu-Gheorghe Crisan

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
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
  MA 02110-1301 USA
*/

#include <Pt/Hmi/SpinBox.h>
#include <Pt/Hmi/Painter.h>

namespace Pt {

namespace Hmi {

SpinBox::SpinBox()
: _isEditable(true)
, _isAccepted(true)
, _isTextChanged(false)
, _spacing(0)
, _hasRenderer(false)
{
    setTextInput(_isEditable);
    setFocusPolicy(Widget::NormalFocus);
}


SpinBox::~SpinBox()
{
}


bool SpinBox::isEditable() const
{
    return _isEditable;
}


void SpinBox::setEditable(bool e)
{
    _isEditable = e;
    setTextInput(_isEditable);
    setFocusPolicy(_isEditable ? Widget::NormalFocus : Widget::NoFocus);
    update();
}


const Pt::String& SpinBox::text() const
{
    return _editor.text();
}


void SpinBox::setText(const Pt::String& str)
{
    _editor.setText(str);
    invalidate();

    _textEdited.send( _editor.text() );
}


bool SpinBox::isEmpty() const
{
    return _editor.isEmpty();
}


Adjustment SpinBox::textAdjustment() const
{
    return _editor.adjustment();
}


void SpinBox::setTextAdjustment(Adjustment a)
{
    _editor.setAdjustment(a);
    invalidate();
}


std::size_t SpinBox::cursorPosition() const
{
    return _editor.cursorPosition();
}


void SpinBox::setCursorPosition(std::size_t n)
{
    _editor.setCursorPosition(n);
    invalidate();
}


bool SpinBox::isAccepted() const
{
    return _isAccepted;
}


void SpinBox::setAccepted(bool a)
{
    _isAccepted = a;
    
    if( ! a )
    {
        setFocusPolicy(Widget::KeepFocus);
    }
    else
    {
        setFocusPolicy(Widget::NormalFocus);
    }
}


Pt::Signal<const Pt::String&>& SpinBox::textEdited()
{
    return _textEdited;
}


Pt::Signal<const Pt::String&>& SpinBox::returnPressed()
{
    return _returnPressed;
}


Pt::Signal<const Pt::String&>& SpinBox::editingFinished()
{
    return _editingFinished;
}


const Gfx::Brush& SpinBox::background() const
{
    return _background ? *_background
                       : Application::instance().styleOptions().textBackground();
}


void SpinBox::setBackground(const Gfx::Brush& b)
{
    _background.reset( new Gfx::Brush(b) );
    invalidate();
}


const Gfx::Brush& SpinBox::foreground() const
{
    return _foreground ? *_foreground
                       : Application::instance().styleOptions().foreground();
}


void SpinBox::setForeground(const Gfx::Brush& b)
{
    _foreground.reset( new Gfx::Brush(b) );
    invalidate();
}


const Gfx::Pen& SpinBox::contour() const
{
    return _contour ? *_contour
                    : Application::instance().styleOptions().contour();
}


void SpinBox::setContour(const Gfx::Pen& p)
{
    _contour.reset( new Gfx::Pen(p) );
    invalidate();
}


const Gfx::Color& SpinBox::textColor() const
{
    return _textColor ? *_textColor
                      : Application::instance().styleOptions().textColor();
}


void SpinBox::setTextColor(const Gfx::Color& color)
{
    _textColor.reset( new Gfx::Color(color) );
    invalidate();
}


const std::string& SpinBox::font() const
{
    return _fontName ? *_fontName
                     : Application::instance().styleOptions().font().name();
}


void SpinBox::setFont(const std::string& fontName)
{
    _fontName.reset( new std::string(fontName) );
    invalidate();
}


std::size_t SpinBox::fontSize() const
{

    return _fontSize ? *_fontSize
                     : Application::instance().styleOptions().font().size();
}


void SpinBox::setFontSize(const std::size_t s)
{
    _fontSize.reset( new std::size_t(s) );
    invalidate();
}


Gfx::Font::Style SpinBox::fontStyle() const
{
    return _fontStyle ? *_fontStyle
                      : Application::instance().styleOptions().font().style();
}


void SpinBox::setFontStyle(Gfx::Font::Style style)
{
    _fontStyle.reset( new Gfx::Font::Style(style) );
    invalidate();
}


void SpinBox::setRenderer(SpinBoxRenderer* renderer)
{
    _renderer.reset(renderer);
    _hasRenderer = renderer != 0;

    invalidate();
}


Gfx::SizeF SpinBox::onMeasure(const SizePolicy& policy)
{
    double itemsWidth = policy.width();
    double itemsHeight = _font.size() * 2;

    return Gfx::SizeF( itemsWidth + padding().leftRight(), 
                       itemsHeight + padding().topBottom() );
}


void SpinBox::onLayout(const Gfx::RectF& rect)
{
}


void SpinBox::onInvalidate()
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
        _renderer.reset( style.get<SpinBoxRenderer>() );
    
    if( ! _renderer )
        return;

    _renderer->prepare(*this, options, _backgroundBrush, _foregroundBrush, 
                       _pen, _font, _textPen);

    _editor.setFont(_font);
    _editor.layout(_line);
}


void SpinBox::onPaint(PaintSurface& surface, const Gfx::RectF& rect)
{
    const StyleOptions& options = Application::instance().styleOptions();

    if( ! _renderer)
        return;

    Painter painter(surface);
    painter.setClip(rect);

    //
    // spin box
    //

    _renderer->renderBackground( *this, options, painter, rect,
                                 _pen, _backgroundBrush );

    _renderer->renderButton( *this, options, painter, rect,
                             _pen, _foregroundBrush );

    //
    // text with cursor
    //
    
    Gfx::RectF cursorRect;
    double cursorWidth = 5; // TODO: cursor

    if( _isEditable && hasFocus() )
    {
        double cursorX = _line.cursorToX( _editor.cursorPosition() );
        cursorX += _line.position().x();
        
        cursorRect.set(Gfx::PointF( cursorX, _line.position().y() ),
                       Gfx::SizeF( cursorWidth, _line.maxHeight() ) );           
    }

    Gfx::PointF clipPos = _editor.position();
    Gfx::SizeF clipSize = _editor.size();
    clipSize.addWidth(cursorWidth);  // TODO: cursor
    
    Gfx::RectF clipRect(clipPos, clipSize);
    painter.setClip( Gfx::RectF(clipPos, clipSize) );

    Gfx::PointF textPos = _line.position();
    textPos.addY( _line.ascent() );

    _renderer->renderText(*this, options, painter, rect, 
                          _editor.text(), textPos, _font, _textPen, cursorRect);
}


void SpinBox::onResizeEvent(const ResizeEvent& ev)
{
    Base::onResizeEvent(ev);
    
    if( ! _renderer )
    {
        const Style& style = Application::instance().style();
        _renderer.reset( style.get<SpinBoxRenderer>() );
    }

    if( ! _renderer )
        return;

    _renderer->prepareLayout(*this, _downButton, _upButton, _textBox);

    Gfx::SizeF editSize = _textBox.size();
    editSize.subWidth(5);  // TODO: cursor

    _editor.setPosition( _textBox.topLeft() );
    _editor.setSize( _textBox.size() );
    _editor.layout(_line);
}


void SpinBox::onKeyEvent(const KeyEvent& ev)
{  
    Base::onKeyEvent(ev);

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
        
        if( isAccepted() )
            _returnPressed.send( _editor.text() );
    }
    else if( ev.key().code() == Pt::Hmi::Key::Delete )
    {
        _isTextChanged = true;
        _editor.del();
        invalidate();

        _textEdited.send( _editor.text() );
    }
    else if( ev.key().code() == Pt::Hmi::Key::Backspace )
    {
        _isTextChanged = true;
        _editor.backspace();
        invalidate();

        _textEdited.send( _editor.text() );
    }
    else
    {
        Pt::Char ch = ev.unicode();
        if( Pt::isprint(ch) )
        {
            _isTextChanged = true;
            _editor.insert(ch);
            invalidate();

            _textEdited.send( _editor.text() );
        }
    }
}


bool SpinBox::onMouseEvent(const MouseEvent& ev)
{
    Base::onMouseEvent(ev);

    if( ! ev.isPress() )
        return true;

    //double buttonX = size().width() - _buttonSize.width();
    //    
    //if( ev.position().x() > buttonX )
    //{
    //    showPopup();
    //}
    //else if(_isEditable)
    //{
    //    std::size_t n = _line.xToCursor( ev.x() );
    //    _editor.setCursorPosition(n);
    //    update();
    //        
    //    Application::instance().inputMethod().begin(*this);
    //}

    return true;
}


void SpinBox::onTouchEvent(const TouchEvent& ev)
{
    Base::onTouchEvent(ev);

    if( ! ev.isPress() )
        return;

    //double buttonX = size().width() - _buttonSize.width();

    //if( ev.position().x() > buttonX )
    //{
    //    showPopup();
    //}
    //else if(_isEditable)
    //{
    //    std::size_t n = _line.xToCursor( ev.x() );
    //    _editor.setCursorPosition(n);
    //    update();

    //    Application::instance().inputMethod().begin(*this);
    //}
}


void SpinBox::onFocusEvent(const FocusEvent& ev)
{
    Base::onFocusEvent(ev);

    if( ! ev.isFocused() )
    {
        if( isAccepted() && _isTextChanged)
        {
            _isTextChanged = false;
            _editingFinished.send( _editor.text() );
        }
    }
    else if(_isEditable)
    {
        Application::instance().inputMethod().begin(*this);
    }
}

} // namespace

} // namespace
