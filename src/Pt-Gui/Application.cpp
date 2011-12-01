/*
 * Copyright (C) 2006-2011 Marc Boris Drner
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "ApplicationImpl.h"

#include "Pt/Gui/Application.h"
#include "Pt/Gui/Event.h"
#include "Pt/Gui/Widget.h"

namespace {

Pt::Gui::Application*& getGuiAppPtr()
{
    static Pt::Gui::Application* _app = 0;
    return _app;
}

}

namespace Pt {

namespace Gui {

Application::Application(int argc, char** argv)
: System::Application(0, argc, argv)
, _appImpl(0) 
{ 
    _appImpl = new AppImpl();
 
    ::getGuiAppPtr() = this;

    this->init(_appImpl->loop());
    _appImpl->loop().event() += Pt::slot(*this, &Application::dispatchGuiEvent);
}


Application::~Application()
{
    delete _appImpl; 
}


Application& Application::instance()
{
    Application* app = ::getGuiAppPtr();
    if( ! app )
        throw std::logic_error("application not initialized");

    return *app;
}

 
void Application::dispatchGuiEvent(const Pt::Event& e)
{
    const Pt::Gui::Event* guiEvent = dynamic_cast<const Pt::Gui::Event*>(&e);

    if (guiEvent) {
        guiEvent->widget().event(*guiEvent);
    }
} 

} // namespace Gui

} // namespace Pt

