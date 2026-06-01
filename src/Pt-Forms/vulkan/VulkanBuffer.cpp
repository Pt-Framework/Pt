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

#include "VulkanBuffer.h"

#include <Pt/System/Logger.h>

#include <stdexcept>
#include <cstring>

#include <unistd.h>
#include <xf86drm.h>
#include <xf86drmMode.h>
#include <drm_fourcc.h>

PT_LOG_DEFINE("Pt.Forms.VulkanBuffer")

namespace Pt {

namespace Forms {

VulkanBuffer::VulkanBuffer()
: _vkDevice(VK_NULL_HANDLE)
, _drmFd(-1)
, _width(0)
, _height(0)
, _image(VK_NULL_HANDLE)
, _memory(VK_NULL_HANDLE)
, _imageView(VK_NULL_HANDLE)
, _framebuffer(VK_NULL_HANDLE)
, _dmaBufFd(-1)
, _drmHandle(0)
, _drmFbId(0)
, _pitch(0)
{
}


VulkanBuffer::~VulkanBuffer()
{
    destroy();
}


void VulkanBuffer::create(VulkanDevice& device,
                          uint32_t width, uint32_t height)
{
    _vkDevice = device.device();
    _drmFd = device.drmFd();
    _width = width;
    _height = height;

    createImage(device);
    exportDmaBuf(device);
    importToDrm();
    createImageView(device);
    createFramebuffer(device);

    PT_LOG_DEBUG("VulkanBuffer created: " << _width << "x" << _height
                 << ", fbId=" << _drmFbId);
}


void VulkanBuffer::destroy()
{
    if( _vkDevice == VK_NULL_HANDLE )
        return;

    if( _framebuffer != VK_NULL_HANDLE )
    {
        vkDestroyFramebuffer(_vkDevice, _framebuffer, 0);
        _framebuffer = VK_NULL_HANDLE;
    }

    if( _imageView != VK_NULL_HANDLE )
    {
        vkDestroyImageView(_vkDevice, _imageView, 0);
        _imageView = VK_NULL_HANDLE;
    }

    if( _drmFbId )
    {
        drmModeRmFB(_drmFd, _drmFbId);
        _drmFbId = 0;
    }

    if( _drmHandle )
    {
        struct drm_gem_close closeReq = {};
        closeReq.handle = _drmHandle;
        drmIoctl(_drmFd, DRM_IOCTL_GEM_CLOSE, &closeReq);
        _drmHandle = 0;
    }

    if( _dmaBufFd >= 0 )
    {
        close(_dmaBufFd);
        _dmaBufFd = -1;
    }

    if( _image != VK_NULL_HANDLE )
    {
        vkDestroyImage(_vkDevice, _image, 0);
        _image = VK_NULL_HANDLE;
    }

    if( _memory != VK_NULL_HANDLE )
    {
        vkFreeMemory(_vkDevice, _memory, 0);
        _memory = VK_NULL_HANDLE;
    }

    _vkDevice = VK_NULL_HANDLE;
}


void VulkanBuffer::createImage(VulkanDevice& device)
{
    // create image with external memory export capability
    VkExternalMemoryImageCreateInfo extMemInfo = {};
    extMemInfo.sType = VK_STRUCTURE_TYPE_EXTERNAL_MEMORY_IMAGE_CREATE_INFO;
    extMemInfo.handleTypes = VK_EXTERNAL_MEMORY_HANDLE_TYPE_DMA_BUF_BIT_EXT;

    VkImageCreateInfo imageInfo = {};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.pNext = &extMemInfo;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.format = VK_FORMAT_B8G8R8A8_UNORM;
    imageInfo.extent.width = _width;
    imageInfo.extent.height = _height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.tiling = VK_IMAGE_TILING_LINEAR;
    imageInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
                      VK_IMAGE_USAGE_TRANSFER_SRC_BIT |
                      VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

    VkResult result = vkCreateImage(device.device(), &imageInfo, 0, &_image);
    if( result != VK_SUCCESS )
        throw std::runtime_error("failed to create Vulkan image for DMA-BUF");

    // allocate exportable memory
    VkMemoryRequirements memReqs;
    vkGetImageMemoryRequirements(device.device(), _image, &memReqs);

    VkExportMemoryAllocateInfo exportInfo = {};
    exportInfo.sType = VK_STRUCTURE_TYPE_EXPORT_MEMORY_ALLOCATE_INFO;
    exportInfo.handleTypes = VK_EXTERNAL_MEMORY_HANDLE_TYPE_DMA_BUF_BIT_EXT;

    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.pNext = &exportInfo;
    allocInfo.allocationSize = memReqs.size;
    allocInfo.memoryTypeIndex = device.findMemoryType(
        memReqs.memoryTypeBits,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    result = vkAllocateMemory(device.device(), &allocInfo, 0, &_memory);
    if( result != VK_SUCCESS )
        throw std::runtime_error("failed to allocate Vulkan memory for DMA-BUF");

    result = vkBindImageMemory(device.device(), _image, _memory, 0);
    if( result != VK_SUCCESS )
        throw std::runtime_error("failed to bind Vulkan image memory");
}


void VulkanBuffer::exportDmaBuf(VulkanDevice& device)
{
    VkMemoryGetFdInfoKHR getFdInfo = {};
    getFdInfo.sType = VK_STRUCTURE_TYPE_MEMORY_GET_FD_INFO_KHR;
    getFdInfo.memory = _memory;
    getFdInfo.handleType = VK_EXTERNAL_MEMORY_HANDLE_TYPE_DMA_BUF_BIT_EXT;

    PFN_vkGetMemoryFdKHR vkGetMemoryFdKHR =
        reinterpret_cast<PFN_vkGetMemoryFdKHR>(
            vkGetDeviceProcAddr(device.device(), "vkGetMemoryFdKHR"));

    if( ! vkGetMemoryFdKHR )
        throw std::runtime_error("vkGetMemoryFdKHR not available");

    VkResult result = vkGetMemoryFdKHR(device.device(), &getFdInfo, &_dmaBufFd);
    if( result != VK_SUCCESS || _dmaBufFd < 0 )
        throw std::runtime_error("failed to export DMA-BUF from Vulkan memory");

    // query subresource layout for pitch
    VkImageSubresource subRes = {};
    subRes.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    subRes.mipLevel = 0;
    subRes.arrayLayer = 0;

    VkSubresourceLayout layout;
    vkGetImageSubresourceLayout(device.device(), _image, &subRes, &layout);

    _pitch = static_cast<uint32_t>(layout.rowPitch);

    PT_LOG_DEBUG("DMA-BUF exported: fd=" << _dmaBufFd
                 << ", pitch=" << _pitch);
}


void VulkanBuffer::importToDrm()
{
    // convert DMA-BUF fd to GEM handle
    int ret = drmPrimeFDToHandle(_drmFd, _dmaBufFd, &_drmHandle);
    if( ret < 0 )
        throw std::runtime_error("drmPrimeFDToHandle failed");

    // create DRM framebuffer from GEM handle
    uint32_t handles[4] = { _drmHandle, 0, 0, 0 };
    uint32_t pitches[4] = { _pitch, 0, 0, 0 };
    uint32_t offsets[4] = { 0, 0, 0, 0 };

    ret = drmModeAddFB2(_drmFd, _width, _height, DRM_FORMAT_XRGB8888,
                        handles, pitches, offsets, &_drmFbId, 0);
    if( ret < 0 )
        throw std::runtime_error("drmModeAddFB2 failed for VulkanBuffer");

    PT_LOG_DEBUG("DRM framebuffer imported: handle=" << _drmHandle
                 << ", fbId=" << _drmFbId);
}


void VulkanBuffer::createImageView(VulkanDevice& device)
{
    VkImageViewCreateInfo viewInfo = {};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = _image;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = VK_FORMAT_B8G8R8A8_UNORM;
    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    VkResult result = vkCreateImageView(device.device(), &viewInfo, 0, &_imageView);
    if( result != VK_SUCCESS )
        throw std::runtime_error("failed to create VulkanBuffer image view");
}


void VulkanBuffer::createFramebuffer(VulkanDevice& device)
{
    VkImageView attachments[] = { _imageView };

    VkFramebufferCreateInfo fbInfo = {};
    fbInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    fbInfo.renderPass = device.renderPass();
    fbInfo.attachmentCount = 1;
    fbInfo.pAttachments = attachments;
    fbInfo.width = _width;
    fbInfo.height = _height;
    fbInfo.layers = 1;

    if( vkCreateFramebuffer(device.device(), &fbInfo, 0, &_framebuffer) != VK_SUCCESS )
        throw std::runtime_error("failed to create VulkanBuffer framebuffer");
}

} // namespace

} // namespace
