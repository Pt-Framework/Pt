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

#include <Pt/Forms/PushButton.h>
#include <Pt/Forms/Style.h>
#include <Pt/Forms/StyleOptions.h>
#include <Pt/Gfx/Painter.h>
#include <Pt/Forms/Application.h>

namespace Pt {

namespace Forms {

PushButton::PushButton()
: _isToggle(false)
, _isPressed(false)
, _isBeingToggled(false)
, _isFlat(false)
, _direction(Left)
, _hasRenderer(false)
, _fontOverride(0)
, _styleGeneration(0)
{
    _font = Application::instance().styleOptions().font();
}


PushButton::~PushButton()
{
}


bool PushButton::isToggle() const
{
    return _isToggle;
}


void PushButton::setToggle(bool toggle)
{
    _isToggle = toggle;
    invalidate();
}


void PushButton::setIcon(const Icon& icon, const Gfx::SizeF& iconSize)
{
    _icon = icon;
    _iconSize = iconSize;
    
    //onIconChanged();
    invalidate();
}


bool PushButton::isFlat() const
{
    return _isFlat;
}


void PushButton::setFlat(bool f)
{
    _isFlat = f;
    invalidate();
}


void PushButton::setLayout(Direction d)
{
    _direction = d;
    invalidate();
}


const Gfx::Brush& PushButton::foreground() const
{
    return _foreground ? *_foreground
                       : Application::instance().styleOptions().foreground();
}


void PushButton::setForeground(const Gfx::Brush& b)
{
    _foreground.reset( new Gfx::Brush(b) );
    invalidate();
}


const Gfx::Pen& PushButton::contour() const
{
    return _contour ? *_contour
                    : Application::instance().styleOptions().contour();
}


void PushButton::setContour(const Gfx::Pen& p)
{
    _contour.reset( new Gfx::Pen(p) );
    invalidate();
}


const Gfx::ColorF& PushButton::accentColor() const
{
    return _accentColor ? *_accentColor
                        : Application::instance().styleOptions().accentColor();
}


void PushButton::setAccentColor(const Gfx::ColorF& color)
{
    _accentColor.reset( new Gfx::ColorF(color) );
    invalidate();
}


const Gfx::ColorF& PushButton::highlightColor() const
{
    return _highlightColor ? *_highlightColor
                           : Application::instance().styleOptions().highlightColor();
}


void PushButton::setHighlightColor(const Gfx::ColorF& color)
{
    _highlightColor.reset( new Gfx::ColorF(color) );
    invalidate();
}


const Gfx::ColorF& PushButton::textColor() const
{
    return _textColor ? *_textColor
                      : Application::instance().styleOptions().textColor();
}


void PushButton::setTextColor(const Gfx::ColorF& color)
{
    _textColor.reset( new Gfx::ColorF(color) );
    invalidate();
}


const Gfx::Font& PushButton::font() const
{
    return _font;
}


void PushButton::setFont(const Gfx::Font& font)
{
    _customFont = font;
    _fontOverride = OverrideAll;

    invalidate();
}


Gfx::Font PushButton::getFont() const
{
    const Gfx::Font& base = Application::instance().styleOptions().font();

    if( _fontOverride == 0 )
        return base;

    if( _fontOverride == OverrideAll )
        return _customFont;

    std::size_t sz = (_fontOverride & OverrideSize) ? _customFont.size()
                                                    : base.size();
    Gfx::Font::Weight wt = (_fontOverride & OverrideWeight) ? _customFont.weight()
                                                            : base.weight();
    Gfx::Font::Slant sl = (_fontOverride & OverrideSlant) ? _customFont.slant()
                                                          : base.slant();

    if( base.hasStyleName() )
        return Gfx::Font(base.family(), sz, base.styleName(), wt, sl, base.stretch());

    if( base.category() != Gfx::Font::Category::None )
        return Gfx::Font(base.category(), sz, wt, sl, base.stretch());

    return Gfx::Font(base.family(), sz, wt, sl, base.stretch());
}


void PushButton::setFontSize(std::size_t size)
{
    _customFont = Gfx::Font(_customFont.family(), size,
                            _customFont.weight(), _customFont.slant(),
                            _customFont.stretch());
    _fontOverride |= OverrideSize;

    invalidate();
}


void PushButton::setFontWeight(Gfx::Font::Weight weight)
{
    _customFont = Gfx::Font(_customFont.family(), _customFont.size(),
                            weight, _customFont.slant(),
                            _customFont.stretch());
    _fontOverride |= OverrideWeight;

    invalidate();
}


void PushButton::setFontSlant(Gfx::Font::Slant slant)
{
    _customFont = Gfx::Font(_customFont.family(), _customFont.size(),
                            _customFont.weight(), slant,
                            _customFont.stretch());
    _fontOverride |= OverrideSlant;

    invalidate();
}


void PushButton::setRenderer(ButtonRenderer* renderer)
{
    _renderer.reset(renderer);
    _hasRenderer = renderer != 0;

    invalidate();
}


bool PushButton::isPressed() const
{
    return _isPressed;
}


void PushButton::setPressed(bool pressed)
{
    _isPressed = pressed;
    _isBeingToggled = false;
    invalidate();
}


void PushButton::onPressed()
{
    Base::onPressed();

    if( isToggle() )
    {
        setPressed( ! isPressed() );
        _isBeingToggled = true;
    }
    else
        setPressed(true);
}


void PushButton::onReleased()
{
    Base::onReleased();

    if(_isBeingToggled)
    {
        _isBeingToggled = false;
        clicked().send();
    }
    else
    {
        setPressed(false);
        clicked().send();
    }
}


void PushButton::onCanceled()
{
    Base::onCanceled();

    if( _isBeingToggled )
    { 
        _isBeingToggled = false;
        setPressed( ! isPressed() );
    }
    else
        setPressed(false);
}


void PushButton::onSetStyleOptions(const StyleOptions& o)
{
    _foreground.reset( new Gfx::Brush(o.foreground()) );
    _contour.reset( new Gfx::Pen( o.contour() ) );
    _textColor.reset( new Gfx::ColorF( o.textColor() ) );
    _accentColor.reset( new Gfx::ColorF( o.accentColor() ) );
    _highlightColor.reset( new Gfx::ColorF( o.highlightColor() ) );
    _customFont = o.font();
    _fontOverride = OverrideAll;
}


Gfx::SizeF PushButton::onMeasure(const SizePolicy& policy)
{
    const Gfx::Scaling& scal = this->scaling();

    Gfx::Painter _painter( surface() );
    _painter.setFont(_font);

    _textMetrics = _painter.textMetrics( text() );
    _fontMetrics = _painter.fontMetrics();

    double spacing = _picture.empty() || text().empty() ? 0 : _fontMetrics.height() * 0.5;

    // use descent as additional spacing
    double textHeight = _fontMetrics.height() + _fontMetrics.descent(); 

    Gfx::SizeF pictureSize = scaling().toLogical( _picture.size() );
    double pictureWidth = _iconSize.isNull() ? pictureSize.width() : _iconSize.width();
    double pictureHeight = _iconSize.isNull() ? pictureSize.height() : _iconSize.height();
    double itemsWidth = 0;
    double itemsHeight = 0;

    switch(_direction)
    {
        default:
        case Left:
        case Right:
            itemsWidth = _textMetrics.advance() + spacing + pictureWidth;
            itemsHeight = std::max<double>(textHeight, pictureHeight);
            break;

        case Top:
        case Bottom:
            itemsWidth = std::max<double>(_textMetrics.advance(), pictureWidth);
            itemsHeight = textHeight + spacing + pictureHeight;
            break;  
    }

    return Gfx::SizeF( itemsWidth + padding().leftRight(), 
                       itemsHeight + padding().topBottom() );
}


void PushButton::onLayout(const Gfx::RectF& rect)
{    
    Base::onLayout(rect);

    const Gfx::Scaling& scaling = this->scaling();

    double spacing = _picture.empty() || text().empty() ? 0 : _fontMetrics.height() * 0.5;
    
    Gfx::SizeF pictureSize = scaling.toLogical( _picture.size() );
    double pictureWidth = _iconSize.isNull() ? pictureSize.width() : _iconSize.width();
    double pictureHeight = _iconSize.isNull() ? pictureSize.height() : _iconSize.height();
    double itemsWidth = _textMetrics.advance() + spacing + pictureWidth;
    double itemsHeight = _fontMetrics.height() + spacing + pictureHeight;

    double pictureX = 0;
    double pictureY = 0;
    double textX = 0;
    double textY = 0;

    switch(_direction)
    {
        default:
        case Left:
            pictureX = (size().width() - itemsWidth) / 2;
            pictureY = (size().height() - pictureHeight) / 2;
    
            textX = pictureX + pictureWidth + spacing;
            textY = ((size().height() - _fontMetrics.height()) / 2) + _fontMetrics.ascent();
            break;

        case Right:
            textX = (size().width() - itemsWidth) / 2;
            textY = ((size().height() - _fontMetrics.height()) / 2) + _fontMetrics.ascent();
            
            pictureX = textX + _textMetrics.advance() + spacing;
            pictureY = (size().height() - pictureHeight) / 2;
            break;

        case Top:
            pictureX = (size().width() - pictureWidth) / 2;
            pictureY = (size().height() - itemsHeight) / 2;
    
            textX = (size().width() - _textMetrics.advance()) / 2;
            textY = pictureY + pictureHeight + spacing + _fontMetrics.ascent();
            break;  

        case Bottom:
            textX = (size().width() - _textMetrics.advance()) / 2;
            textY = ((size().height() - itemsHeight) / 2) + _fontMetrics.ascent();

            pictureX = (size().width() - pictureWidth) / 2;
            pictureY = textY + _fontMetrics.descent() + spacing;
            break;  
    }

    if( ! _picture.empty() )
    {
        double pictureXOff = (pictureWidth - pictureSize.width()) / 2;
        double pictureYOff = (pictureHeight - pictureSize.height()) / 2;

        _iconPos.set(pictureX + pictureXOff, 
                     pictureY + pictureYOff);

        _iconPos = scaling.align(_iconPos);
    }

    _textPos.set(textX, textY);
    _textPos = scaling.align(_textPos);

    repaint();
}


void PushButton::onIconChanged()
{
    Application& app = Application::instance();
    const StyleOptions& options = app.styleOptions();
    const Style& style = app.style();

    if( ! _hasRenderer )
        _renderer.reset( style.get<ButtonRenderer>() );

    if( ! _renderer )
        return;

    if( ! _icon.empty() )
    {
        const Gfx::SizeF scaledSize = scaling().toPhysical(_iconSize);
        const Pt::Gfx::Image& iconImage = _icon.getImage(scaledSize);
        _renderer->prepareIcon(*this, options, iconImage, _picture);
    }
    else
    {
        _picture.reset();
    }
}


void PushButton::onInvalidate()
{
    Application& app = Application::instance();
    const StyleOptions& options = app.styleOptions();
    const Style& style = app.style();

    _brush = foreground();
    _pen = contour();
    _textPen = textColor();
    _font = getFont();

    if( ! _hasRenderer )
        _renderer.reset( style.get<ButtonRenderer>() );

    if( ! _renderer )
        return;

    //// onIconChanged()
    if( ! _icon.empty() )
    {
        const Gfx::SizeF scaledSize = scaling().toPhysical(_iconSize);
        const Pt::Gfx::Image& iconImage = _icon.getImage(scaledSize);
        _renderer->prepareIcon(*this, options, iconImage, _picture);
    }
    else
    {
        _picture.reset();
    }
    //// onIconChanged()

    _renderer->prepare(*this, options, _brush, _pen, _font, _textPen);

    Base::onInvalidate();

    relayout();
}


void PushButton::onPaint(PaintSurface& surface, const Gfx::RectF& rect)
{
    const StyleOptions& options = Application::instance().styleOptions();

    if( ! _renderer )
        return;

    Gfx::Painter painter(surface);
    painter.setClip(rect);

    //
    // button shape
    //

    if( ! _isFlat )
    {
        _renderer->renderBackground(*this, options, painter, rect, 
                                    _brush, _pen);
    }

    painter.setFont(_font);

    //
    // button icon
    //

    if( ! _picture.empty() )
    {
        Gfx::Paint paint;
        paint.setCompositionMode(Gfx::CompositionMode::SourceOver);
        this->surface().drawPixmap(_iconPos, _picture, paint);
    }

    //
    // button text including menomnic
    //

    Gfx::RectF mnemonicRect;
    const Char* m = mnemonic();
    if(m)
    {
        String::size_type n = text().find(*m);
        if(n != String::npos)
        {
            Pt::String mnemonicText(text(), 0, n);
            Gfx::TextMetrics fmLeft = painter.textMetrics(mnemonicText);

            mnemonicText = *m;
            Gfx::TextMetrics fmChar = painter.textMetrics(mnemonicText);
            Gfx::FontMetrics fontMet = painter.fontMetrics();

            mnemonicRect.set( Gfx::PointF(_textPos.x() + fmLeft.advance(), 
                                          _textPos.y() - fontMet.ascent()),
                              Gfx::SizeF(fmChar.advance(), 
                                         fontMet.height()) );
        }
    }

    _renderer->renderText(*this, options, painter, rect,
                          text(), _textPos, _font, _textPen,
                          mnemonicRect);
}


} // namespace

} // namespace
