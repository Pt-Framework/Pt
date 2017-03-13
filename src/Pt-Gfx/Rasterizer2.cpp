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

#include "DrawText2.h"
#include "Rasterizer2.h"


namespace Pt {
namespace Gfx {


// ======================================================================================
// ===== Static Public Member Functions =================================================
// ======================================================================================

// Weighting filter for Xiaolin Wu's anti-aliasing algorithm
// Inspired by http://www.crbond.com/papers/anti_alias.pdf
// y = 1.0 - pow( (x / 255.0), 1.88 )
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
/*
// Use this code to regenerate the above LUT
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

const Rasterizer2::DrawLineMask Rasterizer2::NullLineMask = {
    MAXIMUM_POINT, MAXIMUM_POINT, MAXIMUM_POINT, MAXIMUM_POINT
};

FontMetrics Rasterizer2::fontMetrics( const Font& font, const Pt::String& text )
{
    DrawText2 textRender;
    textRender.setFont(font);

    return textRender.fontMetrics(text);
}


// ======================================================================================
// ===== Public Member Functions ========================================================
// ======================================================================================

Rasterizer2::Rasterizer2(Image& image)
: _image          ( &image )
, _text           ( new DrawText2() )
, _font           ( )
, _compositionMode( CompositionMode::SourceCopy )
, _penPixel       ( _image->view(), 0, 0 )
, _brushPixel     ( _image->view(), 0, 0 )
{
    _text->setFont(_font);
    updateClip();
}

Rasterizer2::~Rasterizer2()
{
    delete _text;
}

void Rasterizer2::setImage( Image& image )
{
    _image = &image;
    _brushBuffer.reset(_image->format(), _brushBuffer.size());
    updateClip();
}

const ImageFormat& Rasterizer2::format() const
{ return _image->format(); }

void Rasterizer2::setPen( const Pen& pen )
{
    _pen = pen;
    _penBuffer.reset(_image->format(), Size(64, 1));
    Gfx::fill(_penBuffer.begin(), _penBuffer.end(), pen.color());

    _penPixel.reset(_penBuffer.view(), 0, 0);

    updatePenPattern();
}

void Rasterizer2::setBrush( const Brush& brush )
{
    _brush      = brush;
    _isGradient = false;
    _isTexture  = false;

    switch( brush.fillStyle() ) {
        case Brush::Solid:
            _brushBuffer.reset( _image->format(), Size(64, 1) );
            Gfx::fill(_brushBuffer.begin(), _brushBuffer.end(), brush.color());
            _brushImage = &_brushBuffer;
            break;

        case Brush::Texture:
            if( brush.texture().format() != _image->format() ) {
                _brushBuffer.reset( _image->format(), brush.texture().size() );
                Gfx::copy( brush.texture().begin(), brush.texture().end(), _brushBuffer.begin() );
                _brushImage = &_brushBuffer;
            }
            else {
                _brushImage = &_brush.texture();
            }
            _isTexture = true;
            break;

        case Brush::HorizontalGradient:
        case Brush::VerticalGradient:
            _isGradient = true;
            _brushImage = &_brushBuffer;
            break;
    }

    _brushPixel.reset(_brushImage->view(), 0, 0);
}

void Rasterizer2::setFont(const Font& font)
{
    _font = font;
    _text->setFont(_font);
}

FontMetrics Rasterizer2::fontMetrics( const String& text ) const
{
    return _text->fontMetrics( text );
}

void Rasterizer2::setClip( const Rect& clip )
{
    _clip = clip;
    updateClip();
}

void Rasterizer2::image(const Point& to, const Image& img)
{
    const Rect imageRect( Point(0,0), img.size() );
    image( to, img, imageRect );
}

void Rasterizer2::image(const Point& to, const Image& from, const Rect& fromRect)
{
    // Clip fromRect to fit into the clip/image rect
    const Point d       = _currentClip.topLeft() - to;
    const Point fromPos = fromRect.topLeft() + d;

    Rect fromClip(fromPos, _currentClip.size());
    fromClip = fromRect.intersect(fromClip);

    if( fromClip.isNull() ) return;

    // Take account for smaller fromRect
    const Point toClip = to + (fromClip.topLeft() - fromRect.topLeft());

    _image->format().copy(_image->view(), toClip, from.view(), fromClip, _compositionMode);
}

void Rasterizer2::strokeText( const Point& to, const Pt::String& text )
{
    _text->setClip(_currentClip);

    if(_aaMode == AntiAliasingMode::None)
        _text->drawMono( *_image, _pen.color(), to, text, _compositionMode );
    else
        _text->draw( *_image, _pen.color(), to, text, _compositionMode );
}


// ======================================================================================
// ===== Private Member Functions =======================================================
// ======================================================================================

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

    switch(_pen.style()) {
        default:
        case Pen::Dot         : patternSel = patternDot;              break;
        case Pen::DoubleDot   : patternSel = patternDoubleDot;        break;
        case Pen::Dash        : patternSel = patternDash;             break;
        case Pen::DoubleDash  : patternSel = patternDoubleDash;       break;
        case Pen::DotDash     : patternSel = patternDotDash;          break;
        case Pen::UserDefined : patternSel = _pen.styleUserPattern(); break;
    }

    // Counter for generating the pattern
    size_t gctr = 0;

    // Generate the pattern
    bool previous = 0;
    for(Pt::int8_t p = 0; p < 64; ++p) { // The pattern has 64 points
        // Get the pattern cell value
        const bool current = patternSel & ((Pt::uint64_t) 1 << p);
        // Pattern cell change from 0 to 0
        if(!previous && !current) {
            for(Pt::uint8_t i = 1; i <= PATTERN_BUFFER_SCALE_FACTOR; ++i) {
                _patternBuffer[gctr++] = 0;
            }
        }
        // Pattern cell change from 1 to 1
        else if(previous && current) {
            for(Pt::uint8_t i = 1; i <= PATTERN_BUFFER_SCALE_FACTOR; ++i) {
                _patternBuffer[gctr++] = 255;
            }
        }
        // Pattern cell change from 0 to 1
        else if(!previous && current) {
            for(Pt::int32_t i = 1; i <= PATTERN_BUFFER_SCALE_FACTOR; ++i) {
                _patternBuffer[gctr++] = i * 255 / PATTERN_BUFFER_SCALE_FACTOR;
            }
        }
        // Pattern cell change from 1 to 0
        else if(previous && !current) {
            for(Pt::int32_t i = 1; i <= PATTERN_BUFFER_SCALE_FACTOR; ++i) {
                _patternBuffer[gctr++] = 255 - i * 255 / PATTERN_BUFFER_SCALE_FACTOR;
            }
        }
        // Copy the pattern cell value
        previous = current;
    }

    // Transfom the pattern - without anti-aliasing
    if(_aaMode == AntiAliasingMode::None) {
        for(size_t i = 0; i < gctr; ++i) {
            if(_patternBuffer[i] > 127) _patternBuffer[i] = 255;
            else                        _patternBuffer[i] = 0;
        }
    }

    // Transfom the pattern - with anti-aliasing
    else {
        for(size_t i = 0; i < gctr; ++i) {
            _patternBuffer[i] = XWAA_WFILTER[ 255 - _patternBuffer[i] ];
        }
    }

    // Store the maximum value of the counter in fixed-point
    _fpatternMaxCtr = FIXED_POINT_FROM_INT(gctr);
}

void Rasterizer2::updateGradientBrush(Pt::int32_t width, Pt::int32_t height)
{
    // Start colors
    Pt::uint8_t rs, gs, bs, as;

    // End colors
    Pt::uint8_t re, ge, be, ae;

    // Resize the brush buffer and the start-end colors
    switch(_brush.fillStyle()) {
        case Pt::Gfx::Brush::HorizontalGradient:
            // Resize the brush buffer
            height = 1;
            _brushBuffer.reset(_image->format(), Size(width, 1));
            // Determine the colors
            rs = _brush.color        ().red  () / 257;
            gs = _brush.color        ().green() / 257;
            bs = _brush.color        ().blue () / 257;
            as = _brush.color        ().alpha() / 257;
            re = _brush.gradientColor().red  () / 257;
            ge = _brush.gradientColor().green() / 257;
            be = _brush.gradientColor().blue () / 257;
            ae = _brush.gradientColor().alpha() / 257;
            break;

        case Pt::Gfx::Brush::VerticalGradient:
            // Resize the brush buffer
            width = 1;
            _brushBuffer.reset(_image->format(), Size(1, height));
            // Determine the colors
            rs = _brush.gradientColor().red  () / 257;
            gs = _brush.gradientColor().green() / 257;
            bs = _brush.gradientColor().blue () / 257;
            as = _brush.gradientColor().alpha() / 257;
            re = _brush.color        ().red  () / 257;
            ge = _brush.color        ().green() / 257;
            be = _brush.color        ().blue () / 257;
            ae = _brush.color        ().alpha() / 257;
            break;

        default:
            return;
    }

    // Create the gradient
    const Pt::int32_t  length = width + height - 1 - 1;
          Pt::uint8_t* pixel  = _brushBuffer.data();
    for(int n = 0; n <= length; ++n) {
        const Pt::int32_t f2 = FIXED_POINT_FROM_INT(n) / length;
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

void Rasterizer2::updateClip()
{
    const Rect imageRect( Point(0,0) , _image->size() );
    _currentClip = _clip.isNull() ? imageRect : _clip.intersect( imageRect );

    _text->setClip(_currentClip);
}


} // namespace
} // namespace
