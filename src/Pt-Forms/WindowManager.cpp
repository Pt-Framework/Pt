/* Copyright (C) 2015 Laurentiu-Gheorghe Crisan
   Copyright (C) 2015 Marc Boris Duerner
  
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
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  
  02110-1301 USA
*/

#include <Pt/Forms/WindowManager.h>
#include <Pt/Forms/WindowFrame.h>
#include <Pt/Forms/Application.h>
#include <Pt/Forms/Window.h>
#include <Pt/Forms/ResizeEvent.h>

namespace Pt {

namespace Forms {

WindowManager::WindowManager()
{
}


WindowManager::~WindowManager()
{
}


void WindowManager::relayout()
{
    onRequestRelayout();
}


void WindowManager::onRequestRelayout()
{
}


void WindowManager::onConnect(Screen& screen)
{
    Base::onConnect(screen);
}


void WindowManager::onDisconnect()
{
    Base::onDisconnect();
}


void WindowManager::onProcessPaintEvent(const PaintEvent& ev)
{
    Base::onProcessPaintEvent(ev);
}


void WindowManager::onPaintEvent(const PaintEvent& ev)
{
    Base::onPaintEvent(ev);
}


void WindowManager::onInit(WindowFrame& w)
{
    Screen* screen = this->screen();
    if(screen)
    {
        w.onConnect(*screen);
    }
}


void WindowManager::onRelease(WindowFrame& w)
{
    Screen* screen = this->screen();
    if(screen)
    {
        w.onDisconnect();
    }
}

} // namespace

} // namespace
