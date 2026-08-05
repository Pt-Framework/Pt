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
#include <Pt/Forms/MenuItemBase.h>
#include <Pt/Forms/Menu.h>
#include <Pt/Forms/Application.h>
#include <Pt/Forms/PaintContext.h>
#include <Pt/Forms/Painter.h>
#include <Pt/Gfx/Painter.h>


namespace Pt {
namespace Forms {

MenuItemBase::MenuItemBase()
: _iconWidth(0)
, _text("(empty)")
, _hasSeparator(false)
, _isHighlighted(false)
, _overrides(0)
{
    setFocusPolicy(Control::AcceptFocus);
    setPadding(Pt::Forms::Spacing(8, 8));
    setMargin(0);
}

Pt::String MenuItemBase::shortcutText(const Pt::Forms::Key& key)
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


MenuItemBase::~MenuItemBase()
{
}


const Pt::String& MenuItemBase::text() const
{
    return _text;
}


void MenuItemBase::setText(const Pt::String& t)
{
    _text = t;
    invalidate();
}


const Pt::Gfx::Image& MenuItemBase::icon() const
{
    return _icon;
}


void MenuItemBase::setIcon(const Pt::Gfx::Image& img)
{
    _icon = img;
    invalidate();
}


double MenuItemBase::iconPadding() const
{
    return _iconWidth;
}


void MenuItemBase::setIconPadding(double w)
{
    _iconWidth = std::max<double>(w, _icon.width());
}


Pt::Signal<MenuItemBase&>& MenuItemBase::triggered()
{
    return _triggered;
}


const Pt::Gfx::Brush& MenuItemBase::background() const
{
    return _background ? *_background
        : Pt::Forms::Application::instance().styleOptions().background();
}


void MenuItemBase::setBackground(const Pt::Gfx::Brush& b)
{
    _background.reset(new Pt::Gfx::Brush(b));
    _overrides |= OverrideBackground;
    invalidate();
}


const Pt::Gfx::Pen& MenuItemBase::contour() const
{
    return _contour ? *_contour
        : Pt::Forms::Application::instance().styleOptions().contour();
}


void MenuItemBase::setContour(const Pt::Gfx::Pen& p)
{
    _contour.reset(new Pt::Gfx::Pen(p));
    _overrides |= OverrideContour;
    invalidate();
}


const Pt::Gfx::Color& MenuItemBase::textColor() const
{
    return _textColor ? *_textColor
        : Pt::Forms::Application::instance().styleOptions().textColor();
}


void MenuItemBase::setTextColor(const Pt::Gfx::Color& color)
{
    _textColor.reset(new Pt::Gfx::Color(color));
    _overrides |= OverrideTextColor;
    invalidate();
}


const Pt::Gfx::Font& MenuItemBase::font() const
{
    return _font;
}


void MenuItemBase::setFont(const Pt::Gfx::Font& font)
{
    _customFont = font;
    _overrides |= OverrideFontAll;

    invalidate();
}


Pt::Gfx::Font MenuItemBase::getFont() const
{
    const Pt::Gfx::Font& base = Pt::Forms::Application::instance().styleOptions().font();

    if( ! (_overrides & OverrideFontAny) )
        return base;

    if( _overrides & OverrideFontAll )
        return _customFont;

    std::size_t sz = (_overrides & OverrideFontSize) ? _customFont.size()
                                                        : base.size();
    Pt::Gfx::Font::Weight wt = (_overrides & OverrideFontWeight) ? _customFont.weight()
                                                                     : base.weight();
    Pt::Gfx::Font::Slant sl = (_overrides & OverrideFontSlant) ? _customFont.slant()
                                                                   : base.slant();

    if( base.hasStyleName() )
        return Pt::Gfx::Font(base.family(), sz, base.styleName(), wt, sl, base.stretch());

    if( base.category() != Pt::Gfx::Font::Category::None )
        return Pt::Gfx::Font(base.category(), sz, wt, sl, base.stretch());

    return Pt::Gfx::Font(base.family(), sz, wt, sl, base.stretch());
}


void MenuItemBase::setFontSize(std::size_t size)
{
    _customFont = _customFont.withSize(size);
    _overrides |= OverrideFontSize;

    invalidate();
}


void MenuItemBase::setFontWeight(Pt::Gfx::Font::Weight weight)
{
    _customFont = _customFont.withWeight(weight);
    _overrides |= OverrideFontWeight;

    invalidate();
}


void MenuItemBase::setFontSlant(Pt::Gfx::Font::Slant slant)
{
    _customFont = _customFont.withSlant(slant);
    _overrides |= OverrideFontSlant;

    invalidate();
}



void MenuItemBase::onTriggered()
{
    _triggered.send(*this);
}



void MenuItemBase::onShortcut(const Pt::Forms::Key& key)
{
    Base::onShortcut(key);

    onTriggered();
}


void MenuItemBase::onInvalidate()
{
    Base::onInvalidate();

    // TODO: use renderer and options from parent

    const Pt::Forms::StyleOptions& options = Pt::Forms::Application::instance().styleOptions();
    const Pt::Forms::Style& style = Pt::Forms::Application::instance().style();

    _brush = background();
    _pen = contour();
    _textPen = textColor();
    _font = getFont();

    _picture.reset(_icon);

    if(_isHighlighted)
        _brush = options.highlightColor();
}


Pt::Gfx::SizeF MenuItemBase::onMeasure(const Pt::Forms::SizePolicy& policy)
{
    Painter _painter(surface());
    _painter.setFont(_font);

    Pt::Gfx::TextMetrics fm = _painter.textMetrics(_text);
    Pt::Gfx::FontMetrics fontMet = _painter.fontMetrics();

    double contentHeight = std::max<Pt::ssize_t>(fontMet.height(), _icon.height());
    double contentWidth = fm.advance() + scaling().toLogical(_picture.size().width());

    const Pt::Forms::Key* sk = shortcut();
    if (sk)
    {
        Pt::String text = shortcutText(*sk);
        contentWidth += fontMet.height() * 2.5; // spacing towards shortcut text
        contentWidth += _painter.textMetrics(text).advance();
    }

//    if (_hasSubMenu)
//        contentWidth += fm.height() * 4;

    return Pt::Gfx::SizeF(contentWidth + padding().leftRight(),
        contentHeight + padding().topBottom());
}


void MenuItemBase::onPaint(PaintContext& context, const Pt::Gfx::RectF& rect)
{
    const Pt::Forms::StyleOptions& options = Pt::Forms::Application::instance().styleOptions();

    Forms::Painter painter(context);
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

    Pt::Gfx::PointF iconPos(iconX, iconY);

    const Gfx::CompositionMode prevMode = painter.compositionMode();
    painter.setCompositionMode(Gfx::CompositionMode::SourceOver);
    painter.drawPixmap(iconPos, _picture);
    painter.setCompositionMode(prevMode);

    // item text
    painter.setFont(_font);
    painter.setPen(_textPen);

    Pt::Gfx::TextMetrics fm = painter.textMetrics(_text);
    Pt::Gfx::FontMetrics fontMet = painter.fontMetrics();
    double textX = padding().left() + _iconWidth;
    double textY = (size().height() - fontMet.height()) / 2;
    textY += fontMet.ascent();
    Pt::Gfx::PointF textPos(textX, textY);

    painter.drawText(textPos, _text);

    Pt::Gfx::RectF mnemonicRect; // TODO


    if (!mnemonicRect.isEmpty())
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

        double skX = size().width() - skm.advance() - padding().right();
        double skY = (size().height() - fontMet.height()) / 2;
        skY += fontMet.ascent();
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


bool MenuItemBase::onMouseEvent(const Pt::Forms::MouseEvent& ev)
{
    Base::onMouseEvent(ev);

    bool inside = Pt::Gfx::RectF(size()).contains(ev.position());

    if (inside && ev.isRelease())
    {
        onTriggered();
    }

    return true;
}


bool MenuItemBase::onTouchEvent(const Pt::Forms::TouchEvent& ev)
{
    Base::onTouchEvent(ev);

    bool inside = Pt::Gfx::RectF(size()).contains(ev.position());

    if (inside && ev.isRelease())
    {
        onTriggered();
    }

    return true;
}


bool MenuItemBase::onEnterEvent( const EnterEvent& ev)
{
    Base::onEnterEvent(ev);

    _isHighlighted = true;

    invalidate();
    return true;
}


bool MenuItemBase::onLeaveEvent(const LeaveEvent& ev)
{
    Base::onLeaveEvent(ev);

    _isHighlighted = false;

    invalidate();
    return true;
}

}}
