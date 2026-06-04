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

#ifndef Pt_Forms_wayland_KeyHandler_h
#define Pt_Forms_wayland_KeyHandler_h

#include <Pt/Forms/KeyEvent.h>
#include <Pt/Types.h>

#include <xkbcommon/xkbcommon.h>

namespace Pt {
namespace Forms {

class KeyHandler
{
    public:
        KeyHandler();
        ~KeyHandler();

        void updateKeymap(uint32_t format, int fd, uint32_t size);
        void updateModifiers(uint32_t modsDepressed, uint32_t modsLatched,
                             uint32_t modsLocked, uint32_t group);

        KeyEvent translateKey(uint32_t key, uint32_t state);

    private:
        Key::Code xkbSymToKeyCode(xkb_keysym_t sym);

    private:
        struct xkb_context* _context;
        struct xkb_keymap*  _keymap;
        struct xkb_state*   _state;
};

} // namespace Forms
} // namespace Pt

#endif
