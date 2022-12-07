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

#include <Pt/Hmi/ListBox.h>
#include <Pt/Hmi/Application.h>
#include <Pt/Gfx/Painter.h>

namespace Pt {

namespace Hmi {

/////////////////////////////////////////////////////////////////////////////
// ListBoxItem
/////////////////////////////////////////////////////////////////////////////

ListBoxItem::ListBoxItem()
: _hasRenderer(false)
, _isSelectable(false)
, _isSelected(false)
{
    setPadding(8);

    // onMeasure can be called before onInvalidate
    _font = Gfx::Font(font(), fontSize(), fontStyle());
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


const Gfx::Brush& ListBoxItem::background() const
{
    return _background ? *_background
                       : Application::instance().styleOptions().background();
}


void ListBoxItem::setBackground(const Gfx::Brush& b)
{
    _background.reset( new Gfx::Brush(b) );
    invalidate();
}


const Gfx::Pen& ListBoxItem::contour() const
{
    return _contour ? *_contour
                    : Application::instance().styleOptions().contour();
}


void ListBoxItem::setContour(const Gfx::Pen& p)
{
    _contour.reset( new Gfx::Pen(p) );
    invalidate();
}


const Gfx::Color& ListBoxItem::textColor() const
{
    return _textColor ? *_textColor
                      : Application::instance().styleOptions().textColor();
}


void ListBoxItem::setTextColor(const Gfx::Color& color)
{
    _textColor.reset( new Gfx::Color(color) );
    invalidate();
}


const std::string& ListBoxItem::font() const
{
    return _fontName ? *_fontName
                     : Application::instance().styleOptions().font().name();
}


void ListBoxItem::setFont(const std::string& fontName)
{
    _fontName.reset( new std::string(fontName) );
    invalidate();
}


std::size_t ListBoxItem::fontSize() const
{

    return _fontSize ? *_fontSize
                     : Application::instance().styleOptions().font().size();
}


void ListBoxItem::setFontSize(const std::size_t s)
{
    _fontSize.reset( new std::size_t(s) );
    invalidate();
}


const std::string& ListBoxItem::fontStyle() const
{
    return _fontStyle ? *_fontStyle
                      : Application::instance().styleOptions().font().style();
}


void ListBoxItem::setFontStyle(const std::string& style)
{
    _fontStyle.reset( new std::string(style) );
    invalidate();
}


void ListBoxItem::setRenderer(ListBoxRenderer* renderer)
{
    _renderer.reset(renderer);
    _hasRenderer = renderer != 0;

    invalidate();
}


Gfx::SizeF ListBoxItem::onMeasure(const SizePolicy& p)
{
    Gfx::FontMetrics fm = PixmapSurface::fontMetrics( _font, _text );

    double spacing = _picture.empty() || _text.empty() ? 0 : fm.height() * 0.5;

    Gfx::SizeF pictureSize = surface().toLogical( _picture.size() );
    double pictureWidth = _iconSize.isNull() ? pictureSize.width() : _iconSize.width();
    double pictureHeight = _iconSize.isNull() ? pictureSize.height() : _iconSize.height();
    double itemsWidth = fm.width() + spacing + pictureWidth;
    double itemsHeight = std::max<double>(fm.height(), pictureHeight);

    return Gfx::SizeF( itemsWidth + padding().leftRight(),
                       itemsHeight + padding().topBottom() );
}


void ListBoxItem::onInvalidate()
{
    // TODO: use renderer and options from parent

    const StyleOptions& options = Application::instance().styleOptions();
    const Style& style = Application::instance().style();

    _brush = background();
    _pen = contour();
    _textPen = textColor();
    _font = Gfx::Font(font(), fontSize(), fontStyle());

    if( ! _hasRenderer )
        _renderer.reset( style.get<ListBoxRenderer>() );
    
    if( ! _renderer )
        return;

    _renderer->prepareItem(*this, options, _brush, _pen, _font, _textPen);

    if (_icon.empty())
    {
        _picture.set(Gfx::Image());
    }
    else
    {
        const Gfx::SizeF scaledSize = surface().toPhysical(_iconSize);
        const Pt::Gfx::Image& iconImage = _icon.getImage(scaledSize);
        _picture.set(iconImage);
    }

    Base::onInvalidate();
}


void ListBoxItem::onPaint(Gfx::PaintSurface& surface, const Gfx::RectF& rect)
{
    const StyleOptions& options = Application::instance().styleOptions();

    if( ! _renderer )
        return;

    Gfx::Painter painter(surface);
    painter.setClip(rect);

    //
    // background
    //
    _renderer->renderItem(*this, options, painter, rect, _brush, _pen);

    onPaintContent(surface, painter);
}


void ListBoxItem::onPaintContent(Gfx::PaintSurface& surface, Gfx::Painter& painter)
{   
    painter.setFont(_font);
    painter.setPen(_textPen);
    
    //
    // layout icon and text
    //

    Gfx::FontMetrics fm = painter.fontMetrics( _text );

    double pictureX = 0;
    double pictureY = 0;
    double textX = 0;
    double textY = 0;

    double spacing = _picture.empty() || _text.empty() ? 0 : fm.height() * 0.5;

    Gfx::SizeF pictureSize = painter.toLogical( _picture.size() );
    double pictureWidth = _iconSize.isNull() ? pictureSize.width() : _iconSize.width();
    double pictureHeight = _iconSize.isNull() ? pictureSize.height() : _iconSize.height();
    double itemsWidth = fm.width() + spacing + pictureWidth;
    double itemsHeight = fm.height() + spacing + pictureHeight;

    pictureX = padding().left();
    pictureY = (size().height() - pictureHeight) / 2;
    
    textX = pictureX + pictureWidth + spacing;
    textY = ((size().height() - fm.height()) / 2) + fm.ascent();

    //
    // icon
    //

    if( ! _picture.empty() )
    {
        painter.setCompositionMode(Pt::Gfx::CompositionMode::SourceOver);
        
        double pictureXOff = (pictureWidth - pictureSize.width()) / 2;
        double pictureYOff = (pictureHeight - pictureSize.height()) / 2;

        Gfx::PointF picturePos(pictureX + pictureXOff, 
                               pictureY + pictureYOff);
        painter.drawSurface(picturePos, _picture);
        
        painter.setCompositionMode(Pt::Gfx::CompositionMode::SourceCopy);
    }

    //
    // text
    //

    Gfx::RectF mnemonicRect;
    Gfx::PointF textPos(textX, textY);

    painter.drawText(textPos, _text);
}

/////////////////////////////////////////////////////////////////////////////
// ListBoxLayout
/////////////////////////////////////////////////////////////////////////////

ListBoxLayout::ListBoxLayout()
: FlowLayout(FlowLayout::Top)
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


void ListBoxLayout::onAddWidget(Widget& w)
{
}


void ListBoxLayout::onRemoveWidget(Widget& w)
{
    ListBoxItem& item = static_cast<ListBoxItem&>(w);

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
, _hasRenderer(false)
{
    //setAcceptInput(false);

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

    return _background ? _background.get() 
                       : &Application::instance().styleOptions().viewBackground();
}


void ListBox::setBackground(const Gfx::Brush& b)
{
    _background.reset( new Gfx::Brush(b) );
    _hasBackground = true;
    
    update();
}


void ListBox::setBackground(bool b)
{
    _hasBackground = b;
    update();
}


const Gfx::Pen* ListBox::contour() const
{
    if( ! _hasFrame )
        return 0;

    return _contour ? _contour.get() 
                    : &Application::instance().styleOptions().contour();
}


void ListBox::setContour(const Gfx::Pen& pen)
{
    _contour.reset( new Gfx::Pen(pen) );
    _hasFrame = true;

    update();
}


void ListBox::setFrame(bool b)
{
    _hasFrame = b;
    update();
}


void ListBox::setRenderer(ListBoxRenderer* renderer)
{
    _renderer.reset(renderer);
    _hasRenderer = renderer != 0;

    invalidate();
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

    //SizePolicy contentPolicy(SizePolicy::Fixed, SizePolicy::Fixed);

    SizePolicy contentPolicy( policy.horizontal(), policy.vertical() );
    contentPolicy.setWidth( policy.size().width() - hspace );
    contentPolicy.setHeight( policy.size().height() - vspace );

    _scrollView.measure(contentPolicy);
    Gfx::SizeF prefSize = _scrollView.preferredSize();
    
    prefSize.setWidth( maximumX() );
    prefSize.setHeight( maximumY() );

    return prefSize;
}


void ListBox::onLayout(const Gfx::RectF& rect)
{
    Base::onLayout(rect);
    
    Gfx::PointF pos(padding().left() + _scrollView.margin().left(), 
                    padding().top()  + _scrollView.margin().top());
        
    double hspace = padding().leftRight() + _scrollView.margin().leftRight();
    double vspace = padding().topBottom() + _scrollView.margin().topBottom();

    Gfx::SizeF size;
    size.setWidth( rect.width() - hspace );
    size.setHeight( rect.height() - vspace );

    _scrollView.move(pos);
    _scrollView.resize(size);
}


void ListBox::onInvalidate()
{
    Base::onInvalidate();

    const StyleOptions& options = Application::instance().styleOptions();
    const Style& style = Application::instance().style();

    const Gfx::Brush* brush = background();
    if( brush )
        _brush = *brush;
    
    const Gfx::Pen* pen = contour();
    if(pen)
        _pen = *pen;

    if( ! _hasRenderer )
        _renderer.reset( style.get<ListBoxRenderer>() );

    if( ! _renderer)
        return;

    _renderer->prepareLayout(_frameSize);
    _scrollView.setMargin(_frameSize);
}


void ListBox::onPaint(Gfx::PaintSurface& surface, const Gfx::RectF& rect)
{
    const StyleOptions& options = Application::instance().styleOptions();

    if( ! _renderer)
        return;

    Gfx::Painter painter(surface);
    painter.setClip(rect);

    if(_hasBackground)
    {
        _renderer->renderBackground(*this, options,
                                    painter, rect, _brush);
    }

    if(_hasFrame)
    {
        _renderer->renderFrame(*this, options,
                               painter, rect, _pen);
    }
}

} // namespace

} // namespace
