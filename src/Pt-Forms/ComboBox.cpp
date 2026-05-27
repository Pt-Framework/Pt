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

#include <Pt/Forms/ComboBox.h>
#include <Pt/Forms/Application.h>
#include <Pt/Forms/PaintContext.h>
#include <Pt/Forms/Painter.h>
#include <Pt/Gfx/Painter.h>

namespace Pt {

namespace Forms {

ComboBox::ComboBox()
: _maxHeight(500)
, _isEditable(true)
, _isAccepted(true)
, _isTextChanged(false)
, _isHighlighted(false)
, _isButtonHighlighted(false)
, _customRenderer(false)
, _styleGeneration(0)
, _overrides(0)
, _pendingCursorX(-1)
{
    setFocusPolicy(Control::AcceptFocus);

    _popup.setContent(&_items);

    // process key events in the combo box edit field when popup is open
    _popup.eventReceived() += Pt::slot(*this, &ComboBox::processKeyEvent);
    
    _items.selected() += Pt::slot(*this, &ComboBox::onItemSelected);
}


ComboBox::~ComboBox()
{
}


void ComboBox::addItem(ListBoxItem& item)
{   
    _items.addItem(item);

    //item.setTextInput(true);
}


void ComboBox::removeItem(ListBoxItem& item)
{
    _items.removeItem(item);
}


bool ComboBox::isEditable() const
{
    return _isEditable;
}


void ComboBox::setEditable(bool e)
{
    _isEditable = e;
    repaint( bounds() );
}


bool ComboBox::isAccepted() const
{
    return _isAccepted;
}


void ComboBox::setAccepted(bool a)
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


const Pt::String& ComboBox::text() const
{
    return _editor.text();
}


void ComboBox::setText(const Pt::String& str)
{
    _editor.setText(str);
    invalidate();

    _textChanged.send( _editor.text() );
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
    _items.setScrollBars(hasScrollBars);
}


void ComboBox::setMaxHeight(double height)
{
    _maxHeight = height;
}


//void ComboBox::showPopup()
//{
//    SizePolicy policy(SizePolicy::Fixed, SizePolicy::Preferred);
//    policy.setWidth( size().width() );
//    policy.setHeight(0);
//    _popup.setSizePolicy(policy);
//
//    _popup.setMaximumHeight(_maxHeight);
//
//    Gfx::SizeF popupSize = _popup.measure(policy);
//    _popup.resize(popupSize);
//    
//    Gfx::PointF popupPos(0, size().height() );
//    popupPos = this->toScreen(popupPos);
//    _popup.move(popupPos);
//
//    _popup.setTopMost(true);
//    _popup.show();
//}


void ComboBox::showPopup()
{
    _popup.setMaximumHeight(_maxHeight);

    SizePolicy policy(SizePolicy::Fixed, SizePolicy::Preferred);
    policy.setWidth( size().width() );
    policy.setHeight(0);
    
    _popup.autoSize(policy);

    Gfx::PointF popupPos(0, size().height() );
    popupPos = this->toGlobal(popupPos);
    _popup.move(popupPos);

    _popup.setAbove(true);
    _popup.setAnchor(this);
    _popup.setName("ComboPopup");
    _popup.show();
    
    //Application::instance().setPopup(_popup);
    //setCapture(true);
}


void ComboBox::hidePopup()
{
    //setCapture(false);   
    //Application::instance().releasePopup(_popup);

    _popup.show(false);
}


bool ComboBox::isHighlighted() const
{
    return _isHighlighted;
}


Pt::Signal<const Pt::String&>& ComboBox::textChanged()
{
    return _textChanged;
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
    return _items.selected();
}


const Gfx::Brush& ComboBox::background() const
{
    if( _renderer )
        return _renderer->background();

    return Application::instance().styleOptions().textBackground();
}


void ComboBox::setBackground(const Gfx::Brush& b)
{
    _background.reset( new Gfx::Brush(b) );
    _overrides |= OverrideBackground;

    if( ComboBoxRenderer* renderer = getRenderer() )
        renderer->setBackground(*_background);

    invalidate();
}


const Gfx::Brush& ComboBox::foreground() const
{
    if( _renderer )
        return _renderer->foreground();

    return Application::instance().styleOptions().foreground();
}


void ComboBox::setForeground(const Gfx::Brush& b)
{
    _foreground.reset( new Gfx::Brush(b) );
    _overrides |= OverrideForeground;

    if( ComboBoxRenderer* renderer = getRenderer() )
        renderer->setForeground(*_foreground);

    invalidate();
}


const Gfx::Pen& ComboBox::contour() const
{
    if( _renderer )
        return _renderer->contour();

    return Application::instance().styleOptions().contour();
}


void ComboBox::setContour(const Gfx::Pen& p)
{
    _contour.reset( new Gfx::Pen(p) );
    _overrides |= OverrideContour;

    if( ComboBoxRenderer* renderer = getRenderer() )
        renderer->setContour(*_contour);

    invalidate();
}


const Gfx::Color& ComboBox::textColor() const
{
    if( _renderer )
        return _renderer->textColor();

    return Application::instance().styleOptions().textColor();
}


void ComboBox::setTextColor(const Gfx::Color& color)
{
    _textColor.reset( new Gfx::Color(color) );
    _overrides |= OverrideTextColor;

    if( ComboBoxRenderer* renderer = getRenderer() )
        renderer->setTextColor( Gfx::Pen(*_textColor) );

    invalidate();
}


const Gfx::Font& ComboBox::font() const
{
    if( _renderer )
        return _renderer->font();

    return Application::instance().styleOptions().font();
}


void ComboBox::setFont(const Gfx::Font& font)
{
    _customFont = font;
    _overrides |= OverrideFontAll;

    if( ComboBoxRenderer* renderer = getRenderer() )
        renderer->setFont( getFont() );

    invalidate();
}


Gfx::Font ComboBox::getFont() const
{
    const Gfx::Font& base = Application::instance().styleOptions().font();

    if( ! (_overrides & OverrideFontAny) )
        return base;

    if( _overrides & OverrideFontAll )
        return _customFont;

    std::size_t sz = (_overrides & OverrideFontSize) ? _customFont.size()
                                                        : base.size();
    Gfx::Font::Weight wt = (_overrides & OverrideFontWeight) ? _customFont.weight()
                                                                 : base.weight();
    Gfx::Font::Slant sl = (_overrides & OverrideFontSlant) ? _customFont.slant()
                                                               : base.slant();

    if( base.hasStyleName() )
        return Gfx::Font(base.family(), sz, base.styleName(), wt, sl, base.stretch());

    if( base.category() != Gfx::Font::Category::None )
        return Gfx::Font(base.category(), sz, wt, sl, base.stretch());

    return Gfx::Font(base.family(), sz, wt, sl, base.stretch());
}


void ComboBox::setFontSize(std::size_t size)
{
    _customFont = _customFont.withSize(size);
    _overrides |= OverrideFontSize;

    if( ComboBoxRenderer* renderer = getRenderer() )
        renderer->setFont( getFont() );

    invalidate();
}


void ComboBox::setFontWeight(Gfx::Font::Weight weight)
{
    _customFont = _customFont.withWeight(weight);
    _overrides |= OverrideFontWeight;

    if( ComboBoxRenderer* renderer = getRenderer() )
        renderer->setFont( getFont() );

    invalidate();
}


void ComboBox::setFontSlant(Gfx::Font::Slant slant)
{
    _customFont = _customFont.withSlant(slant);
    _overrides |= OverrideFontSlant;

    if( ComboBoxRenderer* renderer = getRenderer() )
        renderer->setFont( getFont() );

    invalidate();
}


void ComboBox::setRenderer(ComboBoxRenderer* renderer)
{
    _renderer.reset(renderer);
    _customRenderer = renderer != 0;

    if( renderer )
        applyRenderer(renderer);

    invalidate();
}


void ComboBox::onItemSelected(ListBoxItem& item)
{
    //Application::instance().inputMethod().finish();
    
    _editor.setText( item.text() );

    hidePopup();
    
    invalidate();
}


Gfx::SizeF ComboBox::onMeasure(const SizePolicy& policy)
{
    if( ! _renderer )
        return Gfx::SizeF(policy.width() + padding().leftRight(),
                          padding().topBottom());

    Gfx::SizeF contentSize(policy.width(), 0);
    Gfx::SizeF chromeSize = _renderer->measureChrome(surface(), contentSize);

    return Gfx::SizeF( chromeSize.width() + padding().leftRight(),
                       chromeSize.height() + padding().topBottom() );
}


void ComboBox::onLayout(const Gfx::RectF& rect)
{
    Base::onLayout(rect);

    if( ! _renderer )
        return;

    Gfx::RectF contentRect( Gfx::PointF(0, 0), size() );
    _renderer->layoutControl(surface(), contentRect, _entryRect, _buttonRect, _textRect);

    const Painter& painter = _renderer->textPainter( surface() );

    _editor.setPosition( _textRect.topLeft() );
    _editor.setSize( _textRect.size() );
    _editor.layout(painter, _line);

    if(_pendingCursorX >= 0)
    {
        std::size_t n = _line.xToCursor(painter, _pendingCursorX);
        _editor.setCursorPosition(n);
        _pendingCursorX = -1;
    }

    repaint( bounds() );
}


void ComboBox::onInvalidate()
{
    std::size_t gen = Application::instance().styleOptions().generation();
    if( _styleGeneration != gen )
    {
        _styleGeneration = gen;
        if( ! _customRenderer )
            _renderer.reset();
    }

    if( ! _renderer )
    {
        bool hasOverride = (_overrides != 0);
        if( hasOverride )
        {
            if( ComboBoxRenderer* renderer = getRenderer() )
                applyRenderer(renderer);
        }
        else
        {
            _renderer.reset( Application::instance().style().get<ComboBoxRenderer>() );
        }
    }

    if( ! _renderer )
        return;

    Base::onInvalidate();
    relayout();
}


void ComboBox::onPaint(PaintContext& context, const Gfx::RectF& rect)
{
    if( ! _renderer )
        return;

    ComboBoxStyleFlags state = comboBoxStyleFlags();

    ButtonStyleFlags buttonState;
    if( _isButtonHighlighted )
        buttonState.set(StyleFlags::Highlighted);
    if( _popup.isVisible() )
        buttonState.set(ButtonStyleFlags::Pressed);

    // frame: entry + button
    _renderer->renderControl(context, Gfx::RectF(Gfx::PointF(0,0), size()),
                             _entryRect, _buttonRect, state, buttonState);

    // cursor
    Gfx::RectF cursorRect;

    if( _isEditable && hasFocus() )
    {
        const Painter& painter = _renderer->textPainter( surface() );
        double cursorX = _line.cursorToX(painter, _editor.cursorPosition());
        cursorX += _line.position().x();

        double cursorWidth = 1;
        cursorRect.set( Gfx::PointF(cursorX, _line.position().y()),
                        Gfx::SizeF(cursorWidth, _line.maxHeight()) );
    }

    // text
    Gfx::PointF textPos = _line.position();
    textPos.addY( _line.ascent() );

    _renderer->renderText(context, _textRect, _editor.text(),
                          textPos, cursorRect, state);
}


void ComboBox::onResizeEvent(const ResizeEvent& ev)
{
    Base::onResizeEvent(ev);
    
    if( ! _renderer )
        return;

    Gfx::RectF contentRect( Gfx::PointF(0, 0), ev.size() );
    _renderer->layoutControl(surface(), contentRect, _entryRect, _buttonRect, _textRect);

    _editor.setPosition( _textRect.topLeft() );
    _editor.setSize( _textRect.size() );
}


void ComboBox::processKeyEvent(const KeyEvent& ev)
{
    if( ! ev.isPress() || ! _isEditable )
        return;

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
}


bool ComboBox::onKeyEvent(const KeyEvent& ev)
{
    Base::onKeyEvent(ev);

    processKeyEvent(ev);

    return true;
}


void ComboBox::onProcessMouseEvent(const MouseEvent& ev)
{
    //if( _popup.isVisible() )
    //{
    //    const Gfx::PointF& screenPos = ev.position();
    //    Widget* hit = Application::instance().screen().hitTest(screenPos);

    //    if( hit && hit->isDescendantOf(_popup) )
    //    {
    //        _popup.processEvent(ev);
    //        return;
    //    }
    //}

    Base::onProcessMouseEvent(ev);
}


bool ComboBox::onMouseEvent(const MouseEvent& ev)
{    
    Base::onMouseEvent(ev);

    if( ! ev.isPress() )
        return true;

    Gfx::RectF rect( size() );
    if( ! rect.contains(ev.position()) )
    {
        hidePopup();
        return true;
    }

    if( _buttonRect.contains(ev.position()) )
    {
        if( ! _popup.isVisible() )
            showPopup();
        else
            hidePopup();
    }
    else if(_isEditable)
    {
        _pendingCursorX = ev.x();
        relayout();
            
        Application::instance().inputMethod().begin(*this);
    }

    return true;
}


bool ComboBox::onTouchEvent(const TouchEvent& ev)
{    
    Base::onTouchEvent(ev);

    if( ! ev.isPress() )
        return true;

    if( _buttonRect.contains(ev.position()) )
    {
        showPopup();
    }
    else if(_isEditable)
    {
        _pendingCursorX = ev.x();
        relayout();

        Application::instance().inputMethod().begin(*this);
    }

    return true;
}


bool ComboBox::onEnterEvent(const EnterEvent& ev)
{
    Base::onEnterEvent(ev);

    _isHighlighted = true;
    
    invalidate();
    return true;
}


bool ComboBox::onLeaveEvent(const LeaveEvent& ev)
{
    Base::onLeaveEvent(ev);

    _isHighlighted = false;

    invalidate();
    return true;
}


void ComboBox::onFocusEvent(const FocusEvent& ev)
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


ComboBoxRenderer* ComboBox::getRenderer()
{
    if( ! _renderer )
    {
        const Style& style = Application::instance().style();
        ComboBoxRenderer* proto = style.get<ComboBoxRenderer>();
        if( ! proto )
            return 0;

        _renderer.reset( proto->create() );
    }

    return _renderer.get();
}


void ComboBox::applyRenderer(ComboBoxRenderer* renderer)
{
    if( _overrides & OverrideBackground )
        renderer->setBackground(*_background);

    if( _overrides & OverrideForeground )
        renderer->setForeground(*_foreground);

    if( _overrides & OverrideContour )
        renderer->setContour(*_contour);

    if( _overrides & OverrideTextColor )
        renderer->setTextColor( Gfx::Pen(*_textColor) );

    if( _overrides & OverrideFontAny )
        renderer->setFont( getFont() );
}


ComboBoxStyleFlags ComboBox::comboBoxStyleFlags() const
{
    StyleFlags common;

    if( isEnabled() )
        common.set(StyleFlags::Enabled);
    else
        common.set(StyleFlags::Disabled);

    if( hasFocus() )
        common.set(StyleFlags::Focused);

    if( _isHighlighted )
        common.set(StyleFlags::Highlighted);

    ComboBoxStyleFlags state(common);

    if( _isEditable )
        state.set(ComboBoxStyleFlags::Editable);

    if( _popup.isVisible() )
        state.set(ComboBoxStyleFlags::PopupVisible);

    return state;
}

} // namespace

} // namespace


