/* Copyright (C) 2015-2026 Marc Boris Duerner 
  
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

#include "CursorImpl.h"

#include <cstring>

namespace Pt {
namespace Forms {

CursorImpl::CursorImpl()
: _theme(0)
, _cursorSurface(0)
, _pointer(0)
, _shm(0)
{
}


CursorImpl::~CursorImpl()
{
    destroy();
}


void CursorImpl::destroy()
{
    if( _cursorSurface )
    {
        wl_surface_destroy(_cursorSurface);
        _cursorSurface = 0;
    }

    if( _theme )
    {
        wl_cursor_theme_destroy(_theme);
        _theme = 0;
    }
}


void CursorImpl::init(struct wl_shm* shm, struct wl_pointer* pointer,
                      struct wl_compositor* compositor)
{
    _shm = shm;
    _pointer = pointer;

    if( _shm )
    {
        _theme = wl_cursor_theme_load(0, 24, _shm);
    }

    if( compositor )
    {
        _cursorSurface = wl_compositor_create_surface(compositor);
    }
}


void CursorImpl::setPointer(struct wl_pointer* pointer)
{
    _pointer = pointer;
}


void CursorImpl::setCursor(const Cursor* cursor, uint32_t serial)
{
    if( ! _pointer || ! _theme || ! _shm )
        return;

    const char* name = cursorName(cursor);

    struct wl_cursor* wlCursor = wl_cursor_theme_get_cursor(_theme, name);
    if( ! wlCursor )
        wlCursor = wl_cursor_theme_get_cursor(_theme, "left_ptr");

    if( ! wlCursor || wlCursor->image_count == 0 )
        return;

    struct wl_cursor_image* image = wlCursor->images[0];
    struct wl_buffer* buffer = wl_cursor_image_get_buffer(image);
    if( ! buffer )
        return;

    if( ! _cursorSurface )
        return;

    wl_pointer_set_cursor(_pointer, serial, _cursorSurface,
                          static_cast<int32_t>(image->hotspot_x),
                          static_cast<int32_t>(image->hotspot_y));

    wl_surface_attach(_cursorSurface, buffer, 0, 0);
    wl_surface_damage(_cursorSurface, 0, 0,
                      static_cast<int32_t>(image->width),
                      static_cast<int32_t>(image->height));
    wl_surface_commit(_cursorSurface);
}


void CursorImpl::show(uint32_t serial)
{
    setCursor(&Cursor::arrowCursor(), serial);
}


const char* CursorImpl::cursorName(const Cursor* cursor) const
{
    if( ! cursor )
        return "left_ptr";

    const std::string& name = cursor->name();

    if( name == "arrow" || name.empty() )
        return "left_ptr";
    else if( name == "wait" )
        return "watch";
    else if( name == "sizeNWSE" )
        return "nw-resize";
    else if( name == "sizeNESW" )
        return "ne-resize";
    else if( name == "sizeWE" )
        return "ew-resize";
    else if( name == "sizeNS" )
        return "ns-resize";
    else if( name == "move" )
        return "fleur";

    return "left_ptr";
}

} // namespace Forms
} // namespace Pt
