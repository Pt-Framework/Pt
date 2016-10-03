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
: _backgroundBrush(Gfx::Color::fromRgb8(237,237,237))
, _foregroundPen( Gfx::Color::fromRgb8(0,0,0) )
, _backgroundImage()
, _backgroundImageLayout( ImageLayout::None )
, _borderStyle(Single)
, _borderRound(false)
, _borderWidth(1)
, _borderColor(Gfx::Color::fromRgb8(178,178,178))
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


void Panel::onPaintEvent(const PaintEvent& ev)
{
    Widget::onPaintEvent(ev);

    Gfx::PointF winpos = toWindow( Gfx::PointF(0,0) );
    PaintSurface& windowSurface = this->window()->surface();

    Gfx::RectF paintRect(winpos, size());
    PaintRegion region(windowSurface, paintRect);
    
    onPaintBackground(region, ev.rect());
    onPaintContent(region, ev.rect());
}


void Panel::onPaintBackground(PaintSurface& surface, const Gfx::RectF& updateRect)
{
    const Gfx::SizeF& size = this->size();

    if( size.width() < 0 || size.height() < 0)
        return;

    Painter painter(surface);
    painter.setClip(updateRect);
    painter.setCompositionMode(Gfx::CompositionMode::SourceCopy);

    Gfx::RectF borderRect( Gfx::PointF(0,0), this->size() );

    if(_borderStyle != NoBorder)
    {
        borderRect.setOrigin( Gfx::PointF(_borderWidth/2, _borderWidth/2) );
        borderRect.setSize( Gfx::SizeF(size.width() - _borderWidth, 
                                       size.height() - _borderWidth) );
    }

    double corner = _borderRound ? 2.0 : 0;
    std::vector<Gfx::PointF> outline(9);

    // top left
    outline[0].setX(0);
    outline[0].setY(corner);

    outline[1].setX(corner);
    outline[1].setY(0);

    // top right
    outline[2].setX(borderRect.width() - corner);
    outline[2].setY(0);

    outline[3].setX(borderRect.width());
    outline[3].setY(corner);

    // bottom right
    outline[4].setX(borderRect.width());
    outline[4].setY(borderRect.height() - corner);

    outline[5].setX(borderRect.width() - corner);
    outline[5].setY(borderRect.height());

    // bottom left
    outline[6].setX(corner);
    outline[6].setY(borderRect.height());

    outline[7].setX(0);
    outline[7].setY(borderRect.height() - corner);
            
    outline[8] = outline[0];

    if( _backgroundBrush.color().alpha() != 0)
    {
        painter.setBrush(_backgroundBrush); 
        painter.fillPolygon(&outline[0], outline.size());
    }

    if( ! _backgroundPicture.empty() )
    {
        painter.setCompositionMode(Gfx::CompositionMode::SourceOver);

        switch( _backgroundImageLayout.type() )
        {
            default:
            {
                painter.drawPicture( Pt::Gfx::PointF(0,0), _backgroundPicture );
            }
            break;
            
            case ImageLayout::Tile:
            {
                 for( double x = 0; x < size.width();  x += _backgroundPicture.width() )
                {
                    for( double y = 0; y < size.height();  y += _backgroundPicture.height() )
                        painter.drawPicture(Gfx::PointF(x,y), _backgroundPicture);
                }
            }
            break;

            case ImageLayout::Center:
            {
                const double x = size.width()/2  - _backgroundPicture.width()/2;
                const double y = size.height()/2  - _backgroundPicture.height()/2;
                painter.drawPicture(Gfx::PointF(x, y), _backgroundPicture);
            }
            break;
        }

        painter.setCompositionMode(Gfx::CompositionMode::SourceCopy);
    }


    if( _borderWidth <= 0 )
      return;
            
    switch( _borderStyle )
    {
        case Single:
        {
            Gfx::Pen pen(static_cast<size_t>(_borderWidth), _borderColor);
            painter.setPen(pen);
            painter.drawPolyline(&outline[0], outline.size());
            break;
        }

        case Border3D:
        {
            std::vector<Gfx::PointF> points1(3);
            points1[0].setX(0);
            points1[0].setY(borderRect.height());

            points1[1].setX(0);
            points1[1].setY(0);
        
            points1[2].setX(0 + borderRect.width());
            points1[2].setY(0);

            Gfx::Pen pen(static_cast<size_t>(_borderWidth), _borderColor);
            painter.setPen(pen);
            painter.drawPolyline(&points1[0], points1.size());

            std::vector<Gfx::PointF> points2(3);
            points2[0].setX(0 + borderRect.width());
            points2[0].setY(0);

            points2[1].setX(0 + borderRect.width());
            points2[1].setY(0 + borderRect.height());

            points2[2].setX(0);
            points2[2].setY(0 + borderRect.height());

            Gfx::Color color(_borderColor.red() * 0.9f, 
                             _borderColor.green() * 0.9f,
                             _borderColor.blue() * 0.9f);

            Gfx::Pen pen2( static_cast<size_t>(_borderWidth), color );
            painter.setPen(pen2);
            painter.drawPolyline(&points2[0], points2.size());	
            break;
        }

        default:
            break;
    }
}


void Panel::onPaintContent(PaintSurface& surface, const Gfx::RectF& updateRect)
{
}

} // namespace

} // namespace

