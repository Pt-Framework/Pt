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
  MA  02110-1301  USA
*/

#include <Pt/Hmi/Panel.h>
#include <Pt/Hmi/Layout.h>
#include <Pt/Hmi/Window.h>
#include <Pt/Hmi/PaintSurface.h>
#include <Pt/Hmi/PaintRegion.h>
#include <Pt/Hmi/PaintEvent.h>
#include <Pt/Hmi/Picture.h>
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Pen.h>
#include <Pt/Gfx/Rect.h>
#include <Pt/Gfx/BlockScale.h>

namespace Pt {

namespace Hmi {

Panel::Panel()
: _backgroundImage()
, _backgroundImageLayout( ImageLayout::None )
{
    setAcceptsFocus(false);
}


Panel::~Panel()
{
}


void Panel::onResizeEvent(const ResizeEvent& ev)
{
    Widget::onResizeEvent(ev);

    if(  _backgroundPicture.empty() || ev.size().width() < 1 || ev.size().height() < 1 )
     return;

    switch( _backgroundImageLayout.type() )
    {
        case ImageLayout::Strech:
        {
            Gfx::Size newSize( (int) ev.size().width(), 
                               (int)ev.size().height() );

            Gfx::Image strech(_backgroundImage.format(), newSize);
            
            Gfx::blockScale(_backgroundImage.begin(),_backgroundImage.width(), _backgroundImage.height(),
                            strech.begin(), strech.width(), strech.height() );

            _backgroundPicture.set(strech);
        }
        break;

        case ImageLayout::Zoom:
        {
            const double factor = ev.size().width() / (double)_backgroundImage.width();
            Pt::Gfx::Size newSize( ( size_t)(_backgroundImage.width() * factor), 
                                   (size_t)(_backgroundImage.height() * factor) );

            Gfx::Image strech(_backgroundImage.format(), newSize);
            
            Gfx::blockScale( _backgroundImage.begin(),_backgroundImage.width(), _backgroundImage.height(),
                             strech.begin(), strech.width(), strech.height() );

            _backgroundPicture.set(strech);
        }
        break;
    }  
}


void Panel::onPaintBackground(PaintSurface& surface, const Gfx::RectF& updateRect)
{
    Frame::onPaintBackground(surface, updateRect);

    const PanelRenderer* renderer = getFacet<PanelRenderer>();

    if(renderer)
        renderer->renderBackground(*this, surface, updateRect);
}


void Panel::onPaintContent(PaintSurface& surface, const Gfx::RectF& updateRect)
{
    const PanelRenderer* renderer = getFacet<PanelRenderer>();

    if(renderer)
        renderer->renderContent(*this, surface, updateRect);

    //if( ! _backgroundPicture.empty() )
    //{
    //    painter.setCompositionMode(Gfx::CompositionMode::SourceOver);

    //    switch( _backgroundImageLayout.type() )
    //    {
    //        default:
    //        {
    //            painter.drawPicture( Pt::Gfx::PointF(0,0), _backgroundPicture );
    //        }
    //        break;
    //        
    //        case ImageLayout::Tile:
    //        {
    //             for( double x = 0; x < size.width();  x += _backgroundPicture.width() )
    //            {
    //                for( double y = 0; y < size.height();  y += _backgroundPicture.height() )
    //                    painter.drawPicture(Gfx::PointF(x,y), _backgroundPicture);
    //            }
    //        }
    //        break;

    //        case ImageLayout::Center:
    //        {
    //            const double x = size.width()/2  - _backgroundPicture.width()/2;
    //            const double y = size.height()/2  - _backgroundPicture.height()/2;
    //            painter.drawPicture(Gfx::PointF(x, y), _backgroundPicture);
    //        }
    //        break;
    //    }

    //    painter.setCompositionMode(Gfx::CompositionMode::SourceCopy);
    //}  

  Frame::onPaintContent(surface, updateRect);
}

} // namespace

} // namespace

