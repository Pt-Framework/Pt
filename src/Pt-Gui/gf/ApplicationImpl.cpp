/***************************************************************************
 *   Copyright (C) 2006 Marc Boris Duerner                                 *
 *   Copyright (C) 2006 Aloysius Indrayanto                                *
 *   Copyright (C) 2006 Sebastian Pieck                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/


#include "ApplicationImpl.h"
#include <Pt/System/MutexLock.h>
#include <iostream>
#include <cerrno>
#include <stdexcept>

#define BITS_PER_LONG (sizeof(long) * 8)
#define NBITS(x) ((((x)-1)/BITS_PER_LONG)+1)
#define OFF(x)  ((x)%BITS_PER_LONG)
#define BIT(x)  (1UL<<OFF(x))
#define LONG(x) ((x)/BITS_PER_LONG)
#define test_bit(bit, array)    ((array[LONG(bit)] >> OFF(bit)) & 1)


namespace Pt {

namespace Gui {

InputHandler::InputHandler()
: _exit(false)
, _highestFd(0)
, _fd1(-1)
, _fd2(-1)
, _fd3(-1)
{

}


InputHandler::~InputHandler()
{
    if(_fd1 > 0)
        close(_fd1);

    if(_fd2 > 0)
        close(_fd2);

    if(_fd3 > 0)
        close(_fd3);
}


void InputHandler::run()
{
    while( !_exit )
    {
        sleep(1);
    }

}


GfEventLoop::GfEventLoop()
: _mutex(Pt::System::Mutex::Normal)
{
    connect(_input.keyEvent, *this, &GfEventLoop::handleKeyEvent);
    _input.start();
}


GfEventLoop::~GfEventLoop()
{
    _input.stop();
    _input.wait();
}


void GfEventLoop::registerWidget(Widget& widget)
{
    Pt::System::MutexLock lock(_mutex);
    _widgets.push_back(&widget);
}


void GfEventLoop::unregisterWidget(Widget& widget)
{
    Pt::System::MutexLock lock(_mutex);
    _widgets.remove(&widget);
}


void GfEventLoop::handleKeyEvent(int keycode, int value)
{
}




Screen::Screen()
: m_bufferSize(0)
{
    gf_display_t        display;
    gf_layer_t          layer;


    if (gf_dev_attach(&m_gdev, GF_DEVICE_INDEX(0), &m_gdev_info) != GF_ERR_OK)
        { throw std::logic_error("gf_dev_attach() failed. Gf-Server started?" + PT_SOURCEINFO); }



    for (Pt::uint32_t i = 0; i < m_gdev_info.ndisplays; i++ )
    {
        if (gf_display_attach(&display, m_gdev, i, &m_display_info) != GF_ERR_OK)
           { throw std::logic_error("gf_display_attach() failed."+ PT_SOURCEINFO); }

        if (display != NULL)
        {
                if (gf_layer_attach(&layer, display, m_display_info.main_layer_index, 0) != GF_ERR_OK)
                   { throw std::logic_error("gf_layer_attach() failed."+ PT_SOURCEINFO); }

                if (gf_surface_create(&m_offscreenSurface, m_gdev, m_display_info.xres, m_display_info.yres, GF_FORMAT_PKLE_ARGB1555, NULL, 0) != GF_ERR_OK)
                    { throw std::logic_error("gf_surface_create_layer() failed." + PT_SOURCEINFO); }


                if (gf_surface_create_layer(&m_layerSurface, &layer, 1, 0, m_display_info.xres, m_display_info.yres, GF_FORMAT_PKLE_ARGB1555, NULL, 0) != GF_ERR_OK)
                    { throw  std::logic_error("gf_surface_create_layer failed." + PT_SOURCEINFO); }

                gf_layer_set_surfaces(layer, &m_layerSurface, 1);


                if (gf_layer_update(layer, GF_LAYER_UPDATE_NO_WAIT_VSYNC) != GF_ERR_OK)
                  { throw std::logic_error("gf_layer_update failed!" + PT_SOURCEINFO); }


                if (gf_context_create(&m_context) != GF_ERR_OK)
                  { throw std::logic_error("gf_context_create failed." + PT_SOURCEINFO); }

                if (gf_context_set_surface(m_context, m_layerSurface) != GF_ERR_OK)
                   { throw std::logic_error("gf_context_set_surface failed." + PT_SOURCEINFO); }

         } //if


    } // for


}


Screen::~Screen()
{
    GfEventLoop::instance().exit();
    gf_dev_detach(m_gdev);
}

ApplicationImpl::ApplicationImpl(Application& app)
{
    Screen::instance();
    connect(GfEventLoop::instance().event, app.event);
}


ApplicationImpl::~ApplicationImpl()
{
}


void ApplicationImpl::commitEvent(const Pt::Event& event)
{
    GfEventLoop::instance().commitEvent(event);
}


void ApplicationImpl::queueEvent(const Pt::Event& event)
{
    GfEventLoop::instance().queueEvent(event);
}


void ApplicationImpl::processEvents()
{
    GfEventLoop::instance().processEvents();
}


int ApplicationImpl::run()
{
   GfEventLoop::instance().run();
   return true;
}


void ApplicationImpl::wake()
{
    GfEventLoop::instance().wake();
}


void ApplicationImpl::exit()
{
    GfEventLoop::instance().exit();
}

} // namespace Gui

} // namespace Pt

