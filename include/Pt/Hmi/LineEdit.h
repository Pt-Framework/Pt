/* Copyright (C) 2015 Marc Boris Duerner 
   Copyright (C) 2015 Laurentiu-Gheorghe Crisan
  
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
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
  MA 02110-1301 USA
*/

#ifndef PT_HMI_LINEEDIT_H
#define PT_HMI_LINEEDIT_H

#include <Pt/Hmi/Control.h>
#include <Pt/String.h>

namespace Pt {

namespace Hmi {

// TODO: entered signal
//       validation signal
//       font, textColor

class PT_HMI_API LineEdit : public Control
{
    public:
        typedef Control Base;

        enum EchoMode
        {
            Normal = 0,
            Hidden = 1,
            Masked = 2
        };

        // TODO: separate classs
        enum Alignment
        {
            TopLeft,
            TopCenter,
            TopRight,
            MiddleLeft,
            MiddleCenter,
            MiddleRight,
            BottomLeft,
            BottomCenter,
            BottomRight
        };

    public:
        LineEdit();

        ~LineEdit();

        const Pt::String& text() const;

        void setText(const Pt::String& t);

        const Pt::String& displayText() const;

        const Pt::String& placeholderText() const;

        void setPlaceholderText(const Pt::String& s);

        EchoMode echoMode() const;

        void setEchoMode(EchoMode mode);

        Alignment textAlignment() const;

        void setTextAlignment(Alignment a);

        std::size_t cursorPosition() const;

        void setCursorPosition(std::size_t n);

        double hscroll() const
        { return _hscroll; }
        
        double halign() const
        { return _halign; }

    protected:
        virtual void onKeyEvent(const KeyEvent& ev);

        virtual void onMouseEvent(const MouseEvent& ev);

        virtual void onTouchEvent(const TouchEvent& ev);

        virtual void onResizeEvent(const ResizeEvent& ev);

        virtual void onPaint(PaintSurface& surface, const Gfx::RectF& rect);
    
    private:
        std::size_t xToCursor(double x);

        void layoutText();

    private:
        EchoMode      _echoMode;
        Alignment     _textAlignment;
        Pt::String    _text;
        Pt::String    _displayText;
        Pt::String    _placeholderText;
        std::size_t   _cursorPosition;
        double        _hscroll;
        double        _halign;
};

} // namespace

} // namespace

#endif
