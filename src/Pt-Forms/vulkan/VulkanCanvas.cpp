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

#include "VulkanCanvas.h"
#include "VulkanShaders.h"

#include <Pt/Gfx/Color.h>
#include <Pt/Gfx/Image.h>
#include <Pt/Gfx/Paint.h>
#include <Pt/Gfx/Transform.h>
#include <Pt/System/Logger.h>

#include <stdexcept>
#include <cstring>
#include <cmath>

PT_LOG_DEFINE("Pt.Forms.VulkanCanvas")

namespace Pt {

namespace Forms {

VulkanCanvas::VulkanCanvas(VulkanDevice& device)
: _device(&device)
, _target(0)
, _cmdBuf(VK_NULL_HANDLE)
, _renderPass(VK_NULL_HANDLE)
, _pipelineLayout(VK_NULL_HANDLE)
, _solidFillPipeline(VK_NULL_HANDLE)
, _texturedPipeline(VK_NULL_HANDLE)
, _descriptorSetLayout(VK_NULL_HANDLE)
, _descriptorPool(VK_NULL_HANDLE)
, _vertexBuffer(VK_NULL_HANDLE)
, _vertexMemory(VK_NULL_HANDLE)
, _vertexBufferSize(0)
, _sampler(VK_NULL_HANDLE)
, _inRenderPass(false)
{
    // allocate command buffer
    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = device.commandPool();
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = 1;

    if( vkAllocateCommandBuffers(device.device(), &allocInfo, &_cmdBuf) != VK_SUCCESS )
        throw std::runtime_error("failed to allocate Vulkan command buffer");

    createRenderPass();
    createPipelines();
    createDescriptorPool();

    // create default sampler
    VkSamplerCreateInfo samplerInfo = {};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;

    if( vkCreateSampler(device.device(), &samplerInfo, 0, &_sampler) != VK_SUCCESS )
        throw std::runtime_error("failed to create Vulkan sampler");

    // create initial vertex buffer (64 KB)
    _vertexBufferSize = 64 * 1024;

    VkBufferCreateInfo bufInfo = {};
    bufInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufInfo.size = _vertexBufferSize;
    bufInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    bufInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if( vkCreateBuffer(device.device(), &bufInfo, 0, &_vertexBuffer) != VK_SUCCESS )
        throw std::runtime_error("failed to create vertex buffer");

    VkMemoryRequirements memReqs;
    vkGetBufferMemoryRequirements(device.device(), _vertexBuffer, &memReqs);

    VkMemoryAllocateInfo memAlloc = {};
    memAlloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memAlloc.allocationSize = memReqs.size;
    memAlloc.memoryTypeIndex = device.findMemoryType(
        memReqs.memoryTypeBits,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
        VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    if( vkAllocateMemory(device.device(), &memAlloc, 0, &_vertexMemory) != VK_SUCCESS )
        throw std::runtime_error("failed to allocate vertex buffer memory");

    vkBindBufferMemory(device.device(), _vertexBuffer, _vertexMemory, 0);

    std::memset(&_pushConstants, 0, sizeof(_pushConstants));
}


VulkanCanvas::~VulkanCanvas()
{
    VkDevice dev = _device->device();

    if( _sampler != VK_NULL_HANDLE )
        vkDestroySampler(dev, _sampler, 0);

    if( _vertexBuffer != VK_NULL_HANDLE )
        vkDestroyBuffer(dev, _vertexBuffer, 0);

    if( _vertexMemory != VK_NULL_HANDLE )
        vkFreeMemory(dev, _vertexMemory, 0);

    if( _descriptorPool != VK_NULL_HANDLE )
        vkDestroyDescriptorPool(dev, _descriptorPool, 0);

    if( _solidFillPipeline != VK_NULL_HANDLE )
        vkDestroyPipeline(dev, _solidFillPipeline, 0);

    if( _texturedPipeline != VK_NULL_HANDLE )
        vkDestroyPipeline(dev, _texturedPipeline, 0);

    if( _pipelineLayout != VK_NULL_HANDLE )
        vkDestroyPipelineLayout(dev, _pipelineLayout, 0);

    if( _descriptorSetLayout != VK_NULL_HANDLE )
        vkDestroyDescriptorSetLayout(dev, _descriptorSetLayout, 0);

    if( _renderPass != VK_NULL_HANDLE )
        vkDestroyRenderPass(dev, _renderPass, 0);
}


void VulkanCanvas::setTarget(VulkanBuffer& buffer)
{
    _target = &buffer;
}


void VulkanCanvas::onBeginPaint(const Gfx::Paint& /*paint*/)
{
    if( ! _target )
        return;

    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkResetCommandBuffer(_cmdBuf, 0);
    vkBeginCommandBuffer(_cmdBuf, &beginInfo);

    // transition image to color attachment optimal
    VkImageMemoryBarrier barrier = {};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    barrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = _target->image();
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
    barrier.srcAccessMask = 0;
    barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    vkCmdPipelineBarrier(_cmdBuf,
                         VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                         VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                         0, 0, 0, 0, 0, 1, &barrier);

    beginRenderPass();

    // set default ortho projection
    buildOrthoMatrix(_pushConstants.transform,
                     static_cast<float>(_target->width()),
                     static_cast<float>(_target->height()));

    _pushConstants.color[0] = 1.0f;
    _pushConstants.color[1] = 1.0f;
    _pushConstants.color[2] = 1.0f;
    _pushConstants.color[3] = 1.0f;
}


void VulkanCanvas::onFinishPaint()
{
    if( ! _target )
        return;

    endRenderPass();

    // transition image to general for DRM scanout
    VkImageMemoryBarrier barrier = {};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    barrier.newLayout = VK_IMAGE_LAYOUT_GENERAL;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = _target->image();
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
    barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;

    vkCmdPipelineBarrier(_cmdBuf,
                         VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                         VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
                         0, 0, 0, 0, 0, 1, &barrier);

    vkEndCommandBuffer(_cmdBuf);
}


void VulkanCanvas::onSetTransform(const Gfx::Transform& /*tx*/)
{
}


void VulkanCanvas::onApplyTransform()
{
    if( ! _target )
        return;

    // rebuild ortho matrix with user transform applied
    const Gfx::Transform& tx = transform();
    float w = static_cast<float>(_target->width());
    float h = static_cast<float>(_target->height());

    // ortho * user transform
    float ortho[16];
    buildOrthoMatrix(ortho, w, h);

    // multiply ortho * transform matrix
    float userMat[16] = {
        static_cast<float>(tx.m11()), static_cast<float>(tx.m12()), 0.0f, 0.0f,
        static_cast<float>(tx.m21()), static_cast<float>(tx.m22()), 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        static_cast<float>(tx.dx()), static_cast<float>(tx.dy()), 0.0f, 1.0f
    };

    // mat4 multiply: ortho * userMat
    for(int i = 0; i < 4; ++i)
    {
        for(int j = 0; j < 4; ++j)
        {
            float sum = 0.0f;
            for(int k = 0; k < 4; ++k)
                sum += ortho[i * 4 + k] * userMat[k * 4 + j];

            _pushConstants.transform[i * 4 + j] = sum;
        }
    }
}


void VulkanCanvas::onSetCompositionMode(const Gfx::CompositionMode& mode)
{
    _compositionMode = mode;
}


void VulkanCanvas::onApplyCompositionMode()
{
    // composition mode is baked into pipeline state at creation time;
    // for now we use source-over blending by default
}


void VulkanCanvas::onSetPen(const Gfx::Pen& pen)
{
    _pen = pen;
}


void VulkanCanvas::onApplyPen()
{
    const Gfx::Color& c = _pen.color();
    _pushConstants.color[0] = c.red() / 255.0f;
    _pushConstants.color[1] = c.green() / 255.0f;
    _pushConstants.color[2] = c.blue() / 255.0f;
    _pushConstants.color[3] = c.alpha() / 255.0f;
}


void VulkanCanvas::onSetBrush(const Gfx::Brush& brush)
{
    _brush = brush;
}


void VulkanCanvas::onApplyBrush()
{
    const Gfx::Color& c = _brush.color();
    _pushConstants.color[0] = c.red() / 255.0f;
    _pushConstants.color[1] = c.green() / 255.0f;
    _pushConstants.color[2] = c.blue() / 255.0f;
    _pushConstants.color[3] = c.alpha() / 255.0f;
}


void VulkanCanvas::onSetFont(const Gfx::Font& font)
{
    _font = font;
}


void VulkanCanvas::onApplyFont()
{
    // font metrics are evaluated lazily in onGetFontMetrics
}


void VulkanCanvas::onSetClip(const Gfx::RectF* clip)
{
    if( ! _target || ! _inRenderPass )
        return;

    if( clip )
    {
        VkRect2D scissor;
        scissor.offset.x = static_cast<int32_t>(clip->x());
        scissor.offset.y = static_cast<int32_t>(clip->y());
        scissor.extent.width = static_cast<uint32_t>(clip->width());
        scissor.extent.height = static_cast<uint32_t>(clip->height());
        vkCmdSetScissor(_cmdBuf, 0, 1, &scissor);
    }
    else
    {
        VkRect2D scissor;
        scissor.offset.x = 0;
        scissor.offset.y = 0;
        scissor.extent.width = _target->width();
        scissor.extent.height = _target->height();
        vkCmdSetScissor(_cmdBuf, 0, 1, &scissor);
    }
}


void VulkanCanvas::onApplyClip()
{
    // scissor was already set in onSetClip
}


void VulkanCanvas::onDrawLine(const Gfx::PointF& from, const Gfx::PointF& to)
{
    if( ! _inRenderPass )
        return;

    float vertices[] = {
        static_cast<float>(from.x()), static_cast<float>(from.y()),
        static_cast<float>(to.x()), static_cast<float>(to.y())
    };

    uploadVertices(vertices, sizeof(vertices));

    vkCmdBindPipeline(_cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS,
                      _solidFillPipeline);

    vkCmdPushConstants(_cmdBuf, _pipelineLayout, VK_SHADER_STAGE_ALL,
                       0, sizeof(_pushConstants), &_pushConstants);

    VkDeviceSize offset = 0;
    vkCmdBindVertexBuffers(_cmdBuf, 0, 1, &_vertexBuffer, &offset);
    vkCmdDraw(_cmdBuf, 2, 1, 0, 0);
}


void VulkanCanvas::onDrawPolyline(const Gfx::PointF* pts, const size_t n)
{
    if( ! _inRenderPass || n < 2 )
        return;

    std::vector<float> vertices(n * 2);
    for(size_t i = 0; i < n; ++i)
    {
        vertices[i * 2]     = static_cast<float>(pts[i].x());
        vertices[i * 2 + 1] = static_cast<float>(pts[i].y());
    }

    uploadVertices(vertices.data(), vertices.size() * sizeof(float));

    vkCmdBindPipeline(_cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS,
                      _solidFillPipeline);

    vkCmdPushConstants(_cmdBuf, _pipelineLayout, VK_SHADER_STAGE_ALL,
                       0, sizeof(_pushConstants), &_pushConstants);

    VkDeviceSize offset = 0;
    vkCmdBindVertexBuffers(_cmdBuf, 0, 1, &_vertexBuffer, &offset);
    vkCmdDraw(_cmdBuf, static_cast<uint32_t>(n), 1, 0, 0);
}


void VulkanCanvas::onFillPolygon(const Gfx::PointF* ps, const size_t n)
{
    if( ! _inRenderPass || n < 3 )
        return;

    // triangulate as a fan from the first vertex
    std::vector<float> vertices;
    vertices.reserve((n - 2) * 6);

    for(size_t i = 1; i < n - 1; ++i)
    {
        vertices.push_back( static_cast<float>(ps[0].x()) );
        vertices.push_back( static_cast<float>(ps[0].y()) );
        vertices.push_back( static_cast<float>(ps[i].x()) );
        vertices.push_back( static_cast<float>(ps[i].y()) );
        vertices.push_back( static_cast<float>(ps[i + 1].x()) );
        vertices.push_back( static_cast<float>(ps[i + 1].y()) );
    }

    uploadVertices(vertices.data(), vertices.size() * sizeof(float));

    vkCmdBindPipeline(_cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS,
                      _solidFillPipeline);

    vkCmdPushConstants(_cmdBuf, _pipelineLayout, VK_SHADER_STAGE_ALL,
                       0, sizeof(_pushConstants), &_pushConstants);

    VkDeviceSize offset = 0;
    vkCmdBindVertexBuffers(_cmdBuf, 0, 1, &_vertexBuffer, &offset);
    vkCmdDraw(_cmdBuf, static_cast<uint32_t>((n - 2) * 3), 1, 0, 0);
}


void VulkanCanvas::onDrawRect(const Gfx::RectF& rect)
{
    if( ! _inRenderPass )
        return;

    float x = static_cast<float>(rect.x());
    float y = static_cast<float>(rect.y());
    float w = static_cast<float>(rect.width());
    float h = static_cast<float>(rect.height());

    // 4 line segments as a strip (5 vertices)
    float vertices[] = {
        x, y,
        x + w, y,
        x + w, y + h,
        x, y + h,
        x, y
    };

    uploadVertices(vertices, sizeof(vertices));

    vkCmdBindPipeline(_cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS,
                      _solidFillPipeline);

    vkCmdPushConstants(_cmdBuf, _pipelineLayout, VK_SHADER_STAGE_ALL,
                       0, sizeof(_pushConstants), &_pushConstants);

    VkDeviceSize offset = 0;
    vkCmdBindVertexBuffers(_cmdBuf, 0, 1, &_vertexBuffer, &offset);
    vkCmdDraw(_cmdBuf, 5, 1, 0, 0);
}


void VulkanCanvas::onFillRect(const Gfx::RectF& rect)
{
    if( ! _inRenderPass )
        return;

    float x = static_cast<float>(rect.x());
    float y = static_cast<float>(rect.y());
    float w = static_cast<float>(rect.width());
    float h = static_cast<float>(rect.height());

    // two triangles forming a quad
    float vertices[] = {
        x, y,
        x + w, y,
        x, y + h,
        x + w, y,
        x + w, y + h,
        x, y + h
    };

    uploadVertices(vertices, sizeof(vertices));

    vkCmdBindPipeline(_cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS,
                      _solidFillPipeline);

    vkCmdPushConstants(_cmdBuf, _pipelineLayout, VK_SHADER_STAGE_ALL,
                       0, sizeof(_pushConstants), &_pushConstants);

    VkDeviceSize offset = 0;
    vkCmdBindVertexBuffers(_cmdBuf, 0, 1, &_vertexBuffer, &offset);
    vkCmdDraw(_cmdBuf, 6, 1, 0, 0);
}


void VulkanCanvas::onDrawEllipse(const Gfx::PointF& topLeft,
                                 const Gfx::SizeF& size)
{
    if( ! _inRenderPass )
        return;

    const int segments = 64;
    float cx = static_cast<float>(topLeft.x() + size.width() * 0.5);
    float cy = static_cast<float>(topLeft.y() + size.height() * 0.5);
    float rx = static_cast<float>(size.width() * 0.5);
    float ry = static_cast<float>(size.height() * 0.5);

    std::vector<float> vertices((segments + 1) * 2);

    for(int i = 0; i <= segments; ++i)
    {
        float angle = 2.0f * 3.14159265f * i / segments;
        vertices[i * 2]     = cx + rx * std::cos(angle);
        vertices[i * 2 + 1] = cy + ry * std::sin(angle);
    }

    uploadVertices(vertices.data(), vertices.size() * sizeof(float));

    vkCmdBindPipeline(_cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS,
                      _solidFillPipeline);

    vkCmdPushConstants(_cmdBuf, _pipelineLayout, VK_SHADER_STAGE_ALL,
                       0, sizeof(_pushConstants), &_pushConstants);

    VkDeviceSize offset = 0;
    vkCmdBindVertexBuffers(_cmdBuf, 0, 1, &_vertexBuffer, &offset);
    vkCmdDraw(_cmdBuf, segments + 1, 1, 0, 0);
}


void VulkanCanvas::onFillEllipse(const Gfx::PointF& topLeft,
                                 const Gfx::SizeF& size)
{
    if( ! _inRenderPass )
        return;

    const int segments = 64;
    float cx = static_cast<float>(topLeft.x() + size.width() * 0.5);
    float cy = static_cast<float>(topLeft.y() + size.height() * 0.5);
    float rx = static_cast<float>(size.width() * 0.5);
    float ry = static_cast<float>(size.height() * 0.5);

    // fan triangulation from center
    std::vector<float> vertices(segments * 3 * 2);

    for(int i = 0; i < segments; ++i)
    {
        float a0 = 2.0f * 3.14159265f * i / segments;
        float a1 = 2.0f * 3.14159265f * (i + 1) / segments;

        int idx = i * 6;
        vertices[idx]     = cx;
        vertices[idx + 1] = cy;
        vertices[idx + 2] = cx + rx * std::cos(a0);
        vertices[idx + 3] = cy + ry * std::sin(a0);
        vertices[idx + 4] = cx + rx * std::cos(a1);
        vertices[idx + 5] = cy + ry * std::sin(a1);
    }

    uploadVertices(vertices.data(), vertices.size() * sizeof(float));

    vkCmdBindPipeline(_cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS,
                      _solidFillPipeline);

    vkCmdPushConstants(_cmdBuf, _pipelineLayout, VK_SHADER_STAGE_ALL,
                       0, sizeof(_pushConstants), &_pushConstants);

    VkDeviceSize offset = 0;
    vkCmdBindVertexBuffers(_cmdBuf, 0, 1, &_vertexBuffer, &offset);
    vkCmdDraw(_cmdBuf, static_cast<uint32_t>(segments * 3), 1, 0, 0);
}


void VulkanCanvas::onSetPath(const Gfx::Path& path)
{
    _path = path;
}


void VulkanCanvas::onDrawPath()
{
    // TODO: tessellate path outline and draw as line strip
}


void VulkanCanvas::onFillPath()
{
    // TODO: tessellate path interior and draw as triangles
}


void VulkanCanvas::onDrawPath(const Gfx::Path& /*path*/)
{
    // TODO: tessellate path outline and draw as line strip
}


void VulkanCanvas::onFillPath(const Gfx::Path& /*path*/)
{
    // TODO: tessellate path interior and draw as triangles
}


const Gfx::FontMetrics& VulkanCanvas::onGetFontMetrics() const
{
    return _fontMetrics;
}


Gfx::TextMetrics VulkanCanvas::onGetTextMetrics(const Pt::String& /*text*/) const
{
    // TODO: use FreeType for text measurement
    return Gfx::TextMetrics();
}


void VulkanCanvas::onDrawText(const Gfx::PointF& /*to*/,
                              const Pt::String& /*text*/,
                              const Gfx::Transform* /*transform*/)
{
    // TODO: render glyphs via FreeType glyph atlas
}


void VulkanCanvas::onDrawImage(const Gfx::PointF& to,
                               const Gfx::Image& image,
                               const Gfx::RectF* rect)
{
    if( ! _inRenderPass )
        return;

    // TODO: upload image to VkImage, draw textured quad
    // For now, draw a placeholder rectangle
    float x = static_cast<float>(to.x());
    float y = static_cast<float>(to.y());
    float w = static_cast<float>(rect ? rect->width() : image.width());
    float h = static_cast<float>(rect ? rect->height() : image.height());

    Gfx::RectF r(Gfx::PointF(x, y), Gfx::SizeF(w, h));
    onFillRect(r);
}


void VulkanCanvas::createRenderPass()
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

    if( vkCreateRenderPass(_device->device(), &rpInfo, 0, &_renderPass) != VK_SUCCESS )
        throw std::runtime_error("failed to create Vulkan render pass");
}


void VulkanCanvas::createPipelines()
{
    VkDevice dev = _device->device();

    // descriptor set layout for texture sampling
    VkDescriptorSetLayoutBinding samplerBinding = {};
    samplerBinding.binding = 0;
    samplerBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    samplerBinding.descriptorCount = 1;
    samplerBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    VkDescriptorSetLayoutCreateInfo layoutInfo = {};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = 1;
    layoutInfo.pBindings = &samplerBinding;

    if( vkCreateDescriptorSetLayout(dev, &layoutInfo, 0,
                                    &_descriptorSetLayout) != VK_SUCCESS )
        throw std::runtime_error("failed to create descriptor set layout");

    // push constant range: mat4 transform + vec4 color = 80 bytes
    VkPushConstantRange pushRange = {};
    pushRange.stageFlags = VK_SHADER_STAGE_ALL;
    pushRange.offset = 0;
    pushRange.size = sizeof(PushConstants);

    VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &_descriptorSetLayout;
    pipelineLayoutInfo.pushConstantRangeCount = 1;
    pipelineLayoutInfo.pPushConstantRanges = &pushRange;

    if( vkCreatePipelineLayout(dev, &pipelineLayoutInfo, 0,
                               &_pipelineLayout) != VK_SUCCESS )
        throw std::runtime_error("failed to create pipeline layout");

    // shader modules
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

    // vertex input: vec2 position
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

    // dynamic viewport and scissor
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

    // source-over blending
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

    if( vkCreateGraphicsPipelines(dev, VK_NULL_HANDLE, 1, &pipelineInfo, 0,
                                  &_solidFillPipeline) != VK_SUCCESS )
        throw std::runtime_error("failed to create solid fill pipeline");

    vkDestroyShaderModule(dev, vertModule, 0);
    vkDestroyShaderModule(dev, fragModule, 0);

    // textured pipeline
    VkShaderModule texVertModule = createShaderModule(texturedVertSpv,
                                                     texturedVertSpvSize);
    VkShaderModule texFragModule = createShaderModule(texturedFragSpv,
                                                     texturedFragSpvSize);

    shaderStages[0].module = texVertModule;
    shaderStages[1].module = texFragModule;

    // textured vertex input: vec2 position + vec2 texcoord
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

    if( vkCreateGraphicsPipelines(dev, VK_NULL_HANDLE, 1, &pipelineInfo, 0,
                                  &_texturedPipeline) != VK_SUCCESS )
        throw std::runtime_error("failed to create textured pipeline");

    vkDestroyShaderModule(dev, texVertModule, 0);
    vkDestroyShaderModule(dev, texFragModule, 0);
}


void VulkanCanvas::createDescriptorPool()
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

    if( vkCreateDescriptorPool(_device->device(), &poolInfo, 0,
                               &_descriptorPool) != VK_SUCCESS )
        throw std::runtime_error("failed to create descriptor pool");
}


void VulkanCanvas::beginRenderPass()
{
    if( ! _target || _inRenderPass )
        return;

    // create framebuffer for target if not yet created
    if( _target->framebuffer() == VK_NULL_HANDLE )
    {
        VkImageView attachments[] = { _target->imageView() };

        VkFramebufferCreateInfo fbInfo = {};
        fbInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        fbInfo.renderPass = _renderPass;
        fbInfo.attachmentCount = 1;
        fbInfo.pAttachments = attachments;
        fbInfo.width = _target->width();
        fbInfo.height = _target->height();
        fbInfo.layers = 1;

        VkFramebuffer fb;
        if( vkCreateFramebuffer(_device->device(), &fbInfo, 0, &fb) != VK_SUCCESS )
            throw std::runtime_error("failed to create framebuffer");

        // store via const_cast since VulkanBuffer doesn't expose setter
        const_cast<VulkanBuffer*>(_target)->_framebuffer = fb;
    }

    VkRenderPassBeginInfo rpBegin = {};
    rpBegin.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    rpBegin.renderPass = _renderPass;
    rpBegin.framebuffer = _target->framebuffer();
    rpBegin.renderArea.offset = {0, 0};
    rpBegin.renderArea.extent.width = _target->width();
    rpBegin.renderArea.extent.height = _target->height();

    vkCmdBeginRenderPass(_cmdBuf, &rpBegin, VK_SUBPASS_CONTENTS_INLINE);

    // set default viewport and scissor
    VkViewport viewport = {};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(_target->width());
    viewport.height = static_cast<float>(_target->height());
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(_cmdBuf, 0, 1, &viewport);

    VkRect2D scissor = {};
    scissor.offset = {0, 0};
    scissor.extent.width = _target->width();
    scissor.extent.height = _target->height();
    vkCmdSetScissor(_cmdBuf, 0, 1, &scissor);

    _inRenderPass = true;
}


void VulkanCanvas::endRenderPass()
{
    if( _inRenderPass )
    {
        vkCmdEndRenderPass(_cmdBuf);
        _inRenderPass = false;
    }
}


void VulkanCanvas::uploadVertices(const float* data, size_t size)
{
    if( size > _vertexBufferSize )
    {
        // reallocate larger vertex buffer
        VkDevice dev = _device->device();

        vkDestroyBuffer(dev, _vertexBuffer, 0);
        vkFreeMemory(dev, _vertexMemory, 0);

        _vertexBufferSize = size * 2;

        VkBufferCreateInfo bufInfo = {};
        bufInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufInfo.size = _vertexBufferSize;
        bufInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        bufInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        vkCreateBuffer(dev, &bufInfo, 0, &_vertexBuffer);

        VkMemoryRequirements memReqs;
        vkGetBufferMemoryRequirements(dev, _vertexBuffer, &memReqs);

        VkMemoryAllocateInfo memAlloc = {};
        memAlloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        memAlloc.allocationSize = memReqs.size;
        memAlloc.memoryTypeIndex = _device->findMemoryType(
            memReqs.memoryTypeBits,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        vkAllocateMemory(dev, &memAlloc, 0, &_vertexMemory);
        vkBindBufferMemory(dev, _vertexBuffer, _vertexMemory, 0);
    }

    void* mapped = 0;
    vkMapMemory(_device->device(), _vertexMemory, 0, size, 0, &mapped);
    std::memcpy(mapped, data, size);
    vkUnmapMemory(_device->device(), _vertexMemory);
}


void VulkanCanvas::buildOrthoMatrix(float* mat, float width, float height)
{
    // column-major orthographic projection [0, width] x [0, height] -> [-1, 1]
    std::memset(mat, 0, 16 * sizeof(float));
    mat[0]  =  2.0f / width;
    mat[5]  =  2.0f / height;
    mat[10] =  1.0f;
    mat[12] = -1.0f;
    mat[13] = -1.0f;
    mat[15] =  1.0f;
}


VkShaderModule VulkanCanvas::createShaderModule(const uint32_t* code, size_t size)
{
    VkShaderModuleCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = size;
    createInfo.pCode = code;

    VkShaderModule shaderModule;
    if( vkCreateShaderModule(_device->device(), &createInfo, 0,
                             &shaderModule) != VK_SUCCESS )
        throw std::runtime_error("failed to create Vulkan shader module");

    return shaderModule;
}

} // namespace

} // namespace
