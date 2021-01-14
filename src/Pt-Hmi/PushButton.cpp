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

#include <Pt/Hmi/PushButton.h>
#include <Pt/Hmi/Style.h>
#include <Pt/Hmi/StyleOptions.h>
#include <Pt/Gfx/Painter.h>
#include <Pt/Hmi/Application.h>

namespace Pt {

namespace Hmi {

PushButton::PushButton()
: _isToggle(false)
, _isPressed(false)
, _isBeingToggled(false)
, _isFlat(false)
, _direction(Left)
, _hasRenderer(false)
{
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


const Gfx::Color& PushButton::accentColor() const
{
    return _accentColor ? *_accentColor
                        : Application::instance().styleOptions().accentColor();
}


void PushButton::setAccentColor(const Gfx::Color& color)
{
    _accentColor.reset( new Gfx::Color(color) );
    invalidate();
}


const Gfx::Color& PushButton::highlightColor() const
{
    return _highlightColor ? *_highlightColor
                           : Application::instance().styleOptions().highlightColor();
}


void PushButton::setHighlightColor(const Gfx::Color& color)
{
    _highlightColor.reset( new Gfx::Color(color) );
    invalidate();
}


const Gfx::Color& PushButton::textColor() const
{
    return _textColor ? *_textColor
                      : Application::instance().styleOptions().textColor();
}


void PushButton::setTextColor(const Gfx::Color& color)
{
    _textColor.reset( new Gfx::Color(color) );
    invalidate();
}


const std::string& PushButton::font() const
{
    return _fontName ? *_fontName
                     : Application::instance().styleOptions().font().name();
}


void PushButton::setFont(const std::string& fontName)
{
    _fontName.reset( new std::string(fontName) );
    invalidate();
}


std::size_t PushButton::fontSize() const
{
    return _fontSize ? *_fontSize
                     : Application::instance().styleOptions().font().size();
}


void PushButton::setFontSize(const std::size_t s)
{
    _fontSize.reset( new std::size_t(s) );
    invalidate();
}


const std::string& PushButton::fontStyle() const
{
    return _fontStyle ? *_fontStyle
                      : Application::instance().styleOptions().font().style();
}


void PushButton::setFontStyle(const std::string& style)
{
    _fontStyle.reset( new std::string(style) );
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

    _isBeingToggled = false;

    if( ! isToggle() )
        setPressed(false);

    clicked().send();
}


void PushButton::onCanceled()
{
    Base::onCanceled();

    if( isToggle() )
    { 
        if(_isBeingToggled)
        {
            _isBeingToggled = false;
            setPressed( ! isPressed() );
        }
    }
    else
        setPressed(false);
}


void PushButton::onSetStyleOptions(const StyleOptions& o)
{
    _foreground.reset( new Gfx::Brush(o.foreground()) );
    _contour.reset( new Gfx::Pen( o.contour() ) );
    _textColor.reset( new Gfx::Color( o.textColor() ) );
    _accentColor.reset( new Gfx::Color( o.accentColor() ) );
    _highlightColor.reset( new Gfx::Color( o.highlightColor() ) );
    _fontName.reset( new std::string( o.font().name() ) );
    _fontSize.reset( new std::size_t( o.font().size() ) );
    _fontStyle.reset( new std::string( o.font().style() ) );
}


Gfx::SizeF PushButton::onMeasure(const SizePolicy& policy)
{
    Gfx::FontMetrics fm = PixmapSurface::fontMetrics( _font, text() );

    double spacing = _picture.empty() || text().empty() ? 0 : fm.height() * 0.5;
    double textHeight = fm.height() + fm.descent(); // use descent as additional spacing

    Gfx::SizeF pictureSize = toLogical(Gfx::SizeF(_picture.width(), _picture.height()));
    double pictureWidth = _iconSize.isNull() ? pictureSize.width() : _iconSize.width();
    double pictureHeight = _iconSize.isNull() ? pictureSize.height() : _iconSize.height();
    double itemsWidth = 0;
    double itemsHeight = 0;

    switch(_direction)
    {
        default:
        case Left:
        case Right:
            itemsWidth = fm.width() + spacing + pictureWidth;
            itemsHeight = std::max<double>(textHeight, pictureHeight);
            break;

        case Top:
        case Bottom:
            itemsWidth = std::max<double>(fm.width(), pictureWidth);
            itemsHeight = textHeight + spacing + pictureHeight;
            break;  
    }

    return Gfx::SizeF( itemsWidth + padding().leftRight(), 
                       itemsHeight + padding().topBottom() );
}


void PushButton::onLayout(const Gfx::RectF& rect)
{
    bool resized = rect.size() != size();

    Base::onLayout(rect);

    if( ! resized )
    {
      //static int nn = 0;
      //std::clog << ++nn << " SKIP LAYOUT: " << typeid(*this).name() << std::endl;
      return;
    }

    layoutContent();
}


void PushButton::layoutContent()
{
    Gfx::FontMetrics fm = PixmapSurface::fontMetrics( _font, text() );

    double spacing = _picture.empty() || text().empty() ? 0 : fm.height() * 0.5;
    
    Gfx::SizeF pictureSize = toLogical(Gfx::SizeF(_picture.width(), _picture.height()));
    double pictureWidth = _iconSize.isNull() ? pictureSize.width() : _iconSize.width();
    double pictureHeight = _iconSize.isNull() ? pictureSize.height() : _iconSize.height();
    double itemsWidth = fm.width() + spacing + pictureWidth;
    double itemsHeight = fm.height() + spacing + pictureHeight;

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
            textY = ((size().height() - fm.height()) / 2) + fm.ascent();
            break;

        case Right:
            textX = (size().width() - itemsWidth) / 2;
            textY = ((size().height() - fm.height()) / 2) + fm.ascent();
            
            pictureX = textX + fm.width() + spacing;
            pictureY = (size().height() - pictureHeight) / 2;
            break;

        case Top:
            pictureX = (size().width() - pictureWidth) / 2;
            pictureY = (size().height() - itemsHeight) / 2;
    
            textX = (size().width() - fm.width()) / 2;
            textY = pictureY + pictureHeight + spacing + fm.ascent();
            break;  

        case Bottom:
            textX = (size().width() - fm.width()) / 2;
            textY = ((size().height() - itemsHeight) / 2) + fm.ascent();

            pictureX = (size().width() - pictureWidth) / 2;
            pictureY = textY + fm.descent() + spacing;
            break;  
    }

    if( ! _picture.empty() )
    {
        double pictureXOff = (pictureWidth - pictureSize.width()) / 2;
        double pictureYOff = (pictureHeight - pictureSize.height()) / 2;

        _iconPos.set(pictureX + pictureXOff, 
                     pictureY + pictureYOff);

        _iconPos = align(_iconPos);
    }

    _textPos.set(textX, textY);
    _textPos = align(_textPos);
}


void PushButton::onInvalidate()
{
    Application& app = Application::instance();
    const StyleOptions& options = app.styleOptions();
    const Style& style = app.style();

    _brush = foreground();
    _pen = contour();
    _textPen = textColor();
    _font = Gfx::Font(font(), fontSize(), fontStyle());

    if( ! _hasRenderer )
        _renderer.reset( style.get<ButtonRenderer>() );

    if( ! _renderer )
        return;

    if( !_icon.empty() )
    {
        const Gfx::SizeF scaledSize = toPhysical(_iconSize);
        const Pt::Gfx::Image& iconImage = _icon.getImage(scaledSize);
        _renderer->prepareIcon(*this, options, iconImage, _picture);
    }
    else
    {
        _picture.clear();
    }

    _renderer->prepare(*this, options, _brush, _pen, _font, _textPen);

    //layoutContent();

    Base::onInvalidate();
}


void PushButton::onPaint(Gfx::PaintSurface& surface, const Gfx::RectF& rect)
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
    //Gfx::FontMetrics fm = painter.fontMetrics( text() );

    //
    // button icon
    //

    if( ! _picture.empty() )
    {
        painter.setCompositionMode(Pt::Gfx::CompositionMode::SourceOver);
        painter.drawSurface(_iconPos, _picture);
        painter.setCompositionMode(Pt::Gfx::CompositionMode::SourceCopy);
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
            Gfx::FontMetrics fmLeft = painter.fontMetrics(mnemonicText);

            mnemonicText = *m;
            Gfx::FontMetrics fmChar = painter.fontMetrics(mnemonicText);

            mnemonicRect.set( Gfx::PointF(_textPos.x() + fmLeft.width(), 
                                          _textPos.y() - fmChar.ascent()),
                              Gfx::SizeF(fmChar.width(), 
                                         fmChar.height()) );
        }
    }

    _renderer->renderText(*this, options, painter, rect,
                          text(), _textPos, _font, _textPen,
                          mnemonicRect);
}


} // namespace

} // namespace
