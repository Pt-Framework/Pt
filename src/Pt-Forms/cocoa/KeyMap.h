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
    Key::A,             //   0    0x00    A                        
    Key::S,             //   1    0x01    S               
    Key::D,             //   2    0x02    D               
    Key::F,             //   3    0x03    F               
    Key::H,             //   4    0x04    H               
    Key::G,             //   5    0x05    G               
    Key::Z,             //   6    0x06    Z               
    Key::X,             //   7    0x07    X               
    Key::C,             //   8    0x08    C               
    Key::V,             //   9    0x09    V               
    Key::NoKey,         //  10    0x0A    SectionSign (VK_OEM_3)  
    Key::B,             //  11    0x0B    B               
    Key::Q,             //  12    0x0C    Q               
    Key::W,             //  13    0x0D    W               
    Key::E,             //  14    0x0E    E               
    Key::R,             //  15    0x0F    R               
    Key::Y,             //  16    0x10    Y               
    Key::T,             //  17    0x11    T               
    Key::D1,            //  18    0x12    One             
    Key::D2,            //  19    0x13    Two             
    Key::D3,            //  20    0x14    Three           
    Key::D4,            //  21    0x15    Four            
    Key::D6,            //  22    0x16    Six             
    Key::D5,            //  23    0x17    Five            
    Key::NoKey,         //  24    0x18    Equal (VK_OEM_PLUS)          
    Key::D9,            //  25    0x19    Nine            
    Key::D7,            //  26    0x1A    Seven           
    Key::NoKey,         //  27    0x1B    Minus (VK_OEM_MINUS)          
    Key::D8,            //  28    0x1C    Eight           
    Key::D0,            //  29    0x1D    Zero            
    Key::NoKey,         //  30    0x1E    RightBracket (VK_OEM_6) 
    Key::O,             //  31    0x1F    O               
    Key::U,             //  32    0x20    U               
    Key::NoKey,         //  33    0x21    LeftBracket (VK_OEM_4)  
    Key::I,             //  34    0x22    I               
    Key::P,             //  35    0x23    P               
    Key::Return,        //  36    0x24    Return          
    Key::L,             //  37    0x25    L               
    Key::J,             //  38    0x26    J               
    Key::NoKey,         //  39    0x27    Quote (VK_OEM_7)          
    Key::K,             //  40    0x28    K               
    Key::NoKey,         //  41    0x29    Semicolon (VK_OEM_1)
    Key::NoKey,         //  42    0x2A    Backslash (VK_OEM_5)    
    Key::NoKey,         //  43    0x2B    Comma (VK_OEM_COMMA)         
    Key::NoKey,         //  44    0x2C    Slash (VK_OEM_2)        
    Key::N,             //  45    0x2D    N               
    Key::M,             //  46    0x2E    M               
    Key::NoKey,         //  47    0x2F    Period (VK_OEM_PERIOD)       
    Key::Tab,           //  48    0x30    Tab             
    Key::Space,         //  49    0x31    Space           
    Key::NoKey,         //  50    0x32    Grave (VK_OEM_3)          
    Key::Backspace,     //  51    0x33    Delete          
    Key::Unknown,       //  52    0x34    Linefeed        
    Key::Escape,        //  53    0x35    Escape          
    Key::Meta,          //  54    0x36    RightCommand
    Key::Meta,          //  55    0x37    LeftCommand         
    Key::ShiftKey,      //  56    0x38    LeftShift           
    Key::CapsLock,      //  57    0x39    CapsLock        
    Key::Alt,           //  58    0x3A    LeftOption          
    Key::Control,       //  59    0x3B    LeftControl         
    Key::ShiftKey,      //  60    0x3C    RightShift      
    Key::Alt,           //  61    0x3D    RightOption     
    Key::Control,       //  62    0x3E    RightControl    
    Key::Unknown,       //  63    0x3F    Function        
    Key::F17,           //  64    0x40    F17             
    Key::Decimal,       //  65    0x41    KeypadDecimal   
    Key::Unknown,       //  66    0x42   
    Key::Multiply,      //  67    0x43    KeypadMultiply  
    Key::Unknown,       //  68    0x44   
    Key::Add,           //  69    0x45    KeypadPlus      
    Key::Unknown,       //  70    0x46   
    Key::Clear,         //  71    0x47    KeypadClear     
    Key::VolumeUp,      //  72    0x48    VolumeUp        
    Key::VolumeDown,    //  73    0x49    VolumeDown      
    Key::VolumeMute,    //  74    0x4A    Mute            
    Key::Divide,        //  75    0x4B    KeypadDivide    
    Key::Return,        //  76    0x4C    KeypadEnter     
    Key::Unknown,       //  77    0x4D   
    Key::Subtract,      //  78    0x4E    KeypadMinus     
    Key::F18,           //  79    0x4F    F18             
    Key::F19,           //  80    0x50    F19             
    Key::NoKey,         //  81    0x51    KeypadEquals (VKEY_OEM_PLUS)    
    Key::NumPad0,       //  82    0x52    Keypad0 0       
    Key::NumPad1,       //  83    0x53    Keypad1 1       
    Key::NumPad2,       //  84    0x54    Keypad2 2       
    Key::NumPad3,       //  85    0x55    Keypad3 3       
    Key::NumPad4,       //  86    0x56    Keypad4 4       
    Key::NumPad5,       //  87    0x57    Keypad5 5       
    Key::NumPad6,       //  88    0x58    Keypad6 6       
    Key::NumPad7,       //  89    0x59    Keypad7 7       
    Key::F20,           //  90    0x5A    F20             
    Key::NumPad8,       //  91    0x5B    Keypad8 8       
    Key::NumPad9,       //  92    0x5C    Keypad9 9       
    Key::Unknown,       //  93    0x5D    Yen (JIS Keyboard)                
    Key::Unknown,       //  94    0x5E    Underscore (JIS Keyboard)         
    Key::Unknown,       //  95    0x5F    KeypadComma (JIS Keyboard)        
    Key::F5,            //  96    0x60    F5              
    Key::F6,            //  97    0x61    F6              
    Key::F7,            //  98    0x62    F7              
    Key::F3,            //  99    0x63    F3              
    Key::F8,            // 100    0x64    F8              
    Key::F9,            // 101    0x65    F9              
    Key::Unknown,       // 102    0x66    Eisu (JIS Keyboard)              
    Key::F11,           // 103    0x67    F11             
    Key::Unknown,       // 104    0x68    Kana (JIS Keyboard)              
    Key::F13,           // 105    0x69    F13             
    Key::F16,           // 106    0x6A    F16             
    Key::F14,           // 107    0x6B    F14             
    Key::Unknown,       // 108    0x6C   
    Key::F10,           // 109    0x6D    F10             
    Key::AppsMenu,      // 110    0x6E    Context Menu key                     
    Key::F12,           // 111    0x6F    F12             
    Key::Unknown,       // 112    0x70   
    Key::F15,           // 113    0x71    F15             
    Key::Insert,        // 114    0x72    Help/Insert     
    Key::Home,          // 115    0x73    Home            
    Key::PageUp,        // 116    0x74    PageUp          
    Key::Delete,        // 117    0x75    ForwardDelete   
    Key::F4,            // 118    0x76    F4              
    Key::End,           // 119    0x77    End             
    Key::F2,            // 120    0x78    F2              
    Key::PageDown,      // 121    0x79    PageDown        
    Key::F1,            // 122    0x7A    F1              
    Key::ArrowLeft,     // 123    0x7B    LeftArrow       
    Key::ArrowRight,    // 124    0x7C    RightArrow      
    Key::ArrowDown,     // 125    0x7D    DownArrow       
    Key::ArrowUp,       // 126    0x7E    UpArrow         
    Key::Unknown        // 127    0x7F
};                                 

static const unsigned keyMapSize = sizeof(keyMap) / sizeof(Pt::uint32_t);

} // namespace

} // namespace
