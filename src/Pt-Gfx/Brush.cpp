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
#include <Pt/Gfx/Bitmap.h>
#include <Pt/Gfx/Painter.h>
#include <stdexcept>

namespace Pt {

namespace Gfx {

void ColorStops::calculateInterpolatedColor(Color& res, const float position) const
{
    // If the position is less than or equal to the first position,
    // then simply return the first color
    if(position <= _stops[0].position()) {
        res = _stops[0].color();
        return;
    }

    // If the position is greater than or equal to the last position,
    // then simply return the first color
    if(position >= _stops.back().position()) {;
        res = _stops.back().color();
        return;
    }

    // Find out in what two stops the position is between
    std::size_t stopIndex = 0;
    while(stopIndex < _stops.size()) {
        if(position < _stops[stopIndex].position()) break;
        ++stopIndex;
    }

    // Should never happen, but just for safety
    if(stopIndex == _stops.size()) {
        res = _stops.back().color();
        return;
    }

    // Get the positions and colors
    const float       p1 = _stops[stopIndex - 1]. position     ();
    const Pt::int32_t r1 = _stops[stopIndex - 1]. color().red  ();
    const Pt::int32_t g1 = _stops[stopIndex - 1]. color().green();
    const Pt::int32_t b1 = _stops[stopIndex - 1]. color().blue ();
    const Pt::int32_t a1 = _stops[stopIndex - 1]. color().alpha();

    const float       p2 = _stops[stopIndex    ]. position     ();
    const Pt::int32_t r2 = _stops[stopIndex    ]. color().red  ();
    const Pt::int32_t g2 = _stops[stopIndex    ]. color().green();
    const Pt::int32_t b2 = _stops[stopIndex    ]. color().blue ();
    const Pt::int32_t a2 = _stops[stopIndex    ]. color().alpha();

    // Scale the position
    const float spos = (position - p1) / (p2 - p1);

    // Return the interpolated the color
    res = Color(
        static_cast<Pt::uint8_t>(a1 + (a2 - a1) * spos),
        static_cast<Pt::uint8_t>(r1 + (r2 - r1) * spos),
        static_cast<Pt::uint8_t>(g1 + (g2 - g1) * spos),
        static_cast<Pt::uint8_t>(b1 + (b2 - b1) * spos)
    );
}


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


Brush Brush::verticalGradient(const ColorStops& colorStops)
{
    BrushData* data = new BrushData();
    data->set1DGradient(Brush::Vertical, colorStops);
    return Brush(data);
}


Brush Brush::horizontalGradient(const ColorStops& colorStops)
{
    BrushData* data = new BrushData();
    data->set1DGradient(Brush::Horizontal, colorStops);
    return Brush(data);
}


Brush Brush::linearGradient(const PointF& begin, const PointF& end,
                            const ColorStops& colorStops)
{
    BrushData* data = new BrushData();
    data->setLinearGradient(begin, end, colorStops);
    return Brush(data);
}


Brush Brush::linearGradient(float beginX, float beginY,
                            float endX, float endY,
                            const ColorStops& colorStops)
{
    BrushData* data = new BrushData();
    data->setLinearGradient(beginX, beginY, endX, endY, colorStops);
    return Brush(data);
}


Brush Brush::radialGradient(const PointF& begin, float beginRadius,
                            const PointF& end, float endRadius,
                            const ColorStops& colorStops)
{
    BrushData* data = new BrushData();
    data->setRadialGradient(begin, beginRadius, end, endRadius, colorStops);
    return Brush(data);
}


Brush Brush::radialGradient(float beginX, float beginY, float beginRadius,
                            float endX, float endY, float endRadius,
                            const ColorStops& colorStops)
{
    BrushData* data = new BrushData();
    data->setRadialGradient(beginX, beginY, beginRadius,
                            endX, endY, endRadius, colorStops);
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

    _brushData->setSolid(color);
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


const ColorStops& Brush::gradientStops() const
{
    return _brushData->gradientStops();
}


const PointF& Brush::gradientBegin() const
{
    return _brushData->gradientBegin();
}


float Brush::gradientBeginRadius() const
{
    return _brushData->gradientBeginRadius();
}


const PointF& Brush::gradientEnd() const
{
    return _brushData->gradientEnd();
}


float Brush::gradientEndRadius() const
{
    return _brushData->gradientEndRadius();
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
, _ofsX(0)
, _ofsY(0)
, _texture(0)
{
    setTexture(texture, offsetX, offsetY);
}


BrushData::BrushData(const Color& from, const Color& to,
                     Brush::GradientStyle g)
: _isNull(false)
, _fillStyle(Brush::Gradient)
, _positionMode(Brush::Relative)
, _gradient(g)
, _ofsX(0)
, _ofsY(0)
, _texture(0)
{
    _gradientStops.add(0.0, from);
    _gradientStops.add(1.0, to);

    if(g == Brush::Horizontal)
    {
        _gradientBegin.set(0.0f, 0.5f);
        _gradientEnd.set(1.0f, 0.5f);
    }
    else // Vertical
    {
        _gradientBegin.set(0.5f, 0.0f);
        _gradientEnd.set(0.5f, 1.0f);
    }
}


BrushData::~BrushData()
{
    delete _texture;
}


void BrushData::setSolid(const Color& color)
{
    _fillStyle = Brush::Solid;
    _color     = color;
    _isNull    = false;

    delete _texture;
    _texture = 0;
}


void BrushData::set1DGradient(Brush::GradientStyle g, const ColorStops& colorStops)
{
    _isNull        = false;
    _fillStyle     = Brush::Gradient;
    _positionMode  = Brush::Absolute;

    _gradient      = g;
    _gradientStops = colorStops;

    _ofsX          = 0;
    _ofsY          = 0;
    
    delete _texture;
    _texture = 0;
}


void BrushData::setLinearGradient(const PointF& begin, const PointF& end,
                                  const ColorStops& colorStops)
{
    _isNull        = false;
    _fillStyle     = Brush::Gradient;
    _positionMode  = Brush::Absolute;

    _gradient      = Brush::Linear;
    _gradientStops = colorStops;
    _gradientBegin = begin;
    _gradientEnd   = end;

    _ofsX          = 0;
    _ofsY          = 0;
    
    delete _texture;
    _texture = 0;
}


void BrushData::setLinearGradient(float beginX, float beginY,
                                  float endX, float endY,
                                  const ColorStops& colorStops)
{
    _isNull        = false;
    _fillStyle     = Brush::Gradient;
    _positionMode  = Brush::Relative;

    _gradient      = Brush::Linear;
    _gradientStops = colorStops;
    _gradientBegin.set(beginX, beginY);
    _gradientEnd.set(endX, endY);

    _ofsX          = 0;
    _ofsY          = 0;
    
    delete _texture;
    _texture = 0;
}


void BrushData::setRadialGradient(const PointF& begin, float beginRadius,
                                  const PointF& end, float endRadius,
                                  const ColorStops& colorStops)
{
    _isNull        = false;
    _fillStyle     = Brush::Gradient;
    _positionMode  = Brush::Absolute;

    _gradient      = Brush::Radial;
    _gradientStops = colorStops;
    _gradientBegin = begin;
    _gradientBeginRadius = beginRadius;
    _gradientEnd = end;
    _gradientEndRadius = endRadius;

    _ofsX          = 0;
    _ofsY          = 0;
    
    delete _texture;
    _texture = 0;
}


void BrushData::setRadialGradient(float beginX, float beginY, float beginRadius,
                                  float endX, float endY, float endRadius,
                                  const ColorStops& colorStops)
{
    _isNull        = false;
    _fillStyle     = Brush::Gradient;
    _positionMode  = Brush::Relative;

    _gradient      = Brush::Radial;
    _gradientStops = colorStops;
    _gradientBegin.set(beginX, beginY);
    _gradientBeginRadius = beginRadius;
    _gradientEnd.set(endX, endY);
    _gradientEndRadius = endRadius;

    _ofsX          = 0;
    _ofsY          = 0;
    
    delete _texture;
    _texture = 0;
}



const Image& BrushData::texture() const
{
    if( ! _texture )
        throw std::logic_error("invalid texture");

    return _texture->image();
}


void BrushData::setTexture(const Image& texture,
                           Pt::int32_t offsetX, Pt::int32_t offsetY)
{
    delete _texture;
    _texture = 0;

    _texture = new Bitmap;

    // The texture has no offset
    if( ! offsetX && ! offsetY )
    {
        _texture->reset( texture);
    }
    else // The texture has offset
    {
        // Prepare the destination texture
        _texture->reset( SizeF(texture.width(), texture.height()) );

        Painter painter(*_texture);
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

    _gradientStops.clear();

    _ofsX          = offsetX;
    _ofsY          = offsetY;
}

} // namespace

} // namespace
