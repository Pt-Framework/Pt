/* Copyright (C) 2015 Marc Boris Duerner
   Copyright (C) 2015 Laurentiu-Gheorghe Crisan

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
#include <Pt/Hmi/MenuBaseItem.h>
#include <Pt/Hmi/Menu.h>
#include <Pt/Hmi/Application.h>
#include <Pt/Gfx/Painter.h>


namespace Pt {
namespace Hmi {

MenuBaseItem::MenuBaseItem()
: _iconWidth(0)
, _text("(empty)")
, _hasSeparator(false)
{
    setFocusPolicy(Widget::AcceptFocus);
    setPadding(Pt::Hmi::Spacing(8, 8));
    setMargin(0);
}

Pt::String MenuBaseItem::shortcutText(const Pt::Hmi::Key& key)
{
    Pt::String text;

    bool hasCtrl = key.modifiers().has(Pt::Hmi::Key::Control);
    if (hasCtrl)
    {
        if (!text.empty())
            text += Pt::Char('+');

        text += Pt::Hmi::Key::toString(Pt::Hmi::Key::ControlKey);
    }

    bool hasAlt = key.modifiers().has(Pt::Hmi::Key::Alt);
    if (hasAlt)
    {
        if (!text.empty())
            text += Pt::Char('+');

        text += Pt::Hmi::Key::toString(Pt::Hmi::Key::AltKey);
    }

    bool hasMeta = key.modifiers().has(Pt::Hmi::Key::Meta);
    if (hasMeta)
    {
        if (!text.empty())
            text += Pt::Char('+');

        text += Pt::Hmi::Key::toString(Pt::Hmi::Key::MetaKey);
    }

    bool hasShift = key.modifiers().has(Pt::Hmi::Key::Shift);
    if (hasShift)
    {
        if (!text.empty())
            text += Pt::Char('+');

        text += Pt::Hmi::Key::toString(Pt::Hmi::Key::ShiftKey);
    }

    if (!text.empty())
        text += Pt::Char('+');

    text += Pt::Hmi::Key::toString(key.code());

    return text;
}


MenuBaseItem::~MenuBaseItem()
{
}


const Pt::String& MenuBaseItem::text() const
{
    return _text;
}


void MenuBaseItem::setText(const Pt::String& t)
{
    _text = t;
    invalidate();
}


const Pt::Gfx::Image& MenuBaseItem::icon() const
{
    return _icon;
}


void MenuBaseItem::setIcon(const Pt::Gfx::Image& img)
{
    _icon = img;
    invalidate();
}


double MenuBaseItem::iconPadding() const
{
    return _iconWidth;
}


void MenuBaseItem::setIconPadding(double w)
{
    _iconWidth = std::max<double>(w, _icon.width());
}


Pt::Signal<MenuBaseItem&>& MenuBaseItem::triggered()
{
    return _triggered;
}


const Pt::Gfx::Brush& MenuBaseItem::background() const
{
    return _background ? *_background
        : Pt::Hmi::Application::instance().styleOptions().background();
}


void MenuBaseItem::setBackground(const Pt::Gfx::Brush& b)
{
    _background.reset(new Pt::Gfx::Brush(b));
    invalidate();
}


const Pt::Gfx::Pen& MenuBaseItem::contour() const
{
    return _contour ? *_contour
        : Pt::Hmi::Application::instance().styleOptions().contour();
}


void MenuBaseItem::setContour(const Pt::Gfx::Pen& p)
{
    _contour.reset(new Pt::Gfx::Pen(p));
    invalidate();
}


const Pt::Gfx::Color& MenuBaseItem::textColor() const
{
    return _textColor ? *_textColor
        : Pt::Hmi::Application::instance().styleOptions().textColor();
}


void MenuBaseItem::setTextColor(const Pt::Gfx::Color& color)
{
    _textColor.reset(new Pt::Gfx::Color(color));
    invalidate();
}


const std::string& MenuBaseItem::font() const
{
    return _fontName ? *_fontName
        : Pt::Hmi::Application::instance().styleOptions().font().name();
}


void MenuBaseItem::setFont(const std::string& fontName)
{
    _fontName.reset(new std::string(fontName));
    invalidate();
}


std::size_t MenuBaseItem::fontSize() const
{
    return _fontSize ? *_fontSize
        : Pt::Hmi::Application::instance().styleOptions().font().size();
}


void MenuBaseItem::setFontSize(const std::size_t s)
{
    _fontSize.reset(new std::size_t(s));
    invalidate();
}


const std::string& MenuBaseItem::fontStyle() const
{
    return _fontStyle ? *_fontStyle
        : Pt::Hmi::Application::instance().styleOptions().font().style();
}


void MenuBaseItem::setFontStyle(const std::string& style)
{
    _fontStyle.reset(new std::string(style));
    invalidate();
}



void MenuBaseItem::onTriggered()
{
    _triggered.send(*this);
}



void MenuBaseItem::onShortcut(const Pt::Hmi::KeyEvent& kev)
{
    Base::onShortcut(kev);

    onTriggered();
}


void MenuBaseItem::onInvalidate()
{
    Base::onInvalidate();

    // TODO: use renderer and options from parent

    const Pt::Hmi::StyleOptions& options = Pt::Hmi::Application::instance().styleOptions();
    const Pt::Hmi::Style& style = Pt::Hmi::Application::instance().style();

    _brush = background();
    _pen = contour();
    _textPen = textColor();
    _font = Pt::Gfx::Font(font(), fontSize(), fontStyle());

    _picture.set(_icon);

    if (isHighlighted())
        _brush = options.highlightColor();
}


Pt::Gfx::SizeF MenuBaseItem::onMeasure(const Pt::Hmi::SizePolicy& policy)
{
    Pt::Gfx::Painter _painter(surface());
    _painter.setFont(_font);

    Pt::Gfx::FontMetrics fm = _painter.fontMetrics(_text);

    double contentHeight = std::max<Pt::ssize_t>(fm.height(), _icon.height());
    double contentWidth = fm.width() + surface().toLogical(_picture.size().width());

    const Pt::Hmi::Key* sk = shortcut();
    if (sk)
    {
        Pt::String text = shortcutText(*sk);
        contentWidth += fm.height() * 2.5; // spacing towards shortcut text
        contentWidth += _painter.fontMetrics(text).width();
    }

//    if (_hasSubMenu)
//        contentWidth += fm.height() * 4;

    return Pt::Gfx::SizeF(contentWidth + padding().leftRight(),
        contentHeight + padding().topBottom());
}


void MenuBaseItem::onPaint(Pt::Gfx::PaintSurface& surface, const Pt::Gfx::RectF& rect)
{
    const Pt::Hmi::StyleOptions& options = Pt::Hmi::Application::instance().styleOptions();


    Pt::Gfx::Painter painter(surface);
    painter.setClip(rect);


    // background
    bool highlight = this->isHighlighted();
    if (highlight)
    {
        painter.setBrush(_brush);
        painter.fillRect(rect);
    }


    // icon    
    double iconX = (iconPadding() - icon().width()) / 2;
    double iconY = (size().height() - icon().height()) / 2;

    Pt::Gfx::PointF iconPos(iconX, iconY);
    painter.setCompositionMode(Pt::Gfx::CompositionMode::SourceOver);
    painter.drawSurface(iconPos, _picture);
    painter.setCompositionMode(Pt::Gfx::CompositionMode::SourceCopy);


    // item text    
    painter.setFont(_font);
    painter.setPen(_textPen);

    Pt::Gfx::FontMetrics fm = painter.fontMetrics(_text);
    double textX = padding().left() + _iconWidth;
    double textY = (size().height() - fm.height()) / 2;
    textY += fm.ascent();
    Pt::Gfx::PointF textPos(textX, textY);

    painter.drawText(textPos, _text);


    // shortcut text    
    const Pt::Hmi::Key* sk = shortcut();
    if (sk)
    {
        Pt::String skText = shortcutText(*sk);
        Pt::Gfx::FontMetrics skm = painter.fontMetrics(skText);

        double skX = size().width() - skm.width() - padding().right();
        double skY = (size().height() - skm.height()) / 2;
        skY += skm.ascent();
        Pt::Gfx::PointF skPos(skX, skY);

        painter.drawText(skPos, skText);
    }
  
    // separator
    if (_hasSeparator)
    {
        Pt::Gfx::PointF from(textX, size().height());
        Pt::Gfx::PointF to(size().width(), size().height());

        painter.setPen(this->contour());
        painter.drawLine(from, to);
    }
}


bool MenuBaseItem::onMouseEvent(const Pt::Hmi::MouseEvent& ev)
{
    Base::onMouseEvent(ev);

    bool inside = Pt::Gfx::RectF(size()).contains(ev.position());

    if (inside && ev.isRelease())
    {
        onTriggered();
    }

    return true;
}


bool MenuBaseItem::onTouchEvent(const Pt::Hmi::TouchEvent& ev)
{
    Base::onTouchEvent(ev);

    bool inside = Pt::Gfx::RectF(size()).contains(ev.position());

    if (inside && ev.isRelease())
    {
        onTriggered();
    }

    return true;
}


bool MenuBaseItem::onEnterEvent(const Pt::Hmi::EnterEvent& ev)
{
    return Base::onEnterEvent(ev);
}


bool MenuBaseItem::onLeaveEvent(const Pt::Hmi::LeaveEvent& ev)
{
    return Base::onLeaveEvent(ev);
}

}}
