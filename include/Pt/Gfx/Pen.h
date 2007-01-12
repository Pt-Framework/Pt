/***************************************************************************
 *   Copyright (C) 2006 PTV AG                                             *
 ***************************************************************************/

#ifndef PTV_GFX_PEN_H
#define PTV_GFX_PEN_H

#include <Pt/Api.h>
#include <Pt/Gfx/ARgbColor.h>


namespace Pt {

namespace Gfx {

	/**
	 * @brief A pen which contains of attributes (size, color) for the drawing of outlines.
	 *
	 * Pen objects are used as container of drawing attributes for Painter objects. A size
	 * and a color can be specified per pen. The size and color are used to draw outlined
	 * shapes by the Painter. Outlined shapes for example are lines, outlined rectangles
	 * or ellipses and text.
	 *
	 * Example: When setting the pen color to green and the pen size to 5 and setting this
	 * Pen object as pen for a Painter, painting a line would result in a 5-pixel-sized
	 * green line.
	 *
	 * The Pen object is immutable. Thus a new object has to be created when a pen with
	 * other attributes is needed.
	 */
	class PTV_API Pen
	{
		friend bool operator==(const Pen& a, const Pen& b);

		public:
			/**
			 * @brief Creates a new Pen object using the specified size and color.
			 *
			 * The pen size and color are optional. The default pen size is 1. The
			 * default pen color is black.
			 *
			 * @param size The size of the pen. This parameter is optional. The default is 1.
			 * @param color The color of the pen. This parameter is optional. The default is black.
			 */
			Pen(size_t size = 1, const ARgbColor& color = ARgbColor(0, 0, 0));

			/**
			 * @brief Returns the size of the pen as specified when created.
			 *
			 * @return The size of the pen.
			 */
			size_t size() const;

			/**
			 * @brief Returns a reference to the color of the pen as specified when created.
			 *
			 * @return The color of the pen.
			 */
			const ARgbColor& color() const;

		private:
			size_t    _size;
			ARgbColor _color;
	};

	/**
	 * @brief Equality-operator (==) which compares the given Pen's by comparing their
	 * properties.
	 *
	 * The size and color are compared. If all values are the same, $true$ is returned;
	 * $false$ otherwise.
	 *
	 * @param a The Pen object to compare with Pen object b.
	 * @param b The Pen object to compare with Pen object a.
	 * @return $true$ when the Pen objects are the same; $false$ otherwise.
	 */
	inline bool operator==(const Pen& a, const Pen& b)
	{
		return a._size == b._size && a._color == b._color;
	}

} // namespace Gfx

} // namespace Pt

#endif
