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

bool operator==(const Brush& a, const Brush& b)
{
	return a._brushData->fillStyle() == b._brushData->fillStyle() &&
	       a._brushData->color() == b._brushData->color();
//           && a._brushData->texture() == b._brushData->texture();    //real image comparison needed ???
}

bool operator<(const Brush& a, const Brush& b)
{
	return a._brushData->fillStyle() < b._brushData->fillStyle();
}


void get( const SerializationEntry& e, Gfx::Brush& brush )
{
    brush = Gfx::Brush( Gfx::ARgbColor::fromHtml( e.value().str() ) );
}


void set( SerializationEntry& e, const Gfx::Brush& brush )
{
    e.setValue( brush.color().toHtml() );
    e.setTypeName("Brush");
}

} // namespace Gfx

} // namespace Pt
