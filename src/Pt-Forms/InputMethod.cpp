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

#include <Pt/Forms/InputMethod.h>
#include <Pt/Forms/Application.h>
#include <Pt/Forms/Window.h>
#include <Pt/Forms/Control.h>
#include <Pt/Forms/FlowLayout.h>
#include <Pt/Forms/PushButton.h>
#include <iostream>

namespace Pt {

namespace Forms {

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


void InputMethod::begin(Control& control)
{
    _receiver = control.vid();

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


Visual* InputMethod::receiver() const
{
    if(_receiver == 0)
        return 0;

    Visual* visual = Application::instance().findVisual(_receiver);
    return visual;
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

//#define PT_WITH_TEST_IME 1

DefaultInputMethod::DefaultInputMethod()
: _window(0)
, _layout(0)
, _keyButtonA(0)
, _keyButtonB(0)
{
}


DefaultInputMethod::~DefaultInputMethod()
{
#ifdef PT_WITH_TEST_IME
    delete _keyButtonB;
    delete _keyButtonA;
    delete _layout;
    delete _window;
#endif
}


void DefaultInputMethod::onKeyPress()
{
#ifdef PT_WITH_TEST_IME
    std::clog << "KEY PRESS" << std::endl;

    KeyEvent kev;
    kev.setPress(Key(Key::A), 'a');

    sendEvent(kev);
#endif
}


void DefaultInputMethod::onBegin()
{
#ifdef PT_WITH_TEST_IME
    if( ! _window )
    {
        _window = new Window(0, WindowType::Default);
        _window->setAbove(true);
        _window->move( Gfx::PointF(500, 500) );
        _window->resize( Gfx::SizeF(200, 50) );
        _window->setTitle("Input Method");
    }

    if( ! _layout)
    {
        _layout = new FlowLayout();
        _window->setContent(_layout);
    }

    if( ! _keyButtonA)
    {
        _keyButtonA = new PushButton();
        _keyButtonA->setText("  a  ");
        _layout->addItem(*_keyButtonA);
        _keyButtonA->clicked() += Pt::slot(*this, &DefaultInputMethod::onKeyPress);
    }
    
    if( ! _keyButtonB)
    {
        _keyButtonB = new PushButton();
        _keyButtonB->setText("  b  ");
        _layout->addItem(*_keyButtonB);
    }

    _window->show(true);

    std::clog << "INPUT_METHOD BEGIN" << std::endl;
#endif
}

void DefaultInputMethod::onFinish()
{
#ifdef PT_WITH_TEST_IME
    if( _window->isVisible() )
        _window->show(false);
    
    std::clog << "INPUT_METHOD FINISH" << std::endl;
#endif
}


Window* DefaultInputMethod::onActiveWindow()
{
    return _window;

    //return 0;
}

} // namespace

} // namespace
