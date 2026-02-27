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

#include <Pt/Forms/LineEdit.h>
#include <Pt/Forms/Application.h>
#include <Pt/Gfx/Painter.h>

namespace Pt {

namespace Forms {

LineEdit::LineEdit()
: _isEditable(true)
, _isAccepted(true)
, _isTextChanged(false)
, _isHighlighted(false)
, _echoMode(Normal)
, _spacing(0)
, _hasRenderer(false)
{
    setFocusPolicy(Control::AcceptFocus);
}


LineEdit::~LineEdit()
{
}


bool LineEdit::isEditable() const
{
    return _isEditable;
}


void LineEdit::setEditable(bool e)
{
    _isEditable = e;
    setFocusPolicy(_isEditable ? Control::AcceptFocus : Control::NoFocus);
    repaint();
}


const Pt::String& LineEdit::text() const
{
    return _editor.text();
}


void LineEdit::setText(const Pt::String& str)
{
    _editor.setText(str);
    invalidate();

    _textEdited.send( _editor.text() );
}


bool LineEdit::isEmpty() const
{
    return _editor.isEmpty();
}


const Pt::String& LineEdit::displayText() const
{   
    return _editor.displayText();
}


const Pt::String& LineEdit::placeholderText() const
{
    return _placeholderText;
}


void LineEdit::setPlaceholderText(const Pt::String& s)
{
    _placeholderText = s;
    invalidate();
}


LineEdit::EchoMode LineEdit::echoMode() const
{
    return _echoMode;
}


void LineEdit::setEchoMode(LineEdit::EchoMode mode)
{
    _echoMode = mode;   
    _editor.setMasked(_echoMode == Masked);

    invalidate();
}


Adjustment LineEdit::textAdjustment() const
{
    return _editor.adjustment();
}


void LineEdit::setTextAdjustment(Adjustment a)
{
    _editor.setAdjustment(a);
    invalidate();
}


std::size_t LineEdit::cursorPosition() const
{
    return _editor.cursorPosition();
}


void LineEdit::setCursorPosition(std::size_t n)
{
    _editor.setCursorPosition(n);
    invalidate();
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
        setFocusPolicy(Control::KeepFocus);
    }
    else
    {
        setFocusPolicy(Control::AcceptFocus);
    }
}


bool LineEdit::isHighlighted() const
{
    return _isHighlighted;
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


const Gfx::ColorF& LineEdit::textColor() const
{
    return _textColor ? *_textColor
                      : Application::instance().styleOptions().textColor();
}


void LineEdit::setTextColor(const Gfx::ColorF& color)
{
    _textColor.reset( new Gfx::ColorF(color) );
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


const std::string& LineEdit::fontStyle() const
{
    return _fontStyle ? *_fontStyle
                      : Application::instance().styleOptions().font().style();
}


void LineEdit::setFontStyle(const std::string& style)
{
    _fontStyle.reset( new std::string(style) );
    invalidate();
}


void LineEdit::setRenderer(LineEditRenderer* renderer)
{
    _renderer.reset(renderer);
    _hasRenderer = renderer != 0;

    invalidate();
}


Gfx::SizeF LineEdit::onMeasure(const SizePolicy& policy)
{
    double itemsWidth = policy.width();
    double itemsHeight = _font.size() * 2.5;

    return Gfx::SizeF( itemsWidth + padding().leftRight(), 
                       itemsHeight + padding().topBottom() );
}


void LineEdit::onInvalidate()
{
    Base::onInvalidate();

    const StyleOptions& options = Application::instance().styleOptions();
    const Style& style = Application::instance().style();

    _brush = background();
    _pen = contour();
    _textPen = textColor();
    _font = Gfx::Font(font(), fontSize(), fontStyle());

    if( ! _hasRenderer )
        _renderer.reset( style.get<LineEditRenderer>() );
    
    if( ! _renderer )
        return;

    _renderer->prepare(*this, options, _brush, _pen, _font, _textPen);

    Gfx::Painter _painter( surface() );
    _painter.setFont(_font);

    if( _editor.isEmpty() && ! hasFocus() )
        _editor.layout(_painter, _placeholderText, _line);
    else
        _editor.layout(_painter, _line);
}


void LineEdit::onPaint(PaintSurface& surface, const Gfx::RectF& rect)
{
    const StyleOptions& options = Application::instance().styleOptions();

    if( ! _renderer)
        return;

    Gfx::Painter painter(surface);
    painter.setClip(rect);

    //
    // text box
    //

    _renderer->renderBackground( *this, options, painter, rect,
                                 _pen, _brush );

    //
    // text with cursor
    //
    
    Gfx::RectF cursorRect;

    Gfx::PointF clipPos = _editor.position();
    Gfx::SizeF clipSize = _editor.size();
    clipSize.addWidth(_spacing); // cursor

    if( _isEditable && hasFocus() )
    {
        painter.setFont(_font);
        double cursorX = _line.cursorToX( painter, _editor.cursorPosition() );
        cursorX += _line.position().x();
        
        double cursorWidth = 1;

        cursorRect.set(Gfx::PointF( cursorX, _line.position().y() ),
                       Gfx::SizeF( cursorWidth, _line.maxHeight() ) );
    }

    Gfx::RectF clipRect(clipPos, clipSize);
    painter.setClip( Gfx::RectF(clipPos, clipSize) );

    Gfx::PointF textPos = _line.position();
    textPos.addY( _line.ascent() );

    // TODO: renderer prepare can set placeholder color

    if( _editor.isEmpty() && ! hasFocus() )
    {
        _renderer->renderText(*this, options, painter, rect, 
                              _placeholderText, textPos, _font, _textPen);
    }
    else
    {
        _renderer->renderText(*this, options, painter, rect, 
                              _editor.displayText(), textPos, _font, _textPen);

        _renderer->renderCursor(*this, options, painter, rect, cursorRect);
    }
}


void LineEdit::onResizeEvent(const ResizeEvent& ev)
{
    Base::onResizeEvent(ev);
    
    _spacing = ev.size().height() / 5;
    if(_spacing < 2)
        _spacing = 2;

    Gfx::PointF editPosition(_spacing, 0);
    _editor.setPosition(editPosition);

    Gfx::SizeF editSize = ev.size();
    editSize.addWidth(-3 * _spacing);
    _editor.setSize(editSize);

    Gfx::Painter _painter( surface() );
    _painter.setFont(_font);

    if( _editor.isEmpty() && ! hasFocus() )
        _editor.layout(_painter, _placeholderText, _line);
    else
        _editor.layout(_painter, _line);
}


bool LineEdit::onKeyEvent(const KeyEvent& ev)
{  
    Base::onKeyEvent(ev);

    if( ! ev.isPress() || ! _isEditable )
        return true;

    if( ev.key().code() == Pt::Forms::Key::ArrowLeft )
    {
        _editor.left();
        invalidate();
    }
    else if( ev.key().code() == Pt::Forms::Key::ArrowRight )
    {
        _editor.right();
        invalidate();
    }
    else if( ev.key().code() == Pt::Forms::Key::Return )
    {
        invalidate();
        
        if( isAccepted() )
            _returnPressed.send( _editor.text() );
    }
    else if( ev.key().code() == Pt::Forms::Key::Delete )
    {
        _isTextChanged = true;
        _editor.del();
        invalidate();

        _textEdited.send( _editor.text() );
    }
    else if( ev.key().code() == Pt::Forms::Key::Backspace )
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

    return true;
}


bool LineEdit::onMouseEvent(const MouseEvent& mev)
{
    Base::onMouseEvent(mev);

    if(_echoMode == Hidden)
        return true;

    if( ! mev.isPress() )
        return true;

    if(_isEditable)
    {
        Gfx::Painter _painter( surface() );
        _painter.setFont(_font);

        std::size_t n = _line.xToCursor( _painter, mev.x() );
        _editor.setCursorPosition(n);
        repaint();

        Application::instance().inputMethod().begin(*this);
    }
    
    return true;
}


bool LineEdit::onTouchEvent(const TouchEvent& tev)
{
    Base::onTouchEvent(tev);

    if(_echoMode == Hidden)
        return true;

    if( ! tev.isPress() )
        return true;

    if(_isEditable)
    {
        Gfx::Painter _painter( surface() );
        _painter.setFont(_font);

        std::size_t n = _line.xToCursor( _painter, tev.x() );
        _editor.setCursorPosition(n);
        repaint();

        Application::instance().inputMethod().begin(*this);
    }
    return true;
}


bool LineEdit::onEnterEvent(const EnterEvent& ev)
{
    Base::onEnterEvent(ev);

    _isHighlighted = true;
    
    invalidate();
    return true;
}


bool LineEdit::onLeaveEvent(const LeaveEvent& ev)
{
    Base::onLeaveEvent(ev);

    _isHighlighted = false;

    invalidate();
    return true;
}


void LineEdit::onFocusEvent(const FocusEvent& ev)
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
