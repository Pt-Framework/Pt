/* Copyright (C) 2026 Marc Boris Duerner

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
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
  MA 02110-1301 USA
*/

#ifndef PT_FORMS_PAINT_COMMAND_H
#define PT_FORMS_PAINT_COMMAND_H

#include <Pt/Gfx/Brush.h>
#include <Pt/Gfx/Image.h>
#include <Pt/Gfx/Path.h>
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Rect.h>
#include <Pt/Gfx/Size.h>
#include <Pt/Gfx/Transform.h>
#include <Pt/String.h>

#include "nanovg.h"

#include <string>
#include <vector>

namespace Pt {

namespace Forms {

// One entry per Canvas operation recorded by PixmapCanvas. State commands
// snapshot only the small resolved nanovg parameters. Draw commands own
// copies of all variable-length payloads (points, path, image, text).
// The active type determines which fields are valid.
struct PaintCommand
{
    enum Type
    {
        // state
        SetTransform,
        SetCompositionMode,
        SetClip,
        SetPen,
        SetBrush,
        SetFont,

        // geometry
        DrawLine,
        DrawPolyline,
        FillPolygon,
        DrawRect,
        FillRect,
        DrawEllipse,
        FillEllipse,
        DrawPath,
        FillPath,

        // rich content
        DrawText,
        DrawImage,
        DrawPixmap
    };

    Type type;

    // --- SetTransform ---
    Gfx::Transform    transform;

    // --- SetCompositionMode ---
    int               compositionMode;  // NVG_* constant

    // --- SetClip ---
    bool              hasClip;
    Gfx::RectF        clip;

    // --- SetPen ---
    NVGcolor          penColor;
    float             penWidth;
    int               lineCap;          // NVG_BUTT / NVG_ROUND / NVG_SQUARE
    int               lineJoin;         // NVG_MITER / NVG_ROUND / NVG_BEVEL

    // --- SetBrush (owned; used by fill commands via applyFillForBounds) ---
    Gfx::Brush        brush;

    // --- SetFont ---
    int               fontFace;         // nanovg font handle
    float             fontSize;

    // --- DrawLine ---
    Gfx::PointF       p0;               // from / topLeft / origin
    Gfx::PointF       p1;               // to

    // --- DrawRect / FillRect ---
    Gfx::RectF        rect;

    // --- DrawEllipse / FillEllipse ---
    Gfx::SizeF        ellipseSize;

    // --- DrawPolyline / FillPolygon ---
    std::vector<Gfx::PointF> points;

    // --- DrawPath / FillPath ---
    Gfx::Path         path;

    // --- DrawText ---
    std::string       text;             // UTF-8 pre-encoded
    bool              hasTextTransform;
    Gfx::Transform    textTransform;

    // --- DrawImage ---
    Gfx::Image        image;            // owns pixel data
    bool              hasSrcRect;
    Gfx::RectF        srcRect;

    // --- DrawPixmap (source already flushed at record time) ---
    int               srcPixmapImage;   // nanovg image handle of source pixmap
    int               srcPixmapW;
    int               srcPixmapH;
    // hasSrcRect / srcRect shared with DrawImage

    // Logical destination size for DrawPixmap/DrawImage with srcRect.
    // srcRect holds physical pixel coordinates; destW/destH hold the logical
    // nanovg-unit destination dimensions so the replay can draw at the correct
    // size without double-applying the canvas scale transform.
    float             destW;
    float             destH;


    PaintCommand()
    : type(DrawRect)
    , compositionMode(0)
    , hasClip(false)
    , penColor(nvgRGBA(0, 0, 0, 255))
    , penWidth(1.0f)
    , lineCap(0)
    , lineJoin(0)
    , fontFace(-1)
    , fontSize(12.0f)
    , hasTextTransform(false)
    , hasSrcRect(false)
    , srcPixmapImage(-1)
    , srcPixmapW(0)
    , srcPixmapH(0)
    , destW(0.0f)
    , destH(0.0f)
    {}
};

} // namespace Forms

} // namespace Pt

#endif
