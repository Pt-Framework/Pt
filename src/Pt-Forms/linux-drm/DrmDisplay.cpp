/* Copyright (C) 2026 Marc Boris Duerner

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

#include "DrmDisplay.h"
#include "DrmBuffer.h"

#include <Pt/Forms/Application.h>
#include <Pt/Gfx/Rgb32.h>
#include <Pt/Gfx/Argb32.h>
#include <Pt/System/Application.h>

#include <stdexcept>
#include <iostream>
#include <cstring>

#include <unistd.h>
#include <sys/mman.h>
#include <drm_fourcc.h>

namespace Pt {

namespace Forms {

DrmDisplay::DrmDisplay(int drmFd)
: _fd(drmFd)
, _connId(0)
, _crtcId(0)
, _crtcIndex(0)
, _mode()
, _savedCrtc(0)
, _connector(0)
, _propConnCrtcId(0)
, _propCrtcActive(0)
, _propCrtcModeId(0)
, _propPlaneFbId(0)
, _propPlaneCrtcId(0)
, _propPlaneSrcX(0)
, _propPlaneSrcY(0)
, _propPlaneSrcW(0)
, _propPlaneSrcH(0)
, _propPlaneCrtcX(0)
, _propPlaneCrtcY(0)
, _propPlaneCrtcW(0)
, _propPlaneCrtcH(0)
, _planeId(0)
, _modeBlobId(0)
, _backIndex(0)
, _flipPending(false)
, _format(0)
{
    if( _fd < 0 )
        throw std::runtime_error("invalid DRM file descriptor");

    findConnector();
    findCrtc();
    setupBuffers();
    setInitialMode();

    _format = new Gfx::Rgb32();

    std::clog << "DRM display: " << width() << "x" << height()
              << ", pitch: " << pitch()
              << ", connector: " << _connId
              << ", crtc: " << _crtcId << std::endl;
}


DrmDisplay::~DrmDisplay()
{
    shutdown();
}


void DrmDisplay::shutdown()
{
    // restore saved CRTC
    if( _savedCrtc )
    {
        if( _fd >= 0 )
        {
            drmModeSetCrtc(_fd, _savedCrtc->crtc_id, _savedCrtc->buffer_id,
                           _savedCrtc->x, _savedCrtc->y, &_connId, 1,
                           &_savedCrtc->mode);
        }

        drmModeFreeCrtc(_savedCrtc);
        _savedCrtc = 0;
    }

    if( _modeBlobId )
    {
        if( _fd >= 0 )
            drmModeDestroyPropertyBlob(_fd, _modeBlobId);

        _modeBlobId = 0;
    }

    _buffers[0].destroy();
    _buffers[1].destroy();

    if( _connector )
    {
        drmModeFreeConnector(_connector);
        _connector = 0;
    }

    delete _format;
    _format = 0;

    _flipPending = false;
}


size_t DrmDisplay::width() const
{
    return _mode.hdisplay;
}


size_t DrmDisplay::height() const
{
    return _mode.vdisplay;
}


const Gfx::ImageFormat& DrmDisplay::format() const
{
    return *_format;
}


int DrmDisplay::fd() const
{
    return _fd;
}


uint32_t DrmDisplay::pitch() const
{
    return _buffers[_backIndex].pitch();
}


Pt::uint8_t* DrmDisplay::backBuffer()
{
    return _buffers[_backIndex].map();
}


const Pt::uint8_t* DrmDisplay::backBuffer() const
{
    return _buffers[_backIndex].map();
}


void DrmDisplay::onFlipComplete()
{
    _flipPending = false;
}


void DrmDisplay::findConnector()
{
    drmModeRes* res = drmModeGetResources(_fd);
    if( ! res )
        throw std::runtime_error("drmModeGetResources failed");

    std::string wanted = Pt::System::Application::getEnvVar("PT_DRM_CONNECTOR");

    _connector = 0;

    for(int i = 0; i < res->count_connectors; ++i)
    {
        drmModeConnector* conn = drmModeGetConnector(_fd, res->connectors[i]);
        if( ! conn )
            continue;

        if( conn->connection != DRM_MODE_CONNECTED || conn->count_modes == 0 )
        {
            drmModeFreeConnector(conn);
            continue;
        }

        // if a specific connector is requested via env var, match by name
        if( ! wanted.empty() )
        {
            // build connector name like "HDMI-A-1"
            const char* typeName = drmModeGetConnectorTypeName(conn->connector_type);
            std::string name;
            if( typeName )
            {
                name = typeName;
                name += "-";
                char buf[16];
                snprintf(buf, sizeof(buf), "%u", conn->connector_type_id);
                name += buf;
            }

            if( name != wanted )
            {
                drmModeFreeConnector(conn);
                continue;
            }
        }

        _connector = conn;
        _connId = conn->connector_id;
        break;
    }

    drmModeFreeResources(res);

    if( ! _connector )
        throw std::runtime_error("no connected DRM connector found");

    // use preferred mode or first available mode
    _mode = _connector->modes[0];

    for(int i = 0; i < _connector->count_modes; ++i)
    {
        if( _connector->modes[i].type & DRM_MODE_TYPE_PREFERRED )
        {
            _mode = _connector->modes[i];
            break;
        }
    }
}


void DrmDisplay::findCrtc()
{
    drmModeRes* res = drmModeGetResources(_fd);
    if( ! res )
        throw std::runtime_error("drmModeGetResources failed");

    // try the encoder currently attached to the connector
    if( _connector->encoder_id )
    {
        drmModeEncoder* enc = drmModeGetEncoder(_fd, _connector->encoder_id);
        if( enc )
        {
            for(int i = 0; i < res->count_crtcs; ++i)
            {
                if( res->crtcs[i] == enc->crtc_id )
                {
                    _crtcId = enc->crtc_id;
                    _crtcIndex = i;
                    drmModeFreeEncoder(enc);
                    drmModeFreeResources(res);

                    _savedCrtc = drmModeGetCrtc(_fd, _crtcId);
                    return;
                }
            }

            drmModeFreeEncoder(enc);
        }
    }

    // find a CRTC that can drive this connector
    for(int i = 0; i < _connector->count_encoders; ++i)
    {
        drmModeEncoder* enc = drmModeGetEncoder(_fd, _connector->encoders[i]);
        if( ! enc )
            continue;

        for(int j = 0; j < res->count_crtcs; ++j)
        {
            if( enc->possible_crtcs & (1u << j) )
            {
                _crtcId = res->crtcs[j];
                _crtcIndex = j;
                drmModeFreeEncoder(enc);
                drmModeFreeResources(res);

                _savedCrtc = drmModeGetCrtc(_fd, _crtcId);
                return;
            }
        }

        drmModeFreeEncoder(enc);
    }

    drmModeFreeResources(res);
    throw std::runtime_error("no suitable CRTC found for connector");
}


void DrmDisplay::setupBuffers()
{
    _buffers[0].create(_fd, _mode.hdisplay, _mode.vdisplay);
    _buffers[1].create(_fd, _mode.hdisplay, _mode.vdisplay);
    _backIndex = 1;
}


uint32_t DrmDisplay::getPropertyId(uint32_t objectId, uint32_t objectType,
                                   const char* name) const
{
    drmModeObjectProperties* props = drmModeObjectGetProperties(_fd, objectId, objectType);
    if( ! props )
        return 0;

    uint32_t propId = 0;

    for(uint32_t i = 0; i < props->count_props; ++i)
    {
        drmModePropertyRes* prop = drmModeGetProperty(_fd, props->props[i]);
        if( ! prop )
            continue;

        if( std::strcmp(prop->name, name) == 0 )
            propId = prop->prop_id;

        drmModeFreeProperty(prop);

        if( propId )
            break;
    }

    drmModeFreeObjectProperties(props);
    return propId;
}


void DrmDisplay::setInitialMode()
{
    // find the primary plane for our CRTC
    drmModePlaneRes* planeRes = drmModeGetPlaneResources(_fd);
    if( ! planeRes )
        throw std::runtime_error("drmModeGetPlaneResources failed");

    _planeId = 0;

    for(uint32_t i = 0; i < planeRes->count_planes; ++i)
    {
        drmModePlane* plane = drmModeGetPlane(_fd, planeRes->planes[i]);
        if( ! plane )
            continue;

        if( ! (plane->possible_crtcs & (1u << _crtcIndex)) )
        {
            drmModeFreePlane(plane);
            continue;
        }

        // check if this is a primary plane
        uint32_t typeId = getPropertyId(plane->plane_id,
                                        DRM_MODE_OBJECT_PLANE, "type");
        if( typeId )
        {
            drmModeObjectProperties* props = drmModeObjectGetProperties(
                _fd, plane->plane_id, DRM_MODE_OBJECT_PLANE);

            if( props )
            {
                for(uint32_t j = 0; j < props->count_props; ++j)
                {
                    if( props->props[j] == typeId &&
                        props->prop_values[j] == DRM_PLANE_TYPE_PRIMARY )
                    {
                        _planeId = plane->plane_id;
                        break;
                    }
                }

                drmModeFreeObjectProperties(props);
            }
        }

        drmModeFreePlane(plane);

        if( _planeId )
            break;
    }

    drmModeFreePlaneResources(planeRes);

    if( ! _planeId )
        throw std::runtime_error("no primary plane found for CRTC");

    // get atomic property IDs
    _propConnCrtcId = getPropertyId(_connId, DRM_MODE_OBJECT_CONNECTOR, "CRTC_ID");
    _propCrtcActive = getPropertyId(_crtcId, DRM_MODE_OBJECT_CRTC, "ACTIVE");
    _propCrtcModeId = getPropertyId(_crtcId, DRM_MODE_OBJECT_CRTC, "MODE_ID");
    _propPlaneFbId = getPropertyId(_planeId, DRM_MODE_OBJECT_PLANE, "FB_ID");
    _propPlaneCrtcId = getPropertyId(_planeId, DRM_MODE_OBJECT_PLANE, "CRTC_ID");
    _propPlaneSrcX = getPropertyId(_planeId, DRM_MODE_OBJECT_PLANE, "SRC_X");
    _propPlaneSrcY = getPropertyId(_planeId, DRM_MODE_OBJECT_PLANE, "SRC_Y");
    _propPlaneSrcW = getPropertyId(_planeId, DRM_MODE_OBJECT_PLANE, "SRC_W");
    _propPlaneSrcH = getPropertyId(_planeId, DRM_MODE_OBJECT_PLANE, "SRC_H");
    _propPlaneCrtcX = getPropertyId(_planeId, DRM_MODE_OBJECT_PLANE, "CRTC_X");
    _propPlaneCrtcY = getPropertyId(_planeId, DRM_MODE_OBJECT_PLANE, "CRTC_Y");
    _propPlaneCrtcW = getPropertyId(_planeId, DRM_MODE_OBJECT_PLANE, "CRTC_W");
    _propPlaneCrtcH = getPropertyId(_planeId, DRM_MODE_OBJECT_PLANE, "CRTC_H");

    // create mode blob
    if( drmModeCreatePropertyBlob(_fd, &_mode, sizeof(_mode), &_modeBlobId) < 0 )
        throw std::runtime_error("drmModeCreatePropertyBlob failed");

    // perform initial atomic mode set (blocking)
    drmModeAtomicReq* req = drmModeAtomicAlloc();
    if( ! req )
        throw std::runtime_error("drmModeAtomicAlloc failed");

    drmModeAtomicAddProperty(req, _connId, _propConnCrtcId, _crtcId);
    drmModeAtomicAddProperty(req, _crtcId, _propCrtcActive, 1);
    drmModeAtomicAddProperty(req, _crtcId, _propCrtcModeId, _modeBlobId);

    uint32_t frontFbId = _buffers[0].fbId();
    drmModeAtomicAddProperty(req, _planeId, _propPlaneFbId, frontFbId);
    drmModeAtomicAddProperty(req, _planeId, _propPlaneCrtcId, _crtcId);
    drmModeAtomicAddProperty(req, _planeId, _propPlaneSrcX, 0);
    drmModeAtomicAddProperty(req, _planeId, _propPlaneSrcY, 0);
    drmModeAtomicAddProperty(req, _planeId, _propPlaneSrcW, _mode.hdisplay << 16);
    drmModeAtomicAddProperty(req, _planeId, _propPlaneSrcH, _mode.vdisplay << 16);
    drmModeAtomicAddProperty(req, _planeId, _propPlaneCrtcX, 0);
    drmModeAtomicAddProperty(req, _planeId, _propPlaneCrtcY, 0);
    drmModeAtomicAddProperty(req, _planeId, _propPlaneCrtcW, _mode.hdisplay);
    drmModeAtomicAddProperty(req, _planeId, _propPlaneCrtcH, _mode.vdisplay);

    uint32_t flags = DRM_MODE_ATOMIC_ALLOW_MODESET;

    if( drmModeAtomicCommit(_fd, req, flags, 0) < 0 )
    {
        drmModeAtomicFree(req);
        throw std::runtime_error("initial atomic modeset commit failed");
    }

    drmModeAtomicFree(req);
}


void DrmDisplay::blit(const Pt::uint8_t* frame, const Gfx::RectI& rect)
{
    const Gfx::RectI clipArea = rect.intersect( Gfx::RectI(Gfx::PointI(0, 0), 
                                                           Gfx::SizeI(width(), height())) );

    if( clipArea.isNull() )
        return;

    Pt::uint8_t* dst = backBuffer();
    const uint32_t dstPitch = pitch();
    const size_t srcPitch = width() * 4;
    const size_t pixelSize = 4;
    const size_t copyWidth = clipArea.width() * pixelSize;

    const int clipBottom = clipArea.y() + clipArea.height();

    for(int y = clipArea.y(); y < clipBottom; ++y)
    {
        Pt::uint8_t* dstRow = dst + y * dstPitch + clipArea.x() * pixelSize;
        const Pt::uint8_t* srcRow = frame + y * srcPitch + clipArea.x() * pixelSize;
        std::memcpy(dstRow, srcRow, copyWidth);
    }
}


void DrmDisplay::swapBuffers()
{
    if( _flipPending )
        return;

    drmModeAtomicReq* req = drmModeAtomicAlloc();
    if( ! req )
        return;

    uint32_t fbId = _buffers[_backIndex].fbId();
    drmModeAtomicAddProperty(req, _planeId, _propPlaneFbId, fbId);

    uint32_t flags = DRM_MODE_PAGE_FLIP_EVENT | DRM_MODE_ATOMIC_NONBLOCK;

    if( drmModeAtomicCommit(_fd, req, flags, this) == 0 )
    {
        _flipPending = true;
        _backIndex = 1 - _backIndex;
    }

    drmModeAtomicFree(req);
}


void DrmDisplay::swapBuffers(uint32_t externalFbId)
{
    if( _flipPending )
        return;

    drmModeAtomicReq* req = drmModeAtomicAlloc();
    if( ! req )
        return;

    drmModeAtomicAddProperty(req, _planeId, _propPlaneFbId, externalFbId);

    uint32_t flags = DRM_MODE_PAGE_FLIP_EVENT | DRM_MODE_ATOMIC_NONBLOCK;

    if( drmModeAtomicCommit(_fd, req, flags, this) == 0 )
    {
        _flipPending = true;
    }

    drmModeAtomicFree(req);
}

} // namespace

} // namespace
