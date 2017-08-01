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

#include "Rasterizer2.h"
#include "ClipShape.h"
#include <Pt/Gfx/Transform.h>

namespace Pt {

namespace Gfx {

static inline float triangle(float v)
{
    const float p = 0.5f;

    const Pt::int32_t x = v * 1000.0f;
    const Pt::int32_t m = p * 1000.0f;

    return (1.0f / p) * (p - fabs( ( x % (2 * m) ) * 0.001f - p) );
}


static inline void eqpLerp(Pt::uint8_t resRGBA[4], const Pt::uint8_t srcRGBA[4], const Pt::uint8_t dstRGBA[4], float mf)
{
    const float imf = 1.0f - mf;

#if 1

    // Equivalent-power linear interpolation

    const float srcPower = (float) srcRGBA[0] * srcRGBA[0] + (float) srcRGBA[1] * srcRGBA[1] + (float) srcRGBA[2] + srcRGBA[2];
    const float dstPower = (float) dstRGBA[0] * dstRGBA[0] + (float) dstRGBA[1] * dstRGBA[1] + (float) dstRGBA[2] + dstRGBA[2];
    const float resPower = srcPower * mf + dstPower * imf;

    Pt::int32_t intR = srcRGBA[0] * mf + dstRGBA[0] * imf;
    Pt::int32_t intG = srcRGBA[1] * mf + dstRGBA[1] * imf;
    Pt::int32_t intB = srcRGBA[2] * mf + dstRGBA[2] * imf;
    Pt::int32_t intA = srcRGBA[3] * mf + dstRGBA[3] * imf;

    const float intP = (float) intR * intR + (float) intG * intG + (float) intB + intB;
    const float mulF = sqrtf(resPower / intP);

    intR = intR * mulF;
    intG = intG * mulF;
    intB = intB * mulF;

    resRGBA[0] = (intR >= 255) ? 255 : intR;
    resRGBA[1] = (intG >= 255) ? 255 : intG;
    resRGBA[2] = (intB >= 255) ? 255 : intB;
    resRGBA[3] = (intA >= 255) ? 255 : intA;

#else

    // Normal linear interpolation

    resRGBA[0] = srcRGBA[0] * mf + dstRGBA[0] * imf;
    resRGBA[1] = srcRGBA[1] * mf + dstRGBA[1] * imf;
    resRGBA[2] = srcRGBA[2] * mf + dstRGBA[2] * imf;
    resRGBA[3] = srcRGBA[3] * mf + dstRGBA[3] * imf;

#endif
}


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

    if( pen.style() != Pen::Solid )
        _polygonizer.setPattern( pen.style() );

    updatePenPattern();
}


void Rasterizer2::updatePenPattern()
{
    // Predefined patterns
    static const Pt::uint64_t patternDot        = 0x8080808080808080;// 1000000010000000100000001000000010000000100000001000000010000000
    static const Pt::uint64_t patternDoubleDot  = 0x8400840084008400;// 1000010000000000100001000000000010000100000000001000010000000000
    static const Pt::uint64_t patternDash       = 0xFF00FF00FF00FF00;// 1111111100000000111111110000000011111111000000001111111100000000
    static const Pt::uint64_t patternDoubleDash = 0xFF07F800FF07F800;// 1111111100000111111110000000000011111111000001111111100000000000
    static const Pt::uint64_t patternDotDash    = 0x800FF000800FF000;// 1000000000001111111100000000000010000000000011111111000000000000

    // Select the pattern
    Pt::uint64_t patternSel;

    switch( _pen.style() )
    {
        default:
        case Pen::Dot         : patternSel = patternDot;              break;
        case Pen::DoubleDot   : patternSel = patternDoubleDot;        break;
        case Pen::Dash        : patternSel = patternDash;             break;
        case Pen::DoubleDash  : patternSel = patternDoubleDash;       break;
        case Pen::DotDash     : patternSel = patternDotDash;          break;
        case Pen::UserDefined : patternSel = _pen.styleUserPattern(); break;
    }

    // Counter for generating the patterns
    size_t gctr1P = 0;

    // Generate the pattern
    bool previous = 0;
    for(Pt::int8_t p = 0; p < 64; ++p)
    { // The pattern has 64 points
        // Get the pattern cell value
        const bool current = patternSel & ((Pt::uint64_t) 1 << p);

        // --- One-pixel pattern ---
        // Pattern cell change from 0 to 0
        if(!previous && !current) {
            for(Pt::uint8_t i = 1; i <= PATTERN_BUFFER_SCALE_FACTOR; ++i) {
                _patternBuffer1P[gctr1P++] = 0;
            }
        }
        // Pattern cell change from 1 to 1
        else if(previous && current) {
            for(Pt::uint8_t i = 1; i <= PATTERN_BUFFER_SCALE_FACTOR; ++i) {
                _patternBuffer1P[gctr1P++] = 255;
            }
        }
        // Pattern cell change from 0 to 1
        else if(!previous && current) {
            for(Pt::int32_t i = 1; i <= PATTERN_BUFFER_SCALE_FACTOR; ++i) {
                _patternBuffer1P[gctr1P++] = i * 255 / PATTERN_BUFFER_SCALE_FACTOR;
            }
        }
        // Pattern cell change from 1 to 0
        else if(previous && !current) {
            for(Pt::int32_t i = 1; i <= PATTERN_BUFFER_SCALE_FACTOR; ++i) {
                _patternBuffer1P[gctr1P++] = 255 - i * 255 / PATTERN_BUFFER_SCALE_FACTOR;
            }
        }
        // Copy the pattern cell value
        previous = current;
    }

    // Transfom the pattern - without anti-aliasing
    if( ! _aaMode )
    {
        for(size_t i = 0; i < gctr1P; ++i) {
            if(_patternBuffer1P[i] > 127) _patternBuffer1P[i] = 255;
            else                          _patternBuffer1P[i] = 0;
        }
    }
    // Transfom the pattern - with anti-aliasing
    else
    {
        for(size_t i = 0; i < gctr1P; ++i) {
            _patternBuffer1P[i] = XWAA_WFILTER[ 255 - _patternBuffer1P[i] ];
        }
    }
}


Pt::uint8_t Rasterizer2::patternBuffer1PAlpha(Pt::int32_t idx) const
{
    return _patternBuffer1P[ idx % FIXED_POINT_TO_INT(PATTERN_BUFFER_COUNTER_MAX1P) ];
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
    a0 = (Pt::uint32_t) _patternBuffer1P[ idx % FIXED_POINT_TO_INT(PATTERN_BUFFER_COUNTER_MAX1P) ] * alpha0 / 255;
    a1 = (Pt::uint32_t) _patternBuffer1P[ idx % FIXED_POINT_TO_INT(PATTERN_BUFFER_COUNTER_MAX1P) ] * alpha1 / 255;
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
    /*
    // Adjust the width and height as needed (anti-aliasing can
    // create additional pixels on the edges)
    if(_aaMode != AntiAliasingMode::None) {
        width  += 2;
        height += 2;
    }
    */

    // Resize the brush buffer and the start-end colors
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
    // Determine the start and end colors
    Pt::uint8_t sc[4], ec[4];
    updateGradientBrush_getStartEndColors(sc, ec);

    const Pt::uint8_t rs = sc[0], gs = sc[1], bs = sc[2], as = sc[3];
    const Pt::uint8_t re = ec[0], ge = ec[1], be = ec[2], ae = ec[3];

    // Generate the gradient
    const Pt::int32_t ofs    = (width == 1) ? _brush.offsetY() : _brush.offsetX();
    const Pt::int32_t length = width + height - 1 - 1;

    Pt::uint8_t* pixel = _brushBuffer.data();

    for(Pt::int32_t n = 0; n <= length; ++n) {
        const Pt::int32_t fo = n + ofs;
        const Pt::int32_t fi = (fo <= 0) ? 0 : ( (fo >= length) ? length : fo );
        const Pt::int32_t f2 = FIXED_POINT_FROM_INT(fi) / length;
        const Pt::int32_t f1 = FIXED_POINT_CONSTANT_ONE - f2;
        const Pt::uint8_t r1 = FIXED_POINT_TO_INT(rs * f1);
        const Pt::uint8_t r2 = FIXED_POINT_TO_INT(re * f2);
        const Pt::uint8_t g1 = FIXED_POINT_TO_INT(gs * f1);
        const Pt::uint8_t g2 = FIXED_POINT_TO_INT(ge * f2);
        const Pt::uint8_t b1 = FIXED_POINT_TO_INT(bs * f1);
        const Pt::uint8_t b2 = FIXED_POINT_TO_INT(be * f2);
        const Pt::uint8_t a1 = FIXED_POINT_TO_INT(as * f1);
        const Pt::uint8_t a2 = FIXED_POINT_TO_INT(ae * f2);
        *pixel++ = b1 + b2;
        *pixel++ = g1 + g2;
        *pixel++ = r1 + r2;
        *pixel++ = a1 + a2;
    }
}


void Rasterizer2::updateGradientBrush_gen2DLinearGradient(Pt::int32_t width, Pt::int32_t height)
{
    // Determine the start and end colors
    Pt::uint8_t sc[4], ec[4], rc[4];
    updateGradientBrush_getStartEndColors(sc, ec);

    // Calculate the focus point
    float focusX = 0.0f;
    float focusY = 0.0f;

    if(_brush.positionMode() == Brush::Absolute) {
        focusX = _brush.gradientBegin().x();
        focusY = _brush.gradientBegin().y();
    }
    else { // Brush::Relative
        focusX = width  * _brush.gradientBegin().x();
        focusY = height * _brush.gradientBegin().y();
    }

    // Calculate the rotation
    const float angl = _brush.gradientAngle() + 0.001f;
    const float rad  = angl * DegToRadF - piQuart<float>();
    const float sval = ::sin(rad);
    const float cval = ::cos(rad);

    // Define the reference line
    const float wq = sqrtf(width * width + height * height) * 0.25f;
    const float x1 = -wq;
    const float y1 =  wq;
    const float x2 =  wq;
    const float y2 = -wq;

    // Determine the rotated reference line
    const float rx1 = ( sval * x1 + cval * y1) + focusX;
    const float ry1 = ( cval * x1 - sval * y1) + focusY;
    const float rx2 = ( sval * x2 + cval * y2) + focusX;
    const float ry2 = ( cval * x2 - sval * y2) + focusY;

    // Calculate the gradient of the rotated reference line
    const float rm = (ry2 - ry1) / (rx1 - rx2);

    // Generate the gradient
    Pt::uint8_t* pixel = _brushBuffer.data();

    for(Pt::int32_t y = 0; y < height; ++y) {
        // Calculate the scaling factor
        float const p0 = rm * (y - ry1) + rx1;
        float const p1 = rm * (y - ry2) + rx2;
        float const d  = 1.0f / (p1 - p0);
        for(Pt::int32_t x = 0; x < width; ++x) {
            // Calculate the distance and blending factor
            const float dist = d * (x - p0);
            const float mf   = (dist <= 0.0f) ? 0.0f : ( (dist >= 1.0f) ? 1.0f : dist );
            // Interpolate the color
            eqpLerp(rc, sc, ec, mf);
            // Put the pixel
            *pixel++ = rc[2];
            *pixel++ = rc[1];
            *pixel++ = rc[0];
            *pixel++ = rc[3];
        }
    }
}


void Rasterizer2::updateGradientBrush_gen2DRadialGradient(Pt::int32_t width, Pt::int32_t height)
{
    // Determine the start and end colors
    Pt::uint8_t sc[4], ec[4], rc[4];
    updateGradientBrush_getStartEndColors(sc, ec);

    // Calculate the center point
    const float centerX = width  * 0.5f;
    const float centerY = height * 0.5f;

    // Calculate the inverse scaling factor
    const float radius = std::max(centerX, centerY);
    const float ilen   = 1.0f / radius;

    // Calculate the focus point
    float focusX = 0.0f;
    float focusY = 0.0f;

    if(_brush.positionMode() == Brush::Absolute) {
        focusX = _brush.gradientBegin().x();
        focusY = _brush.gradientBegin().y();
    }
    else { // Brush::Relative
        focusX = width  * _brush.gradientBegin().x();
        focusY = height * _brush.gradientBegin().y();
    }

    // Generate the gradient
    Pt::uint8_t* pixel = _brushBuffer.data();
    for(Pt::int32_t y = 0; y < height; ++y) {
        // Calculate the delta Y from the focus and center points
        const float dy = y - focusY;
        const float cy = y - centerY;
        for(Pt::int32_t x = 0; x < width; ++x) {
            // Calculate the delta X from the focus and center points
            const float dx = x - focusX;
            const float cx = x - centerX;
            // Calculate the inverse distance from the focus and center points
            float dist = 1.0f - sqrtf(dx * dx + dy * dy) * ilen;
            float cent = 1.0f - sqrtf(cx * cx + cy * cy) * ilen;
            if(dist < 0.0f) dist = 0.0f;
            if(cent < 0.0f) cent = 0.0f;
#if 0
            // Perform gradient repeat
            const float repcn = 3.0f;
            dist = triangle(repcn * dist);
#endif
            // Calculate the blending factor
            const float mf = 1.0f - ( dist + 4.0f * dist * powf(cent, 0.5f) ) * 0.2f;
            //const float mf = 1.0f - dist * powf(cent, 0.5f);
            // Interpolate the color
            eqpLerp(rc, sc, ec, mf);
            // Put the pixel
            *pixel++ = rc[2];
            *pixel++ = rc[1];
            *pixel++ = rc[0];
            *pixel++ = rc[3];
        }
    }
}


/*
void Rasterizer2::updateGradientBrush_gen2DRectangularGradient(Pt::int32_t width, Pt::int32_t height)
{
    // Determine the start and end colors
    Pt::uint8_t sc[4], ec[4], rc[4];
    updateGradientBrush_getStartEndColors(sc, ec);

    // Calculate the center point
    const float centerX = width  * 0.5f;
    const float centerY = height * 0.5f;

    // Calculate the inverse scaling factor
    const float radius = std::max(centerX, centerY);
    const float ilen   = 1.0f / radius;

    // Calculate the focus point
    float focusX = 0.0f;
    float focusY = 0.0f;

    if(_brush.positionMode() == Brush::Absolute) {
        focusX = _brush.gradientBegin().x();
        focusY = _brush.gradientBegin().y();
    }
    else { // Brush::Relative
        focusX = width  * _brush.gradientBegin().x();
        focusY = height * _brush.gradientBegin().y();
    }

    // Calculate the rotation
    const float angle = _brush.gradientAngle();
    const float rad   = -angle * DegToRadF;
    const float sval  = ::sin(rad);
    const float cval  = ::cos(rad);

    // Generate the gradient
    Pt::uint8_t* pixel = _brushBuffer.data();
    for(Pt::int32_t y = 0; y < height; ++y) {
        // Calculate the delta Y from the focus point
        const float dy = y - focusY;
        for(Pt::int32_t x = 0; x < width; ++x) {
            // Calculate the delta X from the focus point
            const float dx = x - focusX;
            // Calculate the rotated deltas from the focus point
            const float ry = -sval * dx + cval * dy;
            const float rx =  cval * dx + sval * dy;
            // Calculate the inverse distance from the focus point
            float dist = 1.0f - ( fabs(rx) + fabs(ry) ) * ilen;
            if(dist < 0.0f) dist = 0.0f;
#if 0
            // Perform gradient repeat
            const float repcn = 3.0f;
            dist = triangle(repcn * dist);
#endif
            // Calculate the blending factor
            const float mf = 1.0f - dist;
            // Interpolate the color
            eqpLerp(rc, sc, ec, mf);
            // Put the pixel
            *pixel++ = rc[2];
            *pixel++ = rc[1];
            *pixel++ = rc[0];
            *pixel++ = rc[3];
        }
    }
}


void Rasterizer2::updateGradientBrush_gen2DConicalGradient(Pt::int32_t width, Pt::int32_t height)
{
#define CONICAL_GRADIENT_USE_SMOOTH_TRANSITION

    // Determine the start and end colors
    Pt::uint8_t sc[4], ec[4], rc[4];
    updateGradientBrush_getStartEndColors(sc, ec);

#ifdef CONICAL_GRADIENT_USE_SMOOTH_TRANSITION
    // Calculate the middle color from the start and end colors
    Pt::uint8_t mc[4];
    eqpLerp(mc, sc, ec, 0.5f);

    // Low and high limit for mixing color
    const float loLim = 0.25f;
    const float hiLim = 0.75f;
#endif

    // Calculate the focus point
    float focusX = 0.0f;
    float focusY = 0.0f;

    if(_brush.positionMode() == Brush::Absolute) {
        focusX = _brush.gradientBegin().x();
        focusY = _brush.gradientBegin().y();
    }
    else { // Brush::Relative
        focusX = width  * _brush.gradientBegin().x();
        focusY = height * _brush.gradientBegin().y();
    }

    // Get the rotation angle
    const float angle = _brush.gradientAngle();

#ifndef CONICAL_GRADIENT_USE_SMOOTH_TRANSITION
    // Determine if the transition area needs to be anti-aliased
    const float ang90 =  angle - floor(angle / 90.0f) * 90.0f;
    const bool  useAA = (_aaMode) && (ang90 >= 0.1f);
#endif

    // Calculate the rotation
    const float rad  = angle * DegToRadF - piHalf<float>();
    const float sval = ::sin(rad);
    const float cval = ::cos(rad);

    // Generate the gradient
    Pt::uint8_t* pixel = _brushBuffer.data();

    for(Pt::int32_t y = 0; y < height; ++y) {
        // Calculate the delta Y from the focus point
        const float dy = -(y - focusY); // Sign inversion due to differences between cartesian and computer coordinate systems
        for(Pt::int32_t x = 0; x < width; ++x) {
            // Calculate the delta X from the focus point
            const float dx = x - focusX;
            // Calculate the rotated deltas
            const float ry = (-sval * dx + cval * dy);
            const float rx = ( cval * dx + sval * dy);
            // Calculate the distance
            float dist = (std::atan2(ry, rx) + pi<float>()) / piDouble<float>();
                 if(dist < 0.0f) dist = 0.0f;
            else if(dist > 1.0f) dist = 1.0f;
#ifdef CONICAL_GRADIENT_USE_SMOOTH_TRANSITION
            // Distance: 0.00f <= dist <= loLim --- blend from end color to middle color
            if(dist <= loLim) {
                // Calculate the blending factor
                const float mf = dist / loLim;
                // Interpolate the color
                eqpLerp(rc, ec, mc, mf);
                // Put the pixel
                *pixel++ = rc[2];
                *pixel++ = rc[1];
                *pixel++ = rc[0];
                *pixel++ = rc[3];
            }
            // Distance: hiLim <= dist <= 1.00f --- blend from middle color to start color
            else if(dist >= hiLim) { //
                // Calculate the blending factor
                const float mf = (dist - hiLim) / loLim;
                // Interpolate the color
                eqpLerp(rc, mc, sc, mf);
                // Put the pixel
                *pixel++ = rc[2];
                *pixel++ = rc[1];
                *pixel++ = rc[0];
                *pixel++ = rc[3];
            }
            // Distance: loLim < dist < hiLim --- blend from start color to end color
            else {
                // Calculate the blending factor
                const float mf = (dist - loLim) / (hiLim - loLim);
                // Interpolate the color
                eqpLerp(rc, sc, ec, mf);
                // Put the pixel
                *pixel++ = rc[2];
                *pixel++ = rc[1];
                *pixel++ = rc[0];
                *pixel++ = rc[3];
            }
#else
            // Anti-alias the distance
            if(useAA && (dist < 0.01f || dist > 0.99f)) {
                const float       dd   = 4.0f - ceil( ( (dist < 0.5f) ? dist : (1.0f - dist) ) * 400 );
                const Pt::int32_t ijm  = (dd <= 2.0f) ? 2 : dd;
                const float       ijm2 = (ijm - 1) * 0.5f;
                dist = 0.0f;
                for(Pt::int32_t i = 0; i < ijm; ++i) {
                    for(Pt::int32_t j = 0; j < ijm; ++j) {
                        const float dxs =  (x + i - ijm2 - focusX);
                        const float dys = -(y + j - ijm2 - focusY); // Sign inversion due to differences between cartesian and computer coordinate systems
                        const float rxs = ( cval * dxs + sval * dys);
                        const float rys = (-sval * dxs + cval * dys);
                        dist += (std::atan2(rys, rxs) + pi<float>()) / piDouble<float>();
                    }
                }
                dist /= (ijm * ijm);
            }
            // Calculate the blending factor
            const float mf = (dist <= 0.0f) ? 0.0f : ( (dist >= 1.0f) ? 1.0f : dist );
            // Interpolate the color
            eqpLerp(rc, sc, ec, mf);
            // Put the pixel
            *pixel++ = rc[2];
            *pixel++ = rc[1];
            *pixel++ = rc[0];
            *pixel++ = rc[3];
#endif
        }
    }

#undef CONICAL_GRADIENT_USE_SMOOTH_TRANSITION
}
*/


void Rasterizer2::updateGradientBrush_getStartEndColors(Pt::uint8_t rgbaStart[4], Pt::uint8_t rgbaEnd[4])
{
    rgbaStart[0] = _brush.color().red  () / 257;
    rgbaStart[1] = _brush.color().green() / 257;
    rgbaStart[2] = _brush.color().blue () / 257;
    rgbaStart[3] = _brush.color().alpha() / 257;

    rgbaEnd[0] = _brush.gradientColor().red  () / 257;
    rgbaEnd[1] = _brush.gradientColor().green() / 257;
    rgbaEnd[2] = _brush.gradientColor().blue () / 257;
    rgbaEnd[3] = _brush.gradientColor().alpha() / 257;
}


void Rasterizer2::setFont(const Font& font)
{
    _font = font;

    if( font.name().empty() )
    {
        Font defaultFont(FreeType::instance().defaultFont(), font);

        _faceId = FreeType::instance().findFaceId(defaultFont);
    }
    else
    {
        _faceId = FreeType::instance().findFaceId(font);
    }

    // setup the image type
    _imageType.face_id = _faceId;
    _imageType.width   = font.size();
    _imageType.height  = font.size();
    _imageType.flags   =  FT_LOAD_DEFAULT | FT_LOAD_RENDER;
}


void Rasterizer2::setClip( const Rect& clip )
{
    _clip = clip;
    updateClip();
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

    Rect fromClip(fromPos, _currentClip.size());
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
    FreeType::instance().draw(*_image, _pen.color(), to, text,
                               _currentClip, _compositionMode,
                               transform, _faceId, &_imageType);
}


FontMetrics Rasterizer2::fontMetrics(const String& text) const
{
    FTC_ImageType imageType = const_cast<FTC_ImageType>(&_imageType);

    return FreeType::instance().fontMetrics(text, _faceId, imageType);
}


FontMetrics Rasterizer2::fontMetrics(const Font& font, const Pt::String& text)
{
    FTC_FaceID faceId = FreeType::instance().findFaceId(font);

    FTC_ImageTypeRec imageType;
    imageType.face_id = faceId;
    imageType.width   = font.size();
    imageType.height  = font.size();
    imageType.flags   =  FT_LOAD_DEFAULT | FT_LOAD_RENDER;

    return FreeType::instance().fontMetrics(text, faceId, &imageType);
}


void Rasterizer2::drawLine(const PointF& from, const PointF& to)
{
    if(_pen.size() == 1)
    {
        Point a( lround(from.x()), lround(from.y()) );
        Point b( lround(to  .x()), lround(to  .y()) );

        drawNarrowLine(a, b, 0);
        return;
    }

    PointF points[2] = { from, to };

    std::vector<Polygon> polygons;
    _polygonizer.renderWidePolyline(polygons, points, 2, _pen);

    // no performance benefit to use renderWideLine
    //_polygonizer.renderWideLine( polygons, from, to, _rasterizer->pen() );

    for(std::size_t n = 0; n < polygons.size(); ++n)
    {
        const std::vector<PointF>& polygon = polygons[n].points();
        rasterWideLine( &polygon[0], polygon.size() );
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
    if(_pen.style() == Pen::Solid)
    {
        rasterNarrowSolidLine(x1, y1, x2, y2, _pen.color(), maskInOut);
    }
    else
    {
        Pt::int32_t fpiCtrInOut = PATTERN_BUFFER_COUNTER_START;
        rasterNarrowPatternedLine(x1, y1, x2, y2, _pen.color(), fpiCtrInOut, maskInOut);
    }
}


void Rasterizer2::drawPolyline(const PointF* ps, const size_t pointCount)
{
    if(_pen.size() == 1)
    {
        drawNarrowPolyline(ps, pointCount);
    }
    else
    {
        drawWidePolyline(ps, pointCount);
    }
}


void Rasterizer2::drawNarrowPolyline(const PointF* pointsF, size_t pointCount)
{
    std::vector<PointF> clipped(pointsF, pointsF + pointCount);
    BasicClipShape<double>::clipPolyline(clipped, _currentClip);

    std::vector<Point> points;
    for(std::size_t i = 0; i < clipped.size(); ++i)
    {
        const PointF& pf = clipped[i];
        const Pt::int32_t x = Pt::lround( pf.x() );
        const Pt::int32_t y = Pt::lround( pf.y() );
        points.push_back( Point(x, y) );
    }

    if(points.size() < 2)
        return;

    DrawLineMask mask_nnp1;
    memcpy(mask_nnp1, Rasterizer2::NullLineMask, sizeof(DrawLineMask));

    bool solid = _pen.style() == Pen::Solid;
    Pt::int32_t fpiCtrInOut = PATTERN_BUFFER_COUNTER_START;

    // From point N to point (N + 1), successively
    std::size_t pc1 = points.size() - 1;

    for(std::size_t i = 0; i < pc1; ++i)
    {
        if(solid)
            rasterNarrowSolidLine(points[i].x(), points[i].y(),
                                  points[i + 1].x(), points[i + 1].y(),
                                  _pen.color(), &mask_nnp1);
        else
            rasterNarrowPatternedLine(points[i].x(), points[i].y(),
                                      points[i + 1].x(), points[i + 1].y(),
                                      _pen.color(), fpiCtrInOut, &mask_nnp1);
    }
}


void Rasterizer2::drawWidePolyline(const PointF* points, const size_t pointCount)
{
    std::vector<Polygon> polygons;
    _polygonizer.renderWidePolyline(polygons, points, pointCount, _pen);

    bool isSolid = _pen.style() == Pen::Solid;
    bool isClosed = points[0] == points[pointCount - 1];

    if( isSolid && isClosed )
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
    if(_pen.size() == 1)
    {
        const Point tl( Pt::lround(rect.topLeft().x()),
                        Pt::lround(rect.topLeft().y()) );
        const Point br( Pt::lround(rect.bottomRight().x()),
                        Pt::lround(rect.bottomRight().y()) );

        rasterNarrowRect(tl, br);
        return;
    }

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
    if(_pen.size() == 1)
    {

        const Point tl( Pt::lround(topLeft.x()),
                        Pt::lround(topLeft.y()) );
        const Size  sz( Pt::lround(size.width()),
                        Pt::lround(size.height()) );

        rasterNarrowArc(tl, sz, 0, 0, ArcMode::Open);
        return;
    }

    // use a new pen with bevel join
    Pen newPen = _pen;
    newPen.setJoinStyle(Pen::BevelJoin);

    std::vector<Polygon> polygons;
    _polygonizer.renderEllipse(polygons, topLeft, size, newPen);

    bool isSolid = _pen.style() == Pen::Solid;

    if( isSolid )
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

    bool isSolid = _pen.style() == Pen::Solid;
    bool isClosed = arcMode != ArcMode::Open;

    if( isSolid && isClosed )
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
    std::vector<PointF> clipped(pointsF, pointsF + pointCount);
    BasicClipShape<double>::clipPolyline(clipped, _currentClip);

    if(clipped.size() < 2)
        return;

    DrawLineMask mask_nnp1;
    memcpy(mask_nnp1, Rasterizer2::NullLineMask, sizeof(DrawLineMask));

    bool solid = _pen.style() == Pen::Solid;
    Pt::int32_t fpiCtrInOut = PATTERN_BUFFER_COUNTER_START;

    // From point N to point (N + 1), successively
    std::size_t pc1 = clipped.size() - 1;

    for(std::size_t i = 0; i < pc1; ++i)
    {
        if(solid)
            rasterNarrowSolidLine_F(clipped[i].x(), clipped[i].y(),
                                    clipped[i + 1].x(), clipped[i + 1].y(),
                                    _pen.color(), &mask_nnp1);
        else
            rasterNarrowPatternedLine_F(clipped[i].x(), clipped[i].y(),
                                        clipped[i + 1].x(), clipped[i + 1].y(),
                                        _pen.color(), fpiCtrInOut, &mask_nnp1);
    }
}


void Rasterizer2::fillPolygon(const PointF* ps, std::size_t n)
{
    Pt::int32_t minX =  MAXIMUM_COORD;
    Pt::int32_t minY =  MAXIMUM_COORD;
    Pt::int32_t maxX = -MAXIMUM_COORD;
    Pt::int32_t maxY = -MAXIMUM_COORD;

    std::vector<PointF> clippedPolygon(ps, ps + n);

    BasicClipShape<double>::clipPolygon(clippedPolygon, _currentClip);

    for(size_t j = 0; j < clippedPolygon.size(); ++j)
    {
        const double x = clippedPolygon[j].x();
        const double y = clippedPolygon[j].y();

        if(x < minX) minX = x;
        if(y < minY) minY = y;
        if(x > maxX) maxX = x;
        if(y > maxY) maxY = y;
    }

    if(_isGradient)
        updateGradientBrush(maxX - minX + 1, maxY - minY + 1);

    if( this->isAntiAliasing() )
    {
        rasterPolygonXWAA(&clippedPolygon[0], clippedPolygon.size(),
                           _brush.color(), minX, minY, maxX, maxY);
    }
    else
    {
        rasterPolygonNoAA(&clippedPolygon[0], clippedPolygon.size(),
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

        BasicClipShape<double>::clipPolygon(polygon.points(), _currentClip);

        for(size_t j = 0; j < polygon.size(); ++j)
        {
            const double x = polygon.at(j).x();
            const double y = polygon.at(j).y();

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
    const Point tl( Pt::lround(rect.topLeft().x()),
                    Pt::lround(rect.topLeft().y()) );
    const Point br( Pt::lround(rect.bottomRight().x()),
                    Pt::lround(rect.bottomRight().y()) );

    // Update the gradient as needed
    if(_isGradient)
        updateGradientBrush(br.x() - tl.x() + 1, br.y() - tl.y() + 1);

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
    const Point tl( Pt::lround(topLeft.x()),
                    Pt::lround(topLeft.y ()) );
    const Size  sz( Pt::lround(size.width()),
                    Pt::lround(size.height()) );

    fillEllipse(tl, sz);
}


void Rasterizer2::fillPie(const PointF& topLeft, const SizeF& size,
                          float degBegin, float degEnd)
{
     Point tl( Pt::lround(topLeft.x()),
               Pt::lround(topLeft.y()) );
     Size  sz( Pt::lround(size.width()),
               Pt::lround(size.height()) );

     rasterArcArea(tl, sz, degBegin, degEnd, ArcMode::Pie);
}


void Rasterizer2::fillChord( const PointF& topLeft, const SizeF& size,
                             float degBegin, float degEnd)
{
    const Point tl( Pt::lround(topLeft.x()),
                    Pt::lround(topLeft.y()) );
    const Size  sz( Pt::lround(size.width()),
                    Pt::lround(size.height()) );

     rasterArcArea(tl, sz, degBegin, degEnd, ArcMode::Chord);
}


void Rasterizer2::fillPath(const Path& path, float smoothness)
{
    std::vector<Polygon> polygons;
    path.toPolygons(polygons, smoothness);

    fillPolygons(polygons);
    return;
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
    Pixel pixel(_image->view(), minX + iterL, minY + pixelY);
    _image->format().setPixels(pixel, color, iterR - iterL + 1, _compositionMode);

    //Pt::int32_t iterX     = iterL;
    //Pt::int32_t spanWidth = iterR - iterL + 1;
    //while(spanWidth > 0) {
    //    const Pt::int32_t n = std::min<Pt::int32_t>(_brushBuffer.width(), spanWidth);
    //    if(n) {
    //        Pixel pixel(_image->view(), minX + iterX, minY + pixelY);
    //        _image->format().copy(pixel, _brushPixel, n, _compositionMode);
    //    }
    //    spanWidth -= n;
    //    iterX     += n;
    //}
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
