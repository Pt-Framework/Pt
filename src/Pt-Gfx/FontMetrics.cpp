/***************************************************************************
 *   Copyright (C) 2006 PTV AG                                             *
 ***************************************************************************/

#include "Pt/Gfx/FontMetrics.h"


namespace Pt {

namespace Gfx {

FontMetrics::FontMetrics()
: _ascent( 0)
, _descent(0)
, _width(0)
, _height(0)
{ }

FontMetrics::FontMetrics(size_t ascent, size_t descent, size_t width, size_t height)
: _ascent(ascent)
, _descent(descent)
, _width(width)
, _height(height)
{
}

size_t FontMetrics::ascent() const
{
    return _ascent;
}

size_t FontMetrics::descent() const
{
    return _descent;
}

size_t FontMetrics::width() const
{
    return _width;
}

size_t FontMetrics::height() const
{
    return _height;
}


} // namespace Gfx

} // namespace Pt
