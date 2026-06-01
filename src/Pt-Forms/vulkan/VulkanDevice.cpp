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
#include "VulkanShaders.h"

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
, _drmFd(-1)
, _renderPass(VK_NULL_HANDLE)
, _pipelineLayout(VK_NULL_HANDLE)
, _solidFillPipeline(VK_NULL_HANDLE)
, _texturedPipeline(VK_NULL_HANDLE)
, _descriptorSetLayout(VK_NULL_HANDLE)
, _descriptorPool(VK_NULL_HANDLE)
, _sampler(VK_NULL_HANDLE)
{
    createInstance();
    selectPhysicalDevice();
    createLogicalDevice();
    createCommandPool();
    createRenderPass();
    createPipelines();
    createDescriptorPool();
    createSampler();
}


VulkanDevice::~VulkanDevice()
{
    if( _sampler != VK_NULL_HANDLE )
        vkDestroySampler(_device, _sampler, 0);

    if( _descriptorPool != VK_NULL_HANDLE )
        vkDestroyDescriptorPool(_device, _descriptorPool, 0);

    if( _solidFillPipeline != VK_NULL_HANDLE )
        vkDestroyPipeline(_device, _solidFillPipeline, 0);

    if( _texturedPipeline != VK_NULL_HANDLE )
        vkDestroyPipeline(_device, _texturedPipeline, 0);

    if( _pipelineLayout != VK_NULL_HANDLE )
        vkDestroyPipelineLayout(_device, _pipelineLayout, 0);

    if( _descriptorSetLayout != VK_NULL_HANDLE )
        vkDestroyDescriptorSetLayout(_device, _descriptorSetLayout, 0);

    if( _renderPass != VK_NULL_HANDLE )
        vkDestroyRenderPass(_device, _renderPass, 0);

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


void VulkanDevice::createRenderPass()
{
    VkAttachmentDescription colorAttachment = {};
    colorAttachment.format = VK_FORMAT_B8G8R8A8_UNORM;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference colorAttachmentRef = {};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;

    VkRenderPassCreateInfo rpInfo = {};
    rpInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    rpInfo.attachmentCount = 1;
    rpInfo.pAttachments = &colorAttachment;
    rpInfo.subpassCount = 1;
    rpInfo.pSubpasses = &subpass;

    if( vkCreateRenderPass(_device, &rpInfo, 0, &_renderPass) != VK_SUCCESS )
        throw std::runtime_error("failed to create Vulkan render pass");
}


void VulkanDevice::createPipelines()
{
    VkDescriptorSetLayoutBinding samplerBinding = {};
    samplerBinding.binding = 0;
    samplerBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    samplerBinding.descriptorCount = 1;
    samplerBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    VkDescriptorSetLayoutCreateInfo layoutInfo = {};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = 1;
    layoutInfo.pBindings = &samplerBinding;

    if( vkCreateDescriptorSetLayout(_device, &layoutInfo, 0,
                                    &_descriptorSetLayout) != VK_SUCCESS )
        throw std::runtime_error("failed to create descriptor set layout");

    // push constant range: mat4 transform (64 bytes) + vec4 color (16 bytes) = 80 bytes
    VkPushConstantRange pushRange = {};
    pushRange.stageFlags = VK_SHADER_STAGE_ALL;
    pushRange.offset = 0;
    pushRange.size = 80;

    VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &_descriptorSetLayout;
    pipelineLayoutInfo.pushConstantRangeCount = 1;
    pipelineLayoutInfo.pPushConstantRanges = &pushRange;

    if( vkCreatePipelineLayout(_device, &pipelineLayoutInfo, 0,
                               &_pipelineLayout) != VK_SUCCESS )
        throw std::runtime_error("failed to create pipeline layout");

    VkShaderModule vertModule = createShaderModule(solidFillVertSpv,
                                                   solidFillVertSpvSize);
    VkShaderModule fragModule = createShaderModule(solidFillFragSpv,
                                                   solidFillFragSpvSize);

    VkPipelineShaderStageCreateInfo shaderStages[2] = {};
    shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
    shaderStages[0].module = vertModule;
    shaderStages[0].pName = "main";
    shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    shaderStages[1].module = fragModule;
    shaderStages[1].pName = "main";

    VkVertexInputBindingDescription bindingDesc = {};
    bindingDesc.binding = 0;
    bindingDesc.stride = sizeof(float) * 2;
    bindingDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    VkVertexInputAttributeDescription attrDesc = {};
    attrDesc.binding = 0;
    attrDesc.location = 0;
    attrDesc.format = VK_FORMAT_R32G32_SFLOAT;
    attrDesc.offset = 0;

    VkPipelineVertexInputStateCreateInfo vertexInput = {};
    vertexInput.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInput.vertexBindingDescriptionCount = 1;
    vertexInput.pVertexBindingDescriptions = &bindingDesc;
    vertexInput.vertexAttributeDescriptionCount = 1;
    vertexInput.pVertexAttributeDescriptions = &attrDesc;

    VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    VkDynamicState dynamicStates[] = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };

    VkPipelineDynamicStateCreateInfo dynamicState = {};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = 2;
    dynamicState.pDynamicStates = dynamicStates;

    VkPipelineViewportStateCreateInfo viewportState = {};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.scissorCount = 1;

    VkPipelineRasterizationStateCreateInfo rasterizer = {};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_NONE;
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;

    VkPipelineMultisampleStateCreateInfo multisampling = {};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    VkPipelineColorBlendAttachmentState blendAttachment = {};
    blendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT |
                                     VK_COLOR_COMPONENT_G_BIT |
                                     VK_COLOR_COMPONENT_B_BIT |
                                     VK_COLOR_COMPONENT_A_BIT;
    blendAttachment.blendEnable = VK_TRUE;
    blendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    blendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    blendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
    blendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    blendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    blendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

    VkPipelineColorBlendStateCreateInfo colorBlending = {};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &blendAttachment;

    VkGraphicsPipelineCreateInfo pipelineInfo = {};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.pVertexInputState = &vertexInput;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDynamicState = &dynamicState;
    pipelineInfo.layout = _pipelineLayout;
    pipelineInfo.renderPass = _renderPass;
    pipelineInfo.subpass = 0;

    if( vkCreateGraphicsPipelines(_device, VK_NULL_HANDLE, 1, &pipelineInfo, 0,
                                  &_solidFillPipeline) != VK_SUCCESS )
        throw std::runtime_error("failed to create solid fill pipeline");

    vkDestroyShaderModule(_device, vertModule, 0);
    vkDestroyShaderModule(_device, fragModule, 0);

    // textured pipeline
    VkShaderModule texVertModule = createShaderModule(texturedVertSpv,
                                                      texturedVertSpvSize);
    VkShaderModule texFragModule = createShaderModule(texturedFragSpv,
                                                      texturedFragSpvSize);

    shaderStages[0].module = texVertModule;
    shaderStages[1].module = texFragModule;

    VkVertexInputBindingDescription texBindingDesc = {};
    texBindingDesc.binding = 0;
    texBindingDesc.stride = sizeof(float) * 4;
    texBindingDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    VkVertexInputAttributeDescription texAttrDescs[2] = {};
    texAttrDescs[0].binding = 0;
    texAttrDescs[0].location = 0;
    texAttrDescs[0].format = VK_FORMAT_R32G32_SFLOAT;
    texAttrDescs[0].offset = 0;
    texAttrDescs[1].binding = 0;
    texAttrDescs[1].location = 1;
    texAttrDescs[1].format = VK_FORMAT_R32G32_SFLOAT;
    texAttrDescs[1].offset = sizeof(float) * 2;

    vertexInput.pVertexBindingDescriptions = &texBindingDesc;
    vertexInput.vertexAttributeDescriptionCount = 2;
    vertexInput.pVertexAttributeDescriptions = texAttrDescs;

    pipelineInfo.pStages = shaderStages;

    if( vkCreateGraphicsPipelines(_device, VK_NULL_HANDLE, 1, &pipelineInfo, 0,
                                  &_texturedPipeline) != VK_SUCCESS )
        throw std::runtime_error("failed to create textured pipeline");

    vkDestroyShaderModule(_device, texVertModule, 0);
    vkDestroyShaderModule(_device, texFragModule, 0);
}


void VulkanDevice::createDescriptorPool()
{
    VkDescriptorPoolSize poolSize = {};
    poolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSize.descriptorCount = 16;

    VkDescriptorPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = 1;
    poolInfo.pPoolSizes = &poolSize;
    poolInfo.maxSets = 16;
    poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;

    if( vkCreateDescriptorPool(_device, &poolInfo, 0, &_descriptorPool) != VK_SUCCESS )
        throw std::runtime_error("failed to create descriptor pool");
}


void VulkanDevice::createSampler()
{
    VkSamplerCreateInfo samplerInfo = {};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;

    if( vkCreateSampler(_device, &samplerInfo, 0, &_sampler) != VK_SUCCESS )
        throw std::runtime_error("failed to create Vulkan sampler");
}


VkShaderModule VulkanDevice::createShaderModule(const uint32_t* code, size_t size)
{
    VkShaderModuleCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = size;
    createInfo.pCode = code;

    VkShaderModule shaderModule;
    if( vkCreateShaderModule(_device, &createInfo, 0, &shaderModule) != VK_SUCCESS )
        throw std::runtime_error("failed to create Vulkan shader module");

    return shaderModule;
}

} // namespace

} // namespace
