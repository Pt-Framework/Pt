#include "Pt/Gfx/Font.h"

using namespace std;


namespace Pt {

namespace Gfx {

Font::Font(string name, size_t size, FontStyle fontStyle, size_t angle, Direction direction)
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


Font::FontStyle Font::fontStyle() const
{
	return _fontStyle;
}


size_t Font::size() const
{
	return _size;
}


size_t Font::angle() const
{
	return _angle;
}


Font::Direction Font::direction() const
{
	return _direction;
}


void Font::setName(std::string name)
{
	_name = name;
}


void Font::setFontStyle(Font::FontStyle fontStyle)
{
	_fontStyle = fontStyle;
}


void Font::setSize(size_t size)
{
	_size = size;
}


void Font::setAngle(size_t angle)
{
	_angle = angle;
}


void Font::setDirection(Font::Direction direction)
{
	_direction = direction;
}


} // namespace gfx

} // namespace ptv
