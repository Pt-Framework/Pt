/*
 * Copyright (C) 2006-2007 by Marc Boris Duerner
 * Copyright (C) 2006-2007 PTV AG
 * Copyright (C) 2010 Aloysius Indrayanto
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */
#ifndef PT_GFX_PAINTER_H
#define PT_GFX_PAINTER_H

#include <Pt/Gfx/Api.h>
#include <Pt/Gfx/Gfx.h>
#include <Pt/Gfx/Size.h>
#include <Pt/Gfx/Point.h>
#include <Pt/String.h>

#include <cstddef>
#include <string>
#include <list>


namespace Pt {

namespace Gfx {

    /**
     * \brief A generic painter interface which provides methods to draw graphical primitives to surfaces.
     *
     * A painter in principle allows painting on any device for which a Painter-implementation is provided.
     * Common surfaces in graphical user interfaces are widgets, images, pixmaps, the printer or files.
     *
     * The Painter class provides methods for drawing outlined and filled shapes like rectangles, circles,
     * ellipses, polygons, but also simpler graphical primitives like pixels, lines and polylines.
     *
     * The Painter-API distinguishes between outlined shapes and filled shapes. Outlined shapes for example
     * are points, lines, outlined rectangle, outlined circles/ellipses, polylines and Text. Filled shapes
     * for example are filled rectangles, filled circles/ellipses and polygons.
     *
     * The Pen is used to draw outlined shapes, the Brush is used to draw filled shapes.
     *
     * The Painter object encapsulates state information needed for the rendering of the graphical primitives:
     * - Pen: The Pen is used to draw outlined shapes. It has a color and a size. A blue pen with a pen-size
     * of 5 will draw a line in blue color with a line size of 5 pixels, for example. This is valid for all
     * outlined shapes, except Text-drawing. For Texts only the pen color, but not the pen-size is used.
     * - Brush: The Brush is used to draw filled shapes. It can have two painting occurences: Solid color
     * or Texture. When the brush is set to a solid color, the filling of the rectangle is completely drawn
     * with this color. When a Texture (ARgbImage) is specified as brush Texture, the interior of the rectangle
     * is painted using this Texture.
     * - Font: The Font is used to draw Text. It specifies the font-family name, the font style (normal, italic,
     * bold), the rotation angle and the font size. The pen color is used as color to draw the Text.

     * To set the Pen of the painter the method Painter::setPen() is used. To set the Brush of the painter the
     * method Painter::setBrush() is used. Both properties are separate and don't interfere with each other.
     * To set the font of the painter the method Painter::setFont() is used.
     *
     * To draw a rectangle with both filling and outline, first a filled rectangle and afterwards an outlined
     * rectangle with the same position and size can be drawn.
     *
     * All coordinates which appear as parameter for drawing operations are considered relative to the
     * top-left corner of the drawing area the painter was created for. All drawing or writing is done
     * in the current color, using the current paint mode, and in the current font.
     */

		namespace AlphaBlending
		{	
			enum Type
			{
				NoAlpha,
				AlphaMask,
				AlphaBlend
			};
		}

    class PT_GFX_API Painter
    {
        public:						

            //! @brief Empty virtual destructor.
            virtual ~Painter()
            {}


						virtual void setRenderMode(AlphaBlending::Type mode) = 0;

            /**
             * @brief Sets the pen of this painter to the given pen.
             *
             * All drawing operations which include line-drawing, for example outlines
             * or simple lines, use this pen for drawing.
             * The pen attributes consist of the pen size and the pen color.
             *
             * @param pen The pen to be set as new pen for this painter.
             * @see pen()
             */
            virtual void setPen(const Gfx::Pen& pen) = 0;

            /**
             * @brief Returns the current pen of this painter.
             *
             * @return The current pen of this painter.
             * @see setPen(Pen&)
             */
            virtual const Pen& pen() const = 0;

            /**
             * @brief Sets the brush of this painter to the given brush.
             *
             * All drawing operations which include surface filling, for example the
             * interior of a rectangle or ellipse, use this brush for drawing.
             * The brush attributes consist of the brush color or Texture.
             *
             * Setting a Texture brush that does not have an image or has an image of
             * size 0 will result in undefinied behaviour when trying to draw a filled
             * surface with this brush!
             *
             * @param brush The brush to be set as new brush for this painter.
             * @see brush()
             */
            virtual void setBrush(const Gfx::Brush& brush) = 0;

            /**
             * @brief Returns the current brush of this painter.
             *
             * @return The current brush of this painter.
             * @see setBrush(Brush&)
             */
            virtual const Brush& brush() const = 0;

            /**
             * @brief Sets the font of this painter to the given font.
             *
             * All Text output with this painter will be done using this font.
             * The font attributes const of the font face, the font weight, the
             * font size, the rotation of the font and the writing order.
             *
             * @param font The font to be set as new font for this painter.
             * @see font()
             */
            virtual void setFont(const Gfx::Font& font) = 0;

            /**
             * @brief Returns the current font of this painter.
             *
             * @return The current font of this painter.
             * @see setFont(Font&)
             */
            virtual const Font& font() const = 0;

            //static const std::vector<std::string> listFontNames() const = 0;

            /**
             * @brief Returns the general font metrics of the currently selected font.
             *
             * The font metrics contain the font's ascent, descent and height.
             * The width-attribute of the returned FontMetrics object always is set to 0.
             *
             * To measure the size of a Text, use fontMetrics(std::string).
             *
             * @return The font metrics of the currently selected font.
             * @see fontMetrics(std::string)
             */
            virtual FontMetrics fontMetrics() const = 0;

            /**
             * @brief Returns the metrics of the given Text for the currently selected font.
             *
             * The metrics, which are returned contain the default metrics of the font: ascent,
             * descent and height. Additionally the width for showing the specified string object
             * using the currently selected font is
             * The width-attribute of the returned FontMetrics object always is set to 0.
             *
             * To measure the size of a Text, use fontMetrics(std::string).
             *
             * @return The font metrics of the currently selected font.
             * @see fontMetrics(std::string)
             */
            virtual FontMetrics fontMetrics(Pt::String Text) const = 0;

            /**
             * @brief Returns a list of installed font (family) names on the current platform and device.
             *
             * The returned font family names list contains all font names, that may be used to
             * create a new Font object using the Font constructor. The returned set of font names
             * not only depends on the installed fonts of the platform, but also on the device for
             * which this painter is active. A printer device might provide more, less or different
             * fonts than a display (widget) device.
             *
             * @return A list of installed font names on the current platform and device.
             */
            virtual const std::list<std::string>& fontFamilyNames() = 0;

            /**
             * @brief Draws a single pixel at the specified position.
             *
             * The current pen color of this painter is used to draw the pixel.
             *
             * @param to The pixel is drawn at this point.
             * @see setPen()
             */
            virtual void drawPixel(const Pt::Gfx::PointF& to) = 0;

            /**
             * @brief Draws a line between the two given points, excluding the last point.
             *
             * The line is drawn from the point specified in 'from' to the point specified
             * in 'to'. The current pen color and pen size are used to draw the line.
             *
             * !Attention
             * The last point on the line is not drawn!
             *
             * @param from The line starts from this point and is drawn to 'to'.
             * @param to The line is drawn to this point (exclusively), starting from 'from'.
             * @see setPen()
             */
            virtual void drawLine(const Pt::Gfx::PointF& from, const  Pt::Gfx::PointF& to) = 0;

            /**
             * @brief Draws a Text at the specified position with an outline.
             *
             * The given Text is drawn at the given position (from) using the current font
             * and the current pen color of this painter. The specified point to which the
             * Text is drawn is the base-line of the Text/font.
             *
             * @param to Draws the Text at this position on the painter.
             * @param text The Text to be drawn.
             * @param outline The Text outline.
             * @see setPen()
             * @see setFont()
             */
            virtual void drawText( const Pt::Gfx::PointF& to, const Pt::String& Text, const Pt::Gfx::ARgbColor* outline = 0 ) = 0;

            /**
             * @brief Draws a rectangle outline.
             *
             * The rectangle is drawn with the given rectangle coordinates and sizes using
             * the current pen attributes.
             *
             * @param rect The rectangle is drawn at this rectangular location.
             * @see setPen()
             */
            virtual void drawRect(const Pt::Gfx::RectF& rect) = 0;

            /**
             * @brief Draws a filled rectangle (without an outline)
             *
             * The rectangle is drawn with the given rectangle coordinates and sizes using
             * the current brush attributes.
             *
             * @param rect The rectangle is drawn at this rectangular location.
             * @see setBrush()
             */
            virtual void fillRect(const Pt::Gfx::RectF& rect) = 0;

            /**
             * @brief Draws a circle outline with the given diameter at the specified position.
             *
             * The given point refers to the top-left "corner" of the circle. This point in
             * conjunction with the given diameter spans a bounding box into which the circle
             * is fit.
             *
             * The current pen attributes are used to draw the circle.
             *
             * This method basically calls drawEllipse() with the diameter as width and height
             * for the ellipse.
             *
             * @param topLeft The top-left "corner" of the bounding box for this circle.
             * @param diameter The diameter of the circle.
             * @see setPen()
             */
            inline void drawCircle(const Pt::Gfx::PointF& topLeft, size_t diameter)
            {
                drawEllipse(topLeft, Pt::Gfx::SizeF(diameter, diameter));
            }

            /**
             * @brief Draws a filled circle (without an outline) with the given diameter at the specified position.
             *
             * The given point refers to the top-left "corner" of the circle. This point in
             * conjunction with the given diameter spans a bounding box into which the circle
             * is fit.
             *
             * The current brush attributes are used to draw the circle.
             *
             * This method basically calls fillEllipse() with the diameter as width and height
             * for the ellipse.
             *
             * @param topLeft The top-left "corner" of the bounding box for this circle.
             * @param diameter The diameter of the circle.
             * @see setBrush()
             */
            inline void fillCircle(const Pt::Gfx::PointF& topLeft, size_t diameter)
            {
                fillEllipse(topLeft, Pt::Gfx::SizeF(diameter, diameter));
            }

            /**
             * @brief Draws an ellipse outline with the given size at the specified position.
             *
             * The given point refers to the top-left "corner" of the ellipse. This point in
             * conjunction with the given sizes spans a bounding box into which the ellipse
             * is fit.
             *
             * The current pen attributes are used to draw the ellipse.
             *
             * @param topLeft The top-left "corner" of the bounding box for this ellipse.
             * @param size The horizontal and vertical size of the ellipse.
             * @see setPen()
             */
            virtual void drawEllipse(const Pt::Gfx::PointF& topLeft, const Pt::Gfx::SizeF& size) = 0;

            /**
             * @brief Draws a filled ellipse with the given size at the specified position.
             *
             * The given point refers to the top-left "corner" of the ellipse. This point in
             * conjunction with the given sizes spans a bounding box into which the ellipse
             * is fit.
             *
             * The current brush attributes are used to draw the ellipse.
             *
             * @param topLeft The top-left "corner" of the bounding box for this ellipse.
             * @param size The horizontal and vertical size of the ellipse.
             * @see setBrush()
             */
            virtual void fillEllipse(const Pt::Gfx::PointF& topLeft, const Pt::Gfx::SizeF& size) = 0;

            /**
             * @brief Draws a polyline of multiple line segments connected by points.
             *
             * The points of the polyline are passed in the parameter 'points'. The first
             * line segment is drawn from point 0 to point 1, the second line segment is drawn
             * from point 1 to point 2. The polyline is not closed at. To make a closed shape
             * the last point in the list must be the same as the first point.
             *
             * Only the number of points as given in 'pointCount' is drawn. The number
             * must not be bigger than the number of points in the array, but may be smaller.
             *
             * The current pen attributes are used to draw the polyline.
             *
             * @param points The points of which the polyline is drawn.
             * @param pointCount Specifies the number of points of the points array that should be
             * used to draw the polyline.
             */
            virtual void drawPolyline(const Pt::Gfx::PointF* points, const size_t pointCount) = 0;

            /**
             * @brief Draws/Fills a polygon by connecting the given points to a flat shape.
             *
             * The outlining points of the polygon are passed in the parameter 'points'. The
             * polygon is closed even when the last point does not overlap with the first point.
             *
             * Only the number of points as given in 'pointCount' is used to form the polygon.
             * The numer must not be bigger than the number of points in the array, but may be
             * smaller.
             *
             * The current brush attributes are used to draw the polygon.
             *
             * @param points The points of which the polygon is drawn.
             * @param pointCount Specifies the number of points of the points array that should be
             * used to draw the polygon.
             */
            virtual void fillPolygon(const Pt::Gfx::PointF* points, const size_t pointCount) = 0;

            /**
             * @brief Draws an image at the given position.
             *
             * The given image is drawn at the given position. The image is automatically converted
             * to this painter's color space. The given coordinates may lay outside of the painter's
             * drawing area. The image will then be clipped. The given coordinates may be negative.
             * Only the part of the image which are still on the painter's area are drawn.
             *
             * The image is drawn left to right and top to bottom. The specified position is the
             * top-left corner of the image in this painter's coordinate space.
             *
             * @param to The x|y-position to where the image should be drawn on the painter's area.
             * @param image The image to be drawn.
             */
            virtual void drawImage(const Pt::Gfx::PointF& to, const Gfx::ARgbImage& image) = 0;

            /**
             * @brief Draws a rectangle segment of an image at the given position.
             *
             * An image segment in the size and at the position of the specified region (imageRegion)
             * is "cut out" of the given image (image) before it is drawn at the specified position (to).
             * The coordinates of the Region object are relative to the image's top-left corner and in the
             * image's coordinate space.
             *
             * The image is automatically converted to this painter's color space. The given coordinates
             * may lay outside of the painter's drawing area. The image will then be clipped. The given
             * coordinates may be negative. Only the part of the image which are still on the painter's
             * area are drawn.
             *
             * The image segment is drawn left to right and top to bottom. The specified position is
             * the top-left corner of the cut-out image segment in this painter's coordinate space.
             *
             * To not only draw a part of this image, but all of it, use the method
             * drawImage(const Pt::Gfx::PointF& to, const ARgbImage& image);
             *
             * @param to The x|y-position to where the image semgnet should be drawn on the painter's area.
             * @param image The image of which a segment specified by 'imageRect' should to be drawn.
             * @param imageRegion Specifies the position and size of the segment that is to be cut out
             * of the image to be drawn at the specified position.
             */
            virtual void drawImage(const Pt::Gfx::PointF& to, const Gfx::ARgbImage& image, const Pt::Gfx::Region& imageRegion) = 0;
    };

} // namespace Gfx

} // namespace Pt

#endif
