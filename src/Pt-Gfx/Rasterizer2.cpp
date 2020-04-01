/* Copyright (C) 2017-2017 Aloysius Indrayanto
   Copyright (C) 2006-2015 Marc Boris Duerner
   Copyright (C) 2006-2015 Laurentiu-Gheorghe Crisan

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
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
  02110-1301 USA
*/

#include <iomanip>

#include "Rasterizer2.h"
#include "ClipShape.h"
#include <Pt/Gfx/Transform.h>
#include <Pt/Gfx/Path.h>


namespace Pt {

namespace Gfx {

/*
// Weighting filter for Xiaolin Wu's anti-aliasing algorithm
// Inspired by http://www.crbond.com/papers/anti_alias.pdf
// y = 1.0 - pow( (x / 255.0), 1.88 )
//
lprintf("    static const Pt::uint8_t aaLUT[256] = {\n        ");
for(int c = 0, i = 0; i <= 255; ++i) {
    const double n = (float) i / 255.0;
    const double q = 1.0 - pow(n, 1.88);
    const int    a = q * 255.0; ++c;
    lprintf("%3d%c ", (a > 255) ? 255 : a, (i == 255) ? ' ' : ',');
    if(c >= 20) { c = 0; lprintf("\n        "); }
}
lprintf("\n    };\n\n"); exit(0);
*/
const Pt::uint8_t Rasterizer2::XWAA_WFILTER[256] = {
    255, 254, 254, 254, 254, 254, 254, 254, 254, 254, 254, 254, 254, 254, 253, 253, 253, 253, 253, 253,
    252, 252, 252, 252, 252, 251, 251, 251, 250, 250, 250, 250, 249, 249, 249, 248, 248, 248, 247, 247,
    247, 246, 246, 246, 245, 245, 244, 244, 243, 243, 243, 242, 242, 241, 241, 240, 240, 239, 239, 238,
    238, 237, 237, 236, 236, 235, 234, 234, 233, 233, 232, 231, 231, 230, 230, 229, 228, 228, 227, 226,
    226, 225, 224, 224, 223, 222, 221, 221, 220, 219, 219, 218, 217, 216, 215, 215, 214, 213, 212, 211,
    211, 210, 209, 208, 207, 206, 206, 205, 204, 203, 202, 201, 200, 199, 198, 197, 197, 196, 195, 194,
    193, 192, 191, 190, 189, 188, 187, 186, 185, 184, 183, 182, 181, 179, 178, 177, 176, 175, 174, 173,
    172, 171, 170, 169, 167, 166, 165, 164, 163, 162, 160, 159, 158, 157, 156, 154, 153, 152, 151, 150,
    148, 147, 146, 145, 143, 142, 141, 139, 138, 137, 136, 134, 133, 132, 130, 129, 127, 126, 125, 123,
    122, 121, 119, 118, 116, 115, 114, 112, 111, 109, 108, 106, 105, 103, 102, 101,  99,  98,  96,  95,
     93,  91,  90,  88,  87,  85,  84,  82,  81,  79,  77,  76,  74,  73,  71,  69,  68,  66,  65,  63,
     61,  60,  58,  56,  55,  53,  51,  50,  48,  46,  44,  43,  41,  39,  38,  36,  34,  32,  31,  29,
     27,  25,  23,  22,  20,  18,  16,  14,  13,  11,   9,   7,   5,   3,   1,   0
};


const Rasterizer2::DrawLineMask Rasterizer2::NullLineMask = {
    maxPoint(),
    maxPoint(),
    maxPoint(),
    maxPoint()
};


Rasterizer2::Rasterizer2(Image& image)
: _image( &image )
, _compositionMode( CompositionMode::SourceCopy )
, _aaMode(true)
, _isGradient(false)
, _isTexture(false)
, _brushPixel( _image->view(), 0, 0 )
, _brushImage(0)
, _penPixel( _image->view(), 0, 0 )
, _faceId(0)
, _fontSize(10)
{
    updateClip();

    _faceId = FreeType::instance().defaultFace();

    _imageType.face_id = _faceId;
    _imageType.width   = 12;
    _imageType.height  = 12;
    _imageType.flags   =  FT_LOAD_DEFAULT;
}


Rasterizer2::~Rasterizer2()
{
}


bool Rasterizer2::isAntiAliasing() const
{
    return _aaMode;
}


void Rasterizer2::setAntiAliasing(bool on)
{
    _aaMode = on;
    updatePenPattern();
}


void Rasterizer2::setImage( Image& image )
{
    _image = &image;
    _brushBuffer.reset(_image->format(), _brushBuffer.size());
    updateClip();
}


const ImageFormat& Rasterizer2::format() const
{
    return _image->format();
}


void Rasterizer2::setPen( const Pen& pen )
{
    _pen = pen;
    _penBuffer.reset(_image->format(), Size(64, 1));
    Gfx::fill(_penBuffer.begin(), _penBuffer.end(), pen.color());

    _penPixel.reset(_penBuffer.view(), 0, 0);

    if( !pen.isSolid() )
        _polygonizer.setPattern( pen.style(), pen.capStyle(), pen.dashPattern(), pen.size() );

    updatePenPattern();
}


void Rasterizer2::updatePenPattern()
{
    // Select the pattern
    const std::vector<Pt::uint8_t>* selDashPattern;

    if(!_pen.dashPattern().empty()) {
        selDashPattern = &_pen.dashPattern();
    }
    else {
        switch(_pen.style())
        {
            default        :
            case Pen::Dot  : selDashPattern = &Polygonizer::dashPatternDot;  break;
            case Pen::Dash : selDashPattern = &Polygonizer::dashPatternDash; break;
        }
    }

    const std::vector<Pt::uint8_t>& selPattern  = *selDashPattern;
    const Pt::uint8_t               selPatCount = selPattern.size();

    // Calculate the expanded size of the pattern
    Pt::int32_t expPatCount = 0;

    for(Pt::uint8_t i = 0; i < selPatCount; ++i) {
        expPatCount += selPattern[i];
    }

    // Expand the pattern
    std::vector<bool> expPattern;
    expPattern.resize(expPatCount);

    bool        draw = true;
    Pt::int32_t k    = 0;

    for(Pt::uint8_t i = 0; i < selPatCount; ++i) {
        for(Pt::uint8_t j = 0; j < selPattern[i]; ++j) {
            expPattern[k++] = draw;
        }
        draw = !draw;
    }

    // Resize the pattern buffer
    const Pt::int32_t patternBuffer1PDynSize = expPattern.size() * PATTERN_BUFFER_1P_SCALE_FACTOR;

    _patternBuffer1PDyn.clear();
    _patternBuffer1PDynCntMax = 0;

    _patternBuffer1PDyn.resize(patternBuffer1PDynSize);
    _patternBuffer1PDynCntMax = FIXED_POINT_FROM_INT(patternBuffer1PDynSize);

    size_t gctr1P = 0;

    // Generate the pattern
    bool previous = false;
    for(unsigned p = 0; p < expPattern.size(); ++p)
    {
        // Get the pattern cell value
        const bool current = expPattern[p];

        // --- One-pixel pattern ---
        // Pattern cell change from 0 to 0
        if(!previous && !current) {
            for(Pt::uint8_t i = 1; i <= PATTERN_BUFFER_1P_SCALE_FACTOR; ++i) {
                _patternBuffer1PDyn[gctr1P++] = 0;
            }
        }
        // Pattern cell change from 1 to 1
        else if(previous && current) {
            for(Pt::uint8_t i = 1; i <= PATTERN_BUFFER_1P_SCALE_FACTOR; ++i) {
                _patternBuffer1PDyn[gctr1P++] = 255;
            }
        }
        // Pattern cell change from 0 to 1
        else if(!previous && current) {
            for(Pt::int32_t i = 1; i <= PATTERN_BUFFER_1P_SCALE_FACTOR; ++i) {
                _patternBuffer1PDyn[gctr1P++] = i * 255 / PATTERN_BUFFER_1P_SCALE_FACTOR;
            }
        }
        // Pattern cell change from 1 to 0
        else if(previous && !current) {
            for(Pt::int32_t i = 1; i <= PATTERN_BUFFER_1P_SCALE_FACTOR; ++i) {
                _patternBuffer1PDyn[gctr1P++] = 255 - i * 255 / PATTERN_BUFFER_1P_SCALE_FACTOR;
            }
        }
        // Copy the pattern cell value
        previous = current;
    }

    // Transfom the pattern - without anti-aliasing
    if( ! _aaMode )
    {
        for(size_t i = 0; i < gctr1P; ++i) {
            if(_patternBuffer1PDyn[i] > 127) _patternBuffer1PDyn[i] = 255;
            else                             _patternBuffer1PDyn[i] = 0;
        }
    }
    // Transfom the pattern - with anti-aliasing
    else
    {
        for(size_t i = 0; i < gctr1P; ++i) {
            _patternBuffer1PDyn[i] = XWAA_WFILTER[ 255 - _patternBuffer1PDyn[i] ];
        }
    }

    /*
    if(_pen.style() == Pen::Dot) {
        std::cerr << "### " << (int) expPattern.size() << " -> " << (int) gctr1P << std::endl;
        for(size_t i = 0; i < gctr1P; ++i) {
            std::cerr << (int) _patternBuffer1PDyn[i] << " ";
        }
        std::cerr << std::endl;
    }
    //*/

}


Pt::uint8_t Rasterizer2::patternBuffer1PAlpha(Pt::int32_t idx) const
{
    return _patternBuffer1PDyn[ idx % FIXED_POINT_TO_INT(_patternBuffer1PDynCntMax) ];
}


Pt::uint8_t Rasterizer2::patternBuffer1PAlphaPolar(Pt::int32_t x, Pt::int32_t y, float scale) const
{
    return patternBuffer1PAlpha( toPolar(x, y) * scale);
}


Pt::uint8_t Rasterizer2::patternBuffer1PAlphaPolar(Pt::int32_t x, Pt::int32_t y, float scale, float xyRat) const
{
    const float angle = toPolar(x, y);

    if(xyRat >= 1.0 && angle >= 45) scale /= xyRat;
    if(xyRat <  1.0 && angle <  45) scale *= xyRat;

    return patternBuffer1PAlpha(angle * scale);
}


void Rasterizer2::patternBuffer1PAlpha(Pt::uint8_t& a0, Pt::uint8_t& a1, Pt::int32_t idx, Pt::uint8_t alpha0, Pt::uint8_t alpha1) const
{
    a0 = (Pt::uint32_t) _patternBuffer1PDyn[ idx % FIXED_POINT_TO_INT(_patternBuffer1PDynCntMax) ] * alpha0 / 255;
    a1 = (Pt::uint32_t) _patternBuffer1PDyn[ idx % FIXED_POINT_TO_INT(_patternBuffer1PDynCntMax) ] * alpha1 / 255;
}


void Rasterizer2::patternBuffer1PAlphaPolar(Pt::uint8_t& a0, Pt::uint8_t& a1, Pt::int32_t x, Pt::int32_t y, float scale, Pt::uint8_t alpha0, Pt::uint8_t alpha1) const
{
    patternBuffer1PAlpha(a0, a1, toPolar(x, y) * scale, alpha0, alpha1);
}


void Rasterizer2::patternBuffer1PAlphaPolar(Pt::uint8_t& a0, Pt::uint8_t& a1, Pt::int32_t x, Pt::int32_t y, float scale, float xyRat, Pt::uint8_t alpha0, Pt::uint8_t alpha1) const
{
    const float angle = toPolar(x, y);

    if(xyRat >= 1.0 && angle >= 45) scale /= xyRat;
    if(xyRat <  1.0 && angle <  45) scale *= xyRat;

    patternBuffer1PAlpha(a0, a1, angle * scale, alpha0, alpha1);
}


void Rasterizer2::setBrush( const Brush& brush )
{
    _brush      = brush;
    _isGradient = false;
    _isTexture  = false;

    switch( brush.fillStyle() )
    {
        case Brush::Solid:
            _brushBuffer.reset( _image->format(), Size(64, 1) );
            Gfx::fill(_brushBuffer.begin(), _brushBuffer.end(), brush.color());
            _brushImage = &_brushBuffer;
            break;

        case Brush::Texture:
            if( brush.texture().format() != _image->format() )
            {
                _brushBuffer.reset( _image->format(), brush.texture().size() );
                Gfx::copy( brush.texture().begin(), brush.texture().end(), _brushBuffer.begin() );
                _brushImage = &_brushBuffer;
            }
            else
            {
                _brushImage = &_brush.texture();
            }

            _isTexture = true;
            break;

        case Brush::Gradient:
            _isGradient = true;
            _brushImage = &_brushBuffer;

            _isTexture  = brush.gradient() == Brush::Linear ||
                          brush.gradient() == Brush::Radial;
            break;
    }

    _brushPixel.reset(_brushImage->view(), 0, 0);
}


void Rasterizer2::updateGradientBrush(Pt::int32_t width, Pt::int32_t height)
{
    // Adjust the width and height as needed (anti-aliasing can create additional pixels on the edges)
    if(_aaMode) {
        switch( _brush.gradient() )
        {
            case Pt::Gfx::Brush::Horizontal:
                width  += 2;
                break;

            case Pt::Gfx::Brush::Vertical:
                height += 2;
                break;

            case Pt::Gfx::Brush::Linear:
            case Pt::Gfx::Brush::Radial:
                width  += 2;
                height += 2;
                break;

            default:
                return;
        }
    }

    // Resize the brush buffer
    switch( _brush.gradient() )
    {
        case Pt::Gfx::Brush::Horizontal:
            height = 1;
            break;

        case Pt::Gfx::Brush::Vertical:
            width = 1;
            break;

        default:
            break;
    }

    _brushBuffer.reset(_image->format(), Size(width, height));

    // Create two-dimensional gradient
    switch( _brush.gradient() )
    {
        case Pt::Gfx::Brush::Horizontal:
        case Pt::Gfx::Brush::Vertical:
            updateGradientBrush_gen1DHorVerGradient(width, height);
            break;

        case Pt::Gfx::Brush::Linear:
            updateGradientBrush_gen2DLinearGradient(width, height);
            break;

        case Pt::Gfx::Brush::Radial:
            updateGradientBrush_gen2DRadialGradient(width, height);
            break;

        default:
            return;
    }
}


void Rasterizer2::updateGradientBrush_gen1DHorVerGradient(Pt::int32_t width, Pt::int32_t height)
{
    // Get and check the color stops
    // TODO: Shall we implicitly clear the image if there is no color stop?
    const ColorStops& colStops = _brush.gradientStops();
    if( colStops.empty() )
        return;


    // Prepare the interpolation buffer
    Color colRes;

    // Walk through the pixels and generate the gradient
    const float       length = width + height - 1 - 1;
          Pt::int32_t pixelPos = 0;

    for(Image::PixelIterator pit = _brushBuffer.begin(); pit != _brushBuffer.end(); ++pit) {
        // Calculate the interpolation factor
        const float ratio = (float) pixelPos / length;
        ++pixelPos;
        // Interpolate the color
        colStops.calculateInterpolatedColor(colRes, ratio);
        // Put the pixel
        pit->assign(colRes, CompositionMode::SourceCopy);
    }

    /*
    // Get and check the color stops
    // TODO: Shall we implicitly clear the image if there is no color stop?
    const ColorStops& colStops = _brush.gradientStops();
    if( colStops.size() < 2 )
        return;

    // Get the first and second colors
    const Color& color1 = colStops[0].color();
    const Color& color2 = colStops[1].color();

    const Pt::int32_t rs = color1.red  ();
    const Pt::int32_t gs = color1.green();
    const Pt::int32_t bs = color1.blue ();
    const Pt::int32_t as = color1.alpha();

    const Pt::int32_t rd = color2.red  () - rs;
    const Pt::int32_t gd = color2.green() - gs;
    const Pt::int32_t bd = color2.blue () - bs;
    const Pt::int32_t ad = color2.alpha() - as;

    // Walk through the pixels and generate the gradient
    const float       length = width + height - 1 - 1;
          Pt::int32_t pixelPos = 0;

    for(Image::PixelIterator pit = _brushBuffer.begin(); pit != _brushBuffer.end(); ++pit) {
        // Calculate the interpolation factor
        const float fb = (float) pixelPos / length;
        ++pixelPos;
        // Interpolate the color
        const Color colRes(as + ad * fb, rs + rd * fb, gs + gd * fb, bs + bd * fb);
        // Put the pixel
        pit->assign(colRes, CompositionMode::SourceCopy);
    }
    */
}


// Based on: Smooth HTML5 Canvas Gradients with Floyd-Steinberg Dithering
//           http://rectangleworld.com/blog/archives/713
//           http://rectangleworld.com/demos/DitheredGradient/DitheredGradientExample.html
//           Original code by Rectangle World, 2012
void Rasterizer2::updateGradientBrush_gen2DLinearGradient(Pt::int32_t width, Pt::int32_t height)
{
    // Get and check the color stops
    // TODO: Shall we implicitly clear the image if there is no color stop?
    const ColorStops& colStops = _brush.gradientStops();
    if( colStops.empty() )
        return;

    // Get the start and end parameters
    PointF begPos, endPos;

    if(_brush.positionMode() == Brush::Absolute)
    {
        begPos = _brush.gradientBegin();
        endPos = _brush.gradientEnd();
    }
    else // Brush::Relative
    {
        begPos.set( _brush.gradientBegin().x() * width, _brush.gradientBegin().y() * height );
        endPos.set( _brush.gradientEnd  ().x() * width, _brush.gradientEnd  ().y() * height );
    }

    // Prepare some constants
    const float xDiff = endPos.x() - begPos.x();
    const float yDiff = endPos.y() - begPos.y();
    const float sDiff = 1.0f / (xDiff * xDiff + yDiff * yDiff);

    // TODO: Shall we implicitly clear the image if we have got invalid parameter(s)?
    if( xDiff == 0.0f && yDiff == 0.0f )
        return;

    // Prepare the interpolation buffer
    Color colRes;

    // Walk through the pixels and generate the gradient
    for(Image::PixelIterator pit = _brushBuffer.begin(); pit != _brushBuffer.end(); ++pit) {
        // Calculate the coordinates and their deltas
        const Pt::int32_t x  = pit->x();
        const Pt::int32_t y  = pit->y();
        const float       dx = x - begPos.x();
        const float       dy = y - begPos.y();
        // Calculate the interpolation factor
        float ratio = (xDiff * dx + yDiff * dy) * sDiff;
             if(ratio < 0.0f) ratio = 0.0f;
        else if(ratio > 1.0f) ratio = 1.0f;
        // Interpolate the color
        colStops.calculateInterpolatedColor(colRes, ratio);
        // Put the pixel
        pit->assign(colRes, CompositionMode::SourceCopy);
    }
}


// Based on: Smooth HTML5 Canvas Radial Gradients with Dithering
//           http://rectangleworld.com/blog/archives/833
//           http://rectangleworld.com/demos/DitheredRadial/DitheredRadialExample.html
//           Original code by Rectangle World, 2013
void Rasterizer2::updateGradientBrush_gen2DRadialGradient(Pt::int32_t width, Pt::int32_t height)
{
    // Get and check the color stops
    // TODO: Shall we implicitly clear the image if there is no color stop?
    const ColorStops& colStops = _brush.gradientStops();
    if(colStops.empty()) return;

    // Get the start and end parameters
    PointF begPos, endPos;
    float  begRad, endRad;

    if(_brush.positionMode() == Brush::Absolute) {
        begPos = _brush.gradientBegin();
        endPos = _brush.gradientEnd  ();

        begRad = _brush.gradientBeginRadius();
        endRad = _brush.gradientEndRadius  ();
    }
    else { // Brush::Relative
        begPos.set( _brush.gradientBegin().x() * width, _brush.gradientBegin().y() * height );
        endPos.set( _brush.gradientEnd  ().x() * width, _brush.gradientEnd  ().y() * height );

        begRad =    _brush.gradientBeginRadius() * sqrtf(width * width + height * height);
        endRad =    _brush.gradientEndRadius  () * sqrtf(width * width + height * height);
    }

    // Prepare some constants
    const float xDiff = endPos.x() - begPos.x();
    const float yDiff = endPos.y() - begPos.y();
    const float rDiff = endRad     - begRad;

    const float a       = rDiff * rDiff - xDiff * xDiff - yDiff * yDiff;
    const float r2a     = 1.0f / (2.0f * a);
    const float rBegDif = 2.0f * begRad * rDiff;
    const float rBegSqr = begRad * begRad;

    // TODO: Shall we implicitly clear the image if we have got invalid parameter(s)?
    if(a == 0.0f)
        return;

    // Prepare the interpolation buffer
    Color colRes;

    // Walk through the pixels and generate the gradient
    for(Image::PixelIterator pit = _brushBuffer.begin(); pit != _brushBuffer.end(); ++pit) {
        // Calculate the coordinates and their deltas
        const Pt::int32_t x  = pit->x();
        const Pt::int32_t y  = pit->y();
        const float       dx = x - begPos.x();
        const float       dy = y - begPos.y();
        // Claculate the discriminant
        const float       b    = rBegDif + 2.0f * (dx * xDiff + dy * yDiff);
        const float       c    = rBegSqr - dx * dx - dy * dy;
        const float       dscm = b * b - 4.0f * a * c;
        // Calculate the interpolation factor and then interpolate the color as needed
        if(dscm >= 0.0f) {
            // Calculate the interpolation factor
            float ratio = (-b + sqrtf(dscm)) * r2a;
                 if(ratio < 0.0f) ratio = 0.0f;
            else if(ratio > 1.0f) ratio = 1.0f;
            // Interpolate the color
            colStops.calculateInterpolatedColor(colRes, ratio);
        }
        // Put the pixel
        pit->assign(colRes, CompositionMode::SourceCopy);
    }
}


void Rasterizer2::setFont(const Font& font)
{
    _font = font;

    // findFaceId returns default font for emtpy font names
    _faceId = FreeType::instance().findFaceId(font);

    _fontSize = font.size();

    // setup the image type
    _imageType.face_id = _faceId;
    _imageType.width   = font.size();
    _imageType.height  = font.size();
    _imageType.flags   = FT_LOAD_DEFAULT;
}


void Rasterizer2::setClip(const Rect& clip)
{
    // TODO: RECT-NULL empty rect should clip everything
    if( clip.isNull() )
        _clip = Rect( Point(0, 0), Size(1, 1) );
    else
        _clip = clip;

    updateClip();

    //TODO: clipping routines still assume offset by one
    _currentClip = Rect(_currentClip.topLeft(), Size(_currentClip.width() - 1, _currentClip.height() - 1));
}


void Rasterizer2::resetClip()
{
    // TODO: RECT-NULL
    _clip = Rect();

    updateClip();

    //TODO: clipping routines still assume offset by one
    _currentClip = Rect(_currentClip.topLeft(), Size(_currentClip.width() - 1, _currentClip.height() - 1));
}


void Rasterizer2::updateClip()
{
    Rect imageRect( _image->size() );

    _currentClip = _clip.isNull() ? imageRect
                                  : _clip.intersect( imageRect );
}


void Rasterizer2::drawImage(const Point& to, const Image& img)
{
    const Rect imageRect( Point(0,0), img.size() );
    drawImage( to, img, imageRect );
}


void Rasterizer2::drawImage(const Point& to, const Image& from, const Rect& fromRect)
{
    // Clip fromRect to fit into the clip/image rect
    const Point d       = _currentClip.topLeft() - to;
    const Point fromPos = fromRect.topLeft() + d;

    Rect fromClip(fromPos, Gfx::Size(_currentClip.size().width() + 1, _currentClip.height() + 1));

    fromClip = fromRect.intersect(fromClip);

    if( fromClip.isNull() )
        return;

    // Take account for smaller fromRect
    const Point toClip = to + (fromClip.topLeft() - fromRect.topLeft());

    _image->format().copy(_image->view(), toClip, from.view(), fromClip, _compositionMode);
}


void Rasterizer2::drawText(const Point& to, const Pt::String& text,
                           const Transform& transform)
{
    Rect clip(_currentClip.topLeft(), Size(_currentClip.width() + 1, _currentClip.height() + 1));

    FreeType::instance().draw(*_image, _pen.color(), to, text,
                                clip, _compositionMode,
                               transform, _faceId, _fontSize);
}


FontMetrics Rasterizer2::fontMetrics(const String& text) const
{
    return FreeType::instance().fontMetrics(text, _faceId, _fontSize);
}


FontMetrics Rasterizer2::fontMetrics(const Font& font, const Pt::String& text)
{
    FTC_FaceID faceId = FreeType::instance().findFaceId(font);

    return FreeType::instance().fontMetrics(text, faceId, font.size());
}


void Rasterizer2::drawLine(const PointF& from, const PointF& to)
{
    PointF points[2];

    points[0].set(from.x() - 0.5, from.y() - 0.5);
    points[1].set(to.x() - 0.5, to.y() - 0.5);

    if(_pen.size() == 1)
    {
        Point a( lround(points[0].x() ), lround(points[0].y() ) );
        Point b( lround(points[1].x()), lround(points[1].y()) );

        drawNarrowLine(a, b, 0);
        return;
    }

    std::vector<Polygon> polygons;
    _polygonizer.renderWidePolyline(polygons, points, 2, _pen, true, false);

    // no performance benefit to use renderWideLine
    //_polygonizer.renderWideLine( polygons, from, to, _rasterizer->pen() );

    for(std::size_t n = 0; n < polygons.size(); ++n)
    {
        const std::vector<PointF>& points = polygons[n].points();
        rasterWideLine( &points[0], points.size() );
    }
}


void Rasterizer2::drawNarrowLine(const Point& a, const Point& b, DrawLineMask* maskInOut)
{
    // Clip the points
    Pt::int32_t x1 = a.x();
    Pt::int32_t y1 = a.y();
    Pt::int32_t x2 = b.x();
    Pt::int32_t y2 = b.y();

    if( ! ClipShapeI::clipLine(x1, y1, x2, y2, _currentClip) )
        return;

    // Find the minimum and maximum coordinates
    Pt::int32_t minX, minY, maxX, maxY;

    if(x2 > x1) {
        minX = x1;
        maxX = x2;
    }
    else {
        minX = x2;
        maxX = x1;
    }

    if(y2 > y1) {
        minY = y1;
        maxY = y2;
    }
    else {
        minY = y2;
        maxY = y1;
    }

    // Check the size of the line
    const Pt::int32_t sizeX = maxX - minX + 1;
    const Pt::int32_t sizeY = maxY - minY + 1;

    if( ! sizeX && ! sizeY )
        return;

    // Draw the line
    if(_pen.isSolid())
    {
        rasterNarrowSolidLine(x1, y1, x2, y2, _pen.color(), maskInOut);
    }
    else
    {
        Pt::int32_t fpiCtrInOut = PATTERN_BUFFER_1P_COUNTER_START;
        rasterNarrowPatternedLine(x1, y1, x2, y2, _pen.color(), fpiCtrInOut, maskInOut);
    }
}


void Rasterizer2::drawPolyline(const PointF* ps, size_t n)
{
    std::vector<PointF> polygon;
    polygon.reserve(n);

    for (size_t i = 0; i < n; ++i)
    {
        // Floor the coordinates with an epsilon of 0.001
        PointF p( Pt::lround(ps[i].x() - 0.4999),
                  Pt::lround(ps[i].y() - 0.4999) );

        if( ! polygon.empty() && polygon.back() == p )
          continue;

        polygon.push_back(p);
    }

#if 0
    if(IP2_DEBUG::DUMP_POLYGON_COORDINATES)
    {
        const std::ios_base::fmtflags f(std::cerr.flags());

        std::cerr << (this->isAntiAliasing() ? "WAA: " : "NAA: ") << "Rasterizer2::drawPolyline ### AT ENTRY POINT ###" << std::endl;
        for (size_t i = 0; i < n; ++i) {
            std::cerr << std::fixed << std::setw(5) << std::setprecision(1)
                      << ps[i].x() << ", " << ps[i].y() << std::endl;
        }

        std::cerr << (this->isAntiAliasing() ? "WAA: " : "NAA: ") << "Rasterizer2::drawPolyline ### AFTER FIXED ADJUST ###" << std::endl;
        for (size_t i = 0; i < polygon.size(); ++i) {
            std::cerr << std::fixed << std::setw(5) << std::setprecision(1)
                      << polygon[i].x() << ", " << polygon[i].y() << std::endl;
        }

        std::cerr << std::endl;
        std::cerr.flags(f);
    }
#endif

    if(_pen.size() == 1)
       drawNarrowPolyline( &polygon[0], polygon.size() );
    else
       drawWidePolyline( &polygon[0], polygon.size() );
}


void Rasterizer2::drawNarrowPolyline(const PointF* points, size_t pointCount)
{
    if(pointCount < 2)
        return;

    DrawLineMask mask_nnp1;
    memcpy(mask_nnp1, Rasterizer2::NullLineMask, sizeof(DrawLineMask));

    const bool solid = _pen.isSolid();

    Pt::int32_t fpiCtrInOut = PATTERN_BUFFER_1P_COUNTER_START;

    // From point N to point (N + 1), successively
    std::size_t pc1 = pointCount - 1;

#if 0
    if(IP2_DEBUG::DUMP_POLYGON_COORDINATES && !this->isAntiAliasing()) {
        std::cerr << "### 3 ###\n";
        for (size_t i = 0; i < pointCount; ++i) {
            std::cerr << std::fixed << std::setw(5) << std::setprecision(1)
                      << points[i].x() << ", " << points[i].y() << std::endl;
        }
    }
#endif

    for(std::size_t i = 0; i < pc1; ++i)
    {
        Pt::int32_t x1 = points[i].x();
        Pt::int32_t y1 = points[i].y();
        Pt::int32_t x2 = points[i + 1].x();
        Pt::int32_t y2 = points[i + 1].y();

        if (!BasicClipShape<Pt::int32_t>::clipLine(x1, y1, x2, y2, _currentClip))
            continue;

        if(solid)
            rasterNarrowSolidLine(x1, y1, x2, y2, _pen.color(), &mask_nnp1);
        else
            rasterNarrowPatternedLine(x1, y1, x2, y2,_pen.color(), fpiCtrInOut, &mask_nnp1);
    }
}


void Rasterizer2::drawWidePolyline(const PointF* points, const size_t pointCount)
{
    std::vector<Polygon> polygons;
    _polygonizer.renderWidePolyline(polygons, points, pointCount, _pen, true, false);

    const bool isSolid  = _pen.isSolid();
  //const bool isClosed = points[0] == points[pointCount - 1];

    if( isSolid /*&& isClosed*/ )
    {
        rasterWidePolyline(polygons);
    }
    else
    {
        for(std::size_t n = 0; n < polygons.size(); ++n)
        {
            const std::vector<PointF>& polygon = polygons[n].points();
            rasterWideLine( &polygon[0], polygon.size() );
        }
    }
}


void Rasterizer2::drawRect(const RectF& rect)
{
    // REVIEW: clipping doesn't work
    // INFO: This should work, but how if changing the implementation so:
    //       1. We do not perform clipping here
    //       2. Modify rasterNarrowRect() and replace these:
    //             // Get the minimum and maximum coordinates
    //             Pt::int32_t minX = tl.x();
    //             Pt::int32_t minY = tl.y();
    //             Pt::int32_t maxX = br.x();
    //             Pt::int32_t maxY = br.y();
    //             // Clip the coordinates
    //             if(minX < _currentClip.left  ()) minX = _currentClip.left  ();
    //             if(minY < _currentClip.top   ()) minY = _currentClip.top   ();
    //             if(maxX > _currentClip.right ()) maxX = _currentClip.right ();
    //             if(maxY > _currentClip.bottom()) maxY = _currentClip.bottom();
    //         with something similar which is used in rasterRectArea():
    //              Rect rect(tl, br);
    //              rect = rect.intersect(_currentClip);
    //         or, there could be a problem with Pt::lround(), or even maybe with what coordinates actually
    //         stored in tl and br below (I am thinking that, it could be possible that the coordinates
    //         stored/given to tl and br are somehow swapped by other code that calling this function?)
    //
   /* if(_pen.size() == 1)
    {
        const Point tl( Pt::lround(rect.topLeft().x()),
                        Pt::lround(rect.topLeft().y()) );
        const Point br( Pt::lround(rect.bottomRight().x()),
                        Pt::lround(rect.bottomRight().y()) );

        rasterNarrowRect(tl, br);
        return;
    }*/

    const PointF pointsF[5] = {
        rect.bottomLeft(),
        rect.bottomRight(),
        rect.topRight(),
        rect.topLeft(),
        rect.bottomLeft()
    };

    drawPolyline(pointsF, 5);
}


void Rasterizer2::drawRoundedRect(const RectF& rect, float radius)
{
    if(_pen.size() == 1)
    {
        rasterNarrowRoundedRect(rect, radius);
        return;
    }

    // use a new pen with bevel join
    Pen newPen = _pen;
    newPen.setJoinStyle(Pen::BevelJoin);

    std::vector<Polygon> polygons;
    _polygonizer.renderRoundedRect(polygons, rect, radius, newPen);

    rasterWidePolyline(polygons);
}


void Rasterizer2::drawEllipse(const PointF& topLeft, const SizeF& size)
{
    //TODO: Bug drawing ellipses with even sizes!

    if(_pen.size() == 1)
    {
        const Point tl( Pt::lround(topLeft.x() - 0.5),
                        Pt::lround(topLeft.y() - 0.5) );

        const Size sz(lround(size.width() - 0.5), lround(size.height() - 0.5));

        rasterNarrowArc(tl, sz, 0, 0, ArcMode::Open);
        return;
    }

    // use a new pen with bevel join
    Pen newPen = _pen;
    newPen.setJoinStyle(Pen::BevelJoin);

    std::vector<Polygon> polygons;
    _polygonizer.renderEllipse(polygons, topLeft, size, newPen);

    if( _pen.isSolid() )
    {
        rasterWidePolyline(polygons);
    }
    else
    {
        for(std::size_t n = 0; n < polygons.size(); ++n)
        {
            const std::vector<PointF>& polygon = polygons[n].points();
            rasterWideLine( &polygon[0], polygon.size() );
        }
    }
}


void Rasterizer2::drawArc(const PointF& topLeft, const SizeF& size,
                          float degBegin, float degEnd, const ArcMode& arcMode)
{
    if(_pen.size() == 1)
    {
        const Point tl( Pt::lround(topLeft.x()),
                        Pt::lround(topLeft.y ()) );
        const Size  sz( Pt::lround(size.width()),
                        Pt::lround(size.height()) );

        rasterNarrowArc(tl, sz, degBegin, degEnd, arcMode);
        return;
    }

    // use a new pen with bevel join
    const Pen orgPen = _pen;
    Pen newPen = orgPen;
    newPen.setJoinStyle(Pen::BevelJoin);

    std::vector<Polygon> polygons;
    _polygonizer.renderArc(polygons, arcMode, topLeft, size, degBegin, degEnd, newPen);

    const bool isClosed = arcMode != ArcMode::Open;

    if( _pen.isSolid() && isClosed )
    {
        rasterWidePolyline(polygons);
    }
    else
    {
        for(std::size_t n = 0; n < polygons.size(); ++n)
        {
            const std::vector<PointF>& polygon = polygons[n].points();
            rasterWideLine( &polygon[0], polygon.size() );
        }
    }
}


void Rasterizer2::drawPath(const Path& path, float smoothness)
{
    std::vector<Polygon> polygons;
    path.toPolygons(polygons, smoothness);

    for(std::size_t n = 0; n < polygons.size(); ++n)
    {
        const std::vector<PointF>& pointsF = polygons[n].points();

        if(_pen.size() == 1)
        {
            drawNarrowPath( &pointsF[0], pointsF.size() );
        }
        else
        {
            drawWidePolyline( &pointsF[0], pointsF.size() );
        }
    }
}


void Rasterizer2::drawNarrowPath(const PointF* pointsF, size_t pointCount)
{
    if(pointCount < 2)
        return;

    DrawLineMask mask_nnp1;
    memcpy(mask_nnp1, Rasterizer2::NullLineMask, sizeof(DrawLineMask));

    const bool solid = _pen.isSolid();

    Pt::int32_t fpiCtrInOut = PATTERN_BUFFER_1P_COUNTER_START;

    // From point N to point (N + 1), successively
    std::size_t pc1 = pointCount - 1;

    for(std::size_t i = 0; i < pc1; ++i)
    {
        if(solid)
            rasterNarrowSolidLine_F( pointsF[i].x(), pointsF[i].y(),
                                     pointsF[i + 1].x(), pointsF[i + 1].y(),
                                     _pen.color(), &mask_nnp1
                                   );
        else
            rasterNarrowPatternedLine_F( pointsF[i].x(), pointsF[i].y(),
                                         pointsF[i + 1].x(), pointsF[i + 1].y(),
                                         _pen.color(), fpiCtrInOut, &mask_nnp1
                                       );
    }
}


void Rasterizer2::fillPolygon(const PointF* ps, std::size_t n)
{
#if 0
    if(IP2_DEBUG::DUMP_POLYGON_COORDINATES) {
        const std::ios_base::fmtflags f(std::cerr.flags());
        std::cerr << (this->isAntiAliasing() ? "WAA: " : "NAA: ") << "Rasterizer2::fillPolygon ### AT ENTRY POINT ###" << std::endl;
        for (size_t i = 0; i < n; ++i) {
            std::cerr << std::fixed << std::setw(5) << std::setprecision(1)
                      << ps[i].x() << ", " << ps[i].y() << std::endl;
        }
        std::cerr.flags(f);
    }
#endif

    // Perform coordinate adjustments
    std::vector<PointF> polygon(n);
    size_t              pointCount = 0;


    for (size_t i = 0; i < n; ++i)
    {
        // Foor the coordinates while avoiding rounding errors
        const double x = Pt::lround(ps[i].x() - 0.4999);
        const double y = Pt::lround(ps[i].y() - 0.4999);
        //const double x = ps[i].x();
        //const double y = ps[i].y();
        if(pointCount && polygon[pointCount - 1].x() == x && polygon[pointCount - 1].y() == y) continue;
        polygon[pointCount++].set(x, y);
    }
    polygon.resize(pointCount);

#if 0
    if(IP2_DEBUG::DUMP_POLYGON_COORDINATES) {
        const std::ios_base::fmtflags f(std::cerr.flags());
        std::cerr << (this->isAntiAliasing() ? "WAA: " : "NAA: ") << "Rasterizer2::fillPolygon ### AFTER FIXED ADJUST ###" << std::endl;
        for (size_t i = 0; i < polygon.size(); ++i) {
            std::cerr << std::fixed << std::setw(5) << std::setprecision(1)
                      << polygon[i].x() << ", " << polygon[i].y() << std::endl;
        }
        std::cerr << std::endl;
        std::cerr.flags(f);
    }
#endif

    // Clip the polygon
    BasicClipShape<PointF::ValueT>::clipPolygon(polygon, _currentClip);

    // Find the minimum and maximum coordinates
    Pt::int32_t minX =  MAXIMUM_COORD;
    Pt::int32_t minY =  MAXIMUM_COORD;
    Pt::int32_t maxX = -MAXIMUM_COORD;
    Pt::int32_t maxY = -MAXIMUM_COORD;
    for(size_t j = 0; j < polygon.size(); ++j)
    {
        const PointF::ValueT x = polygon[j].x();
        const PointF::ValueT y = polygon[j].y();
        if(x < minX) minX = x;
        if(y < minY) minY = y;
        if(x > maxX) maxX = x;
        if(y > maxY) maxY = y;
    }

    if(_isGradient)
        updateGradientBrush(maxX - minX + 1, maxY - minY + 1);

    // #@#
    if( this->isAntiAliasing() )
    {
        rasterPolygonXWAA(&polygon[0], polygon.size(),
                          _brush.color(), minX, minY, maxX, maxY);
    }
    else
    {
        rasterPolygonNoAA(&polygon[0], polygon.size(),
                          _brush.color(), minX, minY, maxX, maxY);
    }
}


void Rasterizer2::fillPolygon_NR(const PointF* ps, std::size_t n)
{
    // Clip the polygon
    std::vector<PointF> polygon(n);

    for (size_t i = 0; i < n; ++i)
    {
        const PointF::ValueT x = ps[i].x();
        const PointF::ValueT y = ps[i].y();
        polygon[i].set(x, y);
    }

    BasicClipShape<PointF::ValueT>::clipPolygon(polygon, _currentClip);

    // Find the minimum and maximum coordinates
    Pt::int32_t minX =  MAXIMUM_COORD;
    Pt::int32_t minY =  MAXIMUM_COORD;
    Pt::int32_t maxX = -MAXIMUM_COORD;
    Pt::int32_t maxY = -MAXIMUM_COORD;
    for(size_t j = 0; j < polygon.size(); ++j)
    {
        const PointF::ValueT x1 = floor( polygon[j].x() );
        const PointF::ValueT x2 = ceil ( polygon[j].x() );
        if(x1 < minX) minX = x1;
        if(x2 < minX) minX = x2;
        if(x1 > maxX) maxX = x1;
        if(x2 > maxX) maxX = x2;

        const PointF::ValueT y1 = floor( polygon[j].y() );
        const PointF::ValueT y2 = ceil ( polygon[j].y() );
        if(y1 < minY) minY = y1;
        if(y2 < minY) minY = y2;
        if(y1 > maxY) maxY = y1;
        if(y2 > maxY) maxY = y2;
    }

    if(_isGradient)
        updateGradientBrush(maxX - minX + 1, maxY - minY + 1);

    if( this->isAntiAliasing() )
    {
        rasterPolygonXWAA(&polygon[0], polygon.size(),
                          _brush.color(), minX, minY, maxX, maxY);
    }
    else
    {
        rasterPolygonNoAA(&polygon[0], polygon.size(),
                          _brush.color(), minX, minY, maxX, maxY);
    }
}


void Rasterizer2::fillPolygons(const std::vector<Polygon>& polygons)
{
    Pt::int32_t minX =  MAXIMUM_COORD;
    Pt::int32_t minY =  MAXIMUM_COORD;
    Pt::int32_t maxX = -MAXIMUM_COORD;
    Pt::int32_t maxY = -MAXIMUM_COORD;

    std::vector<Polygon> clippedPolygons = polygons;

    for(size_t i = 0; i < clippedPolygons.size(); ++i)
    {
        Polygon& polygon = clippedPolygons[i];

        BasicClipShape<PointF::ValueT>::clipPolygon(polygon.points(), _currentClip);

        for(size_t j = 0; j < polygon.size(); ++j)
        {
            const PointF::ValueT x = polygon.at(j).x();
            const PointF::ValueT y = polygon.at(j).y();

            if(x < minX) minX = x;
            if(y < minY) minY = y;
            if(x > maxX) maxX = x;
            if(y > maxY) maxY = y;
        }
    }

    if(_isGradient)
        updateGradientBrush(maxX - minX + 1, maxY - minY + 1);


    if( this->isAntiAliasing() )
    {
        rasterPolygonsXWAA(clippedPolygons, _brush.color(), minX, minY, maxX, maxY);
    }
    else
    {
        rasterPolygonsNoAA(clippedPolygons, _brush.color(), minX, minY, maxX, maxY);
    }
}


void Rasterizer2::fillRect(const RectF& rect)
{
    const PointF& topLeft = rect.topLeft();
    const PointF& bottomRight = rect.bottomRight();

    const Point tl(round(topLeft));
    const Point br(round(bottomRight));

    // Update the gradient as needed
    if(_isGradient)
        updateGradientBrush(br.x() - tl.x(), br.y() - tl.y() );

    // Draw the rectangle
    rasterRectArea(tl, br);
}


void Rasterizer2::fillRoundedRect(const RectF& rect, float radius)
{
    std::vector<PointF> pointsF;
    _polygonizer.fillRoundedRect(pointsF, rect, radius);

    fillPolygon( &pointsF[0], pointsF.size() );
}


void Rasterizer2::fillEllipse(const PointF& topLeft, const SizeF& size)
{
    const Point tl(Pt::lround(topLeft.x() - 0.5),
        Pt::lround(topLeft.y() - 0.5));

    const Size sz(lround(size.width() - 0.5), lround(size.height() - 0.5));

    fillEllipse(tl, sz);
}


void Rasterizer2::fillPie(const PointF& topLeft, const SizeF& size,
                          float degBegin, float degEnd)
{
    Point tl(round(topLeft));

    Size  sz(round(size));

    rasterArcArea(tl, sz, degBegin, degEnd, ArcMode::Pie);
}


void Rasterizer2::fillChord( const PointF& topLeft, const SizeF& size,
                             float degBegin, float degEnd)
{
    const Point tl(round(topLeft));

    const Size  sz(round(size));

    rasterArcArea(tl, sz, degBegin, degEnd, ArcMode::Chord);
}


void Rasterizer2::fillPath(const Path& path, float smoothness)
{
    std::vector<Polygon> polygons;
    path.toPolygons(polygons, smoothness);

    fillPolygons(polygons);
}


void Rasterizer2::fillPixel(Pt::int32_t x, Pt::int32_t y,
                            Pt::int32_t minX, Pt::int32_t minY,
                            Pt::uint8_t alpha)
{
    // Check the clipping
    if(!ClipShapeI::insideXRange(x, _currentClip)) return;
    if(!ClipShapeI::insideYRange(y, _currentClip)) return;

    // Draw the pixels using texture or gradient
    if(_isTexture || _isGradient) {
        const Pt::int32_t bw = _brushImage->width();
        const Pt::int32_t bh = _brushImage->height();
        const Pt::int32_t dx = x - minX;
        const Pt::int32_t dy = y - minY;
        const Pt::int32_t tx = _isGradient ? std::min(bw - 1, dx) : (dx % bw);
        const Pt::int32_t ty = _isGradient ? std::min(bh - 1, dy) : (dy % bh);
        ConstPixel srcPixel(_brushImage->view(), tx, ty);
        Pixel      dstPixel(_image->view(), x, y);
        _image->format().setPixel(dstPixel, srcPixel, _compositionMode, alpha);
    }

    // Draw the pixels using solid color
    else {
        Pixel pixel(_image->view(), x, y);
        _image->format().setPixel(pixel, _brush.color(), _compositionMode, alpha);
    }
}


void Rasterizer2::stroke4Pixels(Pt::int32_t x1, Pt::int32_t y1, Pt::int32_t x2, Pt::int32_t y2)
{
    const bool x1Valid = ClipShapeI::insideXRange(x1, _currentClip);
    const bool y1Valid = ClipShapeI::insideYRange(y1, _currentClip);
    const bool x2Valid = ClipShapeI::insideXRange(x2, _currentClip) && (x2 != x1);
    const bool y2Valid = ClipShapeI::insideYRange(y2, _currentClip) && (y2 != y1);

    if( x1Valid && y1Valid ) {
        Pixel pixel(_image->view(), x1, y1);
        _image->format().setPixel(pixel, _pen.color(), _compositionMode);
    }

    if( x1Valid && y2Valid ) {
        Pixel pixel(_image->view(), x1, y2);
        _image->format().setPixel(pixel, _pen.color(), _compositionMode);
    }

    if( x2Valid && y1Valid ) {
        Pixel pixel(_image->view(), x2, y1);
        _image->format().setPixel(pixel, _pen.color(), _compositionMode);
    }

    if( x2Valid && y2Valid ) {
        Pixel pixel(_image->view(), x2, y2);
        _image->format().setPixel(pixel, _pen.color(), _compositionMode);
    }
}


void Rasterizer2::stroke4Pixels(Pt::int32_t x1, Pt::int32_t y1, Pt::int32_t x2, Pt::int32_t y2, const bool mask[4])
{
    const bool x1Valid = ClipShapeI::insideXRange(x1, _currentClip);
    const bool y1Valid = ClipShapeI::insideYRange(y1, _currentClip);
    const bool x2Valid = ClipShapeI::insideXRange(x2, _currentClip) && (x2 != x1);
    const bool y2Valid = ClipShapeI::insideYRange(y2, _currentClip) && (y2 != y1);

    if( x1Valid && y1Valid && mask[0] ) {
        Pixel pixel(_image->view(), x1, y1);
        _image->format().setPixel(pixel, _pen.color(), _compositionMode);
    }

    if( x1Valid && y2Valid && mask[1] ) {
        Pixel pixel(_image->view(), x1, y2);
        _image->format().setPixel(pixel, _pen.color(), _compositionMode);
    }

    if( x2Valid && y1Valid && mask[2] ) {
        Pixel pixel(_image->view(), x2, y1);
        _image->format().setPixel(pixel, _pen.color(), _compositionMode);
    }

    if( x2Valid && y2Valid && mask[3] ) {
        Pixel pixel(_image->view(), x2, y2);
        _image->format().setPixel(pixel, _pen.color(), _compositionMode);
    }
}


void Rasterizer2::stroke4Pixels(Pt::int32_t x1, Pt::int32_t y1,
                                Pt::int32_t x2, Pt::int32_t y2,
                                Pt::uint8_t alpha)
{
    const bool x1Valid = ClipShapeI::insideXRange(x1, _currentClip);
    const bool y1Valid = ClipShapeI::insideYRange(y1, _currentClip);
    const bool x2Valid = ClipShapeI::insideXRange(x2, _currentClip) && (x2 != x1);
    const bool y2Valid = ClipShapeI::insideYRange(y2, _currentClip) && (y2 != y1);

    if (x1Valid && y1Valid ) {
        Pixel pixel(_image->view(), x1, y1);
        _image->format().setPixel(pixel, _pen.color(), _compositionMode, alpha);
    }

    if( x1Valid && y2Valid ) {
        Pixel pixel(_image->view(), x1, y2);
        _image->format().setPixel(pixel, _pen.color(), _compositionMode, alpha);
    }

    if( x2Valid && y1Valid ) {
        Pixel pixel(_image->view(), x2, y1);
        _image->format().setPixel(pixel, _pen.color(), _compositionMode, alpha);
    }

    if( x2Valid && y2Valid ) {
        Pixel pixel(_image->view(), x2, y2);
        _image->format().setPixel(pixel, _pen.color(), _compositionMode, alpha);
    }
}


void Rasterizer2::stroke4Pixels(Pt::int32_t x1, Pt::int32_t y1,
                                Pt::int32_t x2, Pt::int32_t y2,
                                Pt::uint8_t alpha, const bool mask[4])
{
    const bool x1Valid = ClipShapeI::insideXRange(x1, _currentClip);
    const bool y1Valid = ClipShapeI::insideYRange(y1, _currentClip);
    const bool x2Valid = ClipShapeI::insideXRange(x2, _currentClip) && (x2 != x1);
    const bool y2Valid = ClipShapeI::insideYRange(y2, _currentClip) && (y2 != y1);

    if( x1Valid && y1Valid && mask[0] ) {
        Pixel pixel(_image->view(), x1, y1);
        _image->format().setPixel(pixel, _pen.color(), _compositionMode, alpha);
    }

    if( x1Valid && y2Valid && mask[1] ) {
        Pixel pixel(_image->view(), x1, y2);
        _image->format().setPixel(pixel, _pen.color(), _compositionMode, alpha);
    }

    if( x2Valid && y1Valid && mask[2] ) {
        Pixel pixel(_image->view(), x2, y1);
        _image->format().setPixel(pixel, _pen.color(), _compositionMode, alpha);
    }

    if( x2Valid && y2Valid && mask[3] ) {
        Pixel pixel(_image->view(), x2, y2);
        _image->format().setPixel(pixel, _pen.color(), _compositionMode, alpha);
    }
}


void Rasterizer2::fill4Pixels(Pt::int32_t x1, Pt::int32_t y1,
                              Pt::int32_t x2, Pt::int32_t y2,
                              Pt::int32_t minX, Pt::int32_t minY)
{
    // Check the clipping
    const bool x1Valid = ClipShapeI::insideXRange(x1, _currentClip);
    const bool y1Valid = ClipShapeI::insideYRange(y1, _currentClip);
    const bool x2Valid = ClipShapeI::insideXRange(x2, _currentClip) && (x2 != x1);
    const bool y2Valid = ClipShapeI::insideYRange(y2, _currentClip) && (y2 != y1);

    // Draw the pixels using texture or gradient
    if(_isTexture || _isGradient) {
        const Pt::int32_t bw = _brushImage->width();
        const Pt::int32_t bh = _brushImage->height();
        if( x1Valid && y1Valid ) {
            ConstPixel srcPixel(_brushImage->view(), (x1 - minX) % bw, (y1 - minY) % bh);
            Pixel      dstPixel(_image->view(), x1, y1);
            _image->format().setPixel(dstPixel, srcPixel, _compositionMode);
        }
        if( x1Valid && y2Valid ) {
            ConstPixel srcPixel(_brushImage->view(), (x1 - minX) % bw, (y2 - minY) % bh);
            Pixel      dstPixel(_image->view(), x1, y2);
            _image->format().setPixel(dstPixel, srcPixel, _compositionMode);
        }
        if( x2Valid && y1Valid ) {
            ConstPixel srcPixel(_brushImage->view(), (x2 - minX) % bw, (y1 - minY) % bh);
            Pixel      dstPixel(_image->view(), x2, y1);
            _image->format().setPixel(dstPixel, srcPixel, _compositionMode);
        }
        if( x2Valid && y2Valid ) {
            ConstPixel srcPixel(_brushImage->view(), (x2 - minX) % bw, (y2 - minY) % bh);
            Pixel      dstPixel(_image->view(), x2, y2);
            _image->format().setPixel(dstPixel, srcPixel, _compositionMode);
        }
    }

    // Draw the pixels using solid color
    else {
        if( x1Valid && y1Valid ) {
            Pixel pixel(_image->view(), x1, y1);
            _image->format().setPixel(pixel, _brush.color(), _compositionMode);
        }
        if( x1Valid && y2Valid ) {
            Pixel pixel(_image->view(), x1, y2);
            _image->format().setPixel(pixel, _brush.color(), _compositionMode);
        }
        if( x2Valid && y1Valid ) {
            Pixel pixel(_image->view(), x2, y1);
            _image->format().setPixel(pixel, _brush.color(), _compositionMode);
        }
        if( x2Valid && y2Valid ) {
            Pixel pixel(_image->view(), x2, y2);
            _image->format().setPixel(pixel, _brush.color(), _compositionMode);
        }
    }
}


void Rasterizer2::fill4Pixels(Pt::int32_t x1, Pt::int32_t y1,
                              Pt::int32_t x2, Pt::int32_t y2,
                              Pt::int32_t minX, Pt::int32_t minY,
                              const bool mask[4])
{
    // Check the clipping
    const bool x1Valid = ClipShapeI::insideXRange(x1, _currentClip);
    const bool y1Valid = ClipShapeI::insideYRange(y1, _currentClip);
    const bool x2Valid = ClipShapeI::insideXRange(x2, _currentClip) && (x2 != x1);
    const bool y2Valid = ClipShapeI::insideYRange(y2, _currentClip) && (y2 != y1);

    // Draw the pixels using texture or gradient
    if(_isTexture || _isGradient) {
        const Pt::int32_t bw = _brushImage->width();
        const Pt::int32_t bh = _brushImage->height();
        if( x1Valid && y1Valid && mask[0] ) {
            ConstPixel srcPixel(_brushImage->view(), (x1 - minX) % bw, (y1 - minY) % bh);
            Pixel      dstPixel(_image->view(), x1, y1);
            _image->format().setPixel(dstPixel, srcPixel, _compositionMode);
        }
        if( x1Valid && y2Valid && mask[1] ) {
            ConstPixel srcPixel(_brushImage->view(), (x1 - minX) % bw, (y2 - minY) % bh);
            Pixel      dstPixel(_image->view(), x1, y2);
            _image->format().setPixel(dstPixel, srcPixel, _compositionMode);
        }
        if( x2Valid && y1Valid && mask[2] ) {
            ConstPixel srcPixel(_brushImage->view(), (x2 - minX) % bw, (y1 - minY) % bh);
            Pixel      dstPixel(_image->view(), x2, y1);
            _image->format().setPixel(dstPixel, srcPixel, _compositionMode);
        }
        if( x2Valid && y2Valid && mask[3] ) {
            ConstPixel srcPixel(_brushImage->view(), (x2 - minX) % bw, (y2 - minY) % bh);
            Pixel      dstPixel(_image->view(), x2, y2);
            _image->format().setPixel(dstPixel, srcPixel, _compositionMode);
        }
    }

    // Draw the pixels using solid color
    else {
        if( x1Valid && y1Valid && mask[0] ) {
            Pixel pixel(_image->view(), x1, y1);
            _image->format().setPixel(pixel, _brush.color(), _compositionMode);
        }
        if( x1Valid && y2Valid && mask[1] ) {
            Pixel pixel(_image->view(), x1, y2);
            _image->format().setPixel(pixel, _brush.color(), _compositionMode);
        }
        if( x2Valid && y1Valid && mask[2] ) {
            Pixel pixel(_image->view(), x2, y1);
            _image->format().setPixel(pixel, _brush.color(), _compositionMode);
        }
        if( x2Valid && y2Valid && mask[3] ) {
            Pixel pixel(_image->view(), x2, y2);
            _image->format().setPixel(pixel, _brush.color(), _compositionMode);
        }
    }
}


void Rasterizer2::fill4Pixels(Pt::int32_t x1, Pt::int32_t y1,
                              Pt::int32_t x2, Pt::int32_t y2,
                              Pt::int32_t minX, Pt::int32_t minY,
                              Pt::uint8_t alpha)
{
    // Check the clipping
    const bool x1Valid = ClipShapeI::insideXRange(x1, _currentClip);
    const bool y1Valid = ClipShapeI::insideYRange(y1, _currentClip);
    const bool x2Valid = ClipShapeI::insideXRange(x2, _currentClip) && (x2 != x1);
    const bool y2Valid = ClipShapeI::insideYRange(y2, _currentClip) && (y2 != y1);

    // Draw the pixels using texture or gradient
    if(_isTexture || _isGradient) {
        const Pt::int32_t bw = _brushImage->width();
        const Pt::int32_t bh = _brushImage->height();
        if( x1Valid && y1Valid ) {
            ConstPixel srcPixel(_brushImage->view(), (x1 - minX) % bw, (y1 - minY) % bh);
            Pixel      dstPixel(_image->view(), x1, y1);
            _image->format().setPixel(dstPixel, srcPixel, _compositionMode, alpha);
        }
        if( x1Valid && y2Valid ) {
            ConstPixel srcPixel(_brushImage->view(), (x1 - minX) % bw, (y2 - minY) % bh);
            Pixel      dstPixel(_image->view(), x1, y2);
            _image->format().setPixel(dstPixel, srcPixel, _compositionMode, alpha);
        }
        if( x2Valid && y1Valid ) {
            ConstPixel srcPixel(_brushImage->view(), (x2 - minX) % bw, (y1 - minY) % bh);
            Pixel      dstPixel(_image->view(), x2, y1);
            _image->format().setPixel(dstPixel, srcPixel, _compositionMode, alpha);
        }
        if( x2Valid && y2Valid ) {
            ConstPixel srcPixel(_brushImage->view(), (x2 - minX) % bw, (y2 - minY) % bh);
            Pixel      dstPixel(_image->view(), x2, y2);
            _image->format().setPixel(dstPixel, srcPixel, _compositionMode, alpha);
        }
    }

    // Draw the pixels using solid color
    else {
        if( x1Valid && y1Valid ) {
            Pixel pixel(_image->view(), x1, y1);
            _image->format().setPixel(pixel, _brush.color(), _compositionMode, alpha);
        }
        if( x1Valid && y2Valid ) {
            Pixel pixel(_image->view(), x1, y2);
            _image->format().setPixel(pixel, _brush.color(), _compositionMode, alpha);
        }
        if( x2Valid && y1Valid ) {
            Pixel pixel(_image->view(), x2, y1);
            _image->format().setPixel(pixel, _brush.color(), _compositionMode, alpha);
        }
        if( x2Valid && y2Valid ) {
            Pixel pixel(_image->view(), x2, y2);
            _image->format().setPixel(pixel, _brush.color(), _compositionMode, alpha);
        }
    }
}


void Rasterizer2::fill4Pixels(Pt::int32_t x1, Pt::int32_t y1,
                              Pt::int32_t x2, Pt::int32_t y2,
                              Pt::int32_t minX, Pt::int32_t minY,
                              Pt::uint8_t alpha, const bool mask[4])
{
    // Check the clipping
    const bool x1Valid = ClipShapeI::insideXRange(x1, _currentClip);
    const bool y1Valid = ClipShapeI::insideYRange(y1, _currentClip);
    const bool x2Valid = ClipShapeI::insideXRange(x2, _currentClip) && (x2 != x1);
    const bool y2Valid = ClipShapeI::insideYRange(y2, _currentClip) && (y2 != y1);

    // Draw the pixels using texture or gradient
    if(_isTexture || _isGradient) {
        const Pt::int32_t bw = _brushImage->width();
        const Pt::int32_t bh = _brushImage->height();
        if( x1Valid && y1Valid && mask[0] ) {
            ConstPixel srcPixel(_brushImage->view(), (x1 - minX) % bw, (y1 - minY) % bh);
            Pixel      dstPixel(_image->view(), x1, y1);
            _image->format().setPixel(dstPixel, srcPixel, _compositionMode, alpha);
        }
        if( x1Valid && y2Valid && mask[1] ) {
            ConstPixel srcPixel(_brushImage->view(), (x1 - minX) % bw, (y2 - minY) % bh);
            Pixel      dstPixel(_image->view(), x1, y2);
            _image->format().setPixel(dstPixel, srcPixel, _compositionMode, alpha);
        }
        if( x2Valid && y1Valid && mask[2] ) {
            ConstPixel srcPixel(_brushImage->view(), (x2 - minX) % bw, (y1 - minY) % bh);
            Pixel      dstPixel(_image->view(), x2, y1);
            _image->format().setPixel(dstPixel, srcPixel, _compositionMode, alpha);
        }
        if( x2Valid && y2Valid && mask[3] ) {
            ConstPixel srcPixel(_brushImage->view(), (x2 - minX) % bw, (y2 - minY) % bh);
            Pixel      dstPixel(_image->view(), x2, y2);
            _image->format().setPixel(dstPixel, srcPixel, _compositionMode, alpha);
        }
    }

    // Draw the pixels using solid color
    else {
        if( x1Valid && y1Valid && mask[0] ) {
            Pixel pixel(_image->view(), x1, y1);
            _image->format().setPixel(pixel, _brush.color(), _compositionMode, alpha);
        }
        if( x1Valid && y2Valid && mask[1] ) {
            Pixel pixel(_image->view(), x1, y2);
            _image->format().setPixel(pixel, _brush.color(), _compositionMode, alpha);
        }
        if( x2Valid && y1Valid && mask[2] ) {
            Pixel pixel(_image->view(), x2, y1);
            _image->format().setPixel(pixel, _brush.color(), _compositionMode, alpha);
        }
        if( x2Valid && y2Valid && mask[3] ) {
            Pixel pixel(_image->view(), x2, y2);
            _image->format().setPixel(pixel, _brush.color(), _compositionMode, alpha);
        }
    }
}


void Rasterizer2::fill4Pixels(Pt::int32_t x1, Pt::int32_t y1,
                             Pt::int32_t x2, Pt::int32_t y2,
                             Pt::int32_t minX, Pt::int32_t minY,
                             const Pt::uint8_t alphaMask[4])
{
    // Check the clipping
    const bool x1Valid = ClipShapeI::insideXRange(x1, _currentClip);
    const bool y1Valid = ClipShapeI::insideYRange(y1, _currentClip);
    const bool x2Valid = ClipShapeI::insideXRange(x2, _currentClip) && (x2 != x1);
    const bool y2Valid = ClipShapeI::insideYRange(y2, _currentClip) && (y2 != y1);

    // Draw the pixels using texture or gradient
    if(_isTexture || _isGradient) {
        const Pt::int32_t bw = _brushImage->width();
        const Pt::int32_t bh = _brushImage->height();
        if( x1Valid && y1Valid && alphaMask[0] ) {
            ConstPixel srcPixel(_brushImage->view(), (x1 - minX) % bw, (y1 - minY) % bh);
            Pixel      dstPixel(_image->view(), x1, y1);
            _image->format().setPixel(dstPixel, srcPixel, _compositionMode, alphaMask[0]);
        }
        if( x1Valid && y2Valid && alphaMask[1] ) {
            ConstPixel srcPixel(_brushImage->view(), (x1 - minX) % bw, (y2 - minY) % bh);
            Pixel      dstPixel(_image->view(), x1, y2);
            _image->format().setPixel(dstPixel, srcPixel, _compositionMode, alphaMask[1]);
        }
        if( x2Valid && y1Valid && alphaMask[2] ) {
            ConstPixel srcPixel(_brushImage->view(), (x2 - minX) % bw, (y1 - minY) % bh);
            Pixel      dstPixel(_image->view(), x2, y1);
            _image->format().setPixel(dstPixel, srcPixel, _compositionMode, alphaMask[2]);
        }
        if( x2Valid && y2Valid && alphaMask[3] ) {
            ConstPixel srcPixel(_brushImage->view(), (x2 - minX) % bw, (y2 - minY) % bh);
            Pixel      dstPixel(_image->view(), x2, y2);
            _image->format().setPixel(dstPixel, srcPixel, _compositionMode, alphaMask[3]);
        }
    }

    // Draw the pixels using solid color
    else {
        if( x1Valid && y1Valid && alphaMask[0] ) {
            Pixel pixel(_image->view(), x1, y1);
            _image->format().setPixel(pixel, _brush.color(), _compositionMode, alphaMask[0]);
        }
        if( x1Valid && y2Valid && alphaMask[1] ) {
            Pixel pixel(_image->view(), x1, y2);
            _image->format().setPixel(pixel, _brush.color(), _compositionMode, alphaMask[1]);
        }
        if( x2Valid && y1Valid && alphaMask[2] ) {
            Pixel pixel(_image->view(), x2, y1);
            _image->format().setPixel(pixel, _brush.color(), _compositionMode, alphaMask[2]);
        }
        if( x2Valid && y2Valid && alphaMask[3] ) {
            Pixel pixel(_image->view(), x2, y2);
            _image->format().setPixel(pixel, _brush.color(), _compositionMode, alphaMask[3]);
        }
    }
}


void Rasterizer2::rasterScanline(Pt::int32_t iterL, Pt::int32_t iterR,
                                 Pt::int32_t pixelY,
                                 Pt::int32_t minX,
                                 Pt::int32_t minY,
                                 const Color& color)
{

    // Draw the span using texture (or gradient texture)
    if(_isTexture)
    {
        const Pt::int32_t bw        = _brushImage->width();
        const Pt::int32_t bh        = _brushImage->height();
              Pt::int32_t iterX     = iterL;
              Pt::int32_t spanWidth = iterR - iterL + 1;
        while(spanWidth > 0) {
            const Pt::int32_t tx = _isGradient ? std::min(bw - 1, iterX ) : (iterX  % bw);
            const Pt::int32_t ty = _isGradient ? std::min(bh - 1, pixelY) : (pixelY % bh);
            const Pt::int32_t n  = std::min<Pt::int32_t>(spanWidth, bw - tx);
            if(n) {
                ConstPixel srcPixel(_brushImage->view(), tx, ty);
                Pixel      dstPixel(_image->view(), minX + iterX, minY + pixelY);
                _image->format().copy(dstPixel, srcPixel,  n, _compositionMode);
            }
            spanWidth -= n;
            iterX     += n;
        }

        return;
    }

    // Draw the span using gradient
    if(_isGradient)
    {
        Pt::int32_t iterX     = iterL;
        Pt::int32_t spanWidth = iterR - iterL + 1;

        if(_brush.gradient() == Pt::Gfx::Brush::Vertical)
        {
            const Pt::int32_t textureY = std::min<Pt::int32_t>(pixelY,  _brushImage->height() - 1);
            ConstPixel        srcPixel(_brushImage->view(), 0, textureY);
            Pixel             dstPixel(_image->view(), minX + iterX, minY + pixelY);
            _image->format().setPixels(dstPixel, srcPixel, spanWidth, _compositionMode);
        }
        else // Pt::Gfx::Brush::Horizontal
        {
            while(spanWidth > 0) {
                const Pt::int32_t tx = std::min<Pt::int32_t>(iterX,  _brushImage->width () -1);
                const Pt::int32_t n  = std::min<Pt::int32_t>(spanWidth, _brushImage->width() - tx);
                if(n) {
                    ConstPixel srcPixel(_brushImage->view(), tx, 0);
                    Pixel      dstPixel(_image->view(), minX + iterX, minY + pixelY);
                    _image->format().copy(dstPixel, srcPixel,  n, _compositionMode);
                }
                spanWidth -= n;
                iterX     += n;
            }
        }

        return;
    }

    // Draw the span using solid color
#if 0
    if(IP2_DEBUG::DUMP_SCANLINE_COORDINATES) {
        std::cerr << std::fixed << std::setw(5) << std::setprecision(1)
              << "SCNLINE " << (minX + iterL) << ", " << (minY + pixelY) << " LEN " << (iterR - iterL + 1) << std::endl;
    }
#endif

    Pixel pixel(_image->view(), minX + iterL, minY + pixelY);
    _image->format().setPixels(pixel, color, iterR - iterL + 1, _compositionMode);

    //fprintf(stderr, "RS [%3d] = %3d - %3d\n", minY + pixelY, (minX + iterL), (minX + iterL) + (iterR - iterL + 1) - 1);
}


void Rasterizer2::rasterScanlineClipped(Pt::int32_t from, Pt::int32_t to,
                                        Pt::int32_t pixelY,
                                        Pt::int32_t minX,
                                        Pt::int32_t minY)
{
    // Check if the Y coordinate is outside the clipping region
    if( !ClipShapeI::insideYRange(pixelY, _currentClip) ) return;

    // Check and limit the X coordinates
    from = ClipShapeI::clipLeft  (from, _currentClip);
    to   = ClipShapeI::clipRight (to,   _currentClip);

    if(to < from) return;

    // Draw the scanline
    rasterScanline(from - minX, to - minX, pixelY - minY, minX, minY, _brush.color());
}


} // namespace

} // namespace
