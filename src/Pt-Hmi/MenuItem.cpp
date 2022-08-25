/* Copyright (C) 2016 Marc Boris Duerner 
   Copyright (C) 2016 Laurentiu-Gheorghe Crisan

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
   MA  02110-1301  USA
*/

#include <Pt/Hmi/MenuItem.h>
#include <Pt/Hmi/Menu.h>
#include <Pt/Hmi/Application.h>
#include <Pt/Gfx/Painter.h>

namespace {

Pt::String shortcutText(const Pt::Hmi::Key& key)
{
    Pt::String text;

    bool hasCtrl = key.modifiers().has(Pt::Hmi::Key::Control);
    if(hasCtrl)
    {
        if( ! text.empty() )
            text += Pt::Char('+');

        text += Pt::Hmi::Key::toString(Pt::Hmi::Key::ControlKey);
    }
        
    bool hasAlt = key.modifiers().has(Pt::Hmi::Key::Alt);
    if(hasAlt)
    {
        if( ! text.empty() )
            text += Pt::Char('+');
            
        text += Pt::Hmi::Key::toString(Pt::Hmi::Key::AltKey);
    }

    bool hasMeta = key.modifiers().has(Pt::Hmi::Key::Meta);
    if(hasMeta)
    {
        if( ! text.empty() )
            text += Pt::Char('+');
            
        text += Pt::Hmi::Key::toString(Pt::Hmi::Key::MetaKey);
    }

    bool hasShift = key.modifiers().has(Pt::Hmi::Key::Shift);
    if(hasShift)
    {
        if( ! text.empty() )
            text += Pt::Char('+');
            
        text += Pt::Hmi::Key::toString(Pt::Hmi::Key::ShiftKey);
    }

    if( ! text.empty() )
        text += Pt::Char('+');

    text += Pt::Hmi::Key::toString( key.code() );

    return text;
}

} // namespace

namespace Pt {

namespace Hmi {

MenuItem::MenuItem()
: _menu(0)
, _iconWidth(0)
, _text("(empty)")
, _subMenu(0)
, _hasRenderer(false)
{

    setFocusPolicy(Widget::AcceptFocus);
    
    setPadding( Spacing(4, 2, 4, 2) );
    setMargin(0);
}


MenuItem::~MenuItem()
{
    if(_menu)
        _menu->removeItem(*this);
}


const String& MenuItem::text() const 
{
    return _text;
}


void MenuItem::setText(const Pt::String& t)
{
    _text = t;
    invalidate();
}


const Gfx::Image& MenuItem::icon() const
{
    return _icon;
}


void MenuItem::setIcon(const Gfx::Image& img)
{   
    _icon = img;
    invalidate();
}


double MenuItem::iconPadding() const
{
    return _iconWidth;
}


void MenuItem::setIconPadding(double w)
{
    _iconWidth = std::max<double>(w, _icon.width());
}


Menu* MenuItem::subMenu() const
{
    return _subMenu;
}


void MenuItem::setSubMenu(Menu& menu)
{
    _subMenu = &menu;
}


Signal<MenuItem&>& MenuItem::triggered()
{ 
    return _triggered; 
}


const Gfx::Brush& MenuItem::background() const
{
    return _background ? *_background
                       : Application::instance().styleOptions().background();
}


void MenuItem::setBackground(const Gfx::Brush& b)
{
    _background.reset( new Gfx::Brush(b) );
    invalidate();
}


const Gfx::Pen& MenuItem::contour() const
{
    return _contour ? *_contour
                    : Application::instance().styleOptions().contour();
}


void MenuItem::setContour(const Gfx::Pen& p)
{
    _contour.reset( new Gfx::Pen(p) );
    invalidate();
}


const Gfx::Color& MenuItem::textColor() const
{
    return _textColor ? *_textColor
                      : Application::instance().styleOptions().textColor();
}


void MenuItem::setTextColor(const Gfx::Color& color)
{
    _textColor.reset( new Gfx::Color(color) );
    invalidate();
}


const std::string& MenuItem::font() const
{
    return _fontName ? *_fontName
                     : Application::instance().styleOptions().font().name();
}


void MenuItem::setFont(const std::string& fontName)
{
    _fontName.reset( new std::string(fontName) );
    invalidate();
}


std::size_t MenuItem::fontSize() const
{

    return _fontSize ? *_fontSize
                     : Application::instance().styleOptions().font().size();
}


void MenuItem::setFontSize(const std::size_t s)
{
    _fontSize.reset( new std::size_t(s) );
    invalidate();
}


const std::string& MenuItem::fontStyle() const
{
    return _fontStyle ? *_fontStyle
                      : Application::instance().styleOptions().font().style();
}


void MenuItem::setFontStyle(const std::string& style)
{
    _fontStyle.reset( new std::string(style) );
    invalidate();
}


void MenuItem::setRenderer(MenuRenderer* renderer)
{
    _renderer.reset(renderer);
    _hasRenderer = renderer != 0;

    invalidate();
}


void MenuItem::onTriggered()
{   
    _triggered.send(*this);
}


void MenuItem::onParentChanged(View* v)
{
    if( ! v && _menu)
        _menu->removeItem(*this);
}


void MenuItem::onShortcut(const KeyEvent& kev)
{
    Base::onShortcut(kev);
    
    onTriggered();
}


Gfx::SizeF MenuItem::onAutoSize(const SizePolicy& policy) const
{
    Gfx::FontMetrics fm = PixmapSurface::fontMetrics(_font, _text);

    double contentHeight = std::max<Pt::ssize_t>( fm.height(), _icon.height() );
    double contentWidth = fm.width() + _picture.width();

    const Key* sk = shortcut();
    if(sk)
    {
        Pt::String text = shortcutText(*sk);
        contentWidth += fm.height() * 2.5; // spacing towards shortcut text
        contentWidth += PixmapSurface::fontMetrics(_font, text).width();
    }

    if(_subMenu)
        contentWidth += fm.height() * 4;

    return Gfx::SizeF( contentWidth + padding().leftRight(),
                       contentHeight + padding().topBottom() );
}


void MenuItem::onInvalidate()
{
    Base::onInvalidate();

    // TODO: use renderer and options from parent

    const StyleOptions& options = Application::instance().styleOptions();
    const Style& style = Application::instance().style();

    _brush = background();
    _pen = contour();
    _textPen = textColor();
    _font = Gfx::Font(font(), fontSize(), fontStyle());

    if( ! _hasRenderer )
        _renderer.reset( style.get<MenuRenderer>() );
    
    if( ! _renderer )
        return;

    _renderer->prepareItem(*this, options, _icon, 
                           _picture, _brush, _pen, _font, _textPen);
}


void MenuItem::onPaint(Gfx::PaintSurface& surface, const Gfx::RectF& rect)
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
    
    //
    // icon
    //
    double iconX = (iconPadding() - icon().width()) / 2;
    double iconY = (size().height() - icon().height()) / 2;

    Gfx::PointF iconPos(iconX, iconY);
    painter.setCompositionMode(Gfx::CompositionMode::SourceOver);
    painter.drawSurface(iconPos, _picture);
    painter.setCompositionMode(Gfx::CompositionMode::SourceCopy);

    //
    // item text
    //
    painter.setFont(_font);
    painter.setPen(_textPen);

    Gfx::FontMetrics fm = PixmapSurface::fontMetrics(_font, _text);
    double textX = padding().left() + _iconWidth;
    double textY = (size().height() - fm.height()) / 2;
    textY += fm.ascent();
    Gfx::PointF textPos(textX, textY);

    painter.drawText(textPos, _text);

    //
    // shortcut text
    //
    const Key* sk = shortcut();
    if(sk)
    {
        Pt::String skText = shortcutText(*sk);
        Gfx::FontMetrics skm = PixmapSurface::fontMetrics(_font, skText);

        double skX = size().width() - skm.width() - padding().right();
        double skY = (size().height() - skm.height()) / 2;
        skY += skm.ascent();
        Gfx::PointF skPos(skX, skY);

        painter.drawText(skPos, skText);
    }

    //
    // menu indicator
    //
    if( _subMenu)
        _renderer->renderIndicator(*this, options, painter, rect);
}


bool MenuItem::onMouseEvent(const MouseEvent& ev)
{
    Base::onMouseEvent(ev);

    bool inside = Gfx::RectF( size() ).contains( ev.position() );

    if( inside && ev.isRelease() )
    {
        onTriggered();
    }

    return true;
}


bool MenuItem::onTouchEvent(const TouchEvent& ev)
{    
    Base::onTouchEvent(ev);

    bool inside = Gfx::RectF( size() ).contains( ev.position() );
   
    if( inside && ev.isRelease() )
    {
        onTriggered();
    }

    return true;
}


bool MenuItem::onEnterEvent(const EnterEvent& ev)
{
    Base::onEnterEvent(ev);
    update();
    return true;
}


bool MenuItem::onLeaveEvent(const LeaveEvent& ev)
{
    Base::onLeaveEvent(ev);
    update();
    return true;
}


void MenuItem::onResizeEvent(const ResizeEvent& ev)
{
    Base::onResizeEvent(ev);
}

} // namespace

} // namespace
