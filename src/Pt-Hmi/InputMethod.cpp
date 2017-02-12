/* Copyright (C) 2016 Marc Boris Duerner 
   Copyright (C) 2016 Laurentiu-Gheorghe Crisan

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

#include <Pt/Hmi/InputMethod.h>
#include <Pt/Hmi/Application.h>
#include <Pt/Hmi/Window.h>
#include <Pt/Hmi/Widget.h>
#include <iostream>

namespace Pt {

namespace Hmi {

InputMethod::InputMethod()
: _app(0)
, _receiver(0)
, _keyEvent(0)
, _isVisible(false)
, _isGrabbed(false)
{
}


InputMethod::~InputMethod()
{
    if(_app)
        _app->removeInputMethod(*this);
}


void InputMethod::grab()
{
    _isGrabbed = true;
}


void InputMethod::release()
{
    _isGrabbed = false;
}


Window* InputMethod::activeWindow()
{
    if( ! _isVisible )
        return 0;

    return onActiveWindow();
}


bool InputMethod::isVisible() const
{
    return _isVisible;
}


void InputMethod::begin(Widget& w)
{
    _receiver = w.vid();

    if(_isVisible)
        return;

    onShow(true);
    _isVisible = true;
}


void InputMethod::finish()
{
    if(_isGrabbed)
        return;

    _receiver = 0;

    if( ! _isVisible )
        return;

    onShow(false);
    _isVisible = false;
}

// TODO: this might not be neccessary if the virtual keyboard
//       is a text widget aka Widget::isTextInput returns true !!!
void InputMethod::finish(Widget& w)
{
    if(_isGrabbed)
        return;

    if( ! onFinish(w) )
      return;

    finish();
}


void InputMethod::sendKeyEvent(const KeyEvent& ev)
{
    if(_receiver == 0)
        return;

    _keyEvent = ev;
    _keyEvent.setId(_receiver);
     Application::instance().loop().commitEvent(_keyEvent);
}


void InputMethod::registerApplication(Application& app)
{
    _app = &app;
}
        

void InputMethod::unregisterApplication(Application&)
{
    _app = 0;
}


class KeyboardWindow : public Window
{
    public:
        KeyboardWindow()
        {
        }

        Pt::Signal<> keyPressed;

    protected:
        bool onMouseEvent(const MouseEvent& ev)
        {
            if( ev.isPress() )
            {
                keyPressed.send();
            }

            return true;
        }

        void onKeyEvent(const KeyEvent& ev)
        {
            std::clog << "KEY EVENT" << std::endl;
        }
        
};

DefaultInputMethod::DefaultInputMethod()
: _window(0)
{
    //_window = new KeyboardWindow();
    //_window->setTopMost(true);
    //_window->move( Gfx::PointF(500, 500) );
    //_window->resize( Gfx::SizeF(100, 100) );

    //_window->keyPressed += Pt::slot(*this, &DefaultInputMethod::onKeyPress);
}


DefaultInputMethod::~DefaultInputMethod()
{
    //delete _window;
}


Window* DefaultInputMethod::onActiveWindow()
{
    //return _window;
    return 0;
}


//bool DefaultInputMethod::mouseEvent(const MouseEvent& ev)
//{
//    Gfx::PointF pos = _window->fromScreen( ev.position() );
//    Gfx::RectF rect( _window->size() );
//    if( ! rect.contains(pos) )
//        return false;
//
//    MouseEvent ev2(ev);
//    ev2.setId( _window->vid() );
//    ev2.setPosition( _window->fromScreen( ev.position() ) );
//    
//    Application::instance().loop().commitEvent(ev2);
//    return true;
//}


void DefaultInputMethod::onKeyPress()
{
    std::clog << "KEY PRESS" << std::endl;
    KeyEvent kev(0);
    kev.setPress(Key(Key::A), 'a');

    sendKeyEvent(kev);
}


void DefaultInputMethod::onShow(bool show)
{
    if(show)
    {
        std::clog << "INPUTMETHOD SHOW" << std::endl;
    }
    else
    {
        std::clog << "INPUTMETHOD HIDE" << std::endl;
    }

    //_window->show(show);
}


bool DefaultInputMethod::onFinish(Widget& widget)
{
    return true;
}

} // namespace

} // namespace
