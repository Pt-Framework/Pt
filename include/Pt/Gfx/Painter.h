/***************************************************************************
 *   Copyright (C) 2006 PTV AG                                             *
 ***************************************************************************/

#ifndef PT_GFX_PAINTER_H
#define PT_GFX_PAINTER_H

#include <Pt/Api.h>
#include <Pt/Gfx/Gfx.h>
#include <Pt/Math/Size.h>

#include <cstddef>
#include <string>
#include <list>


namespace Pt {

namespace Gfx {

	class PT_API Painter
	{
		public:
			//! @brief Empty virtual destructor.
			virtual ~Painter()
			{}

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
			 * The brush attributes consist of the brush color or texture.
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
			 * All text output with this painter will be done using this font.
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
			 * To measure the size of a text, use fontMetrics(std::string).
			 *
			 * @return The font metrics of the currently selected font.
			 * @see fontMetrics(std::string)
			 */
			virtual FontMetrics fontMetrics() const = 0;

			/**
			 * @brief Returns the metrics of the given text for the currently selected font.
			 *
			 * The metrics, which are returned contain the default metrics of the font: ascent,
			 * descent and height. Additionally the width for showing the specified string object
			 * using the currently selected font is 
			 * The width-attribute of the returned FontMetrics object always is set to 0.
			 *
			 * To measure the size of a text, use fontMetrics(std::string).
			 *
			 * @return The font metrics of the currently selected font.
			 * @see fontMetrics(std::string)
			 */
			virtual FontMetrics fontMetrics(std::string text) const = 0;

			/**
			 * @brief Returns a list of installed font (family) names on the current platform and device.
			 *
			 * The returned font family names list contains all font names, that may be used to
			 * create a new Font object using the Font constructor. The returned set of font names
			 * not only depends on the installed fonts of the platform, but also on the device for
			 * which this painter is active. A printer device might provide more, less or different
			 * fonts than a display (widget) device.
			 *
			 * @param A list of installed font names on the current platform and device.
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
			virtual void drawPixel(const Math::Point& to) = 0;

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
			virtual void drawLine(const Math::Point& from, const Math::Point& to) = 0;

			/**
			 * @brief Draws a text at the specified position.
			 *
			 * The given text is drawn at the given position (from) using the current font
			 * and the current pen color of this painter.
			 *
			 * @param to Draws the text at this position on the painter.
			 * @param text The text to be drawn.
			 * @see setPen()
			 * @see setFont()
			 */
			virtual void drawText(const Math::Point& to, const std::string& text) = 0;

			/**
			 * @brief Draws a rectangle outline.
			 *
			 * The rectangle is drawn with the given rectangle coordinates and sizes using
			 * the current pen attributes.
			 *
			 * @param rect The rectangle is drawn at this rectangular location.
			 * @see setPen()
			 */
			virtual void drawRect(const Math::Rect& rect) = 0;

			/**
			 * @brief Draws a filled rectangle (without an outline)
			 *
			 * The rectangle is drawn with the given rectangle coordinates and sizes using
			 * the current brush attributes.
			 *
			 * @param rect The rectangle is drawn at this rectangular location.
			 * @see setBrush()
			 */
			virtual void fillRect(const Math::Rect& rect) = 0;

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
			inline void drawCircle(const Math::Point& topLeft, size_t diameter)
			{
				drawEllipse(topLeft, Math::Size(diameter, diameter));
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
			inline void fillCircle(const Math::Point& topLeft, size_t diameter)
			{
				fillEllipse(topLeft, Math::Size(diameter, diameter));
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
			virtual void drawEllipse(const Math::Point& topLeft, const Math::Size& size) = 0;

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
			virtual void fillEllipse(const Math::Point& topLeft, const Math::Size& size) = 0;

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
			 * @param The points of which the polyline is drawn.
			 * @param Specifies the number of points of the points array that should be
			 * used to draw the polyline.
			 */
			virtual void drawPolyline(const Math::Point* points, const size_t pointCount) = 0;

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
			 * @param The points of which the polygon is drawn.
			 * @param Specifies the number of points of the points array that should be
			 * used to draw the polygon.
			 */
			virtual void fillPolygon(const Math::Point* points, const size_t pointCount) = 0;

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
			virtual void drawImage(const Math::Point& to, const Gfx::ARgbImage& image) = 0;

			/**
			 * @brief Draws a rectangle segment of an image at the given position.
			 *
			 * An image segment in the size and at the position of the specified rectangle (imageRect)
			 * is "cut out" of the given image (image) before it is drawn at the specified position (to).
			 * The coordinates of the Rect object are relative to the image's top-left corner and in the
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
			 * drawImage(const Point& to, const ARgbImage& image);
			 *
			 * @param to The x|y-position to where the image semgnet should be drawn on the painter's area.
			 * @param image The image of which a segment specified by 'imageRect' should to be drawn.
			 * @param imageRect Specifies the position and size of the segment that is to be cut out
			 * of the image to be drawn at the specified position.
			 */
			virtual void drawImage(const Math::Point& to, const Gfx::ARgbImage& image, const Math::Rect& imageRect) = 0;
	};

} // namespace Gfx

} // namespace Pt

#endif
