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

namespace Pt {

namespace Forms {

static const Pt::uint32_t keyMap[] =
{
    Pt::Forms::Key::Unknown,           //   0   0x00
    Pt::Forms::Key::Unknown,           //   1   0x01   VK_LBUTTON          
    Pt::Forms::Key::Unknown,           //   2   0x02   VK_RBUTTON          
    Pt::Forms::Key::Break,             //   3   0x03   VK_CANCEL           
    Pt::Forms::Key::Unknown,           //   4   0x04   VK_MBUTTON          
    Pt::Forms::Key::Unknown,           //   5   0x05   VK_XBUTTON1         
    Pt::Forms::Key::Unknown,           //   6   0x06   VK_XBUTTON2         
    Pt::Forms::Key::Unknown,           //   7   0x07
    Pt::Forms::Key::Backspace,         //   8   0x08   VK_BACK             
    Pt::Forms::Key::Tab,               //   9   0x09   VK_TAB              
    Pt::Forms::Key::Unknown,           //  10   0x0A
    Pt::Forms::Key::Unknown,           //  11   0x0B
    Pt::Forms::Key::Clear,             //  12   0x0C   VK_CLEAR            
    Pt::Forms::Key::Return,            //  13   0x0D   VK_RETURN           
    Pt::Forms::Key::Unknown,           //  14   0x0E
    Pt::Forms::Key::Unknown,           //  15   0x0F
    Pt::Forms::Key::Shift,             //  16   0x10   VK_SHIFT            
    Pt::Forms::Key::Control,           //  17   0x11   VK_CONTROL          
    Pt::Forms::Key::Alt,               //  18   0x12   VK_MENU             
    Pt::Forms::Key::Pause,             //  19   0x13   VK_PAUSE            
    Pt::Forms::Key::CapsLock,          //  20   0x14   VK_CAPITAL          
    Pt::Forms::Key::Unknown,           //  21   0x15   VK_KANA VK_HANGUL 
    Pt::Forms::Key::Unknown,           //  22   0x16
    Pt::Forms::Key::Unknown,           //  23   0x17   VK_JUNJA            
    Pt::Forms::Key::Unknown,           //  24   0x18   VK_FINAL            
    Pt::Forms::Key::Unknown,           //  25   0x19   VK_HANJA VK_KANJI 
    Pt::Forms::Key::Unknown,           //  26   0x1A
    Pt::Forms::Key::Escape,            //  27   0x1B   VK_ESCAPE           
    Pt::Forms::Key::Unknown,           //  28   0x1C   VK_CONVERT          
    Pt::Forms::Key::Unknown,           //  29   0x1D   VK_NONCONVERT       
    Pt::Forms::Key::Unknown,           //  30   0x1E   VK_ACCEPT           
    Pt::Forms::Key::ModeChange,        //  31   0x1F   VK_MODECHANGE       
    Pt::Forms::Key::Space,             //  32   0x20   VK_SPACE            
    Pt::Forms::Key::PageUp,            //  33   0x21   VK_PRIOR            
    Pt::Forms::Key::PageDown,          //  34   0x22   VK_NEXT             
    Pt::Forms::Key::End,               //  35   0x23   VK_END              
    Pt::Forms::Key::Home,              //  36   0x24   VK_HOME             
    Pt::Forms::Key::ArrowLeft,         //  37   0x25   VK_LEFT             
    Pt::Forms::Key::ArrowUp,           //  38   0x26   VK_UP               
    Pt::Forms::Key::ArrowRight,        //  39   0x27   VK_RIGHT            
    Pt::Forms::Key::ArrowDown,         //  40   0x28   VK_DOWN             
    Pt::Forms::Key::Select,            //  41   0x29   VK_SELECT           
    Pt::Forms::Key::Print,             //  42   0x2A   VK_PRINT            
    Pt::Forms::Key::Execute,           //  43   0x2B   VK_EXECUTE          
    Pt::Forms::Key::PrintScreen,       //  44   0x2C   VK_SNAPSHOT         
    Pt::Forms::Key::Insert,            //  45   0x2D   VK_INSERT           
    Pt::Forms::Key::Delete,            //  46   0x2E   VK_DELETE           
    Pt::Forms::Key::Help,              //  47   0x2F   VK_HELP             
    0,                               //  48   0x30   VK_0              
    0,                               //  49   0x31   VK_1              
    0,                               //  50   0x32   VK_2              
    0,                               //  51   0x33   VK_3              
    0,                               //  52   0x34   VK_4              
    0,                               //  53   0x35   VK_5              
    0,                               //  54   0x36   VK_6              
    0,                               //  55   0x37   VK_7              
    0,                               //  56   0x38   VK_8              
    0,                               //  57   0x39   VK_9              
    Pt::Forms::Key::Unknown,           //  58   0x3A
    Pt::Forms::Key::Unknown,           //  59   0x3B
    Pt::Forms::Key::Unknown,           //  60   0x3C
    Pt::Forms::Key::Unknown,           //  61   0x3D
    Pt::Forms::Key::Unknown,           //  62   0x3E
    Pt::Forms::Key::Unknown,           //  63   0x3F
    Pt::Forms::Key::Unknown,           //  64   0x40
    0,                               //  65   0x41   VK_A              
    0,                               //  66   0x42   VK_B              
    0,                               //  67   0x43   VK_C              
    0,                               //  68   0x44   VK_D              
    0,                               //  69   0x45   VK_E              
    0,                               //  70   0x46   VK_F              
    0,                               //  71   0x47   VK_G              
    0,                               //  72   0x48   VK_H              
    0,                               //  73   0x49   VK_I              
    0,                               //  74   0x4A   VK_J              
    0,                               //  75   0x4B   VK_K              
    0,                               //  76   0x4C   VK_L              
    0,                               //  77   0x4D   VK_M              
    0,                               //  78   0x4E   VK_N              
    0,                               //  79   0x4F   VK_O              
    0,                               //  80   0x50   VK_P              
    0,                               //  81   0x51   VK_Q              
    0,                               //  82   0x52   VK_R              
    0,                               //  83   0x53   VK_S              
    0,                               //  84   0x54   VK_T              
    0,                               //  85   0x55   VK_U              
    0,                               //  86   0x56   VK_V              
    0,                               //  87   0x57   VK_W              
    0,                               //  88   0x58   VK_X              
    0,                               //  89   0x59   VK_Y              
    0,                               //  90   0x5A   VK_Z              
    Pt::Forms::Key::Meta,              //  91   0x5B   VK_LWIN             
    Pt::Forms::Key::Meta,              //  92   0x5C   VK_RWIN             
    Pt::Forms::Key::AppsMenu,          //  93   0x5D   VK_APPS             
    Pt::Forms::Key::Unknown,           //  94   0x5E
    Pt::Forms::Key::Sleep,             //  95   0x5F   VK_SLEEP
    Pt::Forms::Key::NumPad0,           //  96   0x60   VK_NUMPAD0          
    Pt::Forms::Key::NumPad1,           //  97   0x61   VK_NUMPAD1          
    Pt::Forms::Key::NumPad2,           //  98   0x62   VK_NUMPAD2          
    Pt::Forms::Key::NumPad3,           //  99   0x63   VK_NUMPAD3          
    Pt::Forms::Key::NumPad4,           // 100   0x64   VK_NUMPAD4          
    Pt::Forms::Key::NumPad5,           // 101   0x65   VK_NUMPAD5          
    Pt::Forms::Key::NumPad6,           // 102   0x66   VK_NUMPAD6          
    Pt::Forms::Key::NumPad7,           // 103   0x67   VK_NUMPAD7          
    Pt::Forms::Key::NumPad8,           // 104   0x68   VK_NUMPAD8          
    Pt::Forms::Key::NumPad9,           // 105   0x69   VK_NUMPAD9          
    Pt::Forms::Key::Multiply,          // 106   0x6A   VK_MULTIPLY         
    Pt::Forms::Key::Add,               // 107   0x6B   VK_ADD              
    Pt::Forms::Key::Separator,         // 108   0x6C   VK_SEPARATOR        
    Pt::Forms::Key::Subtract,          // 109   0x6D   VK_SUBTRACT         
    Pt::Forms::Key::Decimal,           // 110   0x6E   VK_DECIMAL          
    Pt::Forms::Key::Divide,            // 111   0x6F   VK_DIVIDE           
    Pt::Forms::Key::F1,                // 112   0x70   VK_F1               
    Pt::Forms::Key::F2,                // 113   0x71   VK_F2               
    Pt::Forms::Key::F3,                // 114   0x72   VK_F3               
    Pt::Forms::Key::F4,                // 115   0x73   VK_F4               
    Pt::Forms::Key::F5,                // 116   0x74   VK_F5               
    Pt::Forms::Key::F6,                // 117   0x75   VK_F6               
    Pt::Forms::Key::F7,                // 118   0x76   VK_F7               
    Pt::Forms::Key::F8,                // 119   0x77   VK_F8               
    Pt::Forms::Key::F9,                // 120   0x78   VK_F9               
    Pt::Forms::Key::F10,               // 121   0x79   VK_F10              
    Pt::Forms::Key::F11,               // 122   0x7A   VK_F11              
    Pt::Forms::Key::F12,               // 123   0x7B   VK_F12              
    Pt::Forms::Key::F13,               // 124   0x7C   VK_F13              
    Pt::Forms::Key::F14,               // 125   0x7D   VK_F14              
    Pt::Forms::Key::F15,               // 126   0x7E   VK_F15              
    Pt::Forms::Key::F16,               // 127   0x7F   VK_F16              
    Pt::Forms::Key::F17,               // 128   0x80   VK_F17              
    Pt::Forms::Key::F18,               // 129   0x81   VK_F18              
    Pt::Forms::Key::F19,               // 130   0x82   VK_F19              
    Pt::Forms::Key::F20,               // 131   0x83   VK_F20              
    Pt::Forms::Key::F21,               // 132   0x84   VK_F21              
    Pt::Forms::Key::F22,               // 133   0x85   VK_F22              
    Pt::Forms::Key::F23,               // 134   0x86   VK_F23              
    Pt::Forms::Key::F24,               // 135   0x87   VK_F24              
    Pt::Forms::Key::Unknown,           // 136   0x88
    Pt::Forms::Key::Unknown,           // 137   0x89
    Pt::Forms::Key::Unknown,           // 138   0x8A
    Pt::Forms::Key::Unknown,           // 139   0x8B
    Pt::Forms::Key::Unknown,           // 140   0x8C
    Pt::Forms::Key::Unknown,           // 141   0x8D
    Pt::Forms::Key::Unknown,           // 142   0x8E
    Pt::Forms::Key::Unknown,           // 143   0x8F
    Pt::Forms::Key::NumLock,           // 144   0x90   VK_NUMLOCK          
    Pt::Forms::Key::ScrollLock,        // 145   0x91   VK_SCROLL           
    0,                               // 146   0x92   VK_OEM_FJ_JISHO     
    0,                               // 147   0x93   VK_OEM_FJ_MASSHOU   
    0,                               // 148   0x94   VK_OEM_FJ_TOUROKU   
    0,                               // 149   0x95   VK_OEM_FJ_LOYA  
    0,                               // 150   0x96   VK_OEM_FJ_ROYA  
    Pt::Forms::Key::Unknown,           // 151   0x97
    Pt::Forms::Key::Unknown,           // 152   0x98
    Pt::Forms::Key::Unknown,           // 153   0x99
    Pt::Forms::Key::Unknown,           // 154   0x9A
    Pt::Forms::Key::Unknown,           // 155   0x9B
    Pt::Forms::Key::Unknown,           // 156   0x9C
    Pt::Forms::Key::Unknown,           // 157   0x9D
    Pt::Forms::Key::Unknown,           // 158   0x9E
    Pt::Forms::Key::Unknown,           // 159   0x9F
    Pt::Forms::Key::Shift,             // 160   0xA0   VK_LSHIFT           
    Pt::Forms::Key::Shift,             // 161   0xA1   VK_RSHIFT           
    Pt::Forms::Key::Control,           // 162   0xA2   VK_LCONTROL         
    Pt::Forms::Key::Control,           // 163   0xA3   VK_RCONTROL         
    Pt::Forms::Key::Alt,               // 164   0xA4   VK_LMENU            
    Pt::Forms::Key::Alt,               // 165   0xA5   VK_RMENU            
    Pt::Forms::Key::BrowserBack,       // 166   0xA6   VK_BROWSER_BACK     
    Pt::Forms::Key::BrowserForward,    // 167   0xA7   VK_BROWSER_FORWARD  
    Pt::Forms::Key::BrowserRefresh,    // 168   0xA8   VK_BROWSER_REFRESH  
    Pt::Forms::Key::BrowserStop,       // 169   0xA9   VK_BROWSER_STOP     
    Pt::Forms::Key::BrowserSearch,     // 170   0xAA   VK_BROWSER_SEARCH   
    Pt::Forms::Key::BrowserFavorites,  // 171   0xAB   VK_BROWSER_FAVORITES
    Pt::Forms::Key::BrowserHome,       // 172   0xAC   VK_BROWSER_HOME     
    Pt::Forms::Key::VolumeMute,        // 173   0xAD   VK_VOLUME_MUTE      
    Pt::Forms::Key::VolumeDown,        // 174   0xAE   VK_VOLUME_DOWN      
    Pt::Forms::Key::VolumeUp,          // 175   0xAF   VK_VOLUME_UP        
    Pt::Forms::Key::MediaNext,         // 176   0xB0   VK_MEDIA_NEXT_TRACK 
    Pt::Forms::Key::MediaPrev,         // 177   0xB1   VK_MEDIA_PREV_TRACK 
    Pt::Forms::Key::MediaStop,         // 178   0xB2   VK_MEDIA_STOP       
    Pt::Forms::Key::MediaPlay,         // 179   0xB3   VK_MEDIA_PLAY_PAUSE 
    Pt::Forms::Key::LaunchMail,        // 180   0xB4   VK_LAUNCH_MAIL      
    Pt::Forms::Key::LaunchMedia,       // 181   0xB5   VK_LAUNCH_MEDIA_SELECT
    Pt::Forms::Key::LaunchApp1,        // 182   0xB6   VK_LAUNCH_APP1      
    Pt::Forms::Key::LaunchApp2,        // 183   0xB7   VK_LAUNCH_APP2      
    Pt::Forms::Key::Unknown,           // 184   0xB8
    Pt::Forms::Key::Unknown,           // 185   0xB9
    0,                               // 186   0xBA   VK_OEM_1            
    0,                               // 187   0xBB   VK_OEM_PLUS         
    0,                               // 188   0xBC   VK_OEM_COMMA        
    0,                               // 189   0xBD   VK_OEM_MINUS        
    0,                               // 190   0xBE   VK_OEM_PERIOD       
    0,                               // 191   0xBF   VK_OEM_2            
    0,                               // 192   0xC0   VK_OEM_3            
    Pt::Forms::Key::Unknown,           // 193   0xC1
    Pt::Forms::Key::Unknown,           // 194   0xC2
    Pt::Forms::Key::Unknown,           // 195   0xC3
    Pt::Forms::Key::Unknown,           // 196   0xC4
    Pt::Forms::Key::Unknown,           // 197   0xC5
    Pt::Forms::Key::Unknown,           // 198   0xC6
    Pt::Forms::Key::Unknown,           // 199   0xC7
    Pt::Forms::Key::Unknown,           // 200   0xC8
    Pt::Forms::Key::Unknown,           // 201   0xC9
    Pt::Forms::Key::Unknown,           // 202   0xCA
    Pt::Forms::Key::Unknown,           // 203   0xCB
    Pt::Forms::Key::Unknown,           // 204   0xCC
    Pt::Forms::Key::Unknown,           // 205   0xCD
    Pt::Forms::Key::Unknown,           // 206   0xCE
    Pt::Forms::Key::Unknown,           // 207   0xCF
    Pt::Forms::Key::Unknown,           // 208   0xD0
    Pt::Forms::Key::Unknown,           // 209   0xD1
    Pt::Forms::Key::Unknown,           // 210   0xD2
    Pt::Forms::Key::Unknown,           // 211   0xD3
    Pt::Forms::Key::Unknown,           // 212   0xD4
    Pt::Forms::Key::Unknown,           // 213   0xD5
    Pt::Forms::Key::Unknown,           // 214   0xD6
    Pt::Forms::Key::Unknown,           // 215   0xD7
    Pt::Forms::Key::Unknown,           // 216   0xD8
    Pt::Forms::Key::Unknown,           // 217   0xD9
    Pt::Forms::Key::Unknown,           // 218   0xDA
    0,                               // 219   0xDB   VK_OEM_4            
    0,                               // 220   0xDC   VK_OEM_5            
    0,                               // 221   0xDD   VK_OEM_6            
    0,                               // 222   0xDE   VK_OEM_7            
    0,                               // 223   0xDF   VK_OEM_8
    Pt::Forms::Key::Unknown,           // 224   0xE0
    Pt::Forms::Key::Unknown,           // 225   0xE1   VK_OEM_AX           
    0,                               // 226   0xE2   VK_OEM_102 ("<>" key)       
    Pt::Forms::Key::Unknown,           // 227   0xE3   VK_ICO_HELP         
    Pt::Forms::Key::Unknown,           // 228   0xE4   VK_ICO_00           
    Pt::Forms::Key::Unknown,           // 229   0xE5   VK_PROCESSKEY       
    Pt::Forms::Key::Unknown,           // 230   0xE6   VK_ICO_CLEAR        
    Pt::Forms::Key::Unknown,           // 231   0xE7   VK_PACKET           
    Pt::Forms::Key::Unknown,           // 232   0xE8
    Pt::Forms::Key::Unknown,           // 233   0xE9   VK_OEM_RESET
    Pt::Forms::Key::Unknown,           // 234   0xEA   VK_OEM_JUMP
    Pt::Forms::Key::Unknown,           // 235   0xEB   VK_OEM_PA1
    Pt::Forms::Key::Unknown,           // 236   0xEC   VK_OEM_PA2
    Pt::Forms::Key::Unknown,           // 237   0xED   VK_OEM_PA3
    Pt::Forms::Key::Unknown,           // 238   0xEE   VK_OEM_WSCTRL
    Pt::Forms::Key::Unknown,           // 239   0xEF   VK_OEM_CUSEL
    Pt::Forms::Key::Unknown,           // 240   0xF0   VK_OEM_ATTN
    Pt::Forms::Key::Unknown,           // 241   0xF1   VK_OEM_FINISH
    Pt::Forms::Key::Unknown,           // 242   0xF2   VK_OEM_COPY
    Pt::Forms::Key::Unknown,           // 243   0xF3   VK_OEM_AUTO
    Pt::Forms::Key::Unknown,           // 244   0xF4   VK_OEM_ENLW
    Pt::Forms::Key::Unknown,           // 245   0xF5   VK_OEM_BACKTAB
    Pt::Forms::Key::Unknown,           // 246   0xF6   VK_ATTN             
    Pt::Forms::Key::Unknown,           // 247   0xF7   VK_CRSEL            
    Pt::Forms::Key::Unknown,           // 248   0xF8   VK_EXSEL            
    Pt::Forms::Key::Unknown,           // 249   0xF9   VK_EREOF            
    Pt::Forms::Key::Play,              // 250   0xFA   VK_PLAY             
    Pt::Forms::Key::Zoom,              // 251   0xFB   VK_ZOOM             
    Pt::Forms::Key::Unknown,           // 252   0xFC   VK_NONAME           
    Pt::Forms::Key::Unknown,           // 253   0xFD   VK_PA1              
    Pt::Forms::Key::Clear,             // 254   0xFE   VK_OEM_CLEAR        
    Pt::Forms::Key::Unknown            // 255   0xFF
};

const std::size_t keyMapSize = sizeof(keyMap) / sizeof(Pt::uint32_t);

} // namespace

} // namespace
