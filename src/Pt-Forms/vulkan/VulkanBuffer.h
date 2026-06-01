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

#ifndef Pt_Forms_VulkanBuffer_h
#define Pt_Forms_VulkanBuffer_h

#include "VulkanDevice.h"

#include <vulkan/vulkan.h>

#include <cstdint>

namespace Pt {

namespace Forms {

/** @brief DMA-BUF backed Vulkan render target for zero-copy DRM scanout.

    Allocates a VkImage with exportable DMA-BUF memory and imports it as a
    DRM framebuffer for direct page-flip scanout without CPU memcpy.
*/
class VulkanBuffer
{
    public:
        VulkanBuffer();

        ~VulkanBuffer();

        void create(VulkanDevice& device, int drmFd,
                    uint32_t width, uint32_t height);

        void destroy();

        VkImage image() const
        { return _image; }

        VkImageView imageView() const
        { return _imageView; }

        VkFramebuffer framebuffer() const
        { return _framebuffer; }

        uint32_t drmFbId() const
        { return _drmFbId; }

        uint32_t width() const
        { return _width; }

        uint32_t height() const
        { return _height; }

    private:
        VulkanBuffer(const VulkanBuffer&);
        VulkanBuffer& operator=(const VulkanBuffer&);

        void createImage(VulkanDevice& device);

        void exportDmaBuf(VulkanDevice& device);

        void importToDrm();

        void createImageView(VulkanDevice& device);

    private:
        VkDevice         _vkDevice;
        int              _drmFd;
        uint32_t         _width;
        uint32_t         _height;

        VkImage          _image;
        VkDeviceMemory   _memory;
        VkImageView      _imageView;
        VkFramebuffer    _framebuffer;

        int              _dmaBufFd;
        uint32_t         _drmHandle;
        uint32_t         _drmFbId;
        uint32_t         _pitch;
};

} // namespace

} // namespace

#endif
