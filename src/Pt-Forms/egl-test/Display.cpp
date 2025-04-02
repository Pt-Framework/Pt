 /* Copyright (C) 2015 Marc Boris Duerner 
    Copyright (C) 2015 Laurentiu-Gheorghe Crisan
    Copyright (C) 2015 Ilja Maier
  
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

#include "Display.h"

#include <Pt/System/IOError.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>

namespace {

void setFlipped(int, Pt::uint32_t, Pt::uint32_t, Pt::uint32_t, void *flipping) 
{
    *static_cast<bool*>(flipping) = false;
}

void deleteBufferId(gbm_bo*, void* data) 
{
    Pt::uint32_t *i = static_cast<Pt::uint32_t*>(data);
    delete i;
}

Pt::uint32_t getBufferId(int fd, gbm_bo* bo)
{
  void* data = gbm_bo_get_user_data(bo);
  if( ! data ) 
  { 
      Pt::uint32_t newId = 0;
      
      if( drmModeAddFB(fd, gbm_bo_get_width(bo), gbm_bo_get_height(bo),
                       24, 32, gbm_bo_get_stride(bo),
                       gbm_bo_get_handle(bo).u32, &newId) )
          throw std::runtime_error("drmModeAddFB failed");

      data = new Pt::uint32_t(newId);
      gbm_bo_set_user_data(bo, data, &deleteBufferId);
  }

  Pt::uint32_t* id = static_cast<Pt::uint32_t*>(data);
  return *id;
}

} // namespace

namespace Pt {

namespace Forms {

Display::Display()
  : _fd(-1)
  , _crtc(0)
  , _display(0)
  , _surface(0)
  , _context(0)
  , _gbm_device(0)
  , _gbm_surface(0)
  , _width(0)
  , _height(0)
{
  init();
}


Display::~Display()
{
  eglDestroySurface(_display, _surface);
  eglDestroyContext(_display, _context);
  eglTerminate(_display);

  //gbm_bo_destroy(bo);
  if(_gbm_surface)
    gbm_surface_destroy(_gbm_surface);
  
  if(_gbm_device)
    gbm_device_destroy(_gbm_device);

  //drmClose(_fd);

  if( _fd != -1 )
    close(_fd);
}


void Display::init()
{
  std::cout << "Display::init" << std::endl;

  static const char *devices[] = { "vmwgfx", "i915", "radeon", "nouveau", "omapdrm", "exynos" };
  
  for( int i = 0; i < 6; ++i ) 
  {
    _fd = drmOpen(devices[i], NULL);
    if( _fd >= 0 )
      break;
  }

  if( _fd < 0 ) 
    _fd = open("/dev/dri/card0", O_RDWR | O_CLOEXEC);
  
  if( _fd < 0 )
    throw std::runtime_error("/dev/dri/card0");

  //
  // find DRM mode
  //
  drmModeRes* resources = drmModeGetResources(_fd);
  if( ! resources )
    throw std::runtime_error("drmModeGetResources failed");

  drmModeConnector* connector = 0;
  for( int i = 0; i < resources->count_connectors; ++i ) 
  {
    connector = drmModeGetConnector(_fd, resources->connectors[i]);
    if(  connector->connection == DRM_MODE_CONNECTED
      || connector->connection == DRM_MODE_UNKNOWNCONNECTION ) // For Devkit
      break;
    drmModeFreeConnector(connector);
    connector = 0;
  }
  if( ! connector ) 
  {
    drmModeFreeResources(resources);
    throw std::runtime_error("Could not get DRM connector.");
  }

  drmModeModeInfo* mode = 0;
  for( int i = 0; i < connector->count_modes; ++i ) 
  {
    if( connector->modes[i].type & DRM_MODE_TYPE_PREFERRED ) 
    {
      mode = &connector->modes[i];
      break;
    }
  }
  if( ! mode ) 
  {
    drmModeFreeConnector(connector);
    drmModeFreeResources(resources);
    throw std::runtime_error("Could not get DRM mode.");
  }

  _width  = mode->hdisplay;
  _height = mode->vdisplay;
  std::cout << "_width: " << _width << " height:" << _height << std::endl;

  for( int i = 0; i < connector->count_encoders; ++i) 
  {
    drmModeEncoder* encoder = drmModeGetEncoder(_fd, connector->encoders[i]);
    if( ! encoder )
      continue;

    // Break after first compatible CRTC
    for( int j = 0; j < resources->count_crtcs; ++j ) 
    {
      if( (encoder->possible_crtcs & (1 << j)) ) 
      {
        _crtc = resources->crtcs[j];
        break;
      }
    }
    if( _crtc )
      break;
  }
  
  if( ! _crtc )
    throw std::runtime_error("Could not determine CRTC.");

  //
  // create display from gbm device
  //
  _gbm_device = gbm_create_device(_fd);
  if( ! _gbm_device )
    throw std::runtime_error("Could not determine CRTC.");

  _display = eglGetDisplay( (EGLNativeDisplayType) _gbm_device);
  
  if( ! eglInitialize(_display, 0, 0) )
    throw std::runtime_error("Could not initialize EGL display.");
  
  eglBindAPI(EGL_OPENGL_ES_API);

  EGLint configAttr[] = 
  {   
    EGL_RED_SIZE,         8,
    EGL_GREEN_SIZE,       8,
    EGL_BLUE_SIZE,        8,
    EGL_ALPHA_SIZE,       8,
    EGL_DEPTH_SIZE,       16, 
    EGL_RENDERABLE_TYPE,  EGL_OPENGL_ES2_BIT, 
    //EGL_SURFACE_TYPE,     EGL_PIXMAP_BIT,
    EGL_SURFACE_TYPE,     EGL_WINDOW_BIT,
    EGL_NONE
  };

  EGLConfig   config;
  EGLint      num_config;

  if( ! eglChooseConfig( _display, configAttr, &config, 1, &num_config ) ) 
    throw std::runtime_error("Failed to choose config");

  if( num_config != 1 )
    throw std::runtime_error("Didn't get exactly one config");

  EGLint contextAttr[] = 
  { 
    EGL_CONTEXT_CLIENT_VERSION, 2, 
    EGL_NONE 
  };
  
  _context = eglCreateContext(_display, config, EGL_NO_CONTEXT, contextAttr);
  if( ! _context )
    throw std::runtime_error("Could not create EGL context." );

  //
  // create surface from gbm
  //

  _gbm_surface = gbm_surface_create(_gbm_device, mode->hdisplay, mode->vdisplay,
    GBM_FORMAT_XRGB8888, GBM_BO_USE_SCANOUT | GBM_BO_USE_RENDERING);
  if( ! _gbm_surface )
    throw std::runtime_error("Could not initialize GBM surface.");
  
  _surface = eglCreateWindowSurface(_display, config, (EGLNativeWindowType)_gbm_surface, 0);
  if( _surface == EGL_NO_SURFACE )
    throw std::runtime_error("Could not create EGL window surface.");
  
  //gbm_bo* bo = gbm_bo_create(_gbm_device,	
  //                           mode->hdisplay, mode->vdisplay, 
  //                           GBM_FORMAT_XRGB8888, 
  //                           GBM_BO_USE_SCANOUT | GBM_BO_USE_RENDERING);

  //_surface = eglCreatePixmapSurface(_display, config, (EGLNativePixmapType)bo, 0);
  //if( _surface == EGL_NO_SURFACE )
  //  throw std::runtime_error("Could not create EGL window surface.");

  if( ! eglMakeCurrent(_display, _surface, _surface, _context) )
    throw std::runtime_error("Could not set the current EGL context.");
  
  gbm_bo* bo = gbm_surface_lock_front_buffer(_gbm_surface);
  if( ! bo )
    throw std::runtime_error("gbm_surface_lock_front_buffer failed");

  Pt::uint32_t id = getBufferId(_fd, bo);

  gbm_surface_release_buffer(_gbm_surface, bo);

  if( drmModeSetCrtc(_fd, _crtc, id, 0, 0, &connector->connector_id, 1, mode))
    throw std::runtime_error("drmModeSetCrtc failed.");
}


void Display::updateScreen()
{
  if( ! eglSwapBuffers(_display, _surface) )
    throw std::runtime_error("Could not perform initial buffer swap.");
  
  gbm_bo* bo = gbm_surface_lock_front_buffer(_gbm_surface);
  if( ! bo )
  {
    std::cout << "Display::updateScreen: gbm_surface_lock_front_buffer failed" << std::endl;
    throw std::runtime_error("gbm_surface_lock_front_buffer failed");
  }

  bool flipping = true;
  Pt::uint32_t id = getBufferId(_fd, bo);
  drmModePageFlip(_fd, _crtc, id, DRM_MODE_PAGE_FLIP_EVENT, &flipping);
  
  drmEventContext drmEvent = 
  {
      DRM_EVENT_CONTEXT_VERSION,
      0,
      &setFlipped
  };

  while( flipping )
    drmHandleEvent(_fd, &drmEvent);

  gbm_surface_release_buffer(_gbm_surface, bo);

}

} // namespace Forms

} // namespace Pt
