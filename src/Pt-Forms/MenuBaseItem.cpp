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
#include <Pt/Forms/MenuBaseItem.h>
#include <Pt/Forms/Menu.h>
#include <Pt/Forms/Application.h>
#include <Pt/Gfx/Painter.h>


namespace Pt {
namespace Forms {

MenuBaseItem::MenuBaseItem()
: _iconWidth(0)
, _text("(empty)")
, _hasSeparator(false)
, _isHighlighted(false)
{
    setFocusPolicy(Control::AcceptFocus);
    setPadding(Pt::Forms::Spacing(8, 8));
    setMargin(0);
}

Pt::String MenuBaseItem::shortcutText(const Pt::Forms::Key& key)
{
    Pt::String text;

    bool hasCtrl = key.modifiers().has(Pt::Forms::Key::Control);
    if (hasCtrl)
    {
        if (!text.empty())
            text += Pt::Char('+');

        text += Pt::Forms::Key::toString(Pt::Forms::Key::ControlKey);
    }

    bool hasAlt = key.modifiers().has(Pt::Forms::Key::Alt);
    if (hasAlt)
    {
        if (!text.empty())
            text += Pt::Char('+');

        text += Pt::Forms::Key::toString(Pt::Forms::Key::AltKey);
    }

    bool hasMeta = key.modifiers().has(Pt::Forms::Key::Meta);
    if (hasMeta)
    {
        if (!text.empty())
            text += Pt::Char('+');

        text += Pt::Forms::Key::toString(Pt::Forms::Key::MetaKey);
    }

    bool hasShift = key.modifiers().has(Pt::Forms::Key::Shift);
    if (hasShift)
    {
        if (!text.empty())
            text += Pt::Char('+');

        text += Pt::Forms::Key::toString(Pt::Forms::Key::ShiftKey);
    }

    if (!text.empty())
        text += Pt::Char('+');

    text += Pt::Forms::Key::toString(key.code());

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
        : Pt::Forms::Application::instance().styleOptions().background();
}


void MenuBaseItem::setBackground(const Pt::Gfx::Brush& b)
{
    _background.reset(new Pt::Gfx::Brush(b));
    invalidate();
}


const Pt::Gfx::Pen& MenuBaseItem::contour() const
{
    return _contour ? *_contour
        : Pt::Forms::Application::instance().styleOptions().contour();
}


void MenuBaseItem::setContour(const Pt::Gfx::Pen& p)
{
    _contour.reset(new Pt::Gfx::Pen(p));
    invalidate();
}


const Pt::Gfx::ColorF& MenuBaseItem::textColor() const
{
    return _textColor ? *_textColor
        : Pt::Forms::Application::instance().styleOptions().textColor();
}


void MenuBaseItem::setTextColor(const Pt::Gfx::ColorF& color)
{
    _textColor.reset(new Pt::Gfx::ColorF(color));
    invalidate();
}


const std::string& MenuBaseItem::font() const
{
    return _fontName ? *_fontName
        : Pt::Forms::Application::instance().styleOptions().font().name();
}


void MenuBaseItem::setFont(const std::string& fontName)
{
    _fontName.reset(new std::string(fontName));
    invalidate();
}


std::size_t MenuBaseItem::fontSize() const
{
    return _fontSize ? *_fontSize
        : Pt::Forms::Application::instance().styleOptions().font().size();
}


void MenuBaseItem::setFontSize(const std::size_t s)
{
    _fontSize.reset(new std::size_t(s));
    invalidate();
}


const std::string& MenuBaseItem::fontStyle() const
{
    return _fontStyle ? *_fontStyle
        : Pt::Forms::Application::instance().styleOptions().font().style();
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



void MenuBaseItem::onShortcut(const Pt::Forms::Key& key)
{
    Base::onShortcut(key);

    onTriggered();
}


void MenuBaseItem::onInvalidate()
{
    Base::onInvalidate();

    // TODO: use renderer and options from parent

    const Pt::Forms::StyleOptions& options = Pt::Forms::Application::instance().styleOptions();
    const Pt::Forms::Style& style = Pt::Forms::Application::instance().style();

    _brush = background();
    _pen = contour();
    _textPen = textColor();
    _font = Pt::Gfx::Font(font(), fontSize(), fontStyle());

    _picture.reset(_icon);

    if(_isHighlighted)
        _brush = options.highlightColor();
}


Pt::Gfx::SizeF MenuBaseItem::onMeasure(const Pt::Forms::SizePolicy& policy)
{
    Pt::Gfx::Painter _painter(surface());
    _painter.setFont(_font);

    Pt::Gfx::TextMetrics fm = _painter.textMetrics(_text);

    double contentHeight = std::max<Pt::ssize_t>(fm.height(), _icon.height());
    double contentWidth = fm.width() + scaling().toLogical(_picture.size().width());

    const Pt::Forms::Key* sk = shortcut();
    if (sk)
    {
        Pt::String text = shortcutText(*sk);
        contentWidth += fm.height() * 2.5; // spacing towards shortcut text
        contentWidth += _painter.textMetrics(text).width();
    }

//    if (_hasSubMenu)
//        contentWidth += fm.height() * 4;

    return Pt::Gfx::SizeF(contentWidth + padding().leftRight(),
        contentHeight + padding().topBottom());
}


void MenuBaseItem::onPaint(PaintSurface& surface, const Pt::Gfx::RectF& rect)
{
    const Pt::Forms::StyleOptions& options = Pt::Forms::Application::instance().styleOptions();

    Pt::Gfx::Painter painter(surface);
    painter.setClip(rect);

    // background
    if(_isHighlighted)
    {
        painter.setBrush(_brush);
        painter.fillRect(rect);
    }


    // icon    
    double iconX = (iconPadding() - icon().width()) / 2;
    double iconY = (size().height() - icon().height()) / 2;

    Gfx::Paint paint;
    paint.setCompositionMode(Gfx::CompositionMode::SourceOver);

    Pt::Gfx::PointF iconPos(iconX, iconY);
    this->surface().drawPixmap(iconPos, _picture, paint);

    // item text    
    painter.setFont(_font);
    painter.setPen(_textPen);

    Pt::Gfx::TextMetrics fm = painter.textMetrics(_text);
    double textX = padding().left() + _iconWidth;
    double textY = (size().height() - fm.height()) / 2;
    textY += fm.ascent();
    Pt::Gfx::PointF textPos(textX, textY);

    painter.drawText(textPos, _text);

    Pt::Gfx::RectF mnemonicRect; // TODO

    
    if (!mnemonicRect.isNull())
    {
        double menmonicY = textPos.y() + 1;
        painter.drawLine(Pt::Gfx::PointF(mnemonicRect.left(), menmonicY),
            Pt::Gfx::PointF(mnemonicRect.right(), menmonicY));
    }

    // shortcut text    
    const Pt::Forms::Key* sk = shortcut();
    if (sk)
    {
        Pt::String skText = shortcutText(*sk);
        Pt::Gfx::TextMetrics skm = painter.textMetrics(skText);

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


bool MenuBaseItem::onMouseEvent(const Pt::Forms::MouseEvent& ev)
{
    Base::onMouseEvent(ev);

    bool inside = Pt::Gfx::RectF(size()).contains(ev.position());

    if (inside && ev.isRelease())
    {
        onTriggered();
    }

    return true;
}


bool MenuBaseItem::onTouchEvent(const Pt::Forms::TouchEvent& ev)
{
    Base::onTouchEvent(ev);

    bool inside = Pt::Gfx::RectF(size()).contains(ev.position());

    if (inside && ev.isRelease())
    {
        onTriggered();
    }

    return true;
}


bool MenuBaseItem::onEnterEvent( const EnterEvent& ev)
{
    Base::onEnterEvent(ev);

    _isHighlighted = true;
    
    invalidate();
    return true;
}


bool MenuBaseItem::onLeaveEvent(const LeaveEvent& ev)
{
    Base::onLeaveEvent(ev);

    _isHighlighted = false;

    invalidate();
    return true;
}

}}
