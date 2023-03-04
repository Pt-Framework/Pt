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

MenuBarItem::MenuBarItem(Menu& menu, const Pt::String& text)
: _menu(menu)
, _hasRenderer(false)
{
    setFocusPolicy(Widget::AcceptFocus);
    setText(text);

    setPadding( Spacing(8, 8) );
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


Gfx::SizeF MenuBarItem::onMeasure(const SizePolicy& policy)
{
    Gfx::Painter _painter( surface() );
    _painter.setFont(_font);

    Gfx::FontMetrics fm = _painter.fontMetrics(_text);

    return Gfx::SizeF( fm.width() + padding().leftRight(), 
                       fm.height() + padding().topBottom() );
}


void MenuBarItem::onPaint(Gfx::PaintSurface& surface, const Gfx::RectF& rect)
{
    Base::onPaint(surface, rect);

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
    painter.setFont(_font);
    Gfx::FontMetrics fm = painter.fontMetrics(_text);
    double textX = padding().left();
    double textY = (size().height() - fm.height()) / 2;
    textY += fm.ascent();
    Gfx::PointF textPos(textX, textY);

    Gfx::RectF mnemonicRect; // TODO
    
    _renderer->renderItemText(*this, options, painter, rect,
                              _text, textPos, _font, _textPen, mnemonicRect);
}


bool MenuBarItem::onMouseEvent(const MouseEvent& ev)
{ 
    Base::onMouseEvent(ev);

    if( ev.isPress() )
    {
        _clicked.send(*this);
    }

    return true;
}


bool MenuBarItem::onTouchEvent(const TouchEvent& ev)
{ 
    Base::onTouchEvent(ev);

    if( ev.isPress() )
    {
        _clicked.send(*this);
    }

    return true;
}


bool MenuBarItem::onEnterEvent(const EnterEvent& ev)
{
    return Base::onEnterEvent(ev);
}


bool MenuBarItem::onLeaveEvent(const LeaveEvent& ev)
{
    return Base::onLeaveEvent(ev);
}

///////////////////////////////////////////////////////////////////////////////
// MenuBar
///////////////////////////////////////////////////////////////////////////////

MenuBar::MenuBar()
: _layout(FlowLayout::Left)
, _currentMenu(0)
, _hasRenderer(false)
{
    //this->setBackground( Gfx::Color(58981, 58981, 58981) );
    //this->setBorderColor( Gfx::Color(32767, 32767, 32767)  );
    //this->setBorderStyle(Panel::NoBorder);

    //_layout.setPadding(4);

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


void MenuBar::onAddMenu(Menu& menu, const Pt::String& text)
{
    MenuBarItem* item = new MenuBarItem(menu, text);
    
    _menus.push_back(item);
    _layout.addItem(*item);

    menu.setAnchor(this);

    item->clicked() += Pt::slot(*this, &MenuBar::onItemClicked);
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

            menu.setAnchor(0);
            break;
        }
    }

    if(_currentMenu == &menu)
    {
        _currentMenu = 0;
        //setCapture(false);
    }
}


Visual* MenuBar::onFindMenu(const Gfx::PointF& screenPos)
{ 
    if( ! isVisible() )
        return 0;

    Gfx::PointF pos = this->fromGlobal(screenPos);

    Gfx::RectF rect( Gfx::PointF(0,0), size() );
    if( rect.contains( pos ) )
        return this;

    if( ! _currentMenu)
        return 0;

    return _currentMenu->findMenu(screenPos);
}


void MenuBar::onOpenMenu(Menu& menu)
{
    if(_currentMenu)
        _currentMenu->close();
    
    _currentMenu = &menu;
    //setCapture(true);
}


void MenuBar::onCloseMenu(Menu& menu)
{
    if(_currentMenu == &menu)
    {
        _currentMenu = 0;
        //setCapture(false);
    }
}


void MenuBar::onCancel()
{
    if(_currentMenu)
        _currentMenu->cancel();
}


void MenuBar::onItemClicked(MenuBarItem& item)
{
    Menu& menu = item.menu();

    if( ! menu.isVisible() )
    {     
        Gfx::PointF menuPos( 0, item.size().height() );
        menuPos = item.toGlobal(menuPos);
        menu.move(menuPos);

        SizePolicy policy(SizePolicy::Preferred, SizePolicy::Preferred);
        menu.resizeToFit(policy);

        menu.setAbove(true);
        menu.show();
    }
    else
    {
        menu.close();
    }
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


Gfx::SizeF MenuBar::onMeasure(const SizePolicy& policy)
{
    double hspace = padding().leftRight() + _layout.margin().leftRight();
    double vspace = padding().topBottom() + _layout.margin().topBottom();

    SizePolicy contentPolicy = policy;
    contentPolicy.setWidth( policy.size().width() - hspace );
    contentPolicy.setHeight( policy.size().height() - vspace );
        
    _layout.measure(contentPolicy);
    return _layout.preferredSize();
}


void MenuBar::onLayout(const Gfx::RectF& rect)
{
    Base::onLayout(rect);
    

    Gfx::PointF pos(padding().left() + _layout.margin().left(), 
                    padding().top()  + _layout.margin().top());
        
    double hspace = padding().leftRight() + _layout.margin().leftRight();
    double vspace = padding().topBottom() + _layout.margin().topBottom();

    Gfx::SizeF size;
    size.setWidth( rect.width() - hspace );
    size.setHeight( rect.height() - vspace );

    _layout.move(pos);
    _layout.resize(size);
}


void MenuBar::onPaint(Gfx::PaintSurface& surface, const Gfx::RectF& rect)
{
    Base::onPaint(surface, rect);

    const StyleOptions& options = Application::instance().styleOptions();

    if( ! _renderer )
        return;

    Gfx::Painter painter(surface);
    painter.setClip(rect);

    _renderer->renderBackground(*this, options, painter, rect, 
                                _brush, _pen);
}


void MenuBar::onProcessMouseEvent(const MouseEvent& ev)
{
    const Gfx::PointF& screenPos = ev.position();
    Visual* menu = findMenu(screenPos);
    if(menu)
    {
        if(menu == this)
            Base::onProcessMouseEvent(ev);
        else
            menu->processEvent(ev);
        
        return;
    }

    if( ev.isPress() )
    {
        cancel();
        return;
    }
}


bool MenuBar::onMouseEvent(const MouseEvent& ev)
{ 
    if( _currentMenu && ev.isPress() )
    {
        _currentMenu->close();
    }

    return Base::onMouseEvent(ev);
}


bool MenuBar::onTouchEvent(const TouchEvent& ev)
{ 
    if( _currentMenu && ev.isPress() )
    {
        _currentMenu->close();
    }

    return Base::onTouchEvent(ev);
}

} // namespace

} // namespace
