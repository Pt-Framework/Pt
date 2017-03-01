/* Copyright (C) 2017-2017 Aloysius Indrayanto
   Copyright (C) 2006-2015 Marc Boris Duerner

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


namespace Pt {
namespace Gfx {


// ======================================================================================
// ===== Public Member Functions ========================================================
// ======================================================================================

void Rasterizer2::strokeOnePixelPolybezier(const Point* points, size_t pointCount)
{
    // Mask
    DrawLineMask mask_zero = Rasterizer2::NullLineMask;
    DrawLineMask mask_nnp1 = Rasterizer2::NullLineMask;

    // Counter for pattern buffer
    Pt::int32_t fpiCtrInOut = 0;

    // Draw the curves
    for(size_t i = 0; i < (pointCount - 1); i += 2) {
        // Adjust the current mask for the last curve as needed
        if(i == pointCount - 3) {
            // Rearrange
            mask_zero[2] = mask_zero[0];
            mask_zero[3] = mask_zero[1];
            mask_zero[0] = mask_nnp1[2];
            mask_zero[1] = mask_nnp1[3];
            // Swap
            mask_nnp1[0] = mask_zero[0];
            mask_nnp1[1] = mask_zero[1];
            mask_nnp1[2] = mask_zero[2];
            mask_nnp1[3] = mask_zero[3];
        }
        // Draw one curve
        if(_pen.style() == Pen::Solid) {
            rasterOnePixelSolidBezierCurve(
                points[i    ].x(), points[i    ].y(),
                points[i + 1].x(), points[i + 1].y(),
                points[i + 2].x(), points[i + 2].y(),
                _pen.color(), &mask_nnp1
            );
        }
        else {
            rasterOnePixelPatternedBezierCurve(
                points[i    ].x(), points[i    ].y(),
                points[i + 1].x(), points[i + 1].y(),
                points[i + 2].x(), points[i + 2].y(),
                _pen.color(), fpiCtrInOut, &mask_nnp1
            );
        }
        // Save the zeroth mask
        if(!i) memcpy(&mask_zero, &mask_nnp1, sizeof(mask_zero));
    }
}


// ======================================================================================
// ===== Private Member Functions =======================================================
// ======================================================================================

// Based on: The Beauty of Bresenham's Algorithm
//           http://members.chello.at/easyfilter/bresenham.html
//           Original code by Alois Zingl, 2016.
void Rasterizer2::rasterOnePixelSolidBezierCurve(Pt::int32_t x1, Pt::int32_t y1, Pt::int32_t x2, Pt::int32_t y2, Pt::int32_t x3, Pt::int32_t y3, const Color& color, DrawLineMask* maskInOut)
{
    // Get the mask's coordinates as needed
    Pt::int32_t mx[4] = { MAXIMUM_COORD, MAXIMUM_COORD, MAXIMUM_COORD, MAXIMUM_COORD };
    Pt::int32_t my[4] = { MAXIMUM_COORD, MAXIMUM_COORD, MAXIMUM_COORD, MAXIMUM_COORD };

    if(maskInOut) {
        for(Pt::int32_t i = 0; i < 4; ++i) {
            mx[i] = (*maskInOut)[i].x();
            my[i] = (*maskInOut)[i].y();
        }
    }

    // A helper macro to set pixel
    #define XW_SET_PIXEL(IMG, COL, X, Y, A)                                       \
        do {                                                                      \
            /* Clip the point */                                                  \
            if( (X) < _currentClip.left() || (X) > _currentClip.right () ||       \
                (Y) < _currentClip.top () || (Y) > _currentClip.bottom() ) break; \
            /* Check if we should skip drawing the pixel */                       \
            bool skipDrawing = false;                                             \
            for(Pt::int32_t j = 0; j < 4; ++j) {                                  \
                if( (X) != mx[j] || (Y) != my[j] ) continue;                      \
                skipDrawing = true;                                               \
                break;                                                            \
            }                                                                     \
            if(skipDrawing) break;                                                \
            /* Set the pixel */                                                   \
            Pixel PIX(IMG->view(), X, Y);                                         \
            if(A == 255)                                                          \
                IMG->format().setPixel(PIX, COL, _compositionMode);               \
            else {                                                                \
                const Pt::uint8_t falpha = Rasterizer2::XWAA_WFILTER[A];          \
                IMG->format().setPixel(PIX, COL, _compositionMode, falpha);       \
            }                                                                     \
        } while(false)

    // Use anti-aliasing?
    const bool useAA = (_aaMode != AntiAliasingMode::None);

    // Get the steps
    Pt::int32_t sx = x3 - x2;
    Pt::int32_t sy = y3 - y2;

    // Relative values for checks
    Pt::int32_t xx = x1 - x2;
    Pt::int32_t yy = y1 - y2;
    Pt::int32_t xy;

    // Curvature
    float dx, dy, ed, err;
    float cur = xx * sy - yy * sx;

    // Sign of gradient must not change
    if(xx * sx > 0 || yy * sy > 0) return;

    // Begin with longer part; swap the begin and end points as needed
    if(sx * sx + sy * sy > xx * xx + yy * yy) {
        x3  = x1;
        y3  = y1;
        x1  = sx + x2;
        y1  = sy + y2;
        cur = -cur;
    }

    // Check if the curve is actually a straight line
    if(!cur) {
        rasterOnePixelSolidLine(x1, y1, x3, y3, color, maskInOut);
        return;
    }

    // X step direction
    xx += sx;
    sx  = x1 < x3 ? 1 : -1;
    xx *= sx;

    // Y step direction
    yy += sy;
    sy  = y1 < y3 ? 1 : -1;
    yy *= sy;

    // Differences 2nd degree
    xy  = 2 * xx * yy;
    xx *= xx;
    yy *= yy;

    // Negated curvature?
    if(cur * sx * sy < 0) {
      xx  = -xx;
      yy  = -yy;
      xy  = -xy;
      cur = -cur;
    }

    // Differences 1st degree
    dx = 4 * sy * cur * (x2 - x1) + xx - xy;
    dy = 4 * sx * cur * (y1 - y2) + yy - xy;

    // Error 1st step
    xx  += xx;
    yy  += yy;
    err  = dx + dy + xy;

    // Draw with anti-aliasing
    if(useAA) {
        Pt::uint8_t alpha;
        bool        firstPixel0 = true;
        bool        firstPixel1 = true;
        do {
            // Approximate the error distance
            cur = std::min(dx + xy, -xy - dy);
            ed  = std::max(dx + xy, -xy - dy);
            ed  = (ed + 2 * ed * cur * cur / (4 * ed * ed + cur * cur));
            // Plot curve
            alpha = 255 / ed * ::fabs(err - dx - dy - xy);
            XW_SET_PIXEL(_image, color, x1, y1, alpha);
            if(maskInOut) {
                if(firstPixel0) {
                    (*maskInOut)[0].set(x1, y1);
                    firstPixel0 = false;
                }
                (*maskInOut)[2].set(x1, y1);
            }
            // Check if we have just drawn the last pixel
            if(x1 == x3 && y1 == y3) return;
            x2  = x1;
            cur = dx - err;
            y2  = (2 * err + dy) < 0 ? 1 : 0;
            // X step
            if(2 * err + dx > 0) {
                // Plot curve
                if(err - dy < ed) {
                    // Set pixel
                    alpha = 255 / ed * ::fabs(err - dy);
                    XW_SET_PIXEL(_image, color, x1, y1 + sy, alpha);
                    // Store back the start and end coordinates to the mask as needed
                    if(maskInOut) {
                        if(firstPixel1) {
                            (*maskInOut)[1].set(x1, y1 + sy);
                            firstPixel1 = false;
                        }
                        (*maskInOut)[3].set(x1, y1 + sy);
                    }
                }
                // X step
                x1  += sx;
                dx  -= xy;
                dy  += yy;
                err += dy;
            }
            // Y step
            if(y2) {
                // Plot curve
                if(cur < ed) {
                    // Set pixel
                    alpha = 255 / ed * ::fabs(cur);
                    XW_SET_PIXEL(_image, color, x2 + sx, y1, alpha);
                    // Store back the start and end coordinates to the mask as needed
                    if(maskInOut) {
                        if(firstPixel1) {
                            (*maskInOut)[1].set(x1, y1 + sy);
                            firstPixel1 = false;
                        }
                        (*maskInOut)[3].set(x1, y1 + sy);
                    }
                }
                // Y step
                y1  += sy;
                dy  -= xy;
                dx  += xx;
                err += dx;
            }
        } while(dy < dx); // Done if the gradient negates itself
    }

    // Draw without anti-aliasing
    else {
        // Store back the start and end coordinates to the mask as needed
        if(maskInOut) {
            (*maskInOut)[0].set(x1, y1);
            (*maskInOut)[1] = MAXIMUM_POINT;
            (*maskInOut)[2].set(x3, y3);
            (*maskInOut)[3] = MAXIMUM_POINT;
        }
        // Draw the pixels
        do {
            // Plot curve
            XW_SET_PIXEL(_image, color, x1, y1, 255);
            // Check if we have just drawn the last pixel
            if(x1 == x3 && y1 == y3) {
                return;
            }
            // Save value for test of Y step
            y2 = (2 * err < dx) ? 1 : 0;
            // X step
            if(2 * err > dy) {
                x1  += sx;
                dx  -= xy;
                dy  += yy;
                err += dy;
            }
            // Y step
            if(y2) {
                y1  += sy;
                dy  -= xy;
                dx  += xx;
                err += dx;
            }
        } while(dy < dx); // Done if the gradient negates itself
    }

    // Undefine the helper macro
    #undef XW_SET_PIXEL
}

// Based on: The Beauty of Bresenham's Algorithm
//           http://members.chello.at/easyfilter/bresenham.html
//           Original code by Alois Zingl, 2016.
void Rasterizer2::rasterOnePixelPatternedBezierCurve(Pt::int32_t x1, Pt::int32_t y1, Pt::int32_t x2, Pt::int32_t y2, Pt::int32_t x3, Pt::int32_t y3, const Color& color, Pt::int32_t& fpiCtrInOut, DrawLineMask* maskInOut)
{
    // Get the mask's coordinates as needed
    Pt::int32_t mx[4] = { MAXIMUM_COORD, MAXIMUM_COORD, MAXIMUM_COORD, MAXIMUM_COORD };
    Pt::int32_t my[4] = { MAXIMUM_COORD, MAXIMUM_COORD, MAXIMUM_COORD, MAXIMUM_COORD };

    if(maskInOut) {
        for(Pt::int32_t i = 0; i < 4; ++i) {
            mx[i] = (*maskInOut)[i].x();
            my[i] = (*maskInOut)[i].y();
        }
    }

    // A helper macro to set pixel
    #define XW_SET_PIXEL(IMG, COL, X, Y, A, PA)                                   \
        do {                                                                      \
            /* Clip the point */                                                  \
            if( (X) < _currentClip.left() || (X) > _currentClip.right () ||       \
                (Y) < _currentClip.top () || (Y) > _currentClip.bottom() ) break; \
            /* Check if we should skip drawing the pixel */                       \
            bool skipDrawing = false;                                             \
            for(Pt::int32_t j = 0; j < 4; ++j) {                                  \
                if( (X) != mx[j] || (Y) != my[j] ) continue;                      \
                skipDrawing = true;                                               \
                break;                                                            \
            }                                                                     \
            if(skipDrawing) break;                                                \
            /* Combine the alpha */                                               \
            const Pt::uint8_t falpha = Rasterizer2::XWAA_WFILTER[A];              \
            const Pt::uint8_t calpha = (Pt::uint32_t) falpha * (PA) / 255;        \
            /* Set the pixel */                                                   \
            Pixel PIX(IMG->view(), X, Y);                                         \
            IMG->format().setPixel(PIX, COL, _compositionMode, calpha);           \
        } while(false)

    // Use anti-aliasing?
    const bool useAA = (_aaMode != AntiAliasingMode::None);

    // Get the steps
    Pt::int32_t sx = x3 - x2;
    Pt::int32_t sy = y3 - y2;

    // Relative values for checks
    Pt::int32_t xx = x1 - x2;
    Pt::int32_t yy = y1 - y2;
    Pt::int32_t xy;

    // Curvature
    float dx, dy, ed, err;
    float cur = xx * sy - yy * sx;

    // Sign of gradient must not change
    if(xx * sx > 0 || yy * sy > 0) return;

    // Begin with longer part; swap the begin and end points as needed
    if(sx * sx + sy * sy > xx * xx + yy * yy) {
        x3  = x1;
        y3  = y1;
        x1  = sx + x2;
        y1  = sy + y2;
        cur = -cur;
    }

    // Check if the curve is actually a straight line
    if(!cur) {
        rasterOnePixelPatternedLine(x1, y1, x3, y3, color, fpiCtrInOut, maskInOut);
        return;
    }

    // X step direction
    xx += sx;
    sx  = x1 < x3 ? 1 : -1;
    xx *= sx;

    // Y step direction
    yy += sy;
    sy  = y1 < y3 ? 1 : -1;
    yy *= sy;

    // Differences 2nd degree
    xy  = 2 * xx * yy;
    xx *= xx;
    yy *= yy;

    // Negated curvature?
    if(cur * sx * sy < 0) {
      xx  = -xx;
      yy  = -yy;
      xy  = -xy;
      cur = -cur;
    }

    // Differences 1st degree
    dx = 4 * sy * cur * (x2 - x1) + xx - xy;
    dy = 4 * sx * cur * (y1 - y2) + yy - xy;

    // Error 1st step
    xx  += xx;
    yy  += yy;
    err  = dx + dy + xy;

    // For now, fix the increment
    const Pt::int32_t fpiCtrInc = FIXED_POINT_FROM_INT(1);

    // Draw with anti-aliasing
    if(useAA) {
        Pt::uint8_t alpha;
        bool        firstPixel0 = true;
        bool        firstPixel1 = true;
        do {
            // Get alpha from the pattern
            Pt::uint8_t patAlpha = _patternBuffer[FIXED_POINT_TO_INT(fpiCtrInOut)];
            fpiCtrInOut += fpiCtrInc;
            if(fpiCtrInOut > _fpatternMaxCtr) fpiCtrInOut = 0;
            // Approximate the error distance
            cur = std::min(dx + xy, -xy - dy);
            ed  = std::max(dx + xy, -xy - dy);
            ed  = (ed + 2 * ed * cur * cur / (4 * ed * ed + cur * cur));
            // Plot curve
            alpha = 255 / ed * ::fabs(err - dx - dy - xy);
            XW_SET_PIXEL(_image, color, x1, y1, alpha, patAlpha);
            if(maskInOut) {
                if(firstPixel0) {
                    (*maskInOut)[0].set(x1, y1);
                    firstPixel0 = false;
                }
                (*maskInOut)[2].set(x1, y1);
            }
            // Check if we have just drawn the last pixel
            if(x1 == x3 && y1 == y3) return;
            x2  = x1;
            cur = dx - err;
            y2  = (2 * err + dy) < 0 ? 1 : 0;
            // X step
            if(2 * err + dx > 0) {
                // Plot curve
                if(err - dy < ed) {
                    // Set pixel
                    alpha = 255 / ed * ::fabs(err - dy);
                    XW_SET_PIXEL(_image, color, x1, y1 + sy, alpha, patAlpha);
                    // Store back the start and end coordinates to the mask as needed
                    if(maskInOut) {
                        if(firstPixel1) {
                            (*maskInOut)[1].set(x1, y1 + sy);
                            firstPixel1 = false;
                        }
                        (*maskInOut)[3].set(x1, y1 + sy);
                    }
                }
                // X step
                x1  += sx;
                dx  -= xy;
                dy  += yy;
                err += dy;
            }
            // Y step
            if(y2) {
                // Plot curve
                if(cur < ed) {
                    // Set pixel
                    alpha = 255 / ed * ::fabs(cur);
                    XW_SET_PIXEL(_image, color, x2 + sx, y1, alpha, patAlpha);
                    // Store back the start and end coordinates to the mask as needed
                    if(maskInOut) {
                        if(firstPixel1) {
                            (*maskInOut)[1].set(x1, y1 + sy);
                            firstPixel1 = false;
                        }
                        (*maskInOut)[3].set(x1, y1 + sy);
                    }
                }
                // Y step
                y1  += sy;
                dy  -= xy;
                dx  += xx;
                err += dx;
            }
        } while(dy < dx); // Done if the gradient negates itself
    }

    // Draw without anti-aliasing
    else {
        // Store back the start and end coordinates to the mask as needed
        if(maskInOut) {
            (*maskInOut)[0].set(x1, y1);
            (*maskInOut)[1] = MAXIMUM_POINT;
            (*maskInOut)[2].set(x3, y3);
            (*maskInOut)[3] = MAXIMUM_POINT;
        }
        // Draw the pixels
        do {
            // Get alpha from the pattern
            Pt::uint8_t patAlpha = _patternBuffer[FIXED_POINT_TO_INT(fpiCtrInOut)];
            fpiCtrInOut += fpiCtrInc;
            if(fpiCtrInOut > _fpatternMaxCtr) fpiCtrInOut = 0;
            // Plot curve
            XW_SET_PIXEL(_image, color, x1, y1, 255, patAlpha);
            // Check if we have just drawn the last pixel
            if(x1 == x3 && y1 == y3) {
                return;
            }
            // Save value for test of Y step
            y2 = (2 * err < dx) ? 1 : 0;
            // X step
            if(2 * err > dy) {
                x1  += sx;
                dx  -= xy;
                dy  += yy;
                err += dy;
            }
            // Y step
            if(y2) {
                y1  += sy;
                dy  -= xy;
                dx  += xx;
                err += dx;
            }
        } while(dy < dx); // Done if the gradient negates itself
    }

    // Undefine the helper macro
    #undef XW_SET_PIXEL
}


} // namespace
} // namespace
