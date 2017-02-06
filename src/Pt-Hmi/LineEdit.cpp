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

#include <Pt/Hmi/LineEdit.h>
#include <Pt/Hmi/Painter.h>
#include <Pt/Gfx/ImagePainter.h>

static const char maskChar = '*';

namespace Pt {

namespace Hmi {

LineEdit::LineEdit()
: _echoMode(Normal)
, _textAlignment(TopLeft)
, _isAccepted(true)
, _isTextChanged(false)
, _cursorPosition(0)
, _hscroll(0)
, _halign(0)
, _hasRenderer(false)
{
    setTextInput(true);
    setFocusPolicy(Widget::NormalFocus);
    setPadding(5);
}


LineEdit::~LineEdit()
{
}


void LineEdit::setText(const Pt::String& t)
{
    _text = t;
    setCursorPosition(0);
}


const Pt::String& LineEdit::text() const
{
    return _text;
}


const Pt::String& LineEdit::displayText() const
{
    if(_echoMode == Normal)
        return _text;

    return _displayText;
}


const Pt::String& LineEdit::placeholderText() const
{
    return _placeholderText;
}


void LineEdit::setPlaceholderText(const Pt::String& s)
{
    _placeholderText = s;
    update();
}


LineEdit::EchoMode LineEdit::echoMode() const
{
    return _echoMode;
}


void LineEdit::setEchoMode(LineEdit::EchoMode mode)
{
    _echoMode = mode;

    _displayText.clear();
    
    if(_echoMode == Masked)
        _displayText.assign(_text.size(), maskChar);

    invalidate();
}


LineEdit::Alignment LineEdit::textAlignment() const
{
    return _textAlignment;
}


void LineEdit::setTextAlignment(Alignment a)
{
    _textAlignment = a;
    
    layoutText();
    update();
}


std::size_t LineEdit::cursorPosition() const
{
    return _cursorPosition;
}


void LineEdit::setCursorPosition(std::size_t n)
{
    if( n > _text.size() )
        n = _text.size();

    _cursorPosition = n;

    layoutText();
    update();
}


bool LineEdit::isAccepted() const
{
    return _isAccepted;
}


void LineEdit::setAccepted(bool a)
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


Pt::Signal<const Pt::String&>& LineEdit::textEdited()
{
    return _textEdited;
}


Pt::Signal<const Pt::String&>& LineEdit::returnPressed()
{
    return _returnPressed;
}


Pt::Signal<const Pt::String&>& LineEdit::editingFinished()
{
    return _editingFinished;
}


const Gfx::Brush& LineEdit::background() const
{
    return _background ? *_background
                       : Application::instance().styleOptions().textBackground();
}


void LineEdit::setBackground(const Gfx::Brush& b)
{
    _background.reset( new Gfx::Brush(b) );
    invalidate();
}


const Gfx::Pen& LineEdit::contour() const
{
    return _contour ? *_contour
                    : Application::instance().styleOptions().contour();
}


void LineEdit::setContour(const Gfx::Pen& p)
{
    _contour.reset( new Gfx::Pen(p) );
    invalidate();
}


const Gfx::Color& LineEdit::textColor() const
{
    return _textColor ? *_textColor
                      : Application::instance().styleOptions().textColor();
}


void LineEdit::setTextColor(const Gfx::Color& color)
{
    _textColor.reset( new Gfx::Color(color) );
    invalidate();
}


const std::string& LineEdit::font() const
{
    return _fontName ? *_fontName
                     : Application::instance().styleOptions().font().name();
}


void LineEdit::setFont(const std::string& fontName)
{
    _fontName.reset( new std::string(fontName) );
    invalidate();
}


std::size_t LineEdit::fontSize() const
{

    return _fontSize ? *_fontSize
                     : Application::instance().styleOptions().font().size();
}


void LineEdit::setFontSize(const std::size_t s)
{
    _fontSize.reset( new std::size_t(s) );
    invalidate();
}


Gfx::Font::Style LineEdit::fontStyle() const
{
    return _fontStyle ? *_fontStyle
                      : Application::instance().styleOptions().font().style();
}


void LineEdit::setFontStyle(Gfx::Font::Style style)
{
    _fontStyle.reset( new Gfx::Font::Style(style) );
    invalidate();
}


void LineEdit::setRenderer(LineEditRenderer* renderer)
{
    _renderer.reset(renderer);
    _hasRenderer = renderer != 0;

    invalidate();
}


void LineEdit::onKeyEvent(const KeyEvent& ev)
{  
    Base::onKeyEvent(ev);

    if( ! ev.isPress() )
        return;

    if( ev.key().code() == Pt::Hmi::Key::ArrowLeft )
    {
        if(_cursorPosition > 0)
            setCursorPosition(_cursorPosition - 1);
    }
    else if( ev.key().code() == Pt::Hmi::Key::ArrowRight )
    {
        setCursorPosition(_cursorPosition + 1);
    }
    else if( ev.key().code() == Pt::Hmi::Key::Return )
    {
        if( isAccepted() )
            _returnPressed.send(_text);
    }
    else if( ev.key().code() == Pt::Hmi::Key::Delete )
    {
        if( ! _text.empty() )
            _text.erase(_cursorPosition, 1);

        if( ! _displayText.empty() )
            _displayText.erase(_cursorPosition, 1);
        
        layoutText();
        update();
        _isTextChanged = true;
        _textEdited.send(_text);
    }
    else if( ev.key().code() == Pt::Hmi::Key::Backspace )
    {
        if( _cursorPosition > 0 && ! _text.empty() )
            _text.erase(_cursorPosition - 1, 1);

        if( ! _displayText.empty() )
            _displayText.resize( _displayText.size() - 1 );
        
        if(_cursorPosition > 0)
            setCursorPosition(_cursorPosition - 1);

        _isTextChanged = true;
        _textEdited.send(_text);
    }
    else
    {
        Pt::Char ch = ev.unicode();
        if( Pt::isprint(ch) )
        {
            _text.insert(_cursorPosition, 1, ch);

            if(_echoMode == Masked)
                _displayText += maskChar;

            setCursorPosition(_cursorPosition + 1);
            _isTextChanged = true;
            _textEdited.send(_text);
        }
    }
}


void LineEdit::onMouseEvent(const MouseEvent& mev)
{
    Base::onMouseEvent(mev);

    if(_echoMode == Hidden)
        return;

    if( ! mev.isPress() )
        return;

    Application::instance().inputMethod().begin(*this);

    std::size_t pos = xToCursor( mev.x() );
    setCursorPosition(pos);
}


void LineEdit::onTouchEvent(const TouchEvent& tev)
{
    Base::onTouchEvent(tev);

    if(_echoMode == Hidden)
        return;

    if( ! tev.isPress() )
        return;

    Application::instance().inputMethod().begin(*this);

    std::size_t pos = xToCursor( tev.x() );
    setCursorPosition(pos);
}


void LineEdit::onResizeEvent(const ResizeEvent& ev)
{
    Base::onResizeEvent(ev);
    
    layoutText();
}


void LineEdit::onFocusEvent(const FocusEvent& ev)
{
    Base::onFocusEvent(ev);

    if( ! ev.isFocused() )
    {
        if( isAccepted() && _isTextChanged)
        {
            _isTextChanged = false;
            _editingFinished.send(_text);
        }
    }
    else
    {
        Application::instance().inputMethod().begin(*this);
    }
}


void LineEdit::onInvalidate()
{
    Base::onInvalidate();

    const StyleOptions& options = Application::instance().styleOptions();
    const Style& style = Application::instance().style();

    _brush = background();
    _pen = contour();
    _placeholderPen = contour();
    _textPen = textColor();
    _font = Gfx::Font(font(), fontSize(), fontStyle());

    if( ! _hasRenderer )
        _renderer.reset( style.get<LineEditRenderer>() );
    
    if( ! _renderer )
        return;

    _renderer->prepare(*this, options, _brush, _pen, _font, _textPen, _placeholderPen);
}


void LineEdit::onPaint(PaintSurface& surface, const Gfx::RectF& rect)
{
    const StyleOptions& options = Application::instance().styleOptions();

    if( ! _renderer)
        return;

    Painter painter(surface);
    painter.setClip(rect);
    
    //
    // text box
    //

    _renderer->renderBackground( *this, options, painter, rect,
                                 _pen, _brush );
    
    if(echoMode() == LineEdit::Hidden)
        return;

    //
    // placeholder or entered text
    //

    const Pt::String& text = displayText().empty() ? _placeholderText
                                                   : displayText();
    
    Gfx::FontMetrics fm = painter.fontMetrics(_font, text);

    double textX = padding().left() + _halign - _hscroll;
    
    double textHeight = fm.ascent() + fm.descent();
    double textY = ((size().height() - textHeight) / 2) + fm.ascent();

    if( ! _text.empty() )
    {
        Gfx::PointF textPos(textX, textY);
        
        _renderer->renderText(*this, options, painter, rect, 
                              text, textPos, _font, _textPen);
    }
    else if( ! hasFocus() && ! text.empty() )
    {
        std::size_t align = fm.width() / 2;

        if(align < textX)
            textX -= align;
  
        Gfx::PointF textPos(textX, textY);

        _renderer->renderText(*this, options, painter, rect, 
                              text, textPos, _font, _placeholderPen);
    }

    //
    // text cursor
    //

    if( hasFocus() )
    {
        double cursorX = textX;
        
        if( ! text.empty() )
        {
            Pt::String left = text.substr(0, _cursorPosition);
            Gfx::FontMetrics fm = painter.fontMetrics(left);
            cursorX += fm.width();
        }

        Gfx::RectF cursorRect( Gfx::PointF(cursorX, textY - fm.ascent()),
                               Gfx::SizeF(0, textHeight) );

        _renderer->renderCursor(*this, options, painter, rect, cursorRect);
    }
}


void LineEdit::layoutText()
{
    const Pt::String& str = displayText();

    if(_echoMode == Hidden)
        return;

    Pt::String left;
    if( _cursorPosition <= str.size() && ! str.empty() ) 
        left = str.substr(0, _cursorPosition);
    
    Gfx::FontMetrics fmLeft = Hmi::Painter::fontMetrics( _font, left );
    Gfx::FontMetrics fmText = Hmi::Painter::fontMetrics( _font, str );
    
    double maxWidth = size().width() - padding().leftRight();

    if( fmText.width() < maxWidth )
    {
        _hscroll = 0;

        switch(_textAlignment)
        {
            case TopLeft:
            case MiddleLeft:
            case BottomLeft:
                _halign = 0;
                break;
            
            case TopCenter:
            case MiddleCenter:
            case BottomCenter:
                _halign = (maxWidth - fmText.width()) / 2;
                break;
            
            case TopRight:           
            case MiddleRight:
            case BottomRight:
                _halign = maxWidth - fmText.width();
                break;
        }
    }
    else
    {
        double pos1 = fmLeft.width() + padding().leftRight();
        if(pos1 > size().width() + _hscroll)
            _hscroll = pos1 - size().width();
        else
        {
            double pos = fmLeft.width();
            if( pos < _hscroll + padding().left() )
            {
                double delta = (_hscroll + padding().left()) - pos;
                if(delta > _hscroll)
                    _hscroll = 0;
                else
                    _hscroll -= delta;
            }
        }

        _halign = 0;

        if( _textAlignment == TopRight ||
            _textAlignment == MiddleRight ||
            _textAlignment == BottomRight )
        {
            _hscroll = fmText.width() - maxWidth;
        }
    }
}


std::size_t LineEdit::xToCursor(double x)
{
    const Pt::String& str = displayText();

    if( str.empty() )
        return 0;

    // compensate for text aligmnet
    x -= _halign;

    std::size_t textX = _hscroll;
    if( padding().left() < x)
        textX += x - padding().left();

    // estimate cursor position
    Gfx::FontMetrics fm = Hmi::Painter::fontMetrics( _font, str );
    std::size_t widthPerChar = fm.width() / str.size();
    std::size_t pos = textX / widthPerChar;

    if( pos >= str.size() )
        pos = str.size() - 1;

    Pt::String left = str.substr(0, pos + 1);
    fm = Hmi::Painter::fontMetrics( _font, left );

    if( textX < fm.width() )
    {
        // cursor position was over estimated, so search left
        for( ; pos > 0; --pos)
        {
            left = str.substr(0, pos);
            fm = Hmi::Painter::fontMetrics( _font, left );
      
            if( textX >= fm.width() )
                break;
        }
    }
    else 
    {
        // cursor position was under estimated, so search right
        for(++pos ; pos < str.size(); ++pos)
        {
            left = str.substr(0, pos + 1);
            fm = Hmi::Painter::fontMetrics( _font, left );
      
            if( textX < fm.width() )
                break;
        }
    }

    return pos;
}

} // namespace

} // namespace
