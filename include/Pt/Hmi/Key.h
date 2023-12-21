#ifndef PT_HMI_KEYCODE_H
#define PT_HMI_KEYCODE_H

#include <Pt/Hmi/Api.h>
#include <Pt/Types.h>
#include <Pt/String.h>
#include <iostream>
#include <cassert>

namespace Pt {

namespace Hmi {

class Key
{
    public:
        // see also: https://w3c.github.io/uievents-code/
        enum Code
        {
            NoKey      = 0x0,

            // SPECIAL KEYS ASCII

            Backspace  = 0x08,

            Tab        = 0x09,

            Return     = 0x0D,

            Escape     = 0x1B,


            // CHARACTER KEYS (match unicode)

            // Space key.
            Space  = 32,

            // The 0 key.
            D0     = 48,
        
            // The 1 key.
            D1     = 49,
        
            // The 2 key.
            D2     = 50,
        
            // The 3 key.
            D3     = 51,
        
            // The 4 key.
            D4     = 52,
        
            // The 5 key.
            D5     = 53,
        
            // The 6 key.
            D6     = 54,
        
            // The 7 key.
            D7     = 55,
        
            // The 8 key.
            D8     = 56,
        
            // The 9 key.
            D9     = 57,
        
            // The A key.
            A      = 65,
        
            // The B key.
            B      = 66,
        
            // The C key.
            C      = 67,
        
            // The D key.
            D      = 68,
        
            // The E key.
            E      = 69,
        
            // The F key.
            F      = 70,
        
            // The G key.
            G      = 71,
        
            // The H key.
            H      = 72,
        
            // The I key.
            I      = 73,
        
            // The J key.
            J      = 74,
        
            // The K key.
            K      = 75,
        
            // The L key.
            L      = 76,
        
            // The M key.
            M      = 77,
        
            // The N key.
            N      = 78,
        
            // The O key.
            O      = 79,
        
            // The P key.
            P      = 80,
        
            // The Q key.
            Q      = 81,
        
            // The R key.
            R      = 82,
        
            // The S key.
            S      = 83,
        
            // The T key.
            T      = 84,
        
            // The U key.
            U      = 85,
        
            // The V key.
            V      = 86,
        
            // The W key.
            W      = 87,
        
            // The X key.
            X      = 88,
        
            // The Y key.
            Y      = 89,
        
            // The Z key.
            Z      = 90,


            // MODIFIERS

            Unknown        = 0x100000,
            ModifiersBegin = 0x100000,

            // The Shift Key.
            ShiftKey       = 0x100001, // 1

            // The Control Key.
            ControlKey     = 0x100002, // 2
        
            // The Alt Key.
            AltKey         = 0x100004, // 4

            // The Meta Key.
            MetaKey        = 0x100008, // 8

            // Reserved.
            Modifier5      = 0x100010, // 16
            
            // Reserved.
            Modifier6      = 0x100020, // 32
            
            // Reserved.
            Modifier7      = 0x100040, // 64
            
            // Reserved.
            Modifier8      = 0x100080, // 128

            // Reserved.
            Modifier9      = 0x100100, // 256


            // NAVIGATION KEYS 33 to 63

            ArrowLeft    = 0x100021,
                         
            ArrowRight   = 0x100022,
                         
            ArrowUp      = 0x100023,
                         
            ArrowDown    = 0x100024,
            

            // NUMPAD 65 to 127
                    
            // The NUM LOCK key.
            NumLock     = 0x100041,

            // The 0 key on the numeric keypad.
            NumPad0     = 0x100042,
        
            // The 1 key on the numeric keypad.
            NumPad1     = 0x100043,
        
            // The 2 key on the numeric keypad.
            NumPad2     = 0x100044,
        
            // The 3 key on the numeric keypad.
            NumPad3     = 0x100045,
        
            // The 4 key on the numeric keypad.
            NumPad4     = 0x100046,
        
            // The 5 key on the numeric keypad.
            NumPad5     = 0x100047,
        
            // The 6 key on the numeric keypad.
            NumPad6     = 0x100048,
        
            // The 7 key on the numeric keypad.
            NumPad7     = 0x100049,
        
            // The 8 key on the numeric keypad.
            NumPad8     = 0x10004A,
        
            // The 9 key on the numeric keypad.
            NumPad9     = 0x10004B,
        
            // The multiply key.
            Multiply    = 0x10004C,
        
            // The add key.
            Add         = 0x10004D,
            
            // The divide key.
            Divide      = 0x10004E,

            // The subtract key.
            Subtract    = 0x10004F,
            
            // The separator key.
            Separator   = 0x100050,
        
            // The decimal key.
            Decimal     = 0x100051,


            // FUNCTION KEYS 129 to 191
            
            // The F1 key.
            F1  = 0x100081, // 129
        
            // The F2 key.
            F2  = 0x100082,
        
            // The F3 key.
            F3  = 0x100083,
        
            // The F4 key.
            F4  = 0x100084,
        
            // The F5 key.
            F5  = 0x100085,
        
            // The F6 key.
            F6  = 0x100086,
        
            // The F7 key.
            F7  = 0x100087,
        
            // The F8 key.
            F8  = 0x100088,
        
            // The F9 key.
            F9  = 0x100089,
        
            // The F10 key.
            F10 = 0x10008A,
        
            // The F11 key.
            F11 = 0x10008B,
        
            // The F12 key.
            F12 = 0x10008C,
        
            // The F13 key.
            F13 = 0x10008D,
        
            // The F14 key.
            F14 = 0x10008E,
        
            // The F15 key.
            F15 = 0x10008F,
        
            // The F16 key.
            F16 = 0x100090,
        
            // The F17 key.
            F17 = 0x100091,
        
            // The F18 key.
            F18 = 0x100092,
        
            // The F19 key.
            F19 = 0x100093,
        
            // The F20 key.
            F20 = 0x100094,
        
            // The F21 key.
            F21 = 0x100095,
        
            // The F22 key.
            F22 = 0x100096,
        
            // The F23 key.
            F23 = 0x100097,
        
            // The F24 key.
            F24 = 0x100098,


            // SPECIAL KEYS 192 to 255

            Insert             = 0x1000C0, // 192
                               
            Delete             = 0x1000C1,
                               
            Home               = 0x1000C2,
                               
            End                = 0x1000C3,
                               
            PageUp             = 0x1000C4,
                               
            PageDown           = 0x1000C5,
                               
            CapsLock           = 0x1000C6,
                               
            PrintScreen        = 0x1000C7,
                               
            SysReq             = 0x1000C8,
                               
            ScrollLock         = 0x1000C9,
                               
            Pause              = 0x1000CA,
                               
            Break              = 0x1000CB, // 204
                               
            Clear              = 0x1000E0, // 224
                               
            Sleep              = 0x1000E1,
                               
            Select             = 0x1000E2,
                               
            Print              = 0x1000E3,
                               
            Execute            = 0x1000E4,
                               
            Help               = 0x1000E5,
                               
            AppsMenu           = 0x1000E6,
                               
            ModeChange         = 0x1000E7, // 231


            // SPECIAL APPLICATION KEYS 257 to 511
            
            Play               = 0x100101,
                               
            Zoom               = 0x100102,
                               
            BrowserBack        = 0x100103,
                               
            BrowserForward     = 0x100104,
                               
            BrowserRefresh     = 0x100105,
                               
            BrowserStop        = 0x100106,
                               
            BrowserSearch      = 0x100107,
                               
            BrowserFavorites   = 0x100108,
                               
            BrowserHome        = 0x100109,
                               
            VolumeMute         = 0x10010A,
                               
            VolumeDown         = 0x10010B,
                               
            VolumeUp           = 0x10010C,
                               
            MediaNext          = 0x10010D,
                               
            MediaPrev          = 0x10010E,
                               
            MediaStop          = 0x10010F,
                               
            MediaPlay          = 0x100111,
                               
            LaunchMail         = 0x100112,
                               
            LaunchMedia        = 0x100113,
                               
            LaunchApp1         = 0x100114,
                               
            LaunchApp2         = 0x100115,

            KeyMax = 0x10FFFF
        };

        enum Modifier
        {
            // No modifier pressed.
            NoModifier  = ModifiersBegin,

            // The SHIFT modifier key.
            Shift       = ShiftKey,

            // The CTRL modifier key.
            Control     = ControlKey,

            // The ALT modifier key.
            Alt         = AltKey,

            // The Meta modifier key.
            Meta        = MetaKey,

            ModifierMax = KeyMax
        };

        class Modifiers
        {
            friend Modifiers operator|(Modifier m1, Modifier m2);
            
            public:
                Modifiers()
                : _value(NoModifier)
                { }

                explicit Modifiers(Modifier m)
                : _value(m)
                { }

                Modifiers operator=(Modifier m)
                {
                    _value = m;
                    return *this;
                }

                void clear()
                {
                    _value = NoModifier;
                }

                bool empty() const
                {
                    return _value == NoModifier;
                }

                void add(Modifier m)
                {
                    _value |= m;
                }
                
                bool has(Modifier m) const
                {
                    return (_value & m) == m;
                }
                
                bool has(Modifiers m) const
                {
                    return (_value & m._value) == m._value;
                }

                Modifiers operator|(Modifier m) const
                {
                    return Modifiers(_value | m);
                }

                bool operator==(Modifier m) const
                {
                    return _value == static_cast<Pt::uint32_t>(m);
                }
                
                bool operator==(Modifiers m) const
                {
                    return _value == m._value;
                }
                
                bool operator!=(Modifier m) const
                {
                    return _value != static_cast<Pt::uint32_t>(m);
                }

                bool operator!=(Modifiers m) const
                {
                    return _value != m._value;
                }

                bool operator<(Modifiers m) const
                {
                    return _value < m._value;
                }

            private:
                explicit Modifiers(Pt::uint32_t value)
                : _value(value)
                { }

            private:
                Pt::uint32_t _value;
        };

    public:
        Key()
        : _code(NoKey)
        {}

        explicit Key(Pt::uint32_t c)
        : _code(c)
        {}

        Key(Modifier m, Pt::uint32_t c)
        : _code(c)
        , _modifier(m)
        {}

        Key(Modifiers m, Pt::uint32_t c)
        : _code(c)
        , _modifier( m )
        {}

        void clear()
        {
            _code = NoKey;
            _modifier = Modifiers();
        }

        void set(Pt::uint32_t c)
        {
            _code = c;
            _modifier = Modifiers();
        }

        void set(Modifier m, Pt::uint32_t c)
        {
            _code = c;
            _modifier = m;
        }

        void set(Modifiers m, Pt::uint32_t c)
        {
            _code = c;
            _modifier = m;
        }

        Pt::uint32_t code() const
        { 
            return _code; 
        }

        Modifiers modifiers() const
        { 
            return _modifier; 
        }

        bool operator==(const Key& k) const
        {
            return ! (*this != k);
        }

        bool operator!=(const Key& k) const
        {
            return _code != k._code || 
                   _modifier != k._modifier;
        }

        bool operator<(const Key& k) const
        {
            return _code < k._code || 
                  (_code == k._code && _modifier < k._modifier);
        }

        /** @brief Converts the key code to a string.

            The returned string can be used with the translator.
        */
        static Pt::String toString(Pt::uint32_t code)
        {
            if(code < ModifiersBegin)
                return Pt::String(1, code);

            switch(code)
            {
                case F1:         return "F1";
                case F2:         return "F2";
                case F3:         return "F3";
                case F4:         return "F4";
                case F5:         return "F5";
                case F6:         return "F6";
                case F7:         return "F7";
                case F8:         return "F8";
                case F9:         return "F9";
                case F10:        return "F10";
                case F11:        return "F11";
                case F12:        return "F12";
                case AltKey:     return "Alt";
                case ShiftKey:   return "Shift";
                case ControlKey: return "Ctrl";
                case MetaKey:    return "Meta";
                default:         return "";
            }

            return Pt::String();
        }

        bool empty() const
        {
            return _code  == Key::NoKey;
        }


    private:
        Pt::uint32_t _code;
        Modifiers _modifier;
};

inline Key::Modifiers operator|(Key::Modifier m1, Key::Modifier m2)
{
    Key::Modifiers m(m1);
    return m|m2;
}

} // namespace

} // namespace

#endif
