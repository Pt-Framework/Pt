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
, _imageAlignment(Alignment::Center)
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

    if( const Gfx::Brush* background = _panelStyleOptions.background() )
        return background;

    return &Application::instance().styleOptions().background();
}


void Panel::setBackground(const Gfx::Brush& b)
{
    _panelStyleOptions.setBackground(b);
    _hasBackground = true;

    invalidate();
}


void Panel::setBackground(bool b)
{
    _hasBackground = b;
    invalidate();
}


const Gfx::Pen* Panel::contour() const
{
    if( ! _hasFrame )
        return 0;

    if( const Gfx::Pen* contour = _panelStyleOptions.contour() )
        return contour;

    return &Application::instance().styleOptions().contour();
}


void Panel::setContour(const Gfx::Pen& pen)
{
    _panelStyleOptions.setContour(pen);
    _hasFrame = true;

    invalidate();
}


void Panel::setFrame(bool b)
{
    _hasFrame = b;
    invalidate();
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


void Panel::setRenderer(PanelRenderer* renderer)
{
    const StyleOptions& options = Application::instance().styleOptions();

    if( renderer )
        _panelStyle.bind(*renderer, options, _panelStyleOptions);
    else
        _panelStyle.bind(Application::instance().style(), options,
                         _panelStyleOptions);

    invalidate();
}


void Panel::onInvalidate()
{
    Base::onInvalidate();

    const StyleOptions& options = Application::instance().styleOptions();
    const Style& style = Application::instance().style();

    PanelRenderer* renderer = _panelStyle.rebind(style, options,
                                                 _panelStyleOptions);
    if( ! renderer )
        return;

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
    }

    relayout();
}


Gfx::SizeF Panel::onMeasure(const SizePolicy& policy)
{
    PanelRenderer* renderer = _panelStyle.renderer();
    if( ! renderer )
        return Gfx::SizeF(0, 0);

    Gfx::SizeF contentSize;

    if( _content )
    {
        double hspace = padding().leftRight() + _content->margin().leftRight();
        double vspace = padding().topBottom() + _content->margin().topBottom();

        SizePolicy contentPolicy = policy;
        contentPolicy.setWidth( policy.size().width() - hspace );
        contentPolicy.setHeight( policy.size().height() - vspace );

        _content->measure(contentPolicy);
        contentSize.setWidth( _content->preferredSize().width() + hspace );
        contentSize.setHeight( _content->preferredSize().height() + vspace );
    }
    else
    {
        contentSize.setWidth( padding().leftRight() );
        contentSize.setHeight( padding().topBottom() );
    }

    if( ! _picture.empty() )
    {
        Gfx::SizeF iconSize = _iconSize.isNull()
                            ? surface().scaling().toLogical( _picture.size() )
                            : _iconSize;

        double iconWidth = iconSize.width() + padding().leftRight();
        double iconHeight = iconSize.height() + padding().topBottom();

        if( contentSize.width() < iconWidth )
            contentSize.setWidth(iconWidth);

        if( contentSize.height() < iconHeight )
            contentSize.setHeight(iconHeight);
    }

    return renderer->measureFrame( surface(), contentSize );
}


void Panel::onLayout(const Gfx::RectF& rect)
{
    Base::onLayout(rect);

    PanelRenderer* renderer = _panelStyle.renderer();
    if( ! renderer )
        return;

    _contentRect = renderer->layoutFrame( surface(), Gfx::RectF(size()) );

    if( _content )
    {
        Gfx::PointF pos(_contentRect.left() + padding().left() + _content->margin().left(), 
                        _contentRect.top() + padding().top() + _content->margin().top());

        double hspace = padding().leftRight() + _content->margin().leftRight();
        double vspace = padding().topBottom() + _content->margin().topBottom();

        Gfx::SizeF size;
        size.setWidth( _contentRect.width() - hspace );
        size.setHeight( _contentRect.height() - vspace );

        _content->move(pos);
        _content->resize(size);
    }

    repaint();
}


void Panel::onPaint(PaintContext& context, const Gfx::RectF& /*rect*/)
{
    if( ! _panelStyle.renderer() )
        return;

    Gfx::RectF widgetRect( size() );
    PanelState state = panelState();

    onPaintBackground(context, widgetRect, state);
    onPaintContent(context, _contentRect, state);
    onPaintFrame(context, widgetRect, state);
}

void Panel::onPaintBackground(PaintContext& context,
                              const Gfx::RectF& rect,
                              const PanelState& state)
{
    PanelRenderer* renderer = _panelStyle.renderer();
    if( ! renderer || ! _hasBackground )
        return;

    renderer->renderBackground(context, rect, state);
}


void Panel::onPaintContent(PaintContext& context,
                           const Gfx::RectF& contentRect,
                           const PanelState& state)
{
    PanelRenderer* renderer = _panelStyle.renderer();
    if( ! renderer || _picture.empty() )
        return;

    const Gfx::Scaling& scaling = this->scaling();

    double rightX = contentRect.width() - scaling.toLogical( _picture.size().width() );
    double bottomY = contentRect.height() - scaling.toLogical( _picture.size().height() );

    double centerX = rightX / 2;
    double centerY = bottomY / 2;

    Gfx::PointF imagePosition;

    switch(_imageAlignment)
    {
        case Alignment::TopLeft:
            imagePosition.set(contentRect.left(), contentRect.top());
            break;

        case Alignment::Top:
            imagePosition.set(contentRect.left() + centerX, contentRect.top());
            break;

        case Alignment::TopRight:
            imagePosition.set(contentRect.left() + rightX, contentRect.top());
            break;

        case Alignment::Left:
            imagePosition.set(contentRect.left(), contentRect.top() + centerY);
            break;

        default:
        case Alignment::Center:
            imagePosition.set(contentRect.left() + centerX,
                              contentRect.top() + centerY);
            break;

        case Alignment::Right:
            imagePosition.set(contentRect.left() + rightX,
                              contentRect.top() + centerY);
            break;

        case Alignment::BottomLeft:
            imagePosition.set(contentRect.left(), contentRect.top() + bottomY);
            break;

        case Alignment::Bottom:
            imagePosition.set(contentRect.left() + centerX,
                              contentRect.top() + bottomY);
            break;

        case Alignment::BottomRight:
            imagePosition.set(contentRect.left() + rightX,
                              contentRect.top() + bottomY);
            break;
    }

    renderer->renderIcon(context, contentRect, _picture, imagePosition,
                         state);
}


void Panel::onPaintFrame(PaintContext& context,
                         const Gfx::RectF& rect,
                         const PanelState& state)
{
    PanelRenderer* renderer = _panelStyle.renderer();
    if( ! renderer || ! _hasFrame )
        return;

    renderer->renderFrame(context, rect, state);
}


PanelState Panel::panelState() const
{
    PanelState state;
    state.setEnabled( isEnabled() );
    state.setFocused( hasFocus() );
    return state;
}

} // namespace

} // namespace

