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

#include "VulkanDevice.h"

#include <Pt/System/Logger.h>

#include <stdexcept>
#include <cstring>
#include <vector>

PT_LOG_DEFINE("Pt.Forms.VulkanDevice")

namespace Pt {

namespace Forms {

VulkanDevice::VulkanDevice()
: _instance(VK_NULL_HANDLE)
, _physicalDevice(VK_NULL_HANDLE)
, _device(VK_NULL_HANDLE)
, _graphicsQueue(VK_NULL_HANDLE)
, _graphicsQueueFamily(0)
, _commandPool(VK_NULL_HANDLE)
, _memProps()
{
    createInstance();
    selectPhysicalDevice();
    createLogicalDevice();
    createCommandPool();
}


VulkanDevice::~VulkanDevice()
{
    if( _commandPool != VK_NULL_HANDLE )
        vkDestroyCommandPool(_device, _commandPool, 0);

    if( _device != VK_NULL_HANDLE )
        vkDestroyDevice(_device, 0);

    if( _instance != VK_NULL_HANDLE )
        vkDestroyInstance(_instance, 0);
}


uint32_t VulkanDevice::findMemoryType(uint32_t typeFilter,
                                      VkMemoryPropertyFlags properties) const
{
    for(uint32_t i = 0; i < _memProps.memoryTypeCount; ++i)
    {
        if( (typeFilter & (1u << i)) &&
            (_memProps.memoryTypes[i].propertyFlags & properties) == properties )
        {
            return i;
        }
    }

    throw std::runtime_error("failed to find suitable Vulkan memory type");
}


void VulkanDevice::createInstance()
{
    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Pt::Forms";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "Pt";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

#ifndef NDEBUG
    const char* validationLayers[] = {
        "VK_LAYER_KHRONOS_validation"
    };

    uint32_t layerCount = 0;
    vkEnumerateInstanceLayerProperties(&layerCount, 0);

    std::vector<VkLayerProperties> available(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, available.data());

    bool hasValidation = false;
    for(uint32_t i = 0; i < layerCount; ++i)
    {
        if( std::strcmp(available[i].layerName, validationLayers[0]) == 0 )
        {
            hasValidation = true;
            break;
        }
    }

    if( hasValidation )
    {
        createInfo.enabledLayerCount = 1;
        createInfo.ppEnabledLayerNames = validationLayers;
        PT_LOG_DEBUG("Vulkan validation layers enabled");
    }
#endif

    VkResult result = vkCreateInstance(&createInfo, 0, &_instance);
    if( result != VK_SUCCESS )
        throw std::runtime_error("failed to create Vulkan instance");

    PT_LOG_DEBUG("Vulkan instance created");
}


void VulkanDevice::selectPhysicalDevice()
{
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(_instance, &deviceCount, 0);

    if( deviceCount == 0 )
        throw std::runtime_error("no Vulkan-capable GPU found");

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(_instance, &deviceCount, devices.data());

    // prefer a device that supports DMA-BUF export
    for(uint32_t i = 0; i < deviceCount; ++i)
    {
        VkPhysicalDeviceProperties props;
        vkGetPhysicalDeviceProperties(devices[i], &props);

        // check for external memory fd extension
        uint32_t extCount = 0;
        vkEnumerateDeviceExtensionProperties(devices[i], 0, &extCount, 0);

        std::vector<VkExtensionProperties> exts(extCount);
        vkEnumerateDeviceExtensionProperties(devices[i], 0, &extCount, exts.data());

        bool hasExternalMemoryFd = false;
        bool hasDmaBuf = false;

        for(uint32_t j = 0; j < extCount; ++j)
        {
            if( std::strcmp(exts[j].extensionName,
                           VK_KHR_EXTERNAL_MEMORY_FD_EXTENSION_NAME) == 0 )
                hasExternalMemoryFd = true;

            if( std::strcmp(exts[j].extensionName,
                           VK_EXT_EXTERNAL_MEMORY_DMA_BUF_EXTENSION_NAME) == 0 )
                hasDmaBuf = true;
        }

        if( hasExternalMemoryFd && hasDmaBuf )
        {
            _physicalDevice = devices[i];
            PT_LOG_DEBUG("selected GPU: " << props.deviceName
                         << " (DMA-BUF supported)");
            break;
        }
    }

    // fallback: use the first device if no DMA-BUF capable one found
    if( _physicalDevice == VK_NULL_HANDLE )
    {
        _physicalDevice = devices[0];

        VkPhysicalDeviceProperties props;
        vkGetPhysicalDeviceProperties(_physicalDevice, &props);
        PT_LOG_DEBUG("selected GPU: " << props.deviceName
                     << " (DMA-BUF not verified)");
    }

    vkGetPhysicalDeviceMemoryProperties(_physicalDevice, &_memProps);
}


void VulkanDevice::createLogicalDevice()
{
    // find a graphics queue family
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(_physicalDevice,
                                            &queueFamilyCount, 0);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(_physicalDevice,
                                            &queueFamilyCount,
                                            queueFamilies.data());

    _graphicsQueueFamily = UINT32_MAX;

    for(uint32_t i = 0; i < queueFamilyCount; ++i)
    {
        if( queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT )
        {
            _graphicsQueueFamily = i;
            break;
        }
    }

    if( _graphicsQueueFamily == UINT32_MAX )
        throw std::runtime_error("no graphics queue family found");

    float queuePriority = 1.0f;

    VkDeviceQueueCreateInfo queueCreateInfo = {};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = _graphicsQueueFamily;
    queueCreateInfo.queueCount = 1;
    queueCreateInfo.pQueuePriorities = &queuePriority;

    // request extensions for DMA-BUF export
    const char* deviceExtensions[] = {
        VK_KHR_EXTERNAL_MEMORY_EXTENSION_NAME,
        VK_KHR_EXTERNAL_MEMORY_FD_EXTENSION_NAME,
        VK_EXT_EXTERNAL_MEMORY_DMA_BUF_EXTENSION_NAME
    };

    VkDeviceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.queueCreateInfoCount = 1;
    createInfo.pQueueCreateInfos = &queueCreateInfo;
    createInfo.enabledExtensionCount = 3;
    createInfo.ppEnabledExtensionNames = deviceExtensions;

    VkResult result = vkCreateDevice(_physicalDevice, &createInfo, 0, &_device);
    if( result != VK_SUCCESS )
        throw std::runtime_error("failed to create Vulkan logical device");

    vkGetDeviceQueue(_device, _graphicsQueueFamily, 0, &_graphicsQueue);

    PT_LOG_DEBUG("Vulkan logical device created, queue family: "
                 << _graphicsQueueFamily);
}


void VulkanDevice::createCommandPool()
{
    VkCommandPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = _graphicsQueueFamily;

    VkResult result = vkCreateCommandPool(_device, &poolInfo, 0, &_commandPool);
    if( result != VK_SUCCESS )
        throw std::runtime_error("failed to create Vulkan command pool");
}

} // namespace

} // namespace
