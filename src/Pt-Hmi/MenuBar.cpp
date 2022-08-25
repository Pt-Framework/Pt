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
   MA 02110-1301 USA
*/

#include <Pt/Hmi/MenuBar.h>
#include <Pt/Hmi/Menu.h>
#include <Pt/Gfx/Painter.h>
#include <Pt/Hmi/Window.h>
#include <Pt/Hmi/Application.h>
#include <Pt/Hmi/StyleOptions.h>

namespace Pt {

namespace Hmi {

///////////////////////////////////////////////////////////////////////////////
// MenuBarItem
///////////////////////////////////////////////////////////////////////////////

MenuBarItem::MenuBarItem(MenuBar& mb, Menu& menu, const Pt::String& text)
: _menuBar(mb)
, _menu(menu)
, _hasRenderer(false)
{
    setFocusPolicy(Widget::AcceptFocus);
    setText(text);

    setPadding( Spacing(8, 0, 8, 0) );
    setMargin(0);
}


MenuBarItem::~MenuBarItem()
{
}


const String& MenuBarItem::text() const 
{
    return _text;
}


void MenuBarItem::setText(const Pt::String& t)
{   
    _text = t;
    invalidate();
}


void MenuBarItem::toggle()
{
    if( ! _menu.isVisible() )
    {
        open();
        //grabPointer();
    }
    else
    {
        close();
        //releasePointer();
     }      
}


void MenuBarItem::open()
{
    Gfx::PointF menuPos(0, size().height());
    menuPos = toScreen(menuPos);

    _menu.show(menuPos);
}


void MenuBarItem::close()
{
    _menu.close();       
}


Gfx::SizeF MenuBarItem::onAutoSize(const SizePolicy& policy) const
{
    Gfx::FontMetrics fm = PixmapSurface::fontMetrics(_font, _text);

    return Gfx::SizeF( fm.width() + padding().leftRight(), 
                       fm.height() + padding().topBottom() );
}


bool MenuBarItem::onMouseEvent(const MouseEvent& ev)
{ 
    Base::onMouseEvent(ev);

    bool inside = Gfx::RectF( size() ).contains( ev.position() );

    if( inside && ev.isRelease() )
    {
        toggle();
    }

    // nothing to highlight if outside
    if( inside )
        return true;

    // navigate to sibling item if a sub menu is open
    if( _menuBar.selectedMenu() )
    {
        Gfx::PointF pos = _menuBar.fromWidget( *this, ev.position() );
        MenuBarItem* item = _menuBar.findItem(pos);
        if(item)
        {
            toggle();
            
            item->toggle();
            return true;
        }
    }

    // navigate to open sub menu
    Gfx::PointF screenPos = toScreen( ev.position() );
    MenuShell* menu = _menu.findMenu(screenPos);   
    if(menu)
    {   
        //releasePointer();
        return true;
    }

    // cancel when clicked outside any menu item
    if( ev.isPress() )
    {
        //releasePointer();
        _menu.cancel();
        return true;
    }

    return true;
}


const Gfx::Brush& MenuBarItem::background() const
{
    return _background ? *_background
                       : Application::instance().styleOptions().background();
}


void MenuBarItem::setBackground(const Gfx::Brush& b)
{
    _background.reset( new Gfx::Brush(b) );
    invalidate();
}


const Gfx::Pen& MenuBarItem::contour() const
{
    return _contour ? *_contour
                    : Application::instance().styleOptions().contour();
}


void MenuBarItem::setContour(const Gfx::Pen& p)
{
    _contour.reset( new Gfx::Pen(p) );
    invalidate();
}


const Gfx::Color& MenuBarItem::textColor() const
{
    return _textColor ? *_textColor
                      : Application::instance().styleOptions().textColor();
}


void MenuBarItem::setTextColor(const Gfx::Color& color)
{
    _textColor.reset( new Gfx::Color(color) );
    invalidate();
}


const std::string& MenuBarItem::font() const
{
    return _fontName ? *_fontName
                     : Application::instance().styleOptions().font().name();
}


void MenuBarItem::setFont(const std::string& fontName)
{
    _fontName.reset( new std::string(fontName) );
    invalidate();
}


std::size_t MenuBarItem::fontSize() const
{

    return _fontSize ? *_fontSize
                     : Application::instance().styleOptions().font().size();
}


void MenuBarItem::setFontSize(const std::size_t s)
{
    _fontSize.reset( new std::size_t(s) );
    invalidate();
}


const std::string& MenuBarItem::fontStyle() const
{
    return _fontStyle ? *_fontStyle
                      : Application::instance().styleOptions().font().style();
}


void MenuBarItem::setFontStyle(const std::string& style)
{
    _fontStyle.reset( new std::string(style) );
    invalidate();
}


void MenuBarItem::setRenderer(MenuBarRenderer* renderer)
{
    _renderer.reset(renderer);
    _hasRenderer = renderer != 0;

    invalidate();
}


void MenuBarItem::onInvalidate()
{
    Base::onInvalidate();

    const StyleOptions& options = Application::instance().styleOptions();
    const Style& style = Application::instance().style();

    _brush = background();
    _pen = contour();
    _textPen = textColor();
    _font = Gfx::Font(font(), fontSize(), fontStyle());

    if( ! _hasRenderer )
        _renderer.reset( style.get<MenuBarRenderer>() );
    
    if( ! _renderer )
        return;

    _renderer->prepareItem(*this, options, _brush, _pen, _font, _textPen);
}


void MenuBarItem::onPaint(Gfx::PaintSurface& surface, const Gfx::RectF& rect)
{
    const StyleOptions& options = Application::instance().styleOptions();
    
    if( ! _renderer )
        return;

    Gfx::Painter painter(surface);
    painter.setClip(rect);

    //
    // item background
    //
    _renderer->renderItem(*this, options, painter, rect, 
                          _brush, _pen);

    //
    // item text
    //
    Gfx::FontMetrics fm = painter.fontMetrics(_text);
    double textX = padding().left();
    double textY = (size().height() - fm.height()) / 2;
    textY += fm.ascent();
    Gfx::PointF textPos(textX, textY);

    Gfx::RectF mnemonicRect; // TODO
    
    _renderer->renderItemText(*this, options, painter, rect,
                              _text, textPos, _font, _textPen, mnemonicRect);
}


bool MenuBarItem::onEnterEvent(const EnterEvent& ev)
{
    Base::onEnterEvent(ev);
    update();
    return true;
}


bool MenuBarItem::onLeaveEvent(const LeaveEvent& ev)
{
    Base::onLeaveEvent(ev);
    update();
    return true;
}

///////////////////////////////////////////////////////////////////////////////
// MenuBar
///////////////////////////////////////////////////////////////////////////////

MenuBar::MenuBar()
: _layout(FlowLayout::Left)
, _currentMenu(0)
, _currentMenuItem(0)
, _hasRenderer(false)
{
    //this->setBackground( Gfx::Color(58981, 58981, 58981) );
    //this->setBorderColor( Gfx::Color(32767, 32767, 32767)  );
    //this->setBorderStyle(Panel::NoBorder);

    //_layout.move( Gfx::PointF(0,0) );
    _layout.setPadding(1);

    add(_layout);
}


MenuBar::~MenuBar()
{
    std::vector<MenuBarItem*>::iterator it;
    for(it = _menus.begin(); it != _menus.end(); ++it)
    {
        delete *it;
    }
}


Menu* MenuBar::selectedMenu()
{ 
    return _currentMenu; 
}


MenuBarItem* MenuBar::findItem(const Gfx::PointF& pos)
{
    MenuBarItem* item = 0;

    std::vector<MenuBarItem*>::iterator it;
    for(it = _menus.begin(); it != _menus.end(); ++it)
    {
        if( (*it)->geometry().contains(pos) )
        {
            item = *it;
            break;
        }
    }

    return item;
}


void MenuBar::onAddMenu(Menu& menu, const Pt::String& text)
{
    MenuBarItem* item = new MenuBarItem(*this, menu, text);
//TODO:    item->resize( Gfx::SizeF(50, 0) );

    _menus.push_back(item);
    _layout.addItem(*item);

    update();
}


void MenuBar::onRemoveMenu(Menu& menu)
{
    std::vector<MenuBarItem*>::iterator it;
    for(it = _menus.begin(); it != _menus.end(); ++it)
    {
        if( &(*it)->menu() == &menu )
        {           
            delete *it;
            _menus.erase(it);
            break;
        }
    }

    if(_currentMenu == &menu)
    {
        _currentMenu = 0;
        _currentMenuItem = 0;
    }

    update();
}


void MenuBar::onOpenMenu(Menu& menu)
{
    std::vector<MenuBarItem*>::iterator it;
    for(it = _menus.begin(); it != _menus.end(); ++it)
    {
        if( &(*it)->menu() == &menu)
        {
            _currentMenu = &menu;
            _currentMenuItem = *it;
        }
    }
}


void MenuBar::onCloseMenu(Menu& menu)
{
    if(_currentMenu == &menu)
    {
        _currentMenu = 0;
        _currentMenuItem = 0;
    }
}


void MenuBar::onCancel()
{
    if(_currentMenu)
        _currentMenu->cancel();
}


void MenuBar::onEnter()
{
    //if(_currentMenuItem)
    //    _currentMenuItem->grabPointer();
}


MenuShell* MenuBar::onFindMenu(const Gfx::PointF& screenPos)
{ 
    if( ! isVisible() )
        return 0;

    Gfx::PointF pos = this->fromScreen(screenPos);

    Gfx::RectF rect( Gfx::PointF(0,0), size() );
    if( rect.contains( pos ) )
        return this;

    if( ! _currentMenu)
        return 0;

    return _currentMenu->findMenu(screenPos);
}


const Gfx::Brush& MenuBar::background() const
{
    return _background ? *_background
                       : Application::instance().styleOptions().background();
}


void MenuBar::setBackground(const Gfx::Brush& b)
{
    _background.reset( new Gfx::Brush(b) );
    invalidate();
}


const Gfx::Pen& MenuBar::contour() const
{
    return _contour ? *_contour
                    : Application::instance().styleOptions().contour();
}


void MenuBar::setContour(const Gfx::Pen& p)
{
    _contour.reset( new Gfx::Pen(p) );
    invalidate();
}


void MenuBar::setRenderer(MenuBarRenderer* renderer)
{
    _renderer.reset(renderer);
    _hasRenderer = renderer != 0;

    invalidate();
}


void MenuBar::onInvalidate()
{
    Base::onInvalidate();

    const StyleOptions& options = Application::instance().styleOptions();
    const Style& style = Application::instance().style();
    
    _brush = background();
    _pen = contour();

    if( ! _hasRenderer )
        _renderer.reset( style.get<MenuBarRenderer>() );
    
    if( ! _renderer )
        return;

    _renderer->prepare(*this, options, _brush, _pen);
}


void MenuBar::onPaint(Gfx::PaintSurface& surface, const Gfx::RectF& rect)
{
    const StyleOptions& options = Application::instance().styleOptions();

    if( ! _renderer )
        return;

    Gfx::Painter painter(surface);
    painter.setClip(rect);

    _renderer->renderBackground(*this, options, painter, rect, 
                                _brush, _pen);
}


bool MenuBar::onMouseEvent(const MouseEvent& ev)
{ 
    return Base::onMouseEvent(ev);
}


void MenuBar::onResizeEvent(const ResizeEvent& ev)
{
    for(std::size_t i = 0; i < _layout.widgets().size(); ++i)
    {
        Widget* item = _layout.widgets().at(i);
        Gfx::SizeF itemSize = item->preferredSize();
 //TODO:       item->resize(itemSize);
    }

 //TODO:   _layout.resize( ev.size() );

    // _layout positions the items now in OnResizeEvent
    // TODO: our overall design should make this clearer
    Base::onResizeEvent(ev);
}

} // namespace

} // namespace
