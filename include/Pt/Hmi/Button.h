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
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
   MA  02110-1301  USA
*/

#ifndef Pt_Hmi_Button_H
#define Pt_Hmi_Button_H

#include <Pt/Hmi/Label.h>
#include <Pt/Gfx/Image.h>

namespace Pt {

namespace Hmi {

class MouseEvent;
class TouchEvent;

class PT_HMI_API Button  : public Label
{
    public:
        Button();
    
        virtual ~Button();

        void setImage(const Gfx::Image& i) 
        {
            _image = i;
        }

        Alignment imageAlignment()  const
        {
           return _imageAlign;
        }

        void setImageAlignment(const Alignment& i) 
        {
           _imageAlign = i;
        }
             
        Signal<Button&>& clicked()
        {
            return _clicked;
        }

        Signal<Button&>& pressed()
        {
            return _pressed;
        }

        Signal<Button&>& released()
        {
            return _released;
        }

    protected:
        virtual void onMnemonic();

        virtual void onShortcut(const KeyEvent& kev);

        virtual void onActionKey(const KeyEvent& kev);

        virtual void onMouseEvent(const MouseEvent& ev);

        virtual void onEnterEvent( const EnterEvent& ev );

        virtual void onLeaveEvent(const LeaveEvent& ev );

        virtual void onTouchEvent(const TouchEvent& ev);

        virtual void onFocusEvent(const FocusEvent& ev);

    protected:       
        virtual void onPaintBackground(PaintSurface& surface, const Gfx::RectF& updateRect);

        virtual void onPaintContent(PaintSurface& surface, const Gfx::RectF& updateRect);
        
        virtual void onClicked(const Gfx::PointF& pos);

    private:
        Signal<Button&> _clicked;
        Signal<Button&> _pressed;
        Signal<Button&> _released;
        
        Gfx::Image _image;
        Alignment  _imageAlign;
        bool _isPressed;
};

} // namespace

} // namespace

#endif
