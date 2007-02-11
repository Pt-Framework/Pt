/***************************************************************************
 *   Copyright (C) 2006 PTV AG                                             *
 ***************************************************************************/

#include "Pt/Gfx/Brush.h"
#include <algorithm>


namespace Pt {

namespace Gfx {

Brush::Brush(const ARgbColor& color)
: _brushData( new BrushData(SolidFill, color, 0) )
{
}


Brush::Brush(const ARgbImage* texture)
: _brushData( new BrushData(TextureFill, ARgbColor(0, 0, 0), texture) )
{
}


Brush::FillStyle Brush::fillStyle() const
{
    return _brushData->fillStyle();
}


const ARgbColor& Brush::color() const
{
    return _brushData->color();
}


const ARgbImage& Brush::texture() const
{
    return _brushData->texture();
}


BrushData::BrushData(Brush::FillStyle fillStyle, const ARgbColor& color, const ARgbImage* texture)
: _fillStyle(fillStyle)
, _color(color)
, _texture(0)
{
    if (texture != 0) {
        _texture = new ARgbImage(*texture);
    }
    else
    {
        _texture = new ARgbImage(64, 1, color);
    }
}


BrushData::~BrushData()
{
    delete _texture;
}


Brush::FillStyle BrushData::fillStyle() const
{
    return _fillStyle;
}


const ARgbColor& BrushData::color() const
{
    return _color;
}


const ARgbImage& BrushData::texture() const
{
    return *_texture;
}

} // namespace Gfx

} // namespace Pt
