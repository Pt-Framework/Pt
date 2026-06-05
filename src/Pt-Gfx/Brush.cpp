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
#include <Pt/Gfx/PaintContext.h>
#include <cmath>
#include <stdexcept>

namespace
{

static const Pt::Gfx::Float TextureNearlyZero = static_cast<Pt::Gfx::Float>(1e-6);


Pt::Gfx::Float normalizeTextureShift(Pt::Gfx::Float value, Pt::Gfx::Float extent)
{
    if(extent <= TextureNearlyZero)
        return 0.0f;

    Pt::Gfx::Float shift = std::fmod(value, extent);
    if(shift < 0.0f)
        shift += extent;

    if(shift <= TextureNearlyZero || extent - shift <= TextureNearlyZero)
        return 0.0f;

    return shift;
}


void drawTextureSlice(Pt::Gfx::Painter& painter, const Pt::Gfx::Image& texture,
                      Pt::Gfx::Float destX, Pt::Gfx::Float destY,
                      Pt::Gfx::Float srcX, Pt::Gfx::Float srcY,
                      Pt::Gfx::Float width, Pt::Gfx::Float height)
{
    if(width <= TextureNearlyZero || height <= TextureNearlyZero)
        return;

    painter.drawImage(
        Pt::Gfx::PointF(destX, destY),
        texture,
        Pt::Gfx::RectF(Pt::Gfx::PointF(srcX, srcY), Pt::Gfx::SizeF(width, height))
    );
}

}

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


Brush::Brush(BrushData* data)
: _brushData(data)
{
}


Brush Brush::imageTexture(const Image& image, const PointF& origin)
{
    BrushData* data = new BrushData(image, origin);
    return Brush(data);
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
        SmartPtr<BrushData> brushData( new BrushData(*_brushData) );
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


const Image& Brush::texture() const
{
    return _brushData->texture();
}


Brush::TextureMode Brush::textureMode() const
{
    return _brushData->textureMode();
}


const PointF& Brush::textureOrigin() const
{
    return _brushData->textureOrigin();
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

BrushData::BrushData(const Image& texture, const PointF& textureOrigin)
: _isNull(true)
, _fillStyle(Brush::Solid)
, _positionMode(Brush::Relative)
, _color(0, 0, 0)
, _gradient(Brush::Horizontal)
, _gradientBeginRadius(0.0f)
, _gradientEndRadius(0.0f)
, _textureMode(Brush::SourcePixels)
, _textureOrigin()
, _texture(0)
{
    setImageTexture(texture, textureOrigin);
}


BrushData::BrushData(const BrushData& other)
: _isNull(other._isNull)
, _fillStyle(other._fillStyle)
, _positionMode(other._positionMode)
, _color(other._color)
, _gradient(other._gradient)
, _gradientStops(other._gradientStops)
, _gradientBegin(other._gradientBegin)
, _gradientBeginRadius(other._gradientBeginRadius)
, _gradientEnd(other._gradientEnd)
, _gradientEndRadius(other._gradientEndRadius)
, _textureMode(other._textureMode)
, _textureOrigin(other._textureOrigin)
, _texture(0)
{
    if(other._texture)
    {
        _texture = new Bitmap;
        _texture->reset(other._texture->image());
    }
}


BrushData& BrushData::operator=(const BrushData& other)
{
    if(this == &other)
        return *this;

    Bitmap* texture = 0;
    if(other._texture)
    {
        texture = new Bitmap;
        texture->reset(other._texture->image());
    }

    delete _texture;

    _isNull = other._isNull;
    _fillStyle = other._fillStyle;
    _positionMode = other._positionMode;
    _color = other._color;
    _gradient = other._gradient;
    _gradientStops = other._gradientStops;
    _gradientBegin = other._gradientBegin;
    _gradientBeginRadius = other._gradientBeginRadius;
    _gradientEnd = other._gradientEnd;
    _gradientEndRadius = other._gradientEndRadius;
    _textureMode = other._textureMode;
    _textureOrigin = other._textureOrigin;
    _texture = texture;
    return *this;
}


BrushData::BrushData(const Color& from, const Color& to,
                     Brush::GradientStyle g)
: _isNull(false)
, _fillStyle(Brush::Gradient)
, _positionMode(Brush::Relative)
, _color(0, 0, 0)
, _gradient(g)
, _gradientBeginRadius(0.0f)
, _gradientEndRadius(0.0f)
, _textureMode(Brush::SourcePixels)
, _textureOrigin()
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
    _positionMode = Brush::Relative;
    _color = color;
    _gradient = Brush::Horizontal;
    _gradientStops.clear();
    _gradientBegin.clear();
    _gradientBeginRadius = 0.0f;
    _gradientEnd.clear();
    _gradientEndRadius = 0.0f;
    _textureMode = Brush::SourcePixels;
    _textureOrigin.clear();
    _isNull = false;

    delete _texture;
    _texture = 0;
}


void BrushData::set1DGradient(Brush::GradientStyle g, const ColorStops& colorStops)
{
    _isNull = false;
    _fillStyle = Brush::Gradient;
    _positionMode = Brush::Absolute;
    _gradient = g;
    _gradientStops = colorStops;
    _gradientBeginRadius = 0.0f;
    _gradientEndRadius = 0.0f;
    _textureMode = Brush::SourcePixels;
    _textureOrigin.clear();

    if(g == Brush::Horizontal)
    {
        _gradientBegin.set(0.0f, 0.5f);
        _gradientEnd.set(1.0f, 0.5f);
    }
    else
    {
        _gradientBegin.set(0.5f, 0.0f);
        _gradientEnd.set(0.5f, 1.0f);
    }

    delete _texture;
    _texture = 0;
}


void BrushData::setLinearGradient(const PointF& begin, const PointF& end,
                                  const ColorStops& colorStops)
{
    _isNull = false;
    _fillStyle = Brush::Gradient;
    _positionMode = Brush::Absolute;
    _gradient = Brush::Linear;
    _gradientStops = colorStops;
    _gradientBegin = begin;
    _gradientBeginRadius = 0.0f;
    _gradientEnd = end;
    _gradientEndRadius = 0.0f;
    _textureMode = Brush::SourcePixels;
    _textureOrigin.clear();

    delete _texture;
    _texture = 0;
}


void BrushData::setLinearGradient(float beginX, float beginY,
                                  float endX, float endY,
                                  const ColorStops& colorStops)
{
    _isNull = false;
    _fillStyle = Brush::Gradient;
    _positionMode = Brush::Relative;
    _gradient = Brush::Linear;
    _gradientStops = colorStops;
    _gradientBegin.set(beginX, beginY);
    _gradientBeginRadius = 0.0f;
    _gradientEnd.set(endX, endY);
    _gradientEndRadius = 0.0f;
    _textureMode = Brush::SourcePixels;
    _textureOrigin.clear();

    delete _texture;
    _texture = 0;
}


void BrushData::setRadialGradient(const PointF& begin, float beginRadius,
                                  const PointF& end, float endRadius,
                                  const ColorStops& colorStops)
{
    _isNull = false;
    _fillStyle = Brush::Gradient;
    _positionMode = Brush::Absolute;
    _gradient = Brush::Radial;
    _gradientStops = colorStops;
    _gradientBegin = begin;
    _gradientBeginRadius = beginRadius;
    _gradientEnd = end;
    _gradientEndRadius = endRadius;
    _textureMode = Brush::SourcePixels;
    _textureOrigin.clear();

    delete _texture;
    _texture = 0;
}


void BrushData::setRadialGradient(float beginX, float beginY, float beginRadius,
                                  float endX, float endY, float endRadius,
                                  const ColorStops& colorStops)
{
    _isNull = false;
    _fillStyle = Brush::Gradient;
    _positionMode = Brush::Relative;
    _gradient = Brush::Radial;
    _gradientStops = colorStops;
    _gradientBegin.set(beginX, beginY);
    _gradientBeginRadius = beginRadius;
    _gradientEnd.set(endX, endY);
    _gradientEndRadius = endRadius;
    _textureMode = Brush::SourcePixels;
    _textureOrigin.clear();

    delete _texture;
    _texture = 0;
}



const Image& BrushData::texture() const
{
    if( ! _texture )
        throw std::logic_error("invalid texture");

    return _texture->image();
}


void BrushData::setImageTexture(const Image& texture, const PointF& textureOrigin)
{
    delete _texture;
    _texture = 0;

    _texture = new Bitmap;

    const Pt::Gfx::Float textureWidth = static_cast<Pt::Gfx::Float>(texture.width());
    const Pt::Gfx::Float textureHeight = static_cast<Pt::Gfx::Float>(texture.height());
    const Pt::Gfx::Float shiftX = normalizeTextureShift(textureOrigin.x(), textureWidth);
    const Pt::Gfx::Float shiftY = normalizeTextureShift(textureOrigin.y(), textureHeight);

    if(shiftX <= TextureNearlyZero && shiftY <= TextureNearlyZero)
    {
        _texture->reset(texture);
    }
    else
    {
        _texture->reset(SizeF(textureWidth, textureHeight));

        PaintContext ctx(*_texture);
        Painter painter(ctx);
        painter.setCompositionMode(CompositionMode::SourceCopy);

        const Pt::Gfx::Float rightWidth = textureWidth - shiftX;
        const Pt::Gfx::Float bottomHeight = textureHeight - shiftY;

        drawTextureSlice(painter, texture,
                         0.0f, 0.0f,
                         shiftX, shiftY,
                         rightWidth, bottomHeight);

        drawTextureSlice(painter, texture,
                         rightWidth, 0.0f,
                         0.0f, shiftY,
                         shiftX, bottomHeight);

        drawTextureSlice(painter, texture,
                         0.0f, bottomHeight,
                         shiftX, 0.0f,
                         rightWidth, shiftY);

        drawTextureSlice(painter, texture,
                         rightWidth, bottomHeight,
                         0.0f, 0.0f,
                         shiftX, shiftY);
    }

    _isNull = false;
    _fillStyle = Brush::Texture;
    _positionMode = Brush::Absolute;
    _gradient = Brush::Horizontal;
    _gradientStops.clear();
    _gradientBegin.clear();
    _gradientBeginRadius = 0.0f;
    _gradientEnd.clear();
    _gradientEndRadius = 0.0f;
    _textureMode = Brush::SourcePixels;
    _textureOrigin = textureOrigin;
}

} // namespace

} // namespace
