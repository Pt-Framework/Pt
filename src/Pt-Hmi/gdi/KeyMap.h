/* Copyright (C) 2013 Marc Boris Duerner
   Copyright (C) 2013 Laurentiu-Gheorghe Crisan
   
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

namespace {

    // TODO: is number on numpad same keycode as on main keyboard?
    // TODO: what about punct keys

    //NUMPAD
    case Qt::Key_NumLock:
        return VK_NUMLOCK, NumLock // (90) NUM LOCK key
    case Qt::Key_0:
        return VK_NUMPAD0, NumPad0
    case Qt::Key_1:
        return VK_NUMPAD1, NumPad1
    case Qt::Key_2:
        return  VK_NUMPAD2, NumPad2
    case Qt::Key_3:
        return VK_NUMPAD3, NumPad3
    case Qt::Key_4:
        return VK_NUMPAD4, NumPad4
    case Qt::Key_5:
        return VK_NUMPAD5, NumPad5
    case Qt::Key_6:
        return VK_NUMPAD6, NumPad6
    case Qt::Key_7:
        return VK_NUMPAD7, NumPad7
    case Qt::Key_8:
        return VK_NUMPAD8, NumPad8
    case Qt::Key_9:
        return VK_NUMPAD9, NumPad9
    case Qt::Key_Slash:
        return VK_DIVIDE, Divide
    case Qt::Key_Asterisk:
        return VK_MULTIPLY, Multiply
    case Qt::Key_Minus:
        return VK_SUBTRACT, Subtract
    case Qt::Key_Plus:
        return VK_ADD, Add
    case Qt::Key_Period:
        return VK_DECIMAL, Decimal
    case Qt::Key_Comma:
        return VK_SEPARATOR, Separator
        

    // ARROWS
    case Qt::Key_Left:
        return VK_LEFT, Left // (25) LEFT ARROW key
        
    case Qt::Key_Up:
        return VK_UP, Up // (26) UP ARROW key
        
    case Qt::Key_Right:
        return VK_RIGHT, Right; // (27) RIGHT ARROW key
        
    case Qt::Key_Down:
        return VK_DOWN, Down; // (28) DOWN ARROW key


    // NAVIGATION
    case Qt::Key_Insert:
        return VK_INSERT, Insert; // (2D) INS key
        
    case Qt::Key_Delete:
        return VK_DELETE, Delete; // (2E) DEL key

    case Qt::Key_Home:
        return VK_HOME, Home // (24) HOME key

    case Qt::Key_End:
        return VK_END End // (23) END key
        
    case Qt::Key_PageUp:
        return VK_PRIOR, PageUp // (21) PAGE UP key
        
    case Qt::Key_PageDown:
        return VK_NEXT, PageDown // (22) PAGE DOWN key
    
    case Qt::Key_Clear:
        return VK_CLEAR, Clear; // (0C) CLEAR key
               VK_OEM_CLEAR, Clear


    // SPECIAL FUNCTION KEYS
    case Qt::Key_Escape:
        return VK_ESCAPE, Escape; // (1B) ESC key
        
    case Qt::Key_PrintScreen:
        return VK_SNAPSHOT, PrintScreen// (2C) PRINT SCREEN key

    case Qt::Key_ScrollLock:
        return VK_SCROLL, Scroll; // (91) SCROLL LOCK key

    case Qt::Key_Pause:
        return VK_PAUSE, Pause; // (13) PAUSE key


    // FUNCTION KEYS
    case Qt::Key_F1:
        return VK_F1, F1;
    case Qt::Key_F2:
        return VK_F2, F2;
    case Qt::Key_F3:
        return VK_F3, F3;
    case Qt::Key_F4:
        return VK_F4, F4;
    case Qt::Key_F5:
        return VK_F5, F5;
    case Qt::Key_F6:
        return VK_F6, F6;
    case Qt::Key_F7:
        return VK_F7, F7;
    case Qt::Key_F8:
        return VK_F8, F8;
    case Qt::Key_F9:
        return VK_F9, F9;
    case Qt::Key_F10:
        return VK_F10, F10;
    case Qt::Key_F11:
        return VK_F11, F11;
    case Qt::Key_F12:
        return VK_F12, F12;
    case Qt::Key_F13:
        return VK_F13, F13;
    case Qt::Key_F14:
        return VK_F14, F14;
    case Qt::Key_F15:
        return VK_F15, F15;
    case Qt::Key_F16:
        return VK_F16, F16;
    case Qt::Key_F17:
        return VK_F17, F17;
    case Qt::Key_F18:
        return VK_F18, F18;
    case Qt::Key_F19:
        return VK_F19, F19;
    case Qt::Key_F20:
        return VK_F20, F20;
    case Qt::Key_F21:
        return VK_F21, F21;
    case Qt::Key_F22:
        return VK_F22, F22;
    case Qt::Key_F23:
        return VK_F23, F23;
    case Qt::Key_F24:
        return VK_F24, F24;


    //// INPUT METHODS
    case Qt::Key_Mode_switch
        VK_MODECHANGE, IMEModeChange (1F) IME mode change request
        
        // MAIN KEYBOARD
        case Qt::Key_Backspace:
            return VK_BACK, Back; // (08) BACKSPACE key
        
        case Qt::Key_Tab:
            return VK_TAB, Tab; // (09) TAB key
        
        case Qt::Key_Return:
            return VK_RETURN, Return; //(0D) Return key
        
        case Qt::Key_Shift:
            return VK_SHIFT, Shift; // (10) SHIFT key
            return VK_LSHIFT, Shift; // (10) SHIFT key
            return VK_RSHIFT, Shift; // (10) SHIFT key

        case Qt::Key_Control:
            return VK_CONTROL, Control; // (11) CTRL key
            return VK_LCONTROL, Control; 
            return VK_RCONTROL, Control; 
        
        case Qt::Key_Alt:
            return VK_MENU, Menu; // (12) ALT key
            return VK_LMENU, Menu;
            return VK_RMENU, Menu;

        case Qt::Key_Meta:
            return VK_LWIN, LWin // (5B) Left Windows key (Microsoft Natural keyboard)
        
        case Qt::Key_Meta: 
            return VK_RWIN, RWin; // (5C) Right Windows key (Natural keyboard)

        case Qt::Key_Menu
            VK_APPS, Apps //(5D) Applications key (Natural keyboard)
        
        case Qt::Key_Sleep
            VK_SLEEP, Sleep// (5F) Computer Sleep key
        
        case Qt::Key_CapsLock:
            return VK_CAPITAL, Capital; // (14) CAPS LOCK key
        
        case Qt::Key_Select:
            return VK_SELECT; // (29) SELECT key
        
        case Qt::Key_Cancel:
            return VK_Cancel, Cancel;

        case Qt::Key_Printer:
            return VK_PRINT; // (2A) PRINT key
        
        case Qt::Key_Execute:
            return VK_EXECUTE;// (2B) EXECUTE key

        case Qt::Key_Help:
            return VK_HELP; // (2F) HELP key


        // ASCII ALPHANUMERIC KEYS

        case Qt::Key_Space:
            return VK_SPACE; // (20) SPACEBAR
        
        case Qt::Key_0:
            return VK_0;    //  (30) 0 key
        case Qt::Key_1:
            return VK_1; //  (31) 1
        case Qt::Key_2:
            return VK_2; //  (32) 2
        case Qt::Key_3:
            return VK_3; //case '3'
        case Qt::Key_4:
            return VK_4;
        case Qt::Key_5:
            return VK_5; //  (35) 5 key
        case Qt::Key_6:
            return VK_6; //  (36) 6 key
        case Qt::Key_7:
            return VK_7; //  (37) 7 key
        case Qt::Key_8:
            return VK_8; //  (38) 8 key
        case Qt::Key_9:
            return VK_9; //  (39) 9 key 

        case Qt::Key_A:
            return VK_A; //  (41) A key case 'a': case 'A': return 0x41;
        case Qt::Key_B:
            return VK_B; //  (42) B key case 'b': case 'B': return 0x42;
        case Qt::Key_C:
            return VK_C; //  (43) C key case 'c': case 'C': return 0x43;
        case Qt::Key_D:
            return VK_D; //  (44) D key case 'd': case 'D': return 0x44;
        case Qt::Key_E:
            return VK_E; //  (45) E key case 'e': case 'E': return 0x45;
        case Qt::Key_F:
            return VK_F; //  (46) F key case 'f': case 'F': return 0x46;
        case Qt::Key_G:
            return VK_G; //  (47) G key case 'g': case 'G': return 0x47;
        case Qt::Key_H:
            return VK_H; //  (48) H key case 'h': case 'H': return 0x48;
        case Qt::Key_I:
            return VK_I; //  (49) I key case 'i': case 'I': return 0x49;
        case Qt::Key_J:
            return VK_J; //  (4A) J key case 'j': case 'J': return 0x4A;
        case Qt::Key_K:
            return VK_K; //  (4B) K key case 'k': case 'K': return 0x4B;
        case Qt::Key_L:
            return VK_L; //  (4C) L key case 'l': case 'L': return 0x4C;
        case Qt::Key_M:
            return VK_M; //  (4D) M key case 'm': case 'M': return 0x4D;
        case Qt::Key_N:
            return VK_N; //  (4E) N key case 'n': case 'N': return 0x4E;
        case Qt::Key_O:
            return VK_O; //  (4F) O key case 'o': case 'O': return 0x4F;
        case Qt::Key_P:
            return VK_P; //  (50) P key case 'p': case 'P': return 0x50;
        case Qt::Key_Q:
            return VK_Q; //  (51) Q key case 'q': case 'Q': return 0x51;
        case Qt::Key_R:
            return VK_R; //  (52) R key case 'r': case 'R': return 0x52;
        case Qt::Key_S:
            return VK_S; //  (53) S key case 's': case 'S': return 0x53;
        case Qt::Key_T:
            return VK_T; //  (54) T key case 't': case 'T': return 0x54;
        case Qt::Key_U:
            return VK_U; //  (55) U key case 'u': case 'U': return 0x55;
        case Qt::Key_V:
            return VK_V; //  (56) V key case 'v': case 'V': return 0x56;
        case Qt::Key_W:
            return VK_W; //  (57) W key case 'w': case 'W': return 0x57;
        case Qt::Key_X:
            return VK_X; //  (58) X key case 'x': case 'X': return 0x58;
        case Qt::Key_Y:
            return VK_Y; //  (59) Y key case 'y': case 'Y': return 0x59;
        case Qt::Key_Z:
            return VK_Z; //  (5A) Z key case 'z': case 'Z': return 0x5A;


        // ASCII PUNCTUATION KEYS

        // VK_OEM_1          //  0xBA ';:' for US
        // VK_OEM_PLUS       //  0xBB '+' any country
        // VK_OEM_COMMA      //  0xBC ',' any country
        // VK_OEM_MINUS      //  0xBD '-' any country
        // VK_OEM_PERIOD     //  0xBE '.' any country
        // VK_OEM_2          //  0xBF '/?' for US
        // VK_OEM_3          //  0xC0 '`~' for US
        // VK_OEM_4          //  0xDB '[{' for US
        // VK_OEM_5          //  0xDC '\|' for US
        // VK_OEM_6          //  0xDD ']}' for US
        // VK_OEM_7          //  0xDE ''"' for US
        // VK_OEM_AX         //  0xE1 'AX' key on Japanese AX kbd
        // VK_OEM_102        //  0xE2 "<>" or "\|" on RT 102-key kbd

        // SPECIAL KEYS

        // VK_PLAY (FA) Play key
        // VK_ZOOM (FB) Zoom key

        // VK_BROWSER_BACK (A6) Windows 2000/XP: Browser Back key
        // VK_BROWSER_FORWARD (A7) Windows 2000/XP: Browser Forward key
        // VK_BROWSER_REFRESH (A8) Windows 2000/XP: Browser Refresh key
        // VK_BROWSER_STOP (A9) Windows 2000/XP: Browser Stop key
        // VK_BROWSER_SEARCH (AA) Windows 2000/XP: Browser Search key
        // VK_BROWSER_FAVORITES (AB) Windows 2000/XP: Browser Favorites key
        // VK_BROWSER_HOME (AC) Windows 2000/XP: Browser Start and Home key
        // VK_VOLUME_MUTE (AD) Windows 2000/XP: Volume Mute key
        // VK_VOLUME_DOWN (AE) Windows 2000/XP: Volume Down key
        // VK_VOLUME_UP (AF) Windows 2000/XP: Volume Up key
        // VK_MEDIA_NEXT_TRACK (B0) Windows 2000/XP: Next Track key
        // VK_MEDIA_PREV_TRACK (B1) Windows 2000/XP: Previous Track key
        // VK_MEDIA_STOP (B2) Windows 2000/XP: Stop Media key
        // VK_MEDIA_PLAY_PAUSE (B3) Windows 2000/XP: Play/Pause Media key
        // VK_LAUNCH_MAIL (B4) Windows 2000/XP: Start Mail key
        // VK_LAUNCH_MEDIA_SELECT (B5) Windows 2000/XP: Select Media key
        // VK_LAUNCH_APP1 (B6) Windows 2000/XP: Start Application 1 key
        // VK_LAUNCH_APP2 (B7) Windows 2000/XP: Start Application 2 key



/*
static const uint KeyTbl[] = { // Keyboard mapping table
                        // Dec |  Hex | Windows Virtual key
    Qt::Key_unknown,    //   0   0x00
    Qt::Key_unknown,    //   1   0x01   VK_LBUTTON          | Left mouse button
    Qt::Key_unknown,    //   2   0x02   VK_RBUTTON          | Right mouse button
    Qt::Key_Cancel,     //   3   0x03   VK_CANCEL           | Control-Break processing
    Qt::Key_unknown,    //   4   0x04   VK_MBUTTON          | Middle mouse button
    Qt::Key_unknown,    //   5   0x05   VK_XBUTTON1         | X1 mouse button
    Qt::Key_unknown,    //   6   0x06   VK_XBUTTON2         | X2 mouse button
    Qt::Key_unknown,    //   7   0x07   -- unassigned --
    Qt::Key_Backspace,  //   8   0x08   VK_BACK             | BackSpace key
    Qt::Key_Tab,        //   9   0x09   VK_TAB              | Tab key
    Qt::Key_unknown,    //  10   0x0A   -- reserved --
    Qt::Key_unknown,    //  11   0x0B   -- reserved --
    Qt::Key_Clear,      //  12   0x0C   VK_CLEAR            | Clear key
    Qt::Key_Return,     //  13   0x0D   VK_RETURN           | Enter key
    Qt::Key_unknown,    //  14   0x0E   -- unassigned --
    Qt::Key_unknown,    //  15   0x0F   -- unassigned --
    Qt::Key_Shift,      //  16   0x10   VK_SHIFT            | Shift key
    Qt::Key_Control,    //  17   0x11   VK_CONTROL          | Ctrl key
    Qt::Key_Alt,        //  18   0x12   VK_MENU             | Alt key
    Qt::Key_Pause,      //  19   0x13   VK_PAUSE            | Pause key
    Qt::Key_CapsLock,   //  20   0x14   VK_CAPITAL          | Caps-Lock
    Qt::Key_unknown,    //  21   0x15   VK_KANA / VK_HANGUL | IME Kana or Hangul mode
    Qt::Key_unknown,    //  22   0x16   -- unassigned --
    Qt::Key_unknown,    //  23   0x17   VK_JUNJA            | IME Junja mode
    Qt::Key_unknown,    //  24   0x18   VK_FINAL            | IME final mode
    Qt::Key_unknown,    //  25   0x19   VK_HANJA / VK_KANJI | IME Hanja or Kanji mode
    Qt::Key_unknown,    //  26   0x1A   -- unassigned --
    Qt::Key_Escape,     //  27   0x1B   VK_ESCAPE           | Esc key
    Qt::Key_unknown,    //  28   0x1C   VK_CONVERT          | IME convert
    Qt::Key_unknown,    //  29   0x1D   VK_NONCONVERT       | IME non-convert
    Qt::Key_unknown,    //  30   0x1E   VK_ACCEPT           | IME accept
    Qt::Key_Mode_switch,//  31   0x1F   VK_MODECHANGE       | IME mode change request
    Qt::Key_Space,      //  32   0x20   VK_SPACE            | Spacebar
    Qt::Key_PageUp,     //  33   0x21   VK_PRIOR            | Page Up key
    Qt::Key_PageDown,   //  34   0x22   VK_NEXT             | Page Down key
    Qt::Key_End,        //  35   0x23   VK_END              | End key
    Qt::Key_Home,       //  36   0x24   VK_HOME             | Home key
    Qt::Key_Left,       //  37   0x25   VK_LEFT             | Left arrow key
    Qt::Key_Up,         //  38   0x26   VK_UP               | Up arrow key
    Qt::Key_Right,      //  39   0x27   VK_RIGHT            | Right arrow key
    Qt::Key_Down,       //  40   0x28   VK_DOWN             | Down arrow key
    Qt::Key_Select,     //  41   0x29   VK_SELECT           | Select key
    Qt::Key_Printer,    //  42   0x2A   VK_PRINT            | Print key
    Qt::Key_Execute,    //  43   0x2B   VK_EXECUTE          | Execute key
    Qt::Key_Print,      //  44   0x2C   VK_SNAPSHOT         | Print Screen key
    Qt::Key_Insert,     //  45   0x2D   VK_INSERT           | Ins key
    Qt::Key_Delete,     //  46   0x2E   VK_DELETE           | Del key
    Qt::Key_Help,       //  47   0x2F   VK_HELP             | Help key
    0,                  //  48   0x30   (VK_0)              | 0 key
    0,                  //  49   0x31   (VK_1)              | 1 key
    0,                  //  50   0x32   (VK_2)              | 2 key
    0,                  //  51   0x33   (VK_3)              | 3 key
    0,                  //  52   0x34   (VK_4)              | 4 key
    0,                  //  53   0x35   (VK_5)              | 5 key
    0,                  //  54   0x36   (VK_6)              | 6 key
    0,                  //  55   0x37   (VK_7)              | 7 key
    0,                  //  56   0x38   (VK_8)              | 8 key
    0,                  //  57   0x39   (VK_9)              | 9 key
    Qt::Key_unknown,    //  58   0x3A   -- unassigned --
    Qt::Key_unknown,    //  59   0x3B   -- unassigned --
    Qt::Key_unknown,    //  60   0x3C   -- unassigned --
    Qt::Key_unknown,    //  61   0x3D   -- unassigned --
    Qt::Key_unknown,    //  62   0x3E   -- unassigned --
    Qt::Key_unknown,    //  63   0x3F   -- unassigned --
    Qt::Key_unknown,    //  64   0x40   -- unassigned --
    0,                  //  65   0x41   (VK_A)              | A key
    0,                  //  66   0x42   (VK_B)              | B key
    0,                  //  67   0x43   (VK_C)              | C key
    0,                  //  68   0x44   (VK_D)              | D key
    0,                  //  69   0x45   (VK_E)              | E key
    0,                  //  70   0x46   (VK_F)              | F key
    0,                  //  71   0x47   (VK_G)              | G key
    0,                  //  72   0x48   (VK_H)              | H key
    0,                  //  73   0x49   (VK_I)              | I key
    0,                  //  74   0x4A   (VK_J)              | J key
    0,                  //  75   0x4B   (VK_K)              | K key
    0,                  //  76   0x4C   (VK_L)              | L key
    0,                  //  77   0x4D   (VK_M)              | M key
    0,                  //  78   0x4E   (VK_N)              | N key
    0,                  //  79   0x4F   (VK_O)              | O key
    0,                  //  80   0x50   (VK_P)              | P key
    0,                  //  81   0x51   (VK_Q)              | Q key
    0,                  //  82   0x52   (VK_R)              | R key
    0,                  //  83   0x53   (VK_S)              | S key
    0,                  //  84   0x54   (VK_T)              | T key
    0,                  //  85   0x55   (VK_U)              | U key
    0,                  //  86   0x56   (VK_V)              | V key
    0,                  //  87   0x57   (VK_W)              | W key
    0,                  //  88   0x58   (VK_X)              | X key
    0,                  //  89   0x59   (VK_Y)              | Y key
    0,                  //  90   0x5A   (VK_Z)              | Z key
    Qt::Key_Meta,       //  91   0x5B   VK_LWIN             | Left Windows  - MS Natural kbd
    Qt::Key_Meta,       //  92   0x5C   VK_RWIN             | Right Windows - MS Natural kbd
    Qt::Key_Menu,       //  93   0x5D   VK_APPS             | Application key-MS Natural kbd
    Qt::Key_unknown,    //  94   0x5E   -- reserved --
    Qt::Key_Sleep,      //  95   0x5F   VK_SLEEP
    Qt::Key_0,          //  96   0x60   VK_NUMPAD0          | Numeric keypad 0 key
    Qt::Key_1,          //  97   0x61   VK_NUMPAD1          | Numeric keypad 1 key
    Qt::Key_2,          //  98   0x62   VK_NUMPAD2          | Numeric keypad 2 key
    Qt::Key_3,          //  99   0x63   VK_NUMPAD3          | Numeric keypad 3 key
    Qt::Key_4,          // 100   0x64   VK_NUMPAD4          | Numeric keypad 4 key
    Qt::Key_5,          // 101   0x65   VK_NUMPAD5          | Numeric keypad 5 key
    Qt::Key_6,          // 102   0x66   VK_NUMPAD6          | Numeric keypad 6 key
    Qt::Key_7,          // 103   0x67   VK_NUMPAD7          | Numeric keypad 7 key
    Qt::Key_8,          // 104   0x68   VK_NUMPAD8          | Numeric keypad 8 key
    Qt::Key_9,          // 105   0x69   VK_NUMPAD9          | Numeric keypad 9 key
    Qt::Key_Asterisk,   // 106   0x6A   VK_MULTIPLY         | Multiply key
    Qt::Key_Plus,       // 107   0x6B   VK_ADD              | Add key
    Qt::Key_Comma,      // 108   0x6C   VK_SEPARATOR        | Separator key
    Qt::Key_Minus,      // 109   0x6D   VK_SUBTRACT         | Subtract key
    Qt::Key_Period,     // 110   0x6E   VK_DECIMAL          | Decimal key
    Qt::Key_Slash,      // 111   0x6F   VK_DIVIDE           | Divide key
    Qt::Key_F1,         // 112   0x70   VK_F1               | F1 key
    Qt::Key_F2,         // 113   0x71   VK_F2               | F2 key
    Qt::Key_F3,         // 114   0x72   VK_F3               | F3 key
    Qt::Key_F4,         // 115   0x73   VK_F4               | F4 key
    Qt::Key_F5,         // 116   0x74   VK_F5               | F5 key
    Qt::Key_F6,         // 117   0x75   VK_F6               | F6 key
    Qt::Key_F7,         // 118   0x76   VK_F7               | F7 key
    Qt::Key_F8,         // 119   0x77   VK_F8               | F8 key
    Qt::Key_F9,         // 120   0x78   VK_F9               | F9 key
    Qt::Key_F10,        // 121   0x79   VK_F10              | F10 key
    Qt::Key_F11,        // 122   0x7A   VK_F11              | F11 key
    Qt::Key_F12,        // 123   0x7B   VK_F12              | F12 key
    Qt::Key_F13,        // 124   0x7C   VK_F13              | F13 key
    Qt::Key_F14,        // 125   0x7D   VK_F14              | F14 key
    Qt::Key_F15,        // 126   0x7E   VK_F15              | F15 key
    Qt::Key_F16,        // 127   0x7F   VK_F16              | F16 key
    Qt::Key_F17,        // 128   0x80   VK_F17              | F17 key
    Qt::Key_F18,        // 129   0x81   VK_F18              | F18 key
    Qt::Key_F19,        // 130   0x82   VK_F19              | F19 key
    Qt::Key_F20,        // 131   0x83   VK_F20              | F20 key
    Qt::Key_F21,        // 132   0x84   VK_F21              | F21 key
    Qt::Key_F22,        // 133   0x85   VK_F22              | F22 key
    Qt::Key_F23,        // 134   0x86   VK_F23              | F23 key
    Qt::Key_F24,        // 135   0x87   VK_F24              | F24 key
    Qt::Key_unknown,    // 136   0x88   -- unassigned --
    Qt::Key_unknown,    // 137   0x89   -- unassigned --
    Qt::Key_unknown,    // 138   0x8A   -- unassigned --
    Qt::Key_unknown,    // 139   0x8B   -- unassigned --
    Qt::Key_unknown,    // 140   0x8C   -- unassigned --
    Qt::Key_unknown,    // 141   0x8D   -- unassigned --
    Qt::Key_unknown,    // 142   0x8E   -- unassigned --
    Qt::Key_unknown,    // 143   0x8F   -- unassigned --
    Qt::Key_NumLock,    // 144   0x90   VK_NUMLOCK          | Num Lock key
    Qt::Key_ScrollLock, // 145   0x91   VK_SCROLL           | Scroll Lock key
                        // Fujitsu/OASYS kbd --------------------
    0, //Qt::Key_Jisho, // 146   0x92   VK_OEM_FJ_JISHO     | 'Dictionary' key /
                        //              VK_OEM_NEC_EQUAL  = key on numpad on NEC PC-9800 kbd
    Qt::Key_Massyo,     // 147   0x93   VK_OEM_FJ_MASSHOU   | 'Unregister word' key
    Qt::Key_Touroku,    // 148   0x94   VK_OEM_FJ_TOUROKU   | 'Register word' key
    0, //Qt::Key_Oyayubi_Left,//149   0x95  VK_OEM_FJ_LOYA  | 'Left OYAYUBI' key
    0, //Qt::Key_Oyayubi_Right,//150  0x96  VK_OEM_FJ_ROYA  | 'Right OYAYUBI' key
    Qt::Key_unknown,    // 151   0x97   -- unassigned --
    Qt::Key_unknown,    // 152   0x98   -- unassigned --
    Qt::Key_unknown,    // 153   0x99   -- unassigned --
    Qt::Key_unknown,    // 154   0x9A   -- unassigned --
    Qt::Key_unknown,    // 155   0x9B   -- unassigned --
    Qt::Key_unknown,    // 156   0x9C   -- unassigned --
    Qt::Key_unknown,    // 157   0x9D   -- unassigned --
    Qt::Key_unknown,    // 158   0x9E   -- unassigned --
    Qt::Key_unknown,    // 159   0x9F   -- unassigned --
    Qt::Key_Shift,      // 160   0xA0   VK_LSHIFT           | Left Shift key
    Qt::Key_Shift,      // 161   0xA1   VK_RSHIFT           | Right Shift key
    Qt::Key_Control,    // 162   0xA2   VK_LCONTROL         | Left Ctrl key
    Qt::Key_Control,    // 163   0xA3   VK_RCONTROL         | Right Ctrl key
    Qt::Key_Alt,        // 164   0xA4   VK_LMENU            | Left Menu key
    Qt::Key_Alt,        // 165   0xA5   VK_RMENU            | Right Menu key
    Qt::Key_Back,       // 166   0xA6   VK_BROWSER_BACK     | Browser Back key
    Qt::Key_Forward,    // 167   0xA7   VK_BROWSER_FORWARD  | Browser Forward key
    Qt::Key_Refresh,    // 168   0xA8   VK_BROWSER_REFRESH  | Browser Refresh key
    Qt::Key_Stop,       // 169   0xA9   VK_BROWSER_STOP     | Browser Stop key
    Qt::Key_Search,     // 170   0xAA   VK_BROWSER_SEARCH   | Browser Search key
    Qt::Key_Favorites,  // 171   0xAB   VK_BROWSER_FAVORITES| Browser Favorites key
    Qt::Key_HomePage,   // 172   0xAC   VK_BROWSER_HOME     | Browser Start and Home key
    Qt::Key_VolumeMute, // 173   0xAD   VK_VOLUME_MUTE      | Volume Mute key
    Qt::Key_VolumeDown, // 174   0xAE   VK_VOLUME_DOWN      | Volume Down key
    Qt::Key_VolumeUp,   // 175   0xAF   VK_VOLUME_UP        | Volume Up key
    Qt::Key_MediaNext,  // 176   0xB0   VK_MEDIA_NEXT_TRACK | Next Track key
    Qt::Key_MediaPrevious, //177 0xB1   VK_MEDIA_PREV_TRACK | Previous Track key
    Qt::Key_MediaStop,  // 178   0xB2   VK_MEDIA_STOP       | Stop Media key
    Qt::Key_MediaPlay,  // 179   0xB3   VK_MEDIA_PLAY_PAUSE | Play/Pause Media key
    Qt::Key_LaunchMail, // 180   0xB4   VK_LAUNCH_MAIL      | Start Mail key
    Qt::Key_LaunchMedia,// 181   0xB5   VK_LAUNCH_MEDIA_SELECT Select Media key
    Qt::Key_Launch0,    // 182   0xB6   VK_LAUNCH_APP1      | Start Application 1 key
    Qt::Key_Launch1,    // 183   0xB7   VK_LAUNCH_APP2      | Start Application 2 key
    Qt::Key_unknown,    // 184   0xB8   -- reserved --
    Qt::Key_unknown,    // 185   0xB9   -- reserved --
    0,                  // 186   0xBA   VK_OEM_1            | ';:' for US
    0,                  // 187   0xBB   VK_OEM_PLUS         | '+' any country
    0,                  // 188   0xBC   VK_OEM_COMMA        | ',' any country
    0,                  // 189   0xBD   VK_OEM_MINUS        | '-' any country
    0,                  // 190   0xBE   VK_OEM_PERIOD       | '.' any country
    0,                  // 191   0xBF   VK_OEM_2            | '/?' for US
    0,                  // 192   0xC0   VK_OEM_3            | '`~' for US
    Qt::Key_unknown,    // 193   0xC1   -- reserved --
    Qt::Key_unknown,    // 194   0xC2   -- reserved --
    Qt::Key_unknown,    // 195   0xC3   -- reserved --
    Qt::Key_unknown,    // 196   0xC4   -- reserved --
    Qt::Key_unknown,    // 197   0xC5   -- reserved --
    Qt::Key_unknown,    // 198   0xC6   -- reserved --
    Qt::Key_unknown,    // 199   0xC7   -- reserved --
    Qt::Key_unknown,    // 200   0xC8   -- reserved --
    Qt::Key_unknown,    // 201   0xC9   -- reserved --
    Qt::Key_unknown,    // 202   0xCA   -- reserved --
    Qt::Key_unknown,    // 203   0xCB   -- reserved --
    Qt::Key_unknown,    // 204   0xCC   -- reserved --
    Qt::Key_unknown,    // 205   0xCD   -- reserved --
    Qt::Key_unknown,    // 206   0xCE   -- reserved --
    Qt::Key_unknown,    // 207   0xCF   -- reserved --
    Qt::Key_unknown,    // 208   0xD0   -- reserved --
    Qt::Key_unknown,    // 209   0xD1   -- reserved --
    Qt::Key_unknown,    // 210   0xD2   -- reserved --
    Qt::Key_unknown,    // 211   0xD3   -- reserved --
    Qt::Key_unknown,    // 212   0xD4   -- reserved --
    Qt::Key_unknown,    // 213   0xD5   -- reserved --
    Qt::Key_unknown,    // 214   0xD6   -- reserved --
    Qt::Key_unknown,    // 215   0xD7   -- reserved --
    Qt::Key_unknown,    // 216   0xD8   -- unassigned --
    Qt::Key_unknown,    // 217   0xD9   -- unassigned --
    Qt::Key_unknown,    // 218   0xDA   -- unassigned --
    0,                  // 219   0xDB   VK_OEM_4            | '[{' for US
    0,                  // 220   0xDC   VK_OEM_5            | '\|' for US
    0,                  // 221   0xDD   VK_OEM_6            | ']}' for US
    0,                  // 222   0xDE   VK_OEM_7            | ''"' for US
    0,                  // 223   0xDF   VK_OEM_8
    Qt::Key_unknown,    // 224   0xE0   -- reserved --
    Qt::Key_unknown,    // 225   0xE1   VK_OEM_AX           | 'AX' key on Japanese AX kbd
    Qt::Key_unknown,    // 226   0xE2   VK_OEM_102          | "<>" or "\|" on RT 102-key kbd
    Qt::Key_unknown,    // 227   0xE3   VK_ICO_HELP         | Help key on ICO
    Qt::Key_unknown,    // 228   0xE4   VK_ICO_00           | 00 key on ICO
    Qt::Key_unknown,    // 229   0xE5   VK_PROCESSKEY       | IME Process key
    Qt::Key_unknown,    // 230   0xE6   VK_ICO_CLEAR        |
    Qt::Key_unknown,    // 231   0xE7   VK_PACKET           | Unicode char as keystrokes
    Qt::Key_unknown,    // 232   0xE8   -- unassigned --
                        // Nokia/Ericsson definitions ---------------
    Qt::Key_unknown,    // 233   0xE9   VK_OEM_RESET
    Qt::Key_unknown,    // 234   0xEA   VK_OEM_JUMP
    Qt::Key_unknown,    // 235   0xEB   VK_OEM_PA1
    Qt::Key_unknown,    // 236   0xEC   VK_OEM_PA2
    Qt::Key_unknown,    // 237   0xED   VK_OEM_PA3
    Qt::Key_unknown,    // 238   0xEE   VK_OEM_WSCTRL
    Qt::Key_unknown,    // 239   0xEF   VK_OEM_CUSEL
    Qt::Key_unknown,    // 240   0xF0   VK_OEM_ATTN
    Qt::Key_unknown,    // 241   0xF1   VK_OEM_FINISH
    Qt::Key_unknown,    // 242   0xF2   VK_OEM_COPY
    Qt::Key_unknown,    // 243   0xF3   VK_OEM_AUTO
    Qt::Key_unknown,    // 244   0xF4   VK_OEM_ENLW
    Qt::Key_unknown,    // 245   0xF5   VK_OEM_BACKTAB
    Qt::Key_unknown,    // 246   0xF6   VK_ATTN             | Attn key
    Qt::Key_unknown,    // 247   0xF7   VK_CRSEL            | CrSel key
    Qt::Key_unknown,    // 248   0xF8   VK_EXSEL            | ExSel key
    Qt::Key_unknown,    // 249   0xF9   VK_EREOF            | Erase EOF key
    Qt::Key_Play,       // 250   0xFA   VK_PLAY             | Play key
    Qt::Key_Zoom,       // 251   0xFB   VK_ZOOM             | Zoom key
    Qt::Key_unknown,    // 252   0xFC   VK_NONAME           | Reserved
    Qt::Key_unknown,    // 253   0xFD   VK_PA1              | PA1 key
    Qt::Key_Clear,      // 254   0xFE   VK_OEM_CLEAR        | Clear key
    0
};

// Possible modifier states.
// NOTE: The order of these states match the order in QKeyMapperPrivate::updatePossibleKeyCodes()!
static const Qt::KeyboardModifiers ModsTbl[] = {
    Qt::NoModifier,                                             // 0
    Qt::ShiftModifier,                                          // 1
    Qt::ControlModifier,                                        // 2
    Qt::ControlModifier | Qt::ShiftModifier,                    // 3
    Qt::AltModifier,                                            // 4
    Qt::AltModifier | Qt::ShiftModifier,                        // 5
    Qt::AltModifier | Qt::ControlModifier,                      // 6
    Qt::AltModifier | Qt::ShiftModifier | Qt::ControlModifier,  // 7
    Qt::NoModifier,                                             // Fall-back to raw Key_*
};
*/
}