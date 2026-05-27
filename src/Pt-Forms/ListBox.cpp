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
#include <Pt/Gfx/Painter.h>

namespace Pt {

namespace Forms {

/////////////////////////////////////////////////////////////////////////////
// ListBoxItem
/////////////////////////////////////////////////////////////////////////////

ListBoxItem::ListBoxItem()
: _customRenderer(false)
, _styleGeneration(0)
, _isSelectable(false)
, _isSelected(false)
, _overrideFlags(0)
{
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


Pt::Signal<ListBoxItem&>& ListBoxItem::selected()
{
    return _selected;
}


void ListBoxItem::onPressed()
{
    Base::onPressed();
}


void ListBoxItem::onReleased()
{
    Base::onReleased();

    setSelected( ! _isSelected );
}


void ListBoxItem::onCanceled()
{
    Base::onCanceled();
}


const Gfx::Font& ListBoxItem::font() const
{
    if( _renderer )
        return _renderer->font();

    return Application::instance().styleOptions().font();
}


void ListBoxItem::setFont(const Gfx::Font& f)
{
    _customFont = f;
    _overrideFlags |= OverrideFontAll;

    if( ListItemRenderer* renderer = getRenderer() )
        renderer->setFont(f);

    invalidate();
}


Gfx::Font ListBoxItem::getFont() const
{
    const Gfx::Font& base = Application::instance().styleOptions().font();

    if( ! (_overrideFlags & OverrideFontAny) )
        return base;

    if( _overrideFlags & OverrideFontAll )
        return _customFont;

    std::size_t sz = (_overrideFlags & OverrideFontSize) ? _customFont.size()
                                                        : base.size();
    Gfx::Font::Weight wt = (_overrideFlags & OverrideFontWeight) ? _customFont.weight()
                                                                 : base.weight();
    Gfx::Font::Slant sl = (_overrideFlags & OverrideFontSlant) ? _customFont.slant()
                                                               : base.slant();

    if( base.hasStyleName() )
        return Gfx::Font(base.family(), sz, base.styleName(), wt, sl, base.stretch());

    if( base.category() != Gfx::Font::Category::None )
        return Gfx::Font(base.category(), sz, wt, sl, base.stretch());

    return Gfx::Font(base.family(), sz, wt, sl, base.stretch());
}


void ListBoxItem::setFontSize(std::size_t size)
{
    _customFont = _customFont.withSize(size);
    _overrideFlags |= OverrideFontSize;

    if( ListItemRenderer* renderer = getRenderer() )
        renderer->setFont( getFont() );

    invalidate();
}


void ListBoxItem::setFontWeight(Gfx::Font::Weight weight)
{
    _customFont = _customFont.withWeight(weight);
    _overrideFlags |= OverrideFontWeight;

    if( ListItemRenderer* renderer = getRenderer() )
        renderer->setFont( getFont() );

    invalidate();
}


void ListBoxItem::setFontSlant(Gfx::Font::Slant slant)
{
    _customFont = _customFont.withSlant(slant);
    _overrideFlags |= OverrideFontSlant;

    if( ListItemRenderer* renderer = getRenderer() )
        renderer->setFont( getFont() );

    invalidate();
}


void ListBoxItem::setBackground(const Gfx::Brush& b)
{
    _background.reset( new Gfx::Brush(b) );
    _overrideFlags |= OverrideBackground;

    if( ListItemRenderer* renderer = getRenderer() )
        renderer->setBackground(*_background);

    invalidate();
}


void ListBoxItem::setTextColor(const Gfx::Color& color)
{
    _textColor.reset( new Gfx::Color(color) );
    _overrideFlags |= OverrideTextColor;

    if( ListItemRenderer* renderer = getRenderer() )
        renderer->setTextColor( Gfx::Pen(*_textColor) );

    invalidate();
}


void ListBoxItem::setRenderer(ListItemRenderer* renderer)
{
    _renderer.reset(renderer);
    _customRenderer = renderer != 0;

    if( renderer )
        applyRenderer(renderer);

    invalidate();
}


ListItemRenderer* ListBoxItem::getRenderer()
{
    if( ! _renderer )
    {
        const Style& style = Application::instance().style();
        ListItemRenderer* proto = style.get<ListItemRenderer>();
        if( ! proto )
            return 0;

        _renderer.reset( proto->create() );
    }

    return _renderer.get();
}


void ListBoxItem::applyRenderer(ListItemRenderer* renderer)
{
    if( _overrideFlags & OverrideBackground )
        renderer->setBackground(*_background);

    if( _overrideFlags & OverrideTextColor )
        renderer->setTextColor( Gfx::Pen(*_textColor) );

    if( _overrideFlags & OverrideFontAny )
        renderer->setFont( getFont() );
}


ListItemStyleFlags ListBoxItem::listItemStyleFlags() const
{
    StyleFlags common;

    if( isEnabled() )
        common.set(StyleFlags::Enabled);
    else
        common.set(StyleFlags::Disabled);

    if( isHovered() )
        common.set(StyleFlags::Highlighted);

    if( hasFocus() )
        common.set(StyleFlags::Focused);

    ListItemStyleFlags state(common);

    if( _isSelected )
        state.set(ListItemStyleFlags::Selected);

    return state;
}


Gfx::SizeF ListBoxItem::onMeasure(const SizePolicy& /*p*/)
{
    if( ! _renderer )
        return Gfx::SizeF(0, 0);

    const Painter& painter = _renderer->textPainter( surface() );

    Gfx::TextMetrics tm = painter.textMetrics(_text);
    Gfx::FontMetrics fm = painter.fontMetrics();

    Gfx::SizeF pictureSize = scaling().toLogical( _picture.size() );
    double pictureWidth = _iconSize.isNull() ? pictureSize.width() : _iconSize.width();
    double pictureHeight = _iconSize.isNull() ? pictureSize.height() : _iconSize.height();

    Gfx::SizeF iconSz(pictureWidth, pictureHeight);
    Gfx::SizeF textSz(tm.advance(), fm.height());

    Gfx::SizeF contentSize = _renderer->measureContent(surface(), iconSz, textSz);
    Gfx::SizeF frameSize = _renderer->measureFrame(surface(), contentSize);

    return Gfx::SizeF( frameSize.width() + padding().leftRight(),
                       frameSize.height() + padding().topBottom() );
}


void ListBoxItem::onInvalidate()
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
        bool hasOverride = (_overrideFlags != 0);
        if( hasOverride )
        {
            if( ListItemRenderer* renderer = getRenderer() )
                applyRenderer(renderer);
        }
        else
        {
            _renderer.reset( Application::instance().style().get<ListItemRenderer>() );
        }
    }

    if( ! _renderer )
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


void ListBoxItem::onPaint(PaintContext& context, const Gfx::RectF& /*rect*/)
                        {
    if( ! _renderer )
        return;

    onPaintBackground(context);
    onPaintContent(context);
}


void ListBoxItem::onPaintBackground(PaintContext& context)
{
    if( ! _renderer )
        return;

    ListItemStyleFlags state = listItemStyleFlags();

    Gfx::RectF widgetRect( Gfx::PointF(0, 0), size() );
    _renderer->renderBackground(context, widgetRect, state);
}


void ListBoxItem::onPaintContent(PaintContext& context)
{
    if( ! _renderer )
        return;

    onPaintIcon(context);
    onPaintText(context);
}


void ListBoxItem::onPaintIcon(PaintContext& context)
{
    if( ! _renderer || _picture.empty() )
        return;

    ListItemStyleFlags state = listItemStyleFlags();

    const Painter& painter = _renderer->textPainter( surface() );

    Gfx::TextMetrics tm = painter.textMetrics(_text);
    Gfx::FontMetrics fm = painter.fontMetrics();

    Gfx::SizeF pictureSize = scaling().toLogical( _picture.size() );
    double pictureWidth = _iconSize.isNull() ? pictureSize.width() : _iconSize.width();
    double pictureHeight = _iconSize.isNull() ? pictureSize.height() : _iconSize.height();

    Gfx::SizeF iconSz(pictureWidth, pictureHeight);
    Gfx::SizeF textSz(tm.advance(), fm.height());

    Gfx::RectF contentRect( Gfx::PointF(padding().left(), padding().top()),
                           Gfx::SizeF(size().width() - padding().leftRight(),
                                      size().height() - padding().topBottom()) );

    Gfx::RectF innerRect = _renderer->layoutFrame(surface(), contentRect);

    Gfx::RectF iconRect;
    Gfx::RectF textRect;
    _renderer->layoutContent(surface(), innerRect, iconSz, textSz,
                             iconRect, textRect);

    double pictureXOff = (iconRect.width() - pictureSize.width()) / 2;
    double pictureYOff = (iconRect.height() - pictureSize.height()) / 2;

    Gfx::PointF picturePos(iconRect.x() + pictureXOff,
                           iconRect.y() + pictureYOff);

    _renderer->renderIcon(context, iconRect, _picture, picturePos, state);
}


void ListBoxItem::onPaintText(PaintContext& context)
{
    if( ! _renderer || _text.empty() )
        return;

    ListItemStyleFlags state = listItemStyleFlags();

    const Painter& painter = _renderer->textPainter( surface() );

    Gfx::TextMetrics tm = painter.textMetrics(_text);
    Gfx::FontMetrics fm = painter.fontMetrics();

    Gfx::SizeF pictureSize = scaling().toLogical( _picture.size() );
    double pictureWidth = _iconSize.isNull() ? pictureSize.width() : _iconSize.width();
    double pictureHeight = _iconSize.isNull() ? pictureSize.height() : _iconSize.height();

    Gfx::SizeF iconSz(pictureWidth, pictureHeight);
    Gfx::SizeF textSz(tm.advance(), fm.height());

    Gfx::RectF contentRect( Gfx::PointF(padding().left(), padding().top()),
                           Gfx::SizeF(size().width() - padding().leftRight(),
                                      size().height() - padding().topBottom()) );

    Gfx::RectF innerRect = _renderer->layoutFrame(surface(), contentRect);

    Gfx::RectF iconRect;
    Gfx::RectF textRect;
    _renderer->layoutContent(surface(), innerRect, iconSz, textSz,
                             iconRect, textRect);

    double textY = textRect.y() + (textRect.height() - fm.height()) / 2.0 + fm.ascent();
    Gfx::PointF textPos(textRect.x(), textY);

    _renderer->renderText(context, textRect, _text, textPos, state);
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
, _customRenderer(false)
, _styleGeneration(0)
, _overrideFlags(0)
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

    if( _renderer )
        return _renderer->background();

    return &Application::instance().styleOptions().viewBackground();
}


void ListBox::setBackground(const Gfx::Brush& b)
{
    _background.reset( new Gfx::Brush(b) );
    _hasBackground = true;
    _overrideFlags |= OverrideBackground;

    if( ListBoxRenderer* renderer = getRenderer() )
        renderer->setBackground(*_background);

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

    if( _renderer )
        return _renderer->contour();

    return &Application::instance().styleOptions().contour();
}


void ListBox::setContour(const Gfx::Pen& pen)
{
    _contour.reset( new Gfx::Pen(pen) );
    _hasFrame = true;
    _overrideFlags |= OverrideContour;

    if( ListBoxRenderer* renderer = getRenderer() )
        renderer->setContour(*_contour);

    invalidate();
}


void ListBox::setFrame(bool b)
{
    _hasFrame = b;
    repaint();
}


void ListBox::setRenderer(ListBoxRenderer* renderer)
{
    _renderer.reset(renderer);
    _customRenderer = renderer != 0;

    if( renderer )
        applyRenderer(renderer);

    invalidate();
}


ListBoxRenderer* ListBox::getRenderer()
{
    if( ! _renderer )
    {
        const Style& style = Application::instance().style();
        ListBoxRenderer* proto = style.get<ListBoxRenderer>();
        if( ! proto )
            return 0;

        _renderer.reset( proto->create() );
    }

    return _renderer.get();
}


void ListBox::applyRenderer(ListBoxRenderer* renderer)
{
    if( _overrideFlags & OverrideBackground )
        renderer->setBackground(*_background);

    if( _overrideFlags & OverrideContour )
        renderer->setContour(*_contour);
}


ListBoxStyleFlags ListBox::listBoxStyleFlags() const
{
    StyleFlags common;

    if( isEnabled() )
        common.set(StyleFlags::Enabled);
    else
        common.set(StyleFlags::Disabled);

    if( hasFocus() )
        common.set(StyleFlags::Focused);

    return ListBoxStyleFlags(common);
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


Gfx::SizeF ListBox::onMeasure(const SizePolicy& policy)
{
    double hspace = padding().leftRight() + _scrollView.margin().leftRight();
    double vspace = padding().topBottom() + _scrollView.margin().topBottom();

    if( _renderer && _hasFrame )
    {
        Gfx::SizeF frameSize = _renderer->measureFrame(surface(), Gfx::SizeF(0, 0));
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

    Gfx::RectF widgetRect( Gfx::PointF(0, 0), size() );
    Gfx::RectF contentRect = widgetRect;

    if( _renderer && _hasFrame )
        contentRect = _renderer->layoutFrame(surface(), widgetRect);

    Gfx::PointF pos(contentRect.x() + _scrollView.margin().left(),
                    contentRect.y() + _scrollView.margin().top());

    Gfx::SizeF sz(contentRect.width() - _scrollView.margin().leftRight(),
                  contentRect.height() - _scrollView.margin().topBottom());

    _scrollView.move(pos);
    _scrollView.resize(sz);
}


void ListBox::onInvalidate()
{
    Base::onInvalidate();

    std::size_t gen = Application::instance().styleOptions().generation();
    if( _styleGeneration != gen )
    {
        _styleGeneration = gen;
        if( ! _customRenderer )
            _renderer.reset();
    }

    if( ! _renderer )
    {
        bool hasOverride = (_overrideFlags != 0);
        if( hasOverride )
        {
            if( ListBoxRenderer* renderer = getRenderer() )
                applyRenderer(renderer);
        }
        else
        {
            _renderer.reset( Application::instance().style().get<ListBoxRenderer>() );
        }
    }

    if( ! _renderer )
        return;

    relayout();
}


void ListBox::onPaint(PaintContext& context, const Gfx::RectF& /*rect*/)
{
    if( ! _renderer )
        return;

    ListBoxStyleFlags state = listBoxStyleFlags();

    Gfx::RectF widgetRect( Gfx::PointF(0, 0), size() );

    if( _hasBackground )
    {
        _renderer->renderBackground(context, widgetRect, state);
    }

    if( _hasFrame )
    {
        _renderer->renderChrome(context, widgetRect, state);
    }
}

} // namespace

} // namespace
