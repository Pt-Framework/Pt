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

#include <Pt/Forms/ListBox.h>
#include <Pt/Forms/Application.h>
#include <Pt/Forms/PaintContext.h>
#include <Pt/Forms/Painter.h>
#include <Pt/Forms/StyleOptions.h>

namespace Pt {

namespace Forms {

/////////////////////////////////////////////////////////////////////////////
// ListBoxItem
/////////////////////////////////////////////////////////////////////////////

ListBoxItem::ListBoxItem()
: _onClickBegin(false)
, _isHovered(false)
, _isSelectable(false)
, _isSelected(false)
{
    setFocusPolicy(Control::AcceptFocus);
    setPadding(8);
}


ListBoxItem::~ListBoxItem()
{
}


bool ListBoxItem::isSelectable() const
{
    return _isSelectable;
}


void ListBoxItem::setSelectable(bool b)
{
    _isSelectable = b;

    if(_isSelected && ! _isSelectable)
        setSelected(false);
    else
        invalidate();
}


bool ListBoxItem::isSelected() const
{
    return _isSelected;
}


void ListBoxItem::setSelected(bool b)
{
    if(_isSelected == b)
        return;

    if( ! _isSelectable )
    {
        _selected.send(*this);
        return;
    }

    _isSelected = b;
    invalidate();

    _selected.send(*this);
}


const Pt::String& ListBoxItem::text() const
{
    return _text;
}


void ListBoxItem::setText(const Pt::String& text)
{
    _text = text;
    invalidate();
}


void ListBoxItem::setIcon(const Icon& icon, const Gfx::SizeF& iconSize)
{
    _icon = icon;
    _iconSize = iconSize;
    invalidate();
}


bool ListBoxItem::isHovered() const
{
    return _isHovered;
}


void ListBoxItem::click()
{
    onPressed();
    onReleased();
}


Signal<>& ListBoxItem::clicked()
{
    return _clicked;
}


Pt::Signal<ListBoxItem&>& ListBoxItem::selected()
{
    return _selected;
}


void ListBoxItem::onActionKey(const KeyEvent& kev)
{
    Base::onActionKey(kev);

    if( kev.isPress() )
        onPressed();
    else if( kev.isRelease() )
        onReleased();
}


void ListBoxItem::onShortcut(const Key& key)
{
    Base::onShortcut(key);

    onPressed();
    onReleased();
}


void ListBoxItem::onMnemonic(Pt::Char m)
{
    Base::onMnemonic(m);

    onPressed();
    onReleased();
}


void ListBoxItem::onPressed()
{
}


void ListBoxItem::onReleased()
{
    setSelected( ! _isSelected );
}


void ListBoxItem::onCanceled()
{
}


bool ListBoxItem::onEnterEvent(const EnterEvent& ev)
{
    Base::onEnterEvent(ev);

    _isHovered = true;
    invalidate();
    return true;
}


bool ListBoxItem::onLeaveEvent(const LeaveEvent& ev)
{
    Base::onLeaveEvent(ev);

    _isHovered = false;
    invalidate();
    return true;
}


bool ListBoxItem::onMouseEvent(const MouseEvent& ev)
{
    Base::onMouseEvent(ev);

    if( ev.isPress() )
    {
        _onClickBegin = true;
        onPressed();
    }
    else if( ev.isRelease() )
    {
        const Gfx::PointF& pos = ev.position();

        bool inside = pos.x() >= 0 && pos.x() <= size().width()
                   && pos.y() >= 0 && pos.y() <= size().height();

        bool isClick = _onClickBegin && inside;

        if(_onClickBegin)
            _onClickBegin = false;

        if(isClick)
            onReleased();
        else
            onCanceled();
    }

    return true;
}


bool ListBoxItem::onTouchEvent(const TouchEvent& ev)
{
    Base::onTouchEvent(ev);

    if( ev.isPress() )
    {
        _onClickBegin = true;
        onPressed();
    }
    else if( ev.isRelease() )
    {
        const Gfx::PointF& pos = ev.position();

        bool inside = pos.x() > 0 && pos.x() <= size().width()
                   && pos.y() > 0 && pos.y() <= size().height();

        bool isClick = _onClickBegin && inside;

        if(_onClickBegin)
            _onClickBegin = false;

        if(isClick)
            onReleased();
        else
            onCanceled();
    }

    return true;
}


bool ListBoxItem::onScrollEvent(const ScrollEvent& ev)
{
    if(_onClickBegin)
    {
        _onClickBegin = false;
        onCanceled();
    }

    return Base::onScrollEvent(ev);
}


void ListBoxItem::setBackground(const Gfx::Brush& b)
{
    _listItemOptions.setBackground(b);
    invalidate();
}


const Gfx::Color& ListBoxItem::textColor() const
{
    if( const Gfx::Color* textColor = _listItemOptions.textColor() )
        return *textColor;

    return Application::instance().styleOptions().textColor();
}


void ListBoxItem::setTextColor(const Gfx::Color& color)
{
    _listItemOptions.setTextColor(color);
    invalidate();
}


Gfx::Font ListBoxItem::font() const
{
    const StyleOptions& options = Application::instance().styleOptions();
    return _listItemOptions.getFont(options.font());
}


void ListBoxItem::setFont(const Gfx::Font& f)
{
    _listItemOptions.setFont(f);
    invalidate();
}


void ListBoxItem::setFontSize(std::size_t size)
{
    _listItemOptions.setFontSize(size);
    invalidate();
}


void ListBoxItem::setFontWeight(Gfx::Font::Weight weight)
{
    _listItemOptions.setFontWeight(weight);
    invalidate();
}


void ListBoxItem::setFontSlant(Gfx::Font::Slant slant)
{
    _listItemOptions.setFontSlant(slant);
    invalidate();
}


void ListBoxItem::setRenderer(ListItemRenderer* renderer)
{
    const StyleOptions& options = Application::instance().styleOptions();

    if(renderer)
        _listItemStyle.bind(*renderer, options, _listItemOptions);
    else
        _listItemStyle.bind(Application::instance().style(), options, _listItemOptions);

    invalidate();
}


void ListBoxItem::onInvalidate()
{
    const StyleOptions& options = Application::instance().styleOptions();
    _listItemStyle.rebind(Application::instance().style(), options, _listItemOptions);

    ListItemRenderer* renderer = _listItemStyle.renderer();
    if( ! renderer )
        return;

    if( _icon.empty() )
    {
        _picture.reset(Gfx::Image());
    }
    else
    {
        const Gfx::SizeF scaledSize = scaling().toPhysical(_iconSize);
        const Gfx::Image& iconImage = _icon.getImage(scaledSize);
        _picture.reset(iconImage);
    }

    Base::onInvalidate();
}


Gfx::SizeF ListBoxItem::onMeasure(const SizePolicy& p)
{
    ListItemRenderer* renderer = _listItemStyle.renderer();
    if( ! renderer )
        return Gfx::SizeF(0, 0);

    _fontMetrics = renderer->textPainter( surface() ).fontMetrics();

    _measuredIconSz = onMeasureIcon();
    _measuredTextSz = onMeasureText(_text);

    Gfx::SizeF contentSize = onMeasureContent(p, _measuredIconSz, _measuredTextSz);
    Gfx::SizeF frameSize = renderer->measureFrame(surface(), contentSize);

    return Gfx::SizeF( frameSize.width() + padding().leftRight(),
                       frameSize.height() + padding().topBottom() );
}


Gfx::SizeF ListBoxItem::onMeasureIcon()
{
    if( _picture.empty() )
        return Gfx::SizeF(0, 0);

    Gfx::SizeF pictureSize = scaling().toLogical( _picture.size() );
    double w = _iconSize.isEmpty() ? pictureSize.width() : _iconSize.width();
    double h = _iconSize.isEmpty() ? pictureSize.height() : _iconSize.height();
    return Gfx::SizeF(w, h);
}


Gfx::SizeF ListBoxItem::onMeasureText(const String& text)
{
    if( text.empty() )
        return Gfx::SizeF(0, 0);

    ListItemRenderer* renderer = _listItemStyle.renderer();
    if( ! renderer )
        return Gfx::SizeF(0, 0);

    const Painter& painter = renderer->textPainter( surface() );
    Gfx::TextMetrics tm = painter.textMetrics(text);
    return Gfx::SizeF(tm.advance(), _fontMetrics.height());
}


Gfx::SizeF ListBoxItem::onMeasureContent(const SizePolicy& /*policy*/,
                                         const Gfx::SizeF& iconSz,
                                         const Gfx::SizeF& textSz)
{
    ListItemRenderer* renderer = _listItemStyle.renderer();
    if( ! renderer )
        return Gfx::SizeF(0, 0);

    return renderer->measureContent(surface(), iconSz, textSz);
}


void ListBoxItem::onLayout(const Gfx::RectF& rect)
{
    Base::onLayout(rect);

    ListItemRenderer* renderer = _listItemStyle.renderer();
    if( ! renderer )
        return;

    Gfx::RectF contentRect( Gfx::PointF(padding().left(), padding().top()),
                           Gfx::SizeF(rect.width() - padding().leftRight(),
                                      rect.height() - padding().topBottom()) );

    Gfx::RectF innerRect = renderer->layoutFrame(surface(), contentRect);

    Gfx::RectF iconRect;
    Gfx::RectF textRect;
    onLayoutContent(innerRect, _measuredIconSz, _measuredTextSz, _fontMetrics, iconRect, textRect);

    _iconRect = iconRect;

    if( ! _picture.empty() )
    {
        Gfx::SizeF pictureSize = scaling().toLogical( _picture.size() );
        double pictureXOff = (iconRect.width() - pictureSize.width()) / 2;
        double pictureYOff = (iconRect.height() - pictureSize.height()) / 2;
        _iconPos.setX(iconRect.x() + pictureXOff);
        _iconPos.setY(iconRect.y() + pictureYOff);
    }

    _textRect = textRect;

    if( ! _text.empty() )
    {
        double textY = textRect.y() + (textRect.height() - _fontMetrics.height()) / 2.0 + _fontMetrics.ascent();
        _textPos.setX(textRect.x());
        _textPos.setY(textY);
    }
}


void ListBoxItem::onLayoutContent(const Gfx::RectF& innerRect,
                                   const Gfx::SizeF& iconSz,
                                   const Gfx::SizeF& textSz,
                                   const Gfx::FontMetrics& /*fm*/,
                                   Gfx::RectF& iconRect,
                                   Gfx::RectF& textRect)
{
    ListItemRenderer* r = renderer();
    if(r)
        r->layoutContent(surface(), innerRect, iconSz, textSz, iconRect, textRect);
}


void ListBoxItem::onPaint(PaintContext& context, const Gfx::RectF& /*rect*/)
{
    if( ! _listItemStyle.renderer() )
        return;

    ListItemState st = getState();
    onPaintBackground(context, st);
    onPaintContent(context, st);
}


void ListBoxItem::onPaintBackground(PaintContext& context,
                                     const ListItemState& state)
{
    ListItemRenderer* renderer = _listItemStyle.renderer();
    if( ! renderer )
        return;

    Gfx::RectF widgetRect( Gfx::PointF(0, 0), size() );
    renderer->renderBackground(context, widgetRect, state);
}


void ListBoxItem::onPaintContent(PaintContext& context,
                                  const ListItemState& state)
{
    if( ! _listItemStyle.renderer() )
        return;

    onPaintIcon(context, _iconRect, _picture, _iconPos, state);
    onPaintText(context, _textRect, _text, _textPos, _fontMetrics, state);
}


void ListBoxItem::onPaintIcon(PaintContext& context,
                              const Gfx::RectF& iconRect,
                              const PixmapSurface& picture,
                              const Gfx::PointF& iconPos,
                              const ListItemState& st)
{
    ListItemRenderer* renderer = _listItemStyle.renderer();
    if( ! renderer || picture.empty() )
        return;

    renderer->renderIcon(context, iconRect, picture, iconPos, st);
}


void ListBoxItem::onPaintText(PaintContext& context,
                              const Gfx::RectF& textRect,
                              const String& text,
                              const Gfx::PointF& textPos,
                              const Gfx::FontMetrics& /*fm*/,
                              const ListItemState& st)
{
    ListItemRenderer* renderer = _listItemStyle.renderer();
    if( ! renderer || text.empty() )
        return;

    renderer->renderText(context, textRect, text, textPos, st);
}


ListItemRenderer* ListBoxItem::renderer()
{
    return _listItemStyle.renderer();
}


ListItemState ListBoxItem::getState() const
{
    ListItemState st;
    st.setEnabled( isEnabled() );
    st.setHighlighted( _isHovered );
    st.setFocused( hasFocus() );
    st.setSelected( _isSelected );
    return st;
}

/////////////////////////////////////////////////////////////////////////////
// ListBoxLayout
/////////////////////////////////////////////////////////////////////////////

ListBoxLayout::ListBoxLayout()
: FlowLayout(Direction::Top)
{
}


const std::vector<ListBoxItem*>& ListBoxLayout::selectedItems() const
{
    return _selectedItems;
}


Pt::Signal<ListBoxItem&>& ListBoxLayout::selected()
{
    return _selected;
}


void ListBoxLayout::onAddControl(Control& control)
{
}


void ListBoxLayout::onRemoveControl(Control& control)
{
    ListBoxItem& item = static_cast<ListBoxItem&>(control);

    _selectedItems.erase( std::remove(_selectedItems.begin(),
                                      _selectedItems.end(),
                                      &item),
                          _selectedItems.end() );
}


void ListBoxLayout::onItemSelected(ListBoxItem& item)
{
    if( ! item.isSelected() )
    {
        _selectedItems.erase( std::remove(_selectedItems.begin(),
                                          _selectedItems.end(),
                                          &item),
                              _selectedItems.end() );
    }
    else
    {
        _selectedItems.push_back(&item);
    }

    _selected.send(item);
}

/////////////////////////////////////////////////////////////////////////////
// ListBox
/////////////////////////////////////////////////////////////////////////////

ListBox::ListBox()
: _layout()
, _hasBackground(true)
, _hasFrame(true)
{
    _scrollView.setContent(_layout);
    _scrollView.setContentMode(SizePolicy::Fixed, SizePolicy::Preferred);
    add(_scrollView);
}


ListBox::~ListBox()
{
}


void ListBox::setScrollBars(bool hasScrollBars)
{
    _scrollView.setScrollBars(hasScrollBars);
}


void ListBox::addItem(ListBoxItem& item)
{
    _layout.addItem(item);
    item.selected() += Pt::slot(_layout, &ListBoxLayout::onItemSelected);
}


void ListBox::removeItem(ListBoxItem& item)
{
    _layout.removeItem(item);
    item.selected() -= Pt::slot(_layout, &ListBoxLayout::onItemSelected);
}


const std::vector<ListBoxItem*>& ListBox::selectedItems() const
{
    return _layout.selectedItems();
}


Pt::Signal<ListBoxItem&>& ListBox::selected()
{
    return _layout.selected();
}


const Gfx::Brush* ListBox::background() const
{
    if( ! _hasBackground )
        return 0;

    const Gfx::Brush* b = _listBoxOptions.background();
    if(b)
        return b;

    return &Application::instance().styleOptions().viewBackground();
}


void ListBox::setBackground(const Gfx::Brush& b)
{
    _listBoxOptions.setBackground(b);
    _hasBackground = true;
    invalidate();
}


void ListBox::setBackground(bool b)
{
    _hasBackground = b;
    repaint();
}


const Gfx::Pen* ListBox::contour() const
{
    if( ! _hasFrame )
        return 0;

    const Gfx::Pen* p = _listBoxOptions.contour();
    if(p)
        return p;

    return &Application::instance().styleOptions().contour();
}


void ListBox::setContour(const Gfx::Pen& pen)
{
    _listBoxOptions.setContour(pen);
    _hasFrame = true;
    invalidate();
}


void ListBox::setFrame(bool b)
{
    _hasFrame = b;
    repaint();
}


void ListBox::scrollX(int xpos)
{
    _scrollView.scrollX(xpos);
}


void ListBox::scrollY(int ypos)
{
    _scrollView.scrollY(ypos);
}


int ListBox::maximumX() const
{
    return _scrollView.maximumX() + _scrollView.margin().leftRight();
}


int ListBox::maximumY() const
{
    return _scrollView.maximumY() + _scrollView.margin().topBottom();
}


void ListBox::setRenderer(ListBoxRenderer* renderer)
{
    const StyleOptions& options = Application::instance().styleOptions();

    if(renderer)
        _listBoxStyle.bind(*renderer, options, _listBoxOptions);
    else
        _listBoxStyle.bind(Application::instance().style(), options, _listBoxOptions);

    invalidate();
}


void ListBox::onInvalidate()
{
    Base::onInvalidate();

    const StyleOptions& options = Application::instance().styleOptions();
    _listBoxStyle.rebind(Application::instance().style(), options, _listBoxOptions);

    if( ! _listBoxStyle.renderer() )
        return;

    relayout();
}


Gfx::SizeF ListBox::onMeasure(const SizePolicy& policy)
{
    ListBoxRenderer* renderer = _listBoxStyle.renderer();

    double hspace = padding().leftRight() + _scrollView.margin().leftRight();
    double vspace = padding().topBottom() + _scrollView.margin().topBottom();

    if( renderer && _hasFrame )
    {
        Gfx::SizeF frameSize = renderer->measureFrame(surface(), Gfx::SizeF(0, 0));
        hspace += frameSize.width();
        vspace += frameSize.height();
    }

    SizePolicy contentPolicy( policy.horizontal(), policy.vertical() );
    contentPolicy.setWidth( policy.size().width() - hspace );
    contentPolicy.setHeight( policy.size().height() - vspace );

    _scrollView.measure(contentPolicy);
    Gfx::SizeF prefSize = _scrollView.preferredSize();

    prefSize.setWidth( maximumX() + hspace );
    prefSize.setHeight( maximumY() + vspace );

    return prefSize;
}


void ListBox::onLayout(const Gfx::RectF& rect)
{
    Base::onLayout(rect);

    ListBoxRenderer* renderer = _listBoxStyle.renderer();

    Gfx::RectF widgetRect( Gfx::PointF(0, 0), size() );
    Gfx::RectF contentRect = widgetRect;

    if( renderer && _hasFrame )
        contentRect = renderer->layoutFrame(surface(), widgetRect);

    Gfx::PointF pos(contentRect.x() + _scrollView.margin().left(),
                    contentRect.y() + _scrollView.margin().top());

    Gfx::SizeF sz(contentRect.width() - _scrollView.margin().leftRight(),
                  contentRect.height() - _scrollView.margin().topBottom());

    _scrollView.move(pos);
    _scrollView.resize(sz);
}


void ListBox::onPaint(PaintContext& context, const Gfx::RectF& /*rect*/)
{
    ListBoxRenderer* renderer = _listBoxStyle.renderer();
    if( ! renderer )
        return;

    ListBoxState state;
    state.setEnabled( isEnabled() );
    state.setFocused( hasFocus() );

    Gfx::RectF widgetRect( Gfx::PointF(0, 0), size() );

    if( _hasBackground )
    {
        renderer->renderBackground(context, widgetRect, state);
    }

    if( _hasFrame )
    {
        renderer->renderChrome(context, widgetRect, state);
    }
}

} // namespace

} // namespace
