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
}


void ComboBox::hidePopup()
{
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
    return _styler.background();
}


void ComboBox::setBackground(const Gfx::Brush& b)
{
    _styler.setBackground(b);
    invalidate();
}


const Gfx::Brush& ComboBox::foreground() const
{
    return _styler.foreground();
}


void ComboBox::setForeground(const Gfx::Brush& b)
{
    _styler.setForeground(b);
    invalidate();
}


const Gfx::Pen& ComboBox::contour() const
{
    return _styler.contour();
}


void ComboBox::setContour(const Gfx::Pen& p)
{
    _styler.setContour(p);
    invalidate();
}


const Gfx::Color& ComboBox::textColor() const
{
    return _styler.textColor();
}


void ComboBox::setTextColor(const Gfx::Color& color)
{
    _styler.setTextColor(color);
    invalidate();
}


Gfx::Font ComboBox::font() const
{
    return _styler.font();
}


void ComboBox::setFont(const Gfx::Font& font)
{
    _styler.setFont(font);
    invalidate();
}


void ComboBox::setFontSize(std::size_t size)
{
    _styler.setFontSize(size);
    invalidate();
}


void ComboBox::setFontWeight(Gfx::Font::Weight weight)
{
    _styler.setFontWeight(weight);
    invalidate();
}


void ComboBox::setFontSlant(Gfx::Font::Slant slant)
{
    _styler.setFontSlant(slant);
    invalidate();
}


void ComboBox::setRenderer(ComboBoxRenderer* renderer)
{
    _styler.setRenderer(renderer);
    _styler.bind(Application::instance().style(),
                         Application::instance().styleOptions());

    _entryRect.clear();
    _buttonRect.clear();
    _textRect.clear();
    _cursorRect.clear();
    invalidate();
}


void ComboBox::onItemSelected(ListBoxItem& item)
{
    //Application::instance().inputMethod().finish();

    _editor.setText( item.text() );

    hidePopup();

    invalidate();
}


ComboBoxState ComboBox::comboBoxState() const
{
    ComboBoxState s;
    s.setEnabled( isEnabled() );
    s.setFocused( hasFocus() );
    s.setHighlighted( _isHighlighted );
    s.setEditable( _isEditable );
    s.setPopupVisible( _popup.isVisible() );
    return s;
}


void ComboBox::onInvalidate()
{
    Base::onInvalidate();

    const StyleOptions& options = Application::instance().styleOptions();
    const Style& style = Application::instance().style();

    if( _styler.bind(style, options) )
    {
        _entryRect.clear();
        _buttonRect.clear();
        _textRect.clear();
        _cursorRect.clear();
    }

    relayout();
}


Gfx::SizeF ComboBox::onMeasure(const SizePolicy& policy)
{
    Gfx::SizeF contentSize(policy.width(), 0);
    Gfx::SizeF chromeSize = _styler.measureFrame(surface(), contentSize);

    return Gfx::SizeF( chromeSize.width() + padding().leftRight(),
                       chromeSize.height() + padding().topBottom() );
}


void ComboBox::onLayout(const Gfx::RectF& rect)
{
    Base::onLayout(rect);

    Gfx::RectF contentRect( Gfx::PointF(0, 0), size() );
    _styler.layoutChrome(surface(), contentRect, _entryRect,
                         _buttonRect, _textRect);

    const Painter* painter = _styler.textPainter( surface() );

    _editor.setPosition( _textRect.topLeft() );
    _editor.setSize( _textRect.size() );

    _cursorRect.clear();

    if( painter )
    {
        _editor.layout(*painter, _line);

        if(_pendingCursorX >= 0)
        {
            std::size_t n = _line.xToCursor(*painter, _pendingCursorX);
            _editor.setCursorPosition(n);
            _pendingCursorX = -1;
        }

        if( _isEditable && hasFocus() )
        {
            double cursorX = _line.cursorToX(*painter, _editor.cursorPosition());
            cursorX += _line.position().x();

            _cursorRect.set( Gfx::PointF(cursorX, _line.position().y()),
                             Gfx::SizeF(1, _line.maxHeight()) );
        }
    }

    repaint( bounds() );
}


void ComboBox::onPaint(PaintContext& context, const Gfx::RectF& rect)
{
    ComboBoxState state = comboBoxState();

    ComboBoxButtonState buttonState;
    buttonState.setHighlighted( _isButtonHighlighted );
    buttonState.setPressed( _popup.isVisible() );

    Gfx::RectF widgetRect( Gfx::PointF(0, 0), size() );

    onPaintChrome(context, widgetRect, _entryRect, _buttonRect,
                  state, buttonState);

    Gfx::PointF textPos = _line.position();
    textPos.addY( _line.ascent() );

    onPaintText(context, _textRect, _editor.text(),
                textPos, _cursorRect, state);
}


void ComboBox::onPaintChrome(PaintContext& context,
                             const Gfx::RectF& rect,
                             const Gfx::RectF& entryRect,
                             const Gfx::RectF& buttonRect,
                             const ComboBoxState& state,
                             const ComboBoxButtonState& buttonState)
{
    _styler.renderChrome(context, rect, entryRect, buttonRect,
                                 state, buttonState);
}


void ComboBox::onPaintText(PaintContext& context,
                           const Gfx::RectF& textRect,
                           const String& text,
                           const Gfx::PointF& textPos,
                           const Gfx::RectF& cursor,
                           const ComboBoxState& state)
{
    _styler.renderText(context, textRect, text, textPos, cursor, state);
}


void ComboBox::onResizeEvent(const ResizeEvent& ev)
{
    Base::onResizeEvent(ev);

    Gfx::RectF contentRect( Gfx::PointF(0, 0), ev.size() );
    _styler.layoutChrome(surface(), contentRect, _entryRect,
                                 _buttonRect, _textRect);

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

} // namespace

} // namespace
