/* Copyright (C) 2015-2017 Marc Boris Duerner 
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
  MA  02110-1301  USA
*/

#include <Pt/Forms/Panel.h>
#include <Pt/Forms/Application.h>
#include <Pt/Forms/PaintContext.h>
#include <Pt/Forms/Painter.h>
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Rect.h>
#include <Pt/Gfx/BlockScale.h>

namespace Pt {

namespace Forms {

Panel::Panel()
: _content(0)
, _hasBackground(false)
, _hasFrame(false)
, _customRenderer(false)
, _iconInvalid(false)
{
}


Panel::~Panel()
{
}

void Panel::setIcon(const Icon& icon, const Gfx::SizeF& iconSize, Alignment align)
{
    _icon = icon;
    _iconSize = iconSize;
    _imageAlignment = align;
    _iconInvalid = true;

    invalidate();
}


Control* Panel::content() const
{
  return _content;
}


void Panel::setContent(Control* control)
{
    if(_content)
        remove(*_content);

    _content = control;

    if (control)
        add(*control);
}


void Panel::onRemoveControl(Control& control)
{
    Control::onRemoveControl(control);

    if(&control == _content)
        _content = 0;
}


const Gfx::Brush* Panel::background() const
{
    if( ! _hasBackground )
        return 0;

    if( _renderer )
        return _renderer->background();

    return &Application::instance().styleOptions().background();
}


void Panel::setBackground(const Gfx::Brush& b)
{
    _background.reset( new Gfx::Brush(b) );
    _hasBackground = true;

    if( PanelRenderer* renderer = getRenderer() )
        renderer->setBackground(*_background);

    repaint();
}


void Panel::setBackground(bool b)
{
    _hasBackground = b;
    repaint();
}


const Gfx::Pen* Panel::contour() const
{
    if( ! _hasFrame )
        return 0;

    if( _renderer )
        return _renderer->contour();

    return &Application::instance().styleOptions().contour();
}


void Panel::setContour(const Gfx::Pen& pen)
{
    _contour.reset( new Gfx::Pen(pen) );
    _hasFrame = true;

    if( PanelRenderer* renderer = getRenderer() )
        renderer->setContour(*_contour);

    invalidate();
}


void Panel::setFrame(bool b)
{
    _hasFrame = b;
    repaint();
}


void Panel::setRenderer(PanelRenderer* renderer)
{
    _renderer.reset(renderer);
    _customRenderer = renderer != 0;

    if( renderer )
        applyRenderer(renderer);

    invalidate();
}


PanelRenderer* Panel::getRenderer()
{
    if( ! _customRenderer )
    {
        const Style& style = Application::instance().style();
        PanelRenderer* proto = style.get<PanelRenderer>();
        if( ! proto )
            return 0;

        _renderer.reset( proto->create() );
        _customRenderer = true;
    }

    return _renderer.get();
}


void Panel::applyRenderer(PanelRenderer* renderer)
{
    if( _background )
        renderer->setBackground( *_background );

    if( _contour )
        renderer->setContour( *_contour );
}


void Panel::onInvalidate()
{
    Base::onInvalidate();

    if( ! _renderer )
    {
        bool hasOverride = _background || _contour;
        if(hasOverride)
        {
            if( PanelRenderer* renderer = getRenderer() )
                applyRenderer(renderer);
        }
        else
        {
            _renderer.reset( Application::instance().style().get<PanelRenderer>() );
        }
    }

    if(_iconInvalid)
    {
        _iconInvalid = false;

        if( ! _icon.empty() )
        {
            const Gfx::SizeF scaledSize = scaling().toPhysical(_iconSize);
            const Pt::Gfx::Image& iconImage = _icon.getImage(scaledSize);
            _picture.reset(iconImage);
        }
        else
        {
            _picture.reset(Pt::Gfx::Image());
        }

        relayout();
    }
}


void Panel::onRescaleEvent(const RescaleEvent& ev)
{
    Base::onRescaleEvent(ev);

    if( ! _icon.empty() )
    {
        _iconInvalid = true;
        relayout();
    }
}


Gfx::SizeF Panel::onMeasure(const SizePolicy& policy)
{
    if(_content)
    { 
        double hspace = padding().leftRight() + _content->margin().leftRight();
        double vspace = padding().topBottom() + _content->margin().topBottom();

        SizePolicy contentPolicy = policy;
        contentPolicy.setWidth( policy.size().width() - hspace );
        contentPolicy.setHeight( policy.size().height() - vspace );
        
        _content->measure(contentPolicy);
        return _content->preferredSize();
    }

    return Gfx::SizeF(0, 0);
}


void Panel::onLayout(const Gfx::RectF& rect)
{
    Base::onLayout(rect);
    
    if(_content)
    {
        Gfx::PointF pos(padding().left() + _content->margin().left(), 
                        padding().top()  + _content->margin().top());
        
        double hspace = padding().leftRight() + _content->margin().leftRight();
        double vspace = padding().topBottom() + _content->margin().topBottom();

        Gfx::SizeF size;
        size.setWidth( rect.width() - hspace );
        size.setHeight( rect.height() - vspace );

        _content->move(pos);
        _content->resize(size);
    }
}


void Panel::onPaint(PaintContext& context, const Gfx::RectF& /*rect*/)
{
    if( ! _renderer)
        return;

    Gfx::RectF widgetRect( size() );

    if( _hasBackground )
        _renderer->renderBackground(context, widgetRect, styleFlags());

    Forms::Painter painter(context);
    onPaintContent(context);

    if( _hasFrame )
        _renderer->renderFrame(context, widgetRect, styleFlags());
}


void Panel::onPaintContent(PaintContext& context)
{
    if( _picture.empty() )
        return;

    const Gfx::Scaling& scaling = this->scaling();

    double rightX = size().width() - scaling.toLogical( _picture.size().width() );
    double bottomY = size().height() - scaling.toLogical( _picture.size().height() );

    double centerX = rightX / 2;
    double centerY = bottomY / 2;

    Gfx::PointF imagePosition;

    switch(_imageAlignment)
    {
        case Alignment::TopLeft:
            imagePosition.set(0.0, 0.0);
            break;

        case Alignment::Top:
            imagePosition.set(centerX, 0.0);
            break;

        case Alignment::TopRight:
            imagePosition.set(rightX, 0.0);
            break;

        case Alignment::Left:
            imagePosition.set(0.0, centerY);
            break;

        default:
        case Alignment::Center:
            imagePosition.set(centerX, centerY);
            break;

        case Alignment::Right:
            imagePosition.set(rightX, centerY);
            break;

        case Alignment::BottomLeft:
            imagePosition.set(0.0, bottomY);
            break;

        case Alignment::Bottom:
            imagePosition.set(centerX, bottomY);
            break;

        case Alignment::BottomRight:
            imagePosition.set(rightX, bottomY);
            break;
    }

    Gfx::RectF widgetRect( size() );
    _renderer->renderIcon(context, widgetRect, _picture, imagePosition,
                          styleFlags());
}

} // namespace

} // namespace

