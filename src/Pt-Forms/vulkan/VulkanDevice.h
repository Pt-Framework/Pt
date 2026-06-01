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

#ifndef Pt_Forms_VulkanDevice_h
#define Pt_Forms_VulkanDevice_h

#include <vulkan/vulkan.h>

#include <cstdint>

namespace Pt {

namespace Forms {

class VulkanDevice
{
    public:
        VulkanDevice();

        ~VulkanDevice();

        VkInstance instance() const
        { return _instance; }

        VkPhysicalDevice physicalDevice() const
        { return _physicalDevice; }

        VkDevice device() const
        { return _device; }

        VkQueue graphicsQueue() const
        { return _graphicsQueue; }

        uint32_t graphicsQueueFamily() const
        { return _graphicsQueueFamily; }

        VkCommandPool commandPool() const
        { return _commandPool; }

        VkPhysicalDeviceMemoryProperties memoryProperties() const
        { return _memProps; }

        uint32_t findMemoryType(uint32_t typeFilter,
                                VkMemoryPropertyFlags properties) const;

    private:
        VulkanDevice(const VulkanDevice&);
        VulkanDevice& operator=(const VulkanDevice&);

        void createInstance();

        void selectPhysicalDevice();

        void createLogicalDevice();

        void createCommandPool();

    private:
        VkInstance                       _instance;
        VkPhysicalDevice                 _physicalDevice;
        VkDevice                         _device;
        VkQueue                          _graphicsQueue;
        uint32_t                         _graphicsQueueFamily;
        VkCommandPool                    _commandPool;
        VkPhysicalDeviceMemoryProperties _memProps;
};

} // namespace

} // namespace

#endif
