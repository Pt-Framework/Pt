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

#include <Pt/Hmi/ComboBox.h>
#include <Pt/Hmi/Painter.h>

namespace Pt {

namespace Hmi {

/////////////////////////////////////////////////////////////////////////////
// ComboBoxMenu
/////////////////////////////////////////////////////////////////////////////

ComboBoxMenu::ComboBoxMenu()
: Window(0, Window::Popup)
{
    setMainWidget(&_items);

    _items.selected() += Pt::slot(*this, &ComboBoxMenu::onItemSelected);
}

		
ComboBoxMenu::~ComboBoxMenu()
{
}


void ComboBoxMenu::addItem(ListBoxItem& item)
{   
    _items.addItem(item);
}


void ComboBoxMenu::onItemSelected(ListBoxItem&)
{
    close();
}


void ComboBoxMenu::setScrollBars(bool hasScrollBars)
{
    _items.setScrollBars(hasScrollBars);
}


Pt::Signal<ListBoxItem&>& ComboBoxMenu::selected()
{
    return _items.selected();
}


void ComboBoxMenu::onPaintBackground(const Gfx::RectF& rect)
{
    Base::onPaintBackground(rect);

    const StyleOptions& options = Application::instance().styleOptions();

    Painter painter( surface() );
    painter.setClip(rect);

    //
    // menu border
    //
    Gfx::RectF borderRect(size());
    painter.setPen( options.contour() );
    painter.drawRect(borderRect);
}


void ComboBoxMenu::onMouseEvent(const MouseEvent& ev)
{
    Base::onMouseEvent(ev);

    Gfx::RectF rect( Gfx::PointF(0,0), size() );
    if( rect.contains( ev.position() ) )
        return;

    if( ev.isPress() )
    {
        close();          
    }
}


void ComboBoxMenu::onTouchEvent(const TouchEvent& ev)
{
    Base::onTouchEvent(ev);

    Gfx::RectF rect( Gfx::PointF(0,0), size() );
    if( rect.contains( ev.position() ) )
        return;

    if( ev.isPress() )
    {
        close();          
    }
}


void ComboBoxMenu::onShowEvent(const ShowEvent& ev)
{
    Base::onShowEvent(ev);

    if( ! ev.visible() )
    {
        releasePointer();
    }
    else
    {
        grabPointer();
    }
}

/////////////////////////////////////////////////////////////////////////////
// ComboBox
/////////////////////////////////////////////////////////////////////////////

ComboBox::ComboBox()
{
    setTextInput(true);
    setFocusPolicy(Widget::NormalFocus);

    _item1.resize( Gfx::SizeF(20, 25) );
    _item1.setText("Item 1");
    _item2.resize( Gfx::SizeF(20, 25) );
    _item2.setText("Item 2");
    _item3.resize( Gfx::SizeF(20, 25) );
    _item3.setText("Item 3");
    _item4.resize( Gfx::SizeF(20, 25) );
    _item4.setText("Item 4");
    _item5.resize( Gfx::SizeF(20, 25) );
    _item5.setText("Item 5");
    _item6.resize( Gfx::SizeF(20, 25) );
    _item6.setText("Item 6");
    _item7.resize( Gfx::SizeF(20, 25) );
    _item7.setText("Item 7");
    _item8.resize( Gfx::SizeF(20, 25) );
    _item8.setText("Item 8");

    _menu.addItem(_item1);
    _menu.addItem(_item2);
    _menu.addItem(_item3);
    _menu.addItem(_item4);
    _menu.addItem(_item5);
    _menu.addItem(_item6);
    _menu.addItem(_item7);
    _menu.addItem(_item8);
    _menu.eventReady() += Pt::slot(*this, &ComboBox::onMenuKeyEvent);
    _menu.selected() += Pt::slot(*this, &ComboBox::onItemSelected);
}


ComboBox::~ComboBox()
{
}


void ComboBox::addItem(ListBoxItem& item)
{   
    _menu.addItem(item);
}


void ComboBox::onItemSelected(ListBoxItem& item)
{
    _text = item.text();
    invalidate();
}


void ComboBox::setScrollBars(bool hasScrollBars)
{
    _menu.setScrollBars(hasScrollBars);
}


void ComboBox::onInvalidate()
{
    Base::onInvalidate();
}


void ComboBox::onPaint(PaintSurface& surface, const Gfx::RectF& rect)
{
    const StyleOptions& options = Application::instance().styleOptions();

    Painter painter(surface);
    painter.setClip(rect);
    
    Gfx::Pen pen  = options.contour();
    if(isHighlighted() || this->hasFocus() )
        pen = options.accentColor();

    Gfx::Pen textPen = options.textColor();
    Gfx::Brush brush = options.textBackground();
    Gfx::Font font   = options.font();
    
    painter.setPen(pen);
    painter.setBrush(brush);
    painter.setFont(font);

    painter.fillRect(rect);
    painter.drawRect(rect);

    Gfx::FontMetrics fm = painter.fontMetrics(font, _text);

    Gfx::PointF textPos( 5, fm.ascent() + 5);
    painter.setPen(textPen);
    painter.drawText(textPos, _text);
}


void ComboBox::onMenuKeyEvent(const KeyEvent& ev)
{
    if( ! ev.isPress() )
        return;

    Pt::Char ch = ev.unicode();

    if( ! Pt::isprint(ch) )
        return;

    _text += ch;
    invalidate();
}


void ComboBox::onKeyEvent(const KeyEvent& ev)
{
    Base::onKeyEvent(ev);

    if( ! ev.isPress() )
        return;

    Pt::Char ch = ev.unicode();

    if( ! Pt::isprint(ch) )
        return;

    _text += ch;
    invalidate();
}


void ComboBox::onMouseEvent(const MouseEvent& ev)
{    
    Base::onMouseEvent(ev);

    if( ev.isPress() )
    {
        onOpenCombo();
    }
}


void ComboBox::onTouchEvent(const TouchEvent& ev)
{    
    Base::onTouchEvent(ev);

    if( ev.isPress() )
    {
        onOpenCombo();
    }
}


void ComboBox::onOpenCombo()
{
    _menu.resize( Gfx::SizeF(size().width(), 120) );
    _menu.setTopMost(true);

    Gfx::PointF pos(0, size().height());
    pos = this->toScreen(pos);
    _menu.move(pos);

    _menu.show();
    _menu.activate();
}


bool ComboBox::onScrollEvent(const ScrollEvent& ev)
{
    // TODO: scroll through item list
    return false;
}

} // namespace

} // namespace


