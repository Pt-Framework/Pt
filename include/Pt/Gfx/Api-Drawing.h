/* Copyright (C) 2015 Marc Boris Duerner 
  
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

#ifndef PT_GFX_API_DRAWING_H
#define PT_GFX_API_DRAWING_H

/** @defgroup Pt-Gfx-Drawing Drawing Graphics and Text

    @brief Rendering 2D shapes, text and images on paint surfaces.

    The %Pt::Gfx drawing API provides the abstractions needed to render 2D
    content on different paint surfaces. A %Painter can be used
    to draw lines, rectangles, ellipses, paths, text, images and bitmaps on
    paint surfaces such as %Bitmap. %Pen, %Brush, %Font, %Transform and
    %CompositionMode control how shapes, images and text are rendered.

    Painting always targets a %PaintSurface via a %PaintContext, which represents an
    active painting scope. The %Canvas is used by %Painter to translate the
    high-level drawing commands into paint surface operations. %Bitmap is
    the standard in-memory target and makes rendered content available as an
    image when pixels need to be inspected, copied or reused later.

    Paths, transforms and scaling describe geometry independently from the
    underlying backend. Text drawing uses %Font together with %FontMetrics and
    %TextMetrics so that layout and rendering can share the same font request
    and measurement model.

    The drawing API focuses on rendering operations and paint state. Image,
    view and pixel classes in %Pt::Gfx focus on image storage, format handling
    and direct pixel access.
*/

#endif
