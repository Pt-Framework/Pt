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

#ifndef Pt_Forms_DrmDisplay_h
#define Pt_Forms_DrmDisplay_h

#include "DrmBuffer.h"

#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Size.h>
#include <Pt/Gfx/Rect.h>
#include <Pt/Gfx/Image.h>
#include <Pt/Types.h>

#include <xf86drm.h>
#include <xf86drmMode.h>

#include <string>

namespace Pt {

namespace Forms {

class DrmDisplay
{
    public:
        DrmDisplay(int drmFd);

        ~DrmDisplay();

        void shutdown();

        size_t width() const;

        size_t height() const;

        const Gfx::ImageFormat& format() const;

        void blit(const Pt::uint8_t* frame, const Gfx::RectI& area);

        void swapBuffers();

    protected:
        friend class DrmDevice;
        friend class ScreenImpl;

        void onFlipComplete();

    private:
        DrmDisplay(const DrmDisplay&);

        DrmDisplay& operator=(const DrmDisplay&);

        int fd() const;

        uint32_t pitch() const;

        Pt::uint8_t* backBuffer();

        const Pt::uint8_t* backBuffer() const;

        void findConnector();

        void findCrtc();

        void setupBuffers();

        void setInitialMode();

        uint32_t getPropertyId(uint32_t objectId, uint32_t objectType,
                               const char* name) const;

    private:
        int                _fd;
        uint32_t           _connId;
        uint32_t           _crtcId;
        uint32_t           _crtcIndex;
        drmModeModeInfo    _mode;
        drmModeCrtc*       _savedCrtc;
        drmModeConnector*  _connector;

        // Atomic modesetting property IDs
        uint32_t           _propConnCrtcId;
        uint32_t           _propCrtcActive;
        uint32_t           _propCrtcModeId;
        uint32_t           _propPlaneFbId;
        uint32_t           _propPlaneCrtcId;
        uint32_t           _propPlaneSrcX;
        uint32_t           _propPlaneSrcY;
        uint32_t           _propPlaneSrcW;
        uint32_t           _propPlaneSrcH;
        uint32_t           _propPlaneCrtcX;
        uint32_t           _propPlaneCrtcY;
        uint32_t           _propPlaneCrtcW;
        uint32_t           _propPlaneCrtcH;
        uint32_t           _planeId;
        uint32_t           _modeBlobId;

        DrmBuffer          _buffers[2];
        int                _backIndex;
        bool               _flipPending;

        Gfx::ImageFormat*  _format;
};

} // namespace

} // namespace

#endif
