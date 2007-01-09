/***************************************************************************
 *   Copyright (C) 2006 PTV AG                                             *
 ***************************************************************************/

#include "Pt/Gfx/Font.h"


using namespace std;

namespace Pt {

namespace Gfx {

Font::Font(std::string name, size_t size, FontStyle fontStyle, ssize_t angle, Direction direction)
: _name(name),
  _size(size),
  _fontStyle(fontStyle),
  _angle(angle),
  _direction(direction)
{
}


std::string Font::name() const
{
	return _name;
}


size_t Font::size() const
{
	return _size;
}


Font::FontStyle Font::fontStyle() const
{
	return _fontStyle;
}


ssize_t Font::angle() const
{
	return _angle;
}


Font::Direction Font::direction() const
{
	return _direction;
}


} // namespace Gfx

} // namespace Pt
