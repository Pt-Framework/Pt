/* Copyright (C) 2006-2017 Marc Boris Duerner
   Copyright (C) 2017-2017 Aloysius Indrayanto

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

#include <Pt/Gfx/Brush.h>
#include <Pt/Gfx/ImagePainter.h>
#include <stdexcept>

namespace Pt {

namespace Gfx {

Brush::Brush()
: _brushData( new BrushData() )
{}


Brush::Brush(const Color& color)
: _brushData( new BrushData(color) )
{}


Brush::Brush(const Image& texture, 
             Pt::int32_t offsetX, Pt::int32_t offsetY)
: _brushData( new BrushData(texture, offsetX, offsetY) )
{}


Brush::Brush(const Color& from, const Color& to, GradientDirection g, 
            float angle, Pt::int32_t ofsX, Pt::int32_t ofsY)
: _brushData( new BrushData(from, to, g, angle, ofsX, ofsY) )
{}


Brush::FillStyle Brush::fillStyle() const
{ 
    return _brushData->fillStyle(); 
}


void Brush::setColor(const Color& color)
{
    // COW
    if(_brushData.refs() > 1) {
        SmartPtr<BrushData> brushData( new BrushData() );
        *brushData = *_brushData;
        _brushData = brushData;
    }

    _brushData->setColor(color);
}


const Color& Brush::color() const
{ 
    return _brushData->color(); 
}


void Brush::setGradient(const Color& from, const Color& to, GradientDirection g, 
                        float angle, Pt::int32_t ofsX, Pt::int32_t ofsY)
{
    // COW
    if(_brushData.refs() > 1) {
        SmartPtr<BrushData> brushData( new BrushData() );
        *brushData = *_brushData;
        _brushData = brushData;
    }

    _brushData->setGradient(from, to, g, angle, ofsX, ofsY);
}


void Brush::setGradientOffset(Pt::int32_t ofsX, Pt::int32_t ofsY)
{
    if(!_brushData->isGradient()) 
        throw std::logic_error("brush error: not a gradient");

    // COW
    if(_brushData.refs() > 1) {
        SmartPtr<BrushData> brushData( new BrushData() );
        *brushData = *_brushData;
        _brushData = brushData;
    }

    _brushData->setGradientOffset(ofsX, ofsY);
}


const Color& Brush::gradientColor() const
{ 
    return _brushData->gradientColor(); 
}


void Brush::setTexture(const Image& texture, 
                       Pt::int32_t offsetX, Pt::int32_t offsetY)
{
    // COW
    if(_brushData.refs() > 1) 
    {
        SmartPtr<BrushData> brushData( new BrushData() );
        *brushData = *_brushData;
        _brushData = brushData;
    }

    _brushData->setTexture(texture, offsetX, offsetY);
}


const Image& Brush::texture() const
{ 
    return _brushData->texture(); 
}


float Brush::gradientAngle() const
{ 
    return _brushData->gradientAngle(); 
}


Pt::int32_t Brush::offsetX() const
{ 
    return _brushData->offsetX(); 
}


Pt::int32_t Brush::offsetY() const
{ 
    return _brushData->offsetY(); 
}


bool Brush::isGradient() const
{ 
    return _brushData->isGradient(); 
}


bool Brush::isGradient1D() const
{ 
    return _brushData->isGradient1D(); 
}


bool Brush::isGradient2D() const
{ 
    return _brushData->isGradient2D(); 
}


bool Brush::isTexture() const
{ 
    return _brushData->isTexture(); 
}


bool Brush::isNull() const
{ 
    return _brushData->isNull(); 
}



BrushData::BrushData(const Color& from, const Color& to, 
                     Brush::GradientDirection g, float angle, 
                     Pt::int32_t ofsX, Pt::int32_t ofsY)
: _isNull       (false)
, _color        (from)
, _gradientColor(to)
, _gradientAngle(angle)
, _ofsX         (ofsX)
, _ofsY         (ofsY)
{
    switch(g) 
    {
        case Brush::Horizontal  : _fillStyle = Brush::HorizontalGradient;  break;
        case Brush::Vertical    : _fillStyle = Brush::VerticalGradient;    break;
        case Brush::Linear      : _fillStyle = Brush::LinearGradient;      break;
        case Brush::Rectangular : _fillStyle = Brush::RectangularGradient; break;
        case Brush::Radial      : _fillStyle = Brush::RadialGradient;      break;
        case Brush::Conical     : _fillStyle = Brush::ConicalGradient;     break;
        default                 : _fillStyle = Brush::Solid;               break;
    }
}


void BrushData::setGradient(const Color& from, const Color& to, 
                          Brush::GradientDirection g, float angle, 
                          Pt::int32_t ofsX, Pt::int32_t ofsY)
{
    switch(g) 
    {
        case Brush::Horizontal  : _fillStyle = Brush::HorizontalGradient;  break;
        case Brush::Vertical    : _fillStyle = Brush::VerticalGradient;    break;
        case Brush::Linear      : _fillStyle = Brush::LinearGradient;      break;
        case Brush::Rectangular : _fillStyle = Brush::RectangularGradient; break;
        case Brush::Radial      : _fillStyle = Brush::RadialGradient;      break;
        case Brush::Conical     : _fillStyle = Brush::ConicalGradient;     break;
        default                 : _fillStyle = Brush::Solid;               break;
    }

    _color         = from;
    _gradientColor = to;
    _gradientAngle = angle;
    _ofsX          = ofsX;
    _ofsY          = ofsY;
    _isNull        = false;

    _texture       = Image();
}


void BrushData::setTexture(const Image& texture, 
                           Pt::int32_t offsetX, Pt::int32_t offsetY)
{
    // The texture has no offset
    if( ! offsetX && ! offsetY ) 
    {
        _texture = texture;
    }
    else // The texture has offset
    {
        // Prepare the destination texture
        _texture.reset(texture.format(), texture.size());
        
        // Prepare the image painter
        // ### TODO: Use the new painter later! ###
        ImagePainter painter(_texture);
        painter.setCompositionMode(CompositionMode::SourceCopy);
        
        // Calculate the source and destination coordinate
        Pt::int32_t sx, dx;
        if(offsetX >= 0) 
        {
            sx = offsetX % texture.width();
            dx = 0;
        }
        else 
        {
            sx = 0;
            dx = (-offsetX) % texture.width();
        }
        
        Pt::int32_t sy, dy;
        if(offsetY >= 0) 
        {
            sy = offsetY % texture.height();
            dy = 0;
        }
        else 
        {
            sy = 0;
            dy = (-offsetY) % texture.height();
        }
        
        // Draw on the main area
        painter.drawImage( PointF(dx, dy), texture,
                           RectF( PointF(sx, sy), 
                                  SizeF(texture.width() - sx, texture.height() - sy)) );
        
        if( ! dx && ! dy) // positive offset
        {
            // Draw on the right/top-right hole
            painter.drawImage(
                PointF(texture.width() - sx, dy), texture,
                RectF(PointF(0, sy), SizeF(texture.width() - sx, texture.height() - sy))
            );
            // Draw on the bottom/bottom-left hole
            painter.drawImage(
                 PointF(dx, texture.height() - sy), texture,
                 RectF(PointF(sx, 0), SizeF(texture.width() - sx, texture.height() - sy))
            );
            // Draw on the bottom-right hole
            painter.drawImage(
                PointF(texture.width() - sx, texture.height() - sy), texture,
                RectF(PointF(0, 0), SizeF(texture.width() - sx, texture.height() - sy))
            );
        }
        else if(!sx && !sy) // negative offset
        {
            // Draw on the left/bottom-left hole
            painter.drawImage(
                PointF(0, dy), texture,
                RectF(PointF(texture.width() - dx, 0), SizeF(dx, texture.height() - dy))
            );
            // Draw on the top/top-right hole
            painter.drawImage(
                PointF(dx, 0), texture,
                RectF(PointF(0, texture.height() - dy), SizeF(texture.width() - dx, dy))
            );
            // Draw on the left-top hole
            painter.drawImage(
                PointF(0, 0), texture,
                RectF(PointF(texture.width() - dx, texture.height() - dy), SizeF(dx, dy))
            );
        }
        else if( ! dx && ! sy) // Mixed offset
        {
            // Draw on the top-left hole
            painter.drawImage(
                PointF(0, 0), texture,
                RectF(PointF(sx, texture.height() - dy), SizeF(texture.width() - sx, dy))
            );
            // Draw on the top-right hole
            painter.drawImage(
                PointF(sx, 0), texture,
                RectF(PointF(0, texture.height() - dy), SizeF(sx, dy))
            );
            // Draw on the bottom-right hole
            painter.drawImage(
                PointF(sx, dy), texture,
                RectF(PointF(0, 0), SizeF(sx, dy))
            );
        }
        else if( ! sx && ! dy ) // Mixed offset
        {
            // Draw on the top-left hole
            painter.drawImage(
                PointF(0, 0), texture,
                RectF(PointF(texture.width() - dx, sy), SizeF(dx, texture.height() - sy))
            );
            // Draw on the bottom-left hole
            painter.drawImage(
                PointF(0, sy), texture,
                RectF(PointF(texture.width() - dx, 0), SizeF(dx, sy))
            );
            // Draw on the bottom-right hole
            painter.drawImage(
                PointF(dx, sy), texture,
                RectF(PointF(0, 0), SizeF(dx, sy))
            );
        }
    }

    _fillStyle = Brush::Texture;
    _ofsX      = offsetX;
    _ofsY      = offsetY;
    _isNull    = false;
}

} // namespace

} // namespace
