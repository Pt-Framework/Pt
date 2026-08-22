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
#include <Pt/Forms/Style.h>
#include <Pt/Forms/StyleOptions.h>
#include <Pt/Forms/PaintContext.h>
#include <Pt/Forms/Painter.h>
#include <Pt/Gfx/Painter.h>

namespace Pt {

namespace Forms {

LineEdit::LineEdit()
: _isEditable(true)
, _isAccepted(true)
, _isTextChanged(false)
, _isHighlighted(false)
, _pendingCursorX(-1)
, _echoMode(Normal)
, _spacing(0)
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
    if( const BackgroundOption* background = _lineEditOptions.get<BackgroundOption>() )
        return background->value();

    const StyleOptions& options = Application::instance().styleOptions();
    return options.get<TextBackgroundOption>()->value();
}


void LineEdit::setBackground(const Gfx::Brush& b)
{
    BackgroundOption background(b);
    _lineEditOptions.set(background);
    invalidate();
}


const Gfx::Pen& LineEdit::contour() const
{
    if( const ContourOption* contour = _lineEditOptions.get<ContourOption>() )
        return contour->value();

    const StyleOptions& options = Application::instance().styleOptions();
    return options.get<ContourOption>()->value();
}


void LineEdit::setContour(const Gfx::Pen& p)
{
    ContourOption contour(p);
    _lineEditOptions.set(contour);
    invalidate();
}


const Gfx::Color& LineEdit::textColor() const
{
    if( const TextColorOption* textColor = _lineEditOptions.get<TextColorOption>() )
        return textColor->value();

    const StyleOptions& options = Application::instance().styleOptions();
    return options.get<TextColorOption>()->value();
}


void LineEdit::setTextColor(const Gfx::Color& color)
{
    TextColorOption textColor(color);
    _lineEditOptions.set(textColor);
    invalidate();
}


Gfx::Font LineEdit::font() const
{
    const StyleOptions& options = Application::instance().styleOptions();
    const Gfx::Font& baseFont = options.get<FontOption>()->value();
    const FontOption* localFont = _lineEditOptions.get<FontOption>();
    return localFont ? localFont->getFont(baseFont) : baseFont;
}


void LineEdit::setFont(const Gfx::Font& font)
{
    FontOption fontOption;
    fontOption.setFont(font);
    _lineEditOptions.set(fontOption);
    invalidate();
}


void LineEdit::setFontSize(std::size_t size)
{
    const FontOption* localFont = _lineEditOptions.get<FontOption>();
    FontOption font = localFont ? *localFont : FontOption();
    font.setSize(size);
    _lineEditOptions.set(font);
    invalidate();
}


void LineEdit::setFontWeight(Gfx::Font::Weight weight)
{
    const FontOption* localFont = _lineEditOptions.get<FontOption>();
    FontOption font = localFont ? *localFont : FontOption();
    font.setWeight(weight);
    _lineEditOptions.set(font);
    invalidate();
}


void LineEdit::setFontSlant(Gfx::Font::Slant slant)
{
    const FontOption* localFont = _lineEditOptions.get<FontOption>();
    FontOption font = localFont ? *localFont : FontOption();
    font.setSlant(slant);
    _lineEditOptions.set(font);
    invalidate();
}


void LineEdit::setRenderer(LineEditRenderer* renderer)
{
    const StyleOptions& options = Application::instance().styleOptions();

    if(renderer)
        _lineEditStyle.bind(*renderer, options, _lineEditOptions);
    else
        _lineEditStyle.bind(Application::instance().style(), options, _lineEditOptions);

    invalidate();
}


LineEditState LineEdit::lineEditState() const
{
    LineEditState state;

    state.setEnabled( isEnabled() );
    state.setHighlighted( _isHighlighted );
    state.setFocused( hasFocus() );
    state.setEditable( _isEditable );

    return state;
}


void LineEdit::onInvalidate()
{
    Base::onInvalidate();

    const StyleOptions& options = Application::instance().styleOptions();
    _lineEditStyle.rebind(Application::instance().style(), options, _lineEditOptions);
}


Gfx::SizeF LineEdit::onMeasure(const SizePolicy& policy)
{
    LineEditRenderer* renderer = _lineEditStyle.renderer();
    if( ! renderer )
        return Gfx::SizeF(0, 0);

    Gfx::SizeF contentSize(policy.width(), 0);
    Gfx::SizeF sz = renderer->measureFrame(surface(), contentSize);

    return Gfx::SizeF( sz.width() + padding().leftRight(),
                       sz.height() + padding().topBottom() );
}


void LineEdit::onLayout(const Gfx::RectF& rect)
{
    Base::onLayout(rect);

    LineEditRenderer* renderer = _lineEditStyle.renderer();
    if( ! renderer )
        return;

    _textRect = renderer->layoutFrame( surface(), Gfx::RectF(size()) );

    _editor.setPosition( _textRect.topLeft() );
    _editor.setSize( _textRect.size() );

    const Painter& painter = renderer->textPainter(surface());

    if( _editor.isEmpty() && ! hasFocus() )
        _editor.layout(painter, _placeholderText, _line);
    else
        _editor.layout(painter, _line);

    if(_pendingCursorX >= 0)
    {
        std::size_t n = _line.xToCursor(painter, _pendingCursorX);
        _editor.setCursorPosition(n);
        _pendingCursorX = -1;
    }

    if( _isEditable && hasFocus() )
    {
        double cursorX = _line.cursorToX(painter, _editor.cursorPosition());
        cursorX += _line.position().x();

        Gfx::PointF top(cursorX, _line.position().y());
        Gfx::PointF bottom(cursorX, _line.position().y() + _line.maxHeight());
        _cursorRect = Gfx::RectF(top, bottom);
    }
    else
    {
        _cursorRect = Gfx::RectF();
    }

    repaint( bounds() );
}


void LineEdit::onPaint(PaintContext& context, const Gfx::RectF& rect)
{
    LineEditRenderer* renderer = _lineEditStyle.renderer();
    if( ! renderer )
        return;

    LineEditState state = lineEditState();

    bool placeholder = _editor.isEmpty() && ! hasFocus();
    if(placeholder)
        state.setPlaceholder(true);

    const String& text = placeholder ? _placeholderText
                                     : _editor.displayText();

    Gfx::PointF textPos = _line.position();
    textPos.addY( _line.ascent() );

    Gfx::RectF selection;

    onPaintChrome(context, Gfx::RectF(size()), _textRect,
                  text, textPos, _cursorRect, selection, state);
}


void LineEdit::onPaintChrome(PaintContext& context,
                             const Gfx::RectF& rect,
                             const Gfx::RectF& textRect,
                             const String& text,
                             const Gfx::PointF& textPos,
                             const Gfx::RectF& cursor,
                             const Gfx::RectF& selection,
                             const LineEditState& state)
{
    LineEditRenderer* renderer = _lineEditStyle.renderer();
    if( ! renderer )
        return;

    renderer->renderChrome(context, rect, textRect,
                           text, textPos, cursor, selection, state);
}


void LineEdit::onResizeEvent(const ResizeEvent& ev)
{
    Base::onResizeEvent(ev);

    _spacing = ev.size().height() / 5;
    if(_spacing < 2)
        _spacing = 2;
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
        _pendingCursorX = mev.x();
        relayout();

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
        _pendingCursorX = tev.x();
        relayout();

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
