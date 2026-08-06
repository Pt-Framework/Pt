 /* Copyright (C) 2016 Marc Boris Duerner
   
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
   Foundation, Inc., 51 Franklin Street, Fifth Floor, 
   Boston, MA 02110-1301 USA
*/

#include <Pt/Forms/Key.h>
#include <Pt/Types.h>
#include <map>
#include <string>

namespace Pt {

namespace Forms {

// maps a KeyboardEvent.code string (https://w3c.github.io/uievents-code/)
// to a Pt::Forms::Key::Code
Pt::uint32_t toKeycode(const char* domCode)
{
    static const std::map<std::string, Pt::uint32_t> codeMap = {
        { "Backspace",    Key::Backspace },
        { "Tab",          Key::Tab       },
        { "Enter",        Key::Return    },
        { "Escape",       Key::Escape    },
        { "Space",        Key::Space     },

        { "Digit0",  Key::D0 },
        { "Digit1",  Key::D1 },
        { "Digit2",  Key::D2 },
        { "Digit3",  Key::D3 },
        { "Digit4",  Key::D4 },
        { "Digit5",  Key::D5 },
        { "Digit6",  Key::D6 },
        { "Digit7",  Key::D7 },
        { "Digit8",  Key::D8 },
        { "Digit9",  Key::D9 },

        { "KeyA",  Key::A },
        { "KeyB",  Key::B },
        { "KeyC",  Key::C },
        { "KeyD",  Key::D },
        { "KeyE",  Key::E },
        { "KeyF",  Key::F },
        { "KeyG",  Key::G },
        { "KeyH",  Key::H },
        { "KeyI",  Key::I },
        { "KeyJ",  Key::J },
        { "KeyK",  Key::K },
        { "KeyL",  Key::L },
        { "KeyM",  Key::M },
        { "KeyN",  Key::N },
        { "KeyO",  Key::O },
        { "KeyP",  Key::P },
        { "KeyQ",  Key::Q },
        { "KeyR",  Key::R },
        { "KeyS",  Key::S },
        { "KeyT",  Key::T },
        { "KeyU",  Key::U },
        { "KeyV",  Key::V },
        { "KeyW",  Key::W },
        { "KeyX",  Key::X },
        { "KeyY",  Key::Y },
        { "KeyZ",  Key::Z },

        { "ShiftLeft",     Key::ShiftKey   },
        { "ShiftRight",    Key::ShiftKey   },
        { "ControlLeft",   Key::ControlKey },
        { "ControlRight",  Key::ControlKey },
        { "AltLeft",       Key::AltKey     },
        { "AltRight",      Key::AltKey     },
        { "MetaLeft",      Key::MetaKey    },
        { "MetaRight",     Key::MetaKey    },

        { "ArrowLeft",   Key::ArrowLeft  },
        { "ArrowRight",  Key::ArrowRight },
        { "ArrowUp",     Key::ArrowUp    },
        { "ArrowDown",   Key::ArrowDown  },

        { "NumLock",  Key::NumLock },
        { "Numpad0",  Key::NumPad0 },
        { "Numpad1",  Key::NumPad1 },
        { "Numpad2",  Key::NumPad2 },
        { "Numpad3",  Key::NumPad3 },
        { "Numpad4",  Key::NumPad4 },
        { "Numpad5",  Key::NumPad5 },
        { "Numpad6",  Key::NumPad6 },
        { "Numpad7",  Key::NumPad7 },
        { "Numpad8",  Key::NumPad8 },
        { "Numpad9",  Key::NumPad9 },

        { "NumpadMultiply",  Key::Multiply  },
        { "NumpadAdd",       Key::Add       },
        { "NumpadDivide",    Key::Divide    },
        { "NumpadSubtract",  Key::Subtract  },
        { "NumpadDecimal",   Key::Decimal   },
        { "NumpadEnter",     Key::Return    },

        { "F1",   Key::F1  },
        { "F2",   Key::F2  },
        { "F3",   Key::F3  },
        { "F4",   Key::F4  },
        { "F5",   Key::F5  },
        { "F6",   Key::F6  },
        { "F7",   Key::F7  },
        { "F8",   Key::F8  },
        { "F9",   Key::F9  },
        { "F10",  Key::F10 },
        { "F11",  Key::F11 },
        { "F12",  Key::F12 },
        { "F13",  Key::F13 },
        { "F14",  Key::F14 },
        { "F15",  Key::F15 },
        { "F16",  Key::F16 },
        { "F17",  Key::F17 },
        { "F18",  Key::F18 },
        { "F19",  Key::F19 },
        { "F20",  Key::F20 },
        { "F21",  Key::F21 },
        { "F22",  Key::F22 },
        { "F23",  Key::F23 },
        { "F24",  Key::F24 },

        { "Insert",       Key::Insert      },
        { "Delete",       Key::Delete      },
        { "Home",         Key::Home        },
        { "End",          Key::End         },
        { "PageUp",       Key::PageUp      },
        { "PageDown",     Key::PageDown    },
        { "CapsLock",     Key::CapsLock    },
        { "PrintScreen",  Key::PrintScreen },
        { "ScrollLock",   Key::ScrollLock  },
        { "Pause",        Key::Pause       },
        { "Help",         Key::Help        },
        { "ContextMenu",  Key::AppsMenu    },
    };

    std::map<std::string, Pt::uint32_t>::const_iterator it = codeMap.find(domCode);
    if( it == codeMap.end() )
        return Key::NoKey;

    return it->second;
}

} // namespace

} // namespace
