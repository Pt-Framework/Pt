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

#include "PixmapImpl.h"

#include <Pt/Forms/Pixmap.h>
#include <Pt/Gfx/Canvas.h>
#include <Pt/Gfx/Rgb32.h>
#include <Pt/Gfx/Bitmap.h>
#include <Pt/System/Logger.h>

#include <cassert>
#include <cstring>

PT_LOG_DEFINE("Pt.Forms.PixmapImpl.Vulkan")

namespace Pt {

namespace Forms {

namespace {

static std::string _defaultFontFamily;

} // anonymous


PixmapImpl::PixmapImpl()
: _device(0)
, _drmFd(-1)
, _backIndex(0)
, _canvas(0)
{
}


PixmapImpl::~PixmapImpl()
{
    delete _canvas;
    _buffers[0].destroy();
    _buffers[1].destroy();
}


void PixmapImpl::init(VulkanDevice& device, int drmFd)
{
    _device = &device;
    _drmFd = drmFd;
}


void PixmapImpl::reset(const Gfx::Image& /*image*/)
{
    // image import not implemented for Vulkan renderer yet
}


void PixmapImpl::reset(const Gfx::SizeF& size)
{
    if( ! _device || _drmFd < 0 )
        return;

    _size = size;

    uint32_t w = static_cast<uint32_t>(size.width());
    uint32_t h = static_cast<uint32_t>(size.height());

    if( w == 0 || h == 0 )
        return;

    _buffers[0].destroy();
    _buffers[1].destroy();

    _buffers[0].create(*_device, _drmFd, w, h);
    _buffers[1].create(*_device, _drmFd, w, h);
    _backIndex = 0;

    PT_LOG_DEBUG("PixmapImpl reset: " << w << "x" << h);
}


void PixmapImpl::reset(const Gfx::SizeF& size, std::size_t /*stride*/)
{
    reset(size);
}


void PixmapImpl::reset()
{
    _buffers[0].destroy();
    _buffers[1].destroy();
    _size = Gfx::SizeF(0, 0);
}


void PixmapImpl::setScaleFactor(double scaleFactor)
{
    _scaling = Gfx::Scaling(scaleFactor);
}


void PixmapImpl::drawPixmap(Gfx::Canvas& canvas,
                            const Gfx::PointF& to,
                            const Pixmap& pixmap,
                            const Gfx::RectF* rect)
{
    // TODO: blit via textured quad from pixmap's VulkanBuffer
    (void)canvas;
    (void)to;
    (void)pixmap;
    (void)rect;
}


const Gfx::ImageFormat& PixmapImpl::format() const
{
    static Gfx::Rgb32 fmt;
    return fmt;
}


Gfx::Canvas* PixmapImpl::getCanvas(Gfx::Canvas* reuse)
{
    if( ! _device )
        return 0;

    if( ! _canvas )
    {
        _canvas = new VulkanCanvas(*_device);
    }

    _canvas->setTarget( _buffers[_backIndex] );
    return _canvas;
}


Gfx::Canvas* PixmapImpl::createCanvas(Gfx::Canvas* /*reuse*/)
{
    return 0;
}


void PixmapImpl::releaseCanvas()
{
}


void PixmapImpl::sync()
{
    if( ! _device || ! _canvas )
        return;

    // submit command buffer and wait
    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;

    VkCommandBuffer cmdBuf = _canvas->commandBuffer();
    submitInfo.pCommandBuffers = &cmdBuf;

    VkFenceCreateInfo fenceInfo = {};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

    VkFence fence;
    vkCreateFence(_device->device(), &fenceInfo, 0, &fence);

    vkQueueSubmit(_device->graphicsQueue(), 1, &submitInfo, fence);
    vkWaitForFences(_device->device(), 1, &fence, VK_TRUE, UINT64_MAX);

    vkDestroyFence(_device->device(), fence, 0);
}


void PixmapImpl::finish()
{
    // swap buffers after rendering
    _backIndex = 1 - _backIndex;
}


const std::string& PixmapImpl::defaultFont()
{
    if( _defaultFontFamily.empty() )
        _defaultFontFamily = Gfx::Bitmap::defaultFont();

    return _defaultFontFamily;
}


void PixmapImpl::setDefaultFont(const std::string& family)
{
    _defaultFontFamily = family;
    Gfx::Bitmap::setDefaultFont(family);
}


std::vector<std::string> PixmapImpl::fontFamilies()
{
    return Gfx::Bitmap::fontFamilies();
}


std::vector<Gfx::FontFace> PixmapImpl::fontFaces(const std::string& family)
{
    return Gfx::Bitmap::fontFaces(family);
}

} // namespace

} // namespace
