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
#include <Pt/Hmi/PaintSurface.h>
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Rect.h>
#include <Pt/Gfx/BlockScale.h>

namespace Pt {

namespace Hmi {

Panel::Panel()
: _image()
, _layout( ImageLayout::None )
, _planeColor( Gfx::Color::fromRgb8(0,0,0) )
, _hasPlaneColor(false)
{
    setAcceptsFocus(false);
}


Panel::~Panel()
{
}


const Gfx::Color* Panel::planeColor() const
{
    return _hasPlaneColor ? &_planeColor : 0;
}


void Panel::setPlaneColor(const Gfx::Color& color)
{
    _planeColor = color;
    _hasPlaneColor = true;
    update();
}


void Panel::setImage(const Gfx::Image& image, ImageLayout layout)
{
    if(layout == ImageLayout::Strech || layout ==  ImageLayout::Zoom)
        _image = image;
            
    _layout = layout;
    _picture.set(image);
    update();
}   


void Panel::onResizeEvent(const ResizeEvent& ev)
{
    Widget::onResizeEvent(ev);

    if( _picture.empty() || ev.size().width() < 1 || ev.size().height() < 1 )
     return;

    switch( _layout.type() )
    {
        case ImageLayout::Strech:
        {
            Gfx::Size newSize( (int) ev.size().width(), 
                               (int)ev.size().height() );

            Gfx::Image streched(_image.format(), newSize);
            
            Gfx::blockScale( _image.begin(),_image.width(), _image.height(),
                             streched.begin(), streched.width(), streched.height() );

            _picture.set(streched);
        }
        break;

        case ImageLayout::Zoom:
        {
            const double factor = ev.size().width() / _image.width();
            
            Pt::Gfx::Size newSize( ( size_t)(_image.width() * factor), 
                                   (size_t)(_image.height() * factor) );

            Gfx::Image streched(_image.format(), newSize);
            
            Gfx::blockScale( _image.begin(),_image.width(), _image.height(),
                             streched.begin(), streched.width(), streched.height() );

            _picture.set(streched);
        }
        break;
    }  
}


void Panel::onPaint(PaintSurface& surface, const Gfx::RectF& updateRect)
{
    const PanelRenderer* renderer = getFacet<PanelRenderer>();

    if(renderer)
        renderer->render(*this, surface, updateRect);

    if( ! _picture.empty() )
    {
        Painter painter(surface);
        painter.setCompositionMode(Gfx::CompositionMode::SourceOver);

        const Gfx::SizeF& size = this->size();

        switch( _layout.type() )
        {
            default:
                painter.drawPicture( Pt::Gfx::PointF(0,0), _picture );
                break;

            case ImageLayout::Tile:
            {
                for( double x = 0; x < size.width();  x += _picture.width() )
                {
                    for( double y = 0; y < size.height();  y += _picture.height() )
                        painter.drawPicture(Gfx::PointF(x,y), _picture);
                }
                break;
            }
            
            case ImageLayout::Center:
            {
                const double x = size.width()/2  - _picture.width()/2;
                const double y = size.height()/2  - _picture.height()/2;
                painter.drawPicture(Gfx::PointF(x, y), _picture);
                break;
            }
        }
    }  

    Frame::onPaint(surface, updateRect);
}

} // namespace

} // namespace

