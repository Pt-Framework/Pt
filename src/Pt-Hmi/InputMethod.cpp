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
#include <Pt/Hmi/PushButton.h>
#include <iostream>

namespace Pt {

namespace Hmi {

InputMethod::InputMethod()
: _app(0)
, _receiver(0)
, _isVisible(false)
{
}


InputMethod::~InputMethod()
{
    if(_app)
        _app->removeInputMethod(*this);
}


bool InputMethod::isVisible() const
{
    return _isVisible;
}


Window* InputMethod::activeWindow()
{
    if( ! _isVisible )
        return 0;

    return onActiveWindow();
}


void InputMethod::begin(Widget& w)
{
    _receiver = w.vid();

    if(_isVisible)
        return;

    onBegin();
    _isVisible = true;
}


void InputMethod::finish()
{
    _receiver = 0;

    if( ! _isVisible )
        return;

    onFinish();
    _isVisible = false;
}


void InputMethod::sendEvent(const KeyEvent& ev)
{
    if(_receiver == 0)
        return;

    Visual* visual = Application::instance().findVisual(_receiver);
    if(visual)
    {
        _keyEvent = ev;
        _keyEvent.setVisual(visual);
         Application::instance().loop().commitEvent(_keyEvent);
    }
}


void InputMethod::registerApplication(Application& app)
{
    _app = &app;
}
        

void InputMethod::unregisterApplication(Application&)
{
    _app = 0;
}


DefaultInputMethod::DefaultInputMethod()
: _window(0)
, _keyButton(0)
{

}


DefaultInputMethod::~DefaultInputMethod()
{
    delete _keyButton;
    delete _window;
}


void DefaultInputMethod::onKeyPress()
{
    //std::clog << "KEY PRESS" << std::endl;
    KeyEvent kev;
    kev.setPress(Key(Key::A), 'a');

    sendEvent(kev);
}


void DefaultInputMethod::onBegin()
{
    if( ! _window )
    {
        _window = new Window();
        _window->setTopMost(true);
        _window->move( Gfx::PointF(500, 500) );
        _window->resize( Gfx::SizeF(100, 100) );
        _window->setTitle("Input Method");
    }

    if( ! _keyButton)
    {
        _keyButton = new PushButton();
        _keyButton->setText("a");
        _window->setContent(_keyButton);
        _keyButton->clicked() += Pt::slot(*this, &DefaultInputMethod::onKeyPress);
    }
    
    _window->show(true);

    //std::clog << "INPUT_METHOD BEGIN" << std::endl;
}

void DefaultInputMethod::onFinish()
{
    if( _window->isVisible() )
        _window->show(false);
    
    //std::clog << "INPUT_METHOD FINISH" << std::endl;
}


Window* DefaultInputMethod::onActiveWindow()
{
    return _window;
}

} // namespace

} // namespace
