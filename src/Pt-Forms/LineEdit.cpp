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
, _customRenderer(false)
, _fontOverride(0)
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
    if( _renderer )
        return _renderer->background();

    return Application::instance().styleOptions().textBackground();
}


void LineEdit::setBackground(const Gfx::Brush& b)
{
    _background.reset( new Gfx::Brush(b) );

    if( LineEditRenderer* r = getRenderer() )
        r->setBackground(*_background);

    invalidate();
}


const Gfx::Pen& LineEdit::contour() const
{
    if( _renderer )
        return _renderer->contour();

    return Application::instance().styleOptions().contour();
}


void LineEdit::setContour(const Gfx::Pen& p)
{
    _contour.reset( new Gfx::Pen(p) );

    if( LineEditRenderer* r = getRenderer() )
        r->setContour(*_contour);

    invalidate();
}


const Gfx::Color& LineEdit::textColor() const
{
    if( _renderer )
        return _renderer->textColor();

    return Application::instance().styleOptions().textColor();
}


void LineEdit::setTextColor(const Gfx::Color& color)
{
    _textColor.reset( new Gfx::Color(color) );

    if( LineEditRenderer* r = getRenderer() )
        r->setTextColor( Gfx::Pen(*_textColor) );

    invalidate();
}


const Gfx::Font& LineEdit::font() const
{
    if( _renderer )
        return _renderer->font();

    return Application::instance().styleOptions().font();
}


void LineEdit::setFont(const Gfx::Font& font)
{
    _customFont = font;
    _fontOverride = OverrideAll;

    if( LineEditRenderer* r = getRenderer() )
        r->setFont( getFont() );

    invalidate();
}


Gfx::Font LineEdit::getFont() const
{
    const Gfx::Font& base = Application::instance().styleOptions().font();

    if( _fontOverride == 0 )
        return base;

    if( _fontOverride == OverrideAll )
        return _customFont;

    std::size_t sz = (_fontOverride & OverrideSize) ? _customFont.size()
                                                    : base.size();
    Gfx::Font::Weight wt = (_fontOverride & OverrideWeight) ? _customFont.weight()
                                                            : base.weight();
    Gfx::Font::Slant sl = (_fontOverride & OverrideSlant) ? _customFont.slant()
                                                          : base.slant();

    if( base.hasStyleName() )
        return Gfx::Font(base.family(), sz, base.styleName(), wt, sl, base.stretch());

    if( base.category() != Gfx::Font::Category::None )
        return Gfx::Font(base.category(), sz, wt, sl, base.stretch());

    return Gfx::Font(base.family(), sz, wt, sl, base.stretch());
}


void LineEdit::setFontSize(std::size_t size)
{
    _customFont = _customFont.withSize(size);
    _fontOverride |= OverrideSize;

    if( LineEditRenderer* r = getRenderer() )
        r->setFont( getFont() );

    invalidate();
}


void LineEdit::setFontWeight(Gfx::Font::Weight weight)
{
    _customFont = _customFont.withWeight(weight);
    _fontOverride |= OverrideWeight;

    if( LineEditRenderer* r = getRenderer() )
        r->setFont( getFont() );

    invalidate();
}


void LineEdit::setFontSlant(Gfx::Font::Slant slant)
{
    _customFont = _customFont.withSlant(slant);
    _fontOverride |= OverrideSlant;

    if( LineEditRenderer* r = getRenderer() )
        r->setFont( getFont() );

    invalidate();
}


void LineEdit::setRenderer(LineEditRenderer* renderer)
{
    _renderer.reset(renderer);
    _customRenderer = renderer != 0;

    if( renderer )
        applyRenderer(renderer);

    invalidate();
    repaint();
    relayout();
}


LineEditStyleFlags LineEdit::lineEditStyleFlags() const
{
    LineEditStyleFlags state;

    if( isEnabled() )
        state.set(StyleFlags::Enabled);

    if( _isHighlighted )
        state.set(StyleFlags::Highlighted);

    if( hasFocus() )
        state.set(StyleFlags::Focused);

    if( _isEditable )
        state.set(LineEditStyleFlags::Editable);

    return state;
}


Gfx::SizeF LineEdit::onMeasure(const SizePolicy& policy)
{
    if( ! _renderer )
    {
        const Style& style = Application::instance().style();
        _renderer.reset( style.get<LineEditRenderer>() );
    }

    if( ! _renderer )
        return Gfx::SizeF(0, 0);

    Gfx::SizeF contentSize(policy.width(), 0);
    Gfx::SizeF sz = _renderer->measureFrame(surface(), contentSize);

    return Gfx::SizeF( sz.width() + padding().leftRight(), 
                       sz.height() + padding().topBottom() );
}


void LineEdit::onLayout(const Gfx::RectF& rect)
{
    Base::onLayout(rect);

    if( ! _renderer )
        return;

    _textRect = _renderer->layoutFrame( surface(), Gfx::RectF(size()) );

    _editor.setPosition( _textRect.topLeft() );
    _editor.setSize( _textRect.size() );

    const Painter& painter = _renderer->textPainter(surface());

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

    repaint( bounds() );
}


LineEditRenderer* LineEdit::getRenderer()
{
    if( ! _customRenderer )
    {
        const Style& style = Application::instance().style();
        LineEditRenderer* proto = style.get<LineEditRenderer>();
        if( ! proto )
            return 0;

        _renderer.reset( proto->create() );
        _customRenderer = true;
    }

    return _renderer.get();
}


void LineEdit::applyRenderer(LineEditRenderer* renderer)
{
    if( _background )
        renderer->setBackground(*_background);

    if( _contour )
        renderer->setContour(*_contour);

    if( _textColor )
        renderer->setTextColor( Gfx::Pen(*_textColor) );

    if( _fontOverride )
        renderer->setFont( getFont() );
}


void LineEdit::onInvalidate()
{
    Base::onInvalidate();

    if( ! _renderer )
    {
        bool hasOverride = _background || _contour ||
                           _textColor || _fontOverride;
        if(hasOverride)
        {
            if( LineEditRenderer* renderer = getRenderer() )
                applyRenderer(renderer);
        }
        else
        {
            const Style& style = Application::instance().style();
            _renderer.reset( style.get<LineEditRenderer>() );
        }
    }
}


void LineEdit::onPaint(PaintContext& context, const Gfx::RectF& rect)
{
    if( ! _renderer )
        return;

    LineEditStyleFlags state = lineEditStyleFlags();

    String text;

    if( _editor.isEmpty() && ! hasFocus() )
    {
        state.set(LineEditStyleFlags::Placeholder);
        text = _placeholderText;
    }
    else
    {
        text = _editor.displayText();
    }

    Gfx::PointF textPos = _line.position();
    textPos.addY( _line.ascent() );

    Gfx::RectF cursor;

    if( _isEditable && hasFocus() )
    {
        const Painter& painter = _renderer->textPainter(surface());
        double cursorX = _line.cursorToX(painter, _editor.cursorPosition());
        cursorX += _line.position().x();

        Gfx::PointF top(cursorX, _line.position().y());
        Gfx::PointF bottom(cursorX, _line.position().y() + _line.maxHeight());
        cursor = Gfx::RectF(top, bottom);
    }

    Gfx::RectF selection;

    _renderer->renderFrame(context, Gfx::RectF(size()), _textRect,
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
