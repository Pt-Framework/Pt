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

#include "KeyHandler.h"

#include <Pt/Forms/Key.h>

#include <sys/mman.h>
#include <unistd.h>
#include <cstring>
#include <wayland-client.h>

namespace Pt {
namespace Forms {

KeyHandler::KeyHandler()
: _context( xkb_context_new(XKB_CONTEXT_NO_FLAGS) )
, _keymap(0)
, _state(0)
{
}


KeyHandler::~KeyHandler()
{
    if( _state )
        xkb_state_unref(_state);

    if( _keymap )
        xkb_keymap_unref(_keymap);

    if( _context )
        xkb_context_unref(_context);
}


void KeyHandler::updateKeymap(uint32_t format, int fd, uint32_t size)
{
    if( format != WL_KEYBOARD_KEYMAP_FORMAT_XKB_V1 )
    {
        ::close(fd);
        return;
    }

    char* map = static_cast<char*>(
        mmap(0, size, PROT_READ, MAP_PRIVATE, fd, 0)
    );

    if( map == MAP_FAILED )
    {
        ::close(fd);
        return;
    }

    if( _state )
    {
        xkb_state_unref(_state);
        _state = 0;
    }

    if( _keymap )
    {
        xkb_keymap_unref(_keymap);
        _keymap = 0;
    }

    _keymap = xkb_keymap_new_from_string(_context, map,
                                         XKB_KEYMAP_FORMAT_TEXT_V1,
                                         XKB_KEYMAP_COMPILE_NO_FLAGS);

    munmap(map, size);
    ::close(fd);

    if( _keymap )
        _state = xkb_state_new(_keymap);
}


void KeyHandler::updateModifiers(uint32_t modsDepressed, uint32_t modsLatched,
                                 uint32_t modsLocked, uint32_t group)
{
    if( _state )
    {
        xkb_state_update_mask(_state, modsDepressed, modsLatched,
                              modsLocked, 0, 0, group);
    }
}


KeyEvent KeyHandler::translateKey(uint32_t key, uint32_t state)
{
    KeyEvent ev;

    if( ! _state )
        return ev;

    // Wayland key codes are evdev codes; xkbcommon expects evdev + 8
    xkb_keycode_t keycode = key + 8;
    xkb_keysym_t sym = xkb_state_key_get_one_sym(_state, keycode);

    Key::Code code = xkbSymToKeyCode(sym);

    // Get UTF-32 character
    Pt::Char ch(0);
    char utf8[8] = {};
    int len = xkb_state_key_get_utf8(_state, keycode, utf8, sizeof(utf8));
    if( len > 0 && utf8[0] >= 0x20 )
    {
        // Simple ASCII fast path
        if( static_cast<unsigned char>(utf8[0]) < 0x80 )
            ch = Pt::Char( static_cast<Pt::uint32_t>(utf8[0]) );
        else
            ch = Pt::Char( static_cast<Pt::uint32_t>(code) );
    }

    Key k(code);

    if( state == WL_KEYBOARD_KEY_STATE_PRESSED )
        ev.setPress(k, ch);
    else
        ev.setRelease(k, ch);

    return ev;
}


Key::Code KeyHandler::xkbSymToKeyCode(xkb_keysym_t sym)
{
    // Letters
    if( sym >= XKB_KEY_a && sym <= XKB_KEY_z )
        return static_cast<Key::Code>(Key::A + (sym - XKB_KEY_a));

    if( sym >= XKB_KEY_A && sym <= XKB_KEY_Z )
        return static_cast<Key::Code>(Key::A + (sym - XKB_KEY_A));

    // Digits
    if( sym >= XKB_KEY_0 && sym <= XKB_KEY_9 )
        return static_cast<Key::Code>(Key::D0 + (sym - XKB_KEY_0));

    // Function keys
    if( sym >= XKB_KEY_F1 && sym <= XKB_KEY_F12 )
        return static_cast<Key::Code>(Key::F1 + (sym - XKB_KEY_F1));

    // Numpad
    if( sym >= XKB_KEY_KP_0 && sym <= XKB_KEY_KP_9 )
        return static_cast<Key::Code>(Key::NumPad0 + (sym - XKB_KEY_KP_0));

    switch( sym )
    {
        case XKB_KEY_Return:       return Key::Return;
        case XKB_KEY_KP_Enter:     return Key::Return;
        case XKB_KEY_Escape:       return Key::Escape;
        case XKB_KEY_BackSpace:    return Key::Backspace;
        case XKB_KEY_Tab:          return Key::Tab;
        case XKB_KEY_space:        return Key::Space;

        case XKB_KEY_Left:         return Key::ArrowLeft;
        case XKB_KEY_Right:        return Key::ArrowRight;
        case XKB_KEY_Up:           return Key::ArrowUp;
        case XKB_KEY_Down:         return Key::ArrowDown;

        case XKB_KEY_Shift_L:
        case XKB_KEY_Shift_R:      return Key::ShiftKey;
        case XKB_KEY_Control_L:
        case XKB_KEY_Control_R:    return Key::ControlKey;
        case XKB_KEY_Alt_L:
        case XKB_KEY_Alt_R:        return Key::AltKey;
        case XKB_KEY_Super_L:
        case XKB_KEY_Super_R:      return Key::MetaKey;

        case XKB_KEY_Num_Lock:     return Key::NumLock;
        case XKB_KEY_KP_Multiply:  return Key::Multiply;
        case XKB_KEY_KP_Add:       return Key::Add;
        case XKB_KEY_KP_Subtract:  return Key::Subtract;
        case XKB_KEY_KP_Divide:    return Key::Divide;
        case XKB_KEY_KP_Decimal:   return Key::Decimal;
        case XKB_KEY_KP_Separator: return Key::Separator;

        default: break;
    }

    return Key::Unknown;
}

} // namespace Forms
} // namespace Pt
