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


Brush::Brush(BrushData* data)
: _brushData(data)
{
}

Brush Brush::verticalGradient(const Color& from, const Color& to)
{
    BrushData* data = new BrushData(from, to, Vertical);
    return Brush(data);
}


Brush Brush::horizontalGradient(const Color& from, const Color& to)
{
    BrushData* data = new BrushData(from, to, Horizontal);
    return Brush(data);
}


Brush Brush::linearGradient(const Color& from, const Color& to,
                            float angle)
{
    BrushData* data = new BrushData();
    data->setLinearGradient(from, to, angle);
    return Brush(data);
}


Brush Brush::radialGradient(const Color& from, const Color& to,
                            const PointF& focus)
{
    BrushData* data = new BrushData();
    data->setRadialGradient(from, to, focus);
    return Brush(data);
}


Brush Brush::radialGradient(const Color& from, const Color& to,
                            float rx, float ry)
{
    BrushData* data = new BrushData();
    data->setRadialGradient(from, to, rx, ry);
    return Brush(data);
}


Brush Brush::rectangularGradient(const Color& from, const Color& to,
                                 const PointF& focus, float angle)
{
    BrushData* data = new BrushData();
    data->setRectangularGradient(from, to, focus, angle);
    return Brush(data);
}


Brush Brush::rectangularGradient(const Color& from, const Color& to,
                                 float rx, float ry, float angle)
{
    BrushData* data = new BrushData();
    data->setRectangularGradient(from, to, rx, ry, angle);
    return Brush(data);
}



Brush Brush::conicalGradient(const Color& from, const Color& to,
                                 const PointF& focus, float angle)
{
    BrushData* data = new BrushData();
    data->setConicalGradient(from, to, focus, angle);
    return Brush(data);
}


Brush Brush::conicalGradient(const Color& from, const Color& to,
                                 float rx, float ry, float angle)
{
    BrushData* data = new BrushData();
    data->setConicalGradient(from, to, rx, ry, angle);
    return Brush(data);
}


Brush::FillStyle Brush::fillStyle() const
{
    return _brushData->fillStyle();
}


Brush::PositionMode Brush::positionMode() const
{
    return _brushData->positionMode();
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


Brush::GradientStyle Brush::gradient() const
{
    return _brushData->gradient();
}

const Color& Brush::gradientColor() const
{
    return _brushData->gradientColor();
}


float Brush::gradientAngle() const
{
    return _brushData->gradientAngle();
}


const PointF& Brush::gradientFocus() const
{
    return _brushData->gradientFocus();
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


bool Brush::isTexture() const
{
    return _brushData->isTexture();
}


bool Brush::isNull() const
{
    return _brushData->isNull();
}

//////////////////////////////////////////////////////////////////////////
// BrushData
//////////////////////////////////////////////////////////////////////////

BrushData::BrushData(const Image& texture,
                     Pt::int32_t offsetX, Pt::int32_t offsetY)
: _isNull(true)
, _fillStyle(Brush::Solid)
, _positionMode(Brush::Relative)
, _gradient(Brush::Horizontal)
{
    setTexture(texture, offsetX, offsetY);
}


BrushData::BrushData(const Color& from, const Color& to,
                     Brush::GradientStyle g)
: _isNull(false)
, _fillStyle(Brush::Gradient)
, _positionMode(Brush::Relative)
, _color(from)
, _gradient(g)
, _gradientColor(to)
, _gradientAngle(0.0f)
, _ofsX(0)
, _ofsY(0)
{
}


void BrushData::setLinearGradient(const Color& from, const Color& to,
                                  float angle)
{
    _isNull        = false;
    _fillStyle     = Brush::Gradient;
    _positionMode  = Brush::Relative;
    _color         = from;
    _gradient      = Brush::Linear;
    _gradientColor = to;
    _gradientAngle = angle;
    _gradientFocus.set(0.5, 0.5);
    _ofsX          = 0;
    _ofsY          = 0;
    _texture       = Image();
}


void BrushData::setRadialGradient(const Color& from, const Color& to,
                                  const PointF& focus)
{
    _isNull        = false;
    _fillStyle     = Brush::Gradient;
    _positionMode  = Brush::Absolute;
    _color         = from;
    _gradient      = Brush::Radial;
    _gradientColor = to;
    _gradientAngle = 0.0f;
    _gradientFocus = focus;
    _ofsX          = 0;
    _ofsY          = 0;
    _texture       = Image();
}


void BrushData::setRadialGradient(const Color& from, const Color& to,
                                  float rx, float ry)
{
    _isNull        = false;
    _fillStyle     = Brush::Gradient;
    _positionMode  = Brush::Relative;
    _color         = from;
    _gradient      = Brush::Radial;
    _gradientColor = to;
    _gradientAngle = 0.0f;
    _gradientFocus.set(rx, ry);
    _ofsX          = 0;
    _ofsY          = 0;
    _texture       = Image();
}


void BrushData::setRectangularGradient(const Color& from, const Color& to,
                                       const PointF& focus, float angle)
{
    _isNull        = false;
    _fillStyle     = Brush::Gradient;
    _positionMode  = Brush::Absolute;
    _color         = from;
    _gradient      = Brush::Rectangular;
    _gradientColor = to;
    _gradientAngle = angle;
    _gradientFocus = focus;
    _ofsX          = 0;
    _ofsY          = 0;
    _texture       = Image();
}


void BrushData::setRectangularGradient(const Color& from, const Color& to,
                                       float rx, float ry, float angle)
{
    _isNull        = false;
    _fillStyle     = Brush::Gradient;
    _positionMode  = Brush::Relative;
    _color         = from;
    _gradient      = Brush::Rectangular;
    _gradientColor = to;
    _gradientAngle = angle;
    _gradientFocus.set(rx, ry);
    _ofsX          = 0;
    _ofsY          = 0;
    _texture       = Image();
}


void BrushData::setConicalGradient(const Color& from, const Color& to,
                                   const PointF& focus, float angle)
{
    _isNull        = false;
    _fillStyle     = Brush::Gradient;
    _positionMode  = Brush::Absolute;
    _color         = from;
    _gradient      = Brush::Conical;
    _gradientColor = to;
    _gradientAngle = angle;
    _gradientFocus = focus;
    _ofsX          = 0;
    _ofsY          = 0;
    _texture       = Image();
}


void BrushData::setConicalGradient(const Color& from, const Color& to,
                                   float rx, float ry, float angle)
{
    _isNull        = false;
    _fillStyle     = Brush::Gradient;
    _positionMode  = Brush::Relative;
    _color         = from;
    _gradient      = Brush::Conical;
    _gradientColor = to;
    _gradientAngle = angle;
    _gradientFocus.set(rx, ry);
    _ofsX          = 0;
    _ofsY          = 0;
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

    _isNull        = false;
    _fillStyle     = Brush::Texture;
    _positionMode  = Brush::Absolute;

    _gradient      = Brush::Linear;
    _gradientFocus.set(0.0, 0.0);
    _gradientAngle = 0;

    _ofsX          = offsetX;
    _ofsY          = offsetY;
}

} // namespace

} // namespace
