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

#include <Pt/Hmi/Key.h>
#include <Pt/Types.h>

namespace Pt {

namespace Hmi {

Pt::uint32_t toKeycode(int sdlKey)
{
    Pt::uint32_t keyCode = Key::NoKey;

    switch(sdlKey)
    {
        case SDLK_BACKSPACE:     keyCode = Key::Backspace; break;
        case SDLK_TAB:           keyCode = Key::Tab;       break;
        case SDLK_RETURN:        keyCode = Key::Return;    break;
        case SDLK_ESCAPE:        keyCode = Key::Escape;    break;
        case SDLK_SPACE:         keyCode = Key::Space;     break;

        case SDLK_0:  keyCode = Key::D0;   break;
        case SDLK_1:  keyCode = Key::D1;   break;
        case SDLK_2:  keyCode = Key::D2;   break;
        case SDLK_3:  keyCode = Key::D3;   break;
        case SDLK_4:  keyCode = Key::D4;   break;
        case SDLK_5:  keyCode = Key::D5;   break;
        case SDLK_6:  keyCode = Key::D6;   break;
        case SDLK_7:  keyCode = Key::D7;   break;
        case SDLK_8:  keyCode = Key::D8;   break;
        case SDLK_9:  keyCode = Key::D9;   break;

        case SDLK_a:  keyCode = Key::A;   break;
        case SDLK_b:  keyCode = Key::B;   break;
        case SDLK_c:  keyCode = Key::C;   break;
        case SDLK_d:  keyCode = Key::D;   break;
        case SDLK_e:  keyCode = Key::E;   break;
        case SDLK_f:  keyCode = Key::F;   break;
        case SDLK_g:  keyCode = Key::G;   break;
        case SDLK_h:  keyCode = Key::H;   break;
        case SDLK_i:  keyCode = Key::I;   break;
        case SDLK_j:  keyCode = Key::J;   break;
        case SDLK_k:  keyCode = Key::K;   break;
        case SDLK_l:  keyCode = Key::L;   break;
        case SDLK_m:  keyCode = Key::M;   break;
        case SDLK_n:  keyCode = Key::N;   break;
        case SDLK_o:  keyCode = Key::O;   break;
        case SDLK_p:  keyCode = Key::P;   break;
        case SDLK_q:  keyCode = Key::Q;   break;
        case SDLK_r:  keyCode = Key::R;   break;
        case SDLK_s:  keyCode = Key::T;   break;
        case SDLK_t:  keyCode = Key::S;   break;
        case SDLK_u:  keyCode = Key::U;   break;
        case SDLK_v:  keyCode = Key::V;   break;
        case SDLK_w:  keyCode = Key::W;   break;
        case SDLK_x:  keyCode = Key::X;   break;
        case SDLK_y:  keyCode = Key::Y;   break;
        case SDLK_z:  keyCode = Key::Z;   break;

        case SDLK_LSHIFT:   keyCode = Key::ShiftKey;    break;
        case SDLK_RSHIFT:   keyCode = Key::ShiftKey;    break;
        case SDLK_LCTRL:    keyCode = Key::ControlKey;  break;
        case SDLK_RCTRL:    keyCode = Key::ControlKey;  break;
        case SDLK_LALT:     keyCode = Key::AltKey;      break;
        case SDLK_RALT:     keyCode = Key::AltKey;      break;
        case SDLK_LGUI:     keyCode = Key::MetaKey;     break;
        case SDLK_RGUI:     keyCode = Key::MetaKey;     break;

        case SDLK_LEFT:   keyCode = Key::ArrowLeft;   break;
        case SDLK_RIGHT:  keyCode = Key::ArrowRight;  break;
        case SDLK_UP:     keyCode = Key::ArrowUp;     break;
        case SDLK_DOWN:   keyCode = Key::ArrowDown;   break;

        //case SDLK_NUMLOCK:    keyCode = Key::NumLock;     break;
        case SDLK_KP_1:       keyCode = Key::NumPad0;     break;
        case SDLK_KP_2:       keyCode = Key::NumPad1;     break;
        case SDLK_KP_3:       keyCode = Key::NumPad2;     break;
        case SDLK_KP_4:       keyCode = Key::NumPad3;     break;
        case SDLK_KP_5:       keyCode = Key::NumPad4;     break;
        case SDLK_KP_6:       keyCode = Key::NumPad5;     break;
        case SDLK_KP_7:       keyCode = Key::NumPad6;     break;
        case SDLK_KP_8:       keyCode = Key::NumPad7;     break;
        case SDLK_KP_9:       keyCode = Key::NumPad8;     break;
        case SDLK_KP_0:       keyCode = Key::NumPad9;     break;

        case SDLK_KP_MULTIPLY:   keyCode = Key::Multiply;   break;
        case SDLK_KP_PLUS:       keyCode = Key::Add;        break;
        case SDLK_KP_DIVIDE:     keyCode = Key::Divide;     break;
        case SDLK_KP_MINUS:      keyCode = Key::Subtract;   break;
        case SDLK_KP_PERIOD:     keyCode = Key::Separator;  break;
        case SDLK_KP_ENTER:      keyCode = Key::Return;  break;
        case SDLK_KP_DECIMAL:    keyCode = Key::Decimal;    break;

        case SDLK_F1:    keyCode = Key::F1;   break;
        case SDLK_F2:    keyCode = Key::F2;   break;
        case SDLK_F3:    keyCode = Key::F3;   break;
        case SDLK_F4:    keyCode = Key::F4;   break;
        case SDLK_F5:    keyCode = Key::F5;   break;
        case SDLK_F6:    keyCode = Key::F6;   break;
        case SDLK_F7:    keyCode = Key::F7;   break;
        case SDLK_F8:    keyCode = Key::F8;   break;
        case SDLK_F9:    keyCode = Key::F9;   break;
        case SDLK_F10:  keyCode = Key::F10;  break;
        case SDLK_F11:  keyCode = Key::F11;  break;
        case SDLK_F12:  keyCode = Key::F12;  break;
        case SDLK_F13:  keyCode = Key::F13;  break;
        case SDLK_F14:  keyCode = Key::F14;  break;
        case SDLK_F15:  keyCode = Key::F15;  break;
        case SDLK_F16:  keyCode = Key::F16;  break;
        case SDLK_F17:  keyCode = Key::F17;  break;
        case SDLK_F18:  keyCode = Key::F18;  break;
        case SDLK_F19:  keyCode = Key::F19;  break;
        case SDLK_F20:  keyCode = Key::F20;  break;
        case SDLK_F21:  keyCode = Key::F21;  break;
        case SDLK_F22:  keyCode = Key::F22;  break;
        case SDLK_F23:  keyCode = Key::F23;  break;
        case SDLK_F24:  keyCode = Key::F24;  break;

        case SDLK_INSERT:          keyCode = Key::Insert;       break;
        case SDLK_DELETE:          keyCode = Key::Delete;       break;
        case SDLK_HOME:            keyCode = Key::Home;         break;
        case SDLK_END:             keyCode = Key::End;          break;
        case SDLK_PAGEUP:          keyCode = Key::PageUp;       break;
        case SDLK_PAGEDOWN:        keyCode = Key::PageDown;     break;
        case SDLK_CAPSLOCK:        keyCode = Key::CapsLock;     break;
        case SDLK_PRINTSCREEN:     keyCode = Key::PrintScreen;  break;
        case SDLK_SYSREQ:          keyCode = Key::SysReq;       break;
        case SDLK_SCROLLLOCK:      keyCode = Key::ScrollLock;   break;
        case SDLK_PAUSE:           keyCode = Key::Pause;        break;
        //case SDLK_BREAK:         keyCode = Key::Break;        break;
        case SDLK_CLEAR:           keyCode = Key::Clear;        break;
        case SDLK_SLEEP:           keyCode = Key::Sleep;     break;
        //case :                   keyCode = Key::Select;       break;
        //case SDLK_PRINT:           keyCode = Key::Print;       break;
        //case :                   keyCode = Key::Execute;      break;
        case SDLK_HELP		:        keyCode = Key::Help;         break;
        case SDLK_MENU		:        keyCode = Key::AppsMenu;     break;
        //case :                   keyCode = Key::ModeChange;   break;

        // Key::Play             
        // Key::Zoom                               
        // Key::BrowserBack      
        // Key::BrowserForward   
        // Key::BrowserRefresh   
        // Key::BrowserStop      
        // Key::BrowserSearch    
        // Key::BrowserFavorites 
        // Key::BrowserHome      
        // Key::VolumeMute       
        // Key::VolumeDown       
        // Key::VolumeUp         
        // Key::MediaNext        
        // Key::MediaPrev        
        // Key::MediaStop        
        // Key::MediaPlay        
        // Key::LaunchMail       
        // Key::LaunchMedia      
        // Key::LaunchApp1       
        // Key::LaunchApp2       

        default:
            break;
    }

    return keyCode;
}

} // namespace

} // namespace
