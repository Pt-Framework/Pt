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
#include <Pt/Hmi/Painter.h>

namespace Pt {

namespace Hmi {

/////////////////////////////////////////////////////////////////////////////
// ListBoxItem
/////////////////////////////////////////////////////////////////////////////

ListBoxItem::ListBoxItem()
: _hasRenderer(false)
{
    setPadding(8);
}


ListBoxItem::~ListBoxItem()
{
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


void ListBoxItem::setIcon(const Gfx::Image& image)
{
    _image = image;
    _picture.set(image);
    
    invalidate();
}


void ListBoxItem::setIconSize(const Gfx::Size& size)
{
    _iconSize = size;
    update();
}


void ListBoxItem::setIconSize(Pt::ssize_t width, Pt::ssize_t height)
{
    setIconSize( Gfx::Size(width, height) );
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
    _selected.send(*this);
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


Gfx::Font::Style ListBoxItem::fontStyle() const
{
    return _fontStyle ? *_fontStyle
                      : Application::instance().styleOptions().font().style();
}


void ListBoxItem::setFontStyle(Gfx::Font::Style style)
{
    _fontStyle.reset( new Gfx::Font::Style(style) );
    invalidate();
}


void ListBoxItem::setRenderer(ListBoxRenderer* renderer)
{
    _renderer.reset(renderer);
    _hasRenderer = renderer != 0;

    invalidate();
}


Gfx::Size ListBoxItem::onMeasure(const SizePolicy& p)
{
    Gfx::FontMetrics fm = Painter::fontMetrics( _font, _text );

    Pt::ssize_t spacing = _picture.empty() || _text.empty() ? 0 : fm.height() /2;
    Pt::ssize_t pictureWidth = _iconSize.isNull() ? _picture.width() : _iconSize.width();
    Pt::ssize_t pictureHeight = _iconSize.isNull() ? _picture.height() : _iconSize.height();
    Pt::ssize_t itemsWidth = fm.width() + spacing + pictureWidth;
    Pt::ssize_t itemsHeight = std::max<Pt::ssize_t>(fm.height(), pictureHeight);

    return Gfx::Size( itemsWidth + padding().leftRight(),
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

    Base::onInvalidate();
}


void ListBoxItem::onPaint(PaintSurface& surface, const Gfx::Rect& rect)
{
    const StyleOptions& options = Application::instance().styleOptions();

    if( ! _renderer )
        return;

    Painter painter(surface);
    painter.setClip(rect);

    //
    // background
    //
    _renderer->renderItem(*this, options, painter, rect, _brush, _pen);

    onPaintContent(painter);
}


void ListBoxItem::onPaintContent(Painter& painter)
{   
    painter.setFont(_font);
    painter.setPen(_textPen);
    
    //
    // layout icon and text
    //

    Gfx::FontMetrics fm = painter.fontMetrics( _text );

    Pt::ssize_t pictureX = 0;
    Pt::ssize_t pictureY = 0;
    Pt::ssize_t textX = 0;
    Pt::ssize_t textY = 0;

    Pt::ssize_t spacing = _picture.empty() || _text.empty() ? 0 : fm.height() / 2;
    Pt::ssize_t pictureWidth = _iconSize.isNull() ? _picture.width() : _iconSize.width();
    Pt::ssize_t pictureHeight = _iconSize.isNull() ? _picture.height() : _iconSize.height();
    Pt::ssize_t itemsWidth = fm.width() + spacing + pictureWidth;

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
        
        Pt::ssize_t pictureXOff = (pictureWidth - _picture.width()) / 2;
        Pt::ssize_t pictureYOff = (pictureHeight - _picture.height()) / 2;

        Gfx::Point picturePos(pictureX + pictureXOff, 
                               pictureY + pictureYOff);
        painter.drawPicture(picturePos, _picture);
        
        painter.setCompositionMode(Pt::Gfx::CompositionMode::SourceCopy);
    }

    //
    // text
    //

    Gfx::Rect mnemonicRect;
    Gfx::Point textPos(textX, textY);
    
    painter.drawText(textPos, _text);
}

/////////////////////////////////////////////////////////////////////////////
// ListBox
/////////////////////////////////////////////////////////////////////////////

ListBox::ListBox()
: _layout(FlowLayout::Top)
, _hasBackground(true)
, _hasFrame(true)
, _hasRenderer(false)
{
    setAcceptInput(false);

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
    item.selected() += Pt::slot(*this, &ListBox::onItemSelected);
}


void ListBox::removeItem(ListBoxItem& item)
{
    _layout.removeItem(item);
    item.selected() -= Pt::slot(*this, &ListBox::onItemSelected);
}


void ListBox::onItemSelected(ListBoxItem& item)
{
    _selected.send(item);
}


Pt::Signal<ListBoxItem&>& ListBox::selected()
{
    return _selected;
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


int ListBox::maximumX() const
{
    return _scrollView.maximumX() + _scrollView.margin().leftRight();
}


int ListBox::maximumY() const
{
    return _scrollView.maximumY() + _scrollView.margin().topBottom();
}


Gfx::Size ListBox::onMeasure(const SizePolicy& policy)
{
    Pt::ssize_t hspace = padding().leftRight() + _scrollView.margin().leftRight();
    Pt::ssize_t vspace = padding().topBottom() + _scrollView.margin().topBottom();

    SizePolicy contentPolicy(SizePolicy::Fixed, SizePolicy::Fixed);
    contentPolicy.setWidth( policy.size().width() - hspace );
    contentPolicy.setHeight( policy.size().height() - vspace );

    _scrollView.measure(contentPolicy);
    
    return policy.size();
}


void ListBox::onLayout(const Gfx::Rect& rect)
{
    Gfx::Point pos(padding().left() + _scrollView.margin().left(), 
                    padding().top()  + _scrollView.margin().top());
        
    Pt::ssize_t hspace = padding().leftRight() + _scrollView.margin().leftRight();
    Pt::ssize_t vspace = padding().topBottom() + _scrollView.margin().topBottom();

    Gfx::Size size;
    size.setWidth( rect.width() - hspace );
    size.setHeight( rect.height() - vspace );

    _scrollView.layout( pos, size );
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


void ListBox::onPaint(PaintSurface& surface, const Gfx::Rect& rect)
{
    const StyleOptions& options = Application::instance().styleOptions();

    if( ! _renderer)
        return;

    Painter painter(surface);
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


