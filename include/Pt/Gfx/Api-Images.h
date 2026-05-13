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

#ifndef PT_GFX_API_IMAGES_H
#define PT_GFX_API_IMAGES_H

/** @defgroup Pt-Gfx-Images Images and Image Views

    @brief Images, image formats and image views.

    The %Pt::Gfx image API provides classes for storing image data, accessing
    pixels and working with image formats. Images own their pixel data and
    support a range of pixel formats such as %Argb32, %Rgb32, %Rgb16 and
    %Yuv12. Views and pixel views provide lightweight, non-owning access to
    regions of image data without copying.

    Each pixel format has a concrete pixel type for direct, format-aware pixel
    access and a generic pixel type for runtime-polymorphic access via
    %ImageFormat.
*/

#endif
