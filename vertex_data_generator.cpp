/*
    Orignal Source: txt plugin for Okular:
    SPDX-FileCopyrightText: 2013 Azat Khuzhin <a3at.mail@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later

    Code has been modified by Benjamin Bingham to become: vertex_data_generator
*/
#include <iostream>

#include "vertex_data_generator.h"
#include <QPainter>
#include <QAbstractTextDocumentLayout>
#include <KAboutData>
#include <KConfigDialog>
#include <KLocalizedString>
#include <core/page.h>
#include <core/utils.h>
#include <vector>
#include <fstream>
#include <cstring>

#include <core/textdocumentgenerator.h>
#include <core/textdocumentgenerator_p.h>

OKULAR_EXPORT_PLUGIN(VertexDataGenerator, "libokularGenerator_vertex_data.json")

void error_callback(int error, const char* description) {
    std::cout << "GLFW Error[" << error << "]:" << description << std::endl;
}

VertexDataGenerator::VertexDataGenerator(QObject *parent, const QVariantList &args) {
    Q_UNUSED(parent);
    Q_UNUSED(args);

    if (m_VertexDataGeneratorCount == 0) {
                                    // m_App.initWindow();
                                    // m_App.initVulkan();
    }

    m_VertexDataGeneratorCount++;
}

VertexDataGenerator::~VertexDataGenerator() {
    if (m_VertexDataGeneratorCount == 1) {
                                            // m_App.cleanup();
    }

    m_VertexDataGeneratorCount--;
}

bool VertexDataGenerator::doCloseDocument() {
    return true;
}

bool VertexDataGenerator::loadDocument(const QString &fileName, QVector<Okular::Page *> &pagesVector) {
    std::ifstream file(fileName.toStdString());
    std::string str;
    std::string content;
    while (std::getline(file, str)) {
        content += str;
        content += ' ';
    }

    Okular::Page* page = new Okular::Page(0, 850, 1100, Okular::Rotation0);

    Okular::TextPage* txtPage = new Okular::TextPage{};
    txtPage->append(QString::fromStdString(content), new Okular::NormalizedRect( QRect(0.0, 0.2, 0.2, 0.2), 0.2, 0.2 ));
    page->setTextPage(txtPage);

    pagesVector.append(page);

    return true;
}

void insertImageMemoryBarrier(
    VkCommandBuffer cmdbuffer,
    VkImage image,
    VkAccessFlags srcAccessMask,
    VkAccessFlags dstAccessMask,
    VkImageLayout oldImageLayout,
    VkImageLayout newImageLayout,
    VkPipelineStageFlags srcStageMask,
    VkPipelineStageFlags dstStageMask,
    VkImageSubresourceRange subresourceRange)
{
    VkImageMemoryBarrier imageMemoryBarrier{};
    imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    imageMemoryBarrier.srcAccessMask = srcAccessMask;
    imageMemoryBarrier.dstAccessMask = dstAccessMask;
    imageMemoryBarrier.oldLayout = oldImageLayout;
    imageMemoryBarrier.newLayout = newImageLayout;
    imageMemoryBarrier.image = image;
    imageMemoryBarrier.subresourceRange = subresourceRange;

    vkCmdPipelineBarrier(
        cmdbuffer,
        srcStageMask,
        dstStageMask,
        0,
        0, nullptr,
        0, nullptr,
        1, &imageMemoryBarrier
    );
}

void flushCommandBuffer(VkCommandBuffer commandBuffer, VkQueue queue, VkCommandPool pool, bool free, HelloTriangleApplication& m_App)
{
    if (commandBuffer == VK_NULL_HANDLE) {
        return;
    }

    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
        std::cout << "Failed to end command buffer" << std::endl;
    }

    VkSubmitInfo submitInfo;
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;
    submitInfo.waitSemaphoreCount = 0;
    submitInfo.pNext = nullptr;
    submitInfo.pSignalSemaphores = nullptr;
    submitInfo.signalSemaphoreCount = 0;
    // Create fence to ensure that the command buffer has finished executing
    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    VkFence fence;
    if(vkCreateFence(m_App.m_Device, &fenceInfo, nullptr, &fence) != VK_SUCCESS) {
        std::cout << "Failed to create Fence" << std::endl;
    }

    // Submit to the queue
    if(vkQueueSubmit(queue, 1, &submitInfo, fence) != VK_SUCCESS) {
        std::cout << "Failed to submit queue" << std::endl;
    }
    // Wait for the fence to signal that command buffer has finished executing
    if (vkWaitForFences(m_App.m_Device, 1, &fence, VK_TRUE, UINT64_MAX) != VK_SUCCESS) {
        std::cout << "Couldent wait for fence" << std::endl;
    }
    vkDestroyFence(m_App.m_Device, fence, nullptr);
    if (free) {
        vkFreeCommandBuffers(m_App.m_Device, pool, 1, &commandBuffer);
    }
}

void VertexDataGenerator::generatePixmap(Okular::PixmapRequest* request) {
    std::string content = request->page()->text().toStdString();

    std::string delimeter = "\n";
    std::string currentToken;
    std::vector<float> tokens;

    for (auto c : content) {
        if (c == ' ') {
            if (currentToken != "" && currentToken != " " && currentToken != "  ") {
                tokens.push_back(std::stof(currentToken));
                currentToken = "";
            }
        }
        else {
            currentToken += c;
        }
    }

    std::vector<Vertex> freshVertices;

    for (int i = 0; i < tokens.size(); i += 2) {
        freshVertices.push_back(Vertex{ { tokens[i], tokens[i + 1] }, { 1.0f, 0.0f, 0.0f } });
    }

    // glfwPollEvents();

                                                                                            // m_App.createVertexBuffer(freshVertices); 

    // if (m_App.m_Drawn) { return; }

    // m_App.drawFrame(freshVertices.size()); =======================================================================

    std::cout << "a" << std::endl;

                                                                                            // // Wait for last frame to finish rendering
                                                                                            // vkWaitForFences(m_App.m_Device, 1, &m_App.m_InFlightFences[m_App.m_CurrentFrame], VK_TRUE, UINT64_MAX);
                                                                                            // vkDeviceWaitIdle(m_App.m_Device);

                                                                                            // unsigned int imageIndex;
                                                                                            // VkResult result = vkAcquireNextImageKHR(m_App.m_Device, m_App.m_SwapChain, UINT64_MAX, m_App.m_ImageAvailableSemaphores[m_App.m_CurrentFrame], VK_NULL_HANDLE, &imageIndex);

                                                                                            // if (result == VK_ERROR_OUT_OF_DATE_KHR) {
                                                                                            //     m_App.recreateSwapChain();
                                                                                            //     return;
                                                                                            // } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
                                                                                            //     throw std::runtime_error("Failed to acquire swap chain image");
                                                                                            // }

                                                                                            // vkResetFences(m_App.m_Device, 1, &m_App.m_InFlightFences[m_App.m_CurrentFrame]);

                                                                                            // vkResetCommandBuffer(m_App.m_CommandBuffers[m_App.m_CurrentFrame], 0);
                                                                                            // m_App.recordCommandBuffer(m_App.m_CommandBuffers[m_App.m_CurrentFrame], imageIndex, freshVertices.size());

                                                                                            // VkSubmitInfo submitInfo{};
                                                                                            // submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
                                                                                            // submitInfo.pNext = nullptr;

                                                                                            // // The command buffer will not be submitted until the semaphore is signled
                                                                                            // VkSemaphore waitSemaphores[] = { m_App.m_ImageAvailableSemaphores[m_App.m_CurrentFrame] };
                                                                                            // VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
                                                                                            // submitInfo.waitSemaphoreCount = 1;
                                                                                            // submitInfo.pWaitSemaphores = waitSemaphores;
                                                                                            // submitInfo.pWaitDstStageMask = waitStages;
                                                                                            // submitInfo.commandBufferCount = 1;
                                                                                            // submitInfo.pCommandBuffers = &m_App.m_CommandBuffers[m_App.m_CurrentFrame];

                                                                                            // // The signaled semaphore will be signaled once the command buffer is done executing
                                                                                            // std::cout << m_App.m_CurrentFrame << std::endl;
                                                                                            // VkSemaphore signalSemaphores[] = { m_App.m_RenderFinishedSemaphores[m_App.m_CurrentFrame] };
                                                                                            // submitInfo.signalSemaphoreCount = 1;
                                                                                            // submitInfo.pSignalSemaphores = signalSemaphores;

                                                                                            // std::cout << "b" << std::endl;

                                                                                            // // int j = 0;
                                                                                            // // std::cout << "m_RenderFinishedSemaphores" << std::endl;
                                                                                            // // for (auto semaphore : m_App.m_RenderFinishedSemaphores) {
                                                                                            // //     std::cout << "  Semaphore " << j << ": " << semaphore << std::endl;

                                                                                            // //     j++;
                                                                                            // // }

                                                                                            // // int b = 0;
                                                                                            // // std::cout << "m_ImageAvailableSemaphores" << std::endl;
                                                                                            // // for (auto semaphore : m_App.m_ImageAvailableSemaphores) {
                                                                                            // //     std::cout << "  Semaphore " << b << ": " << semaphore << std::endl;

                                                                                            // //     b++;
                                                                                            // // }

                                                                                            // // The fence will be signaled once the command buffer is finished executing
                                                                                            // if (vkQueueSubmit(m_App.m_GraphicsQueue, 1, &submitInfo, m_App.m_InFlightFences[m_App.m_CurrentFrame]) != VK_SUCCESS) {
                                                                                            //     throw std::runtime_error("Failed to submit draw command buffer");
                                                                                            //     std::cout << "Failed to submit draw command buffer ==============================" << std::endl;
                                                                                            // }
                                                                                            // vkQueueWaitIdle(m_App.m_GraphicsQueue);

                                                                                            // VkPresentInfoKHR presentInfo{};
                                                                                            // presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
                                                                                            // presentInfo.waitSemaphoreCount = 1;
                                                                                            // presentInfo.pWaitSemaphores = signalSemaphores; // The semaphores that will be waited on before we present

                                                                                            // VkSwapchainKHR swapChains[] = { m_App.m_SwapChain };
                                                                                            // presentInfo.swapchainCount = 1;
                                                                                            // presentInfo.pSwapchains = swapChains;
                                                                                            // presentInfo.pImageIndices = &imageIndex;
                                                                                            // presentInfo.pResults = nullptr;

                                                                                            // result = vkQueuePresentKHR(m_App.m_PresentQueue, &presentInfo);

                                                                                            // std::cout << "c" << std::endl;

                                                                                            // if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_App.m_FramebufferResized) {
                                                                                            //     m_App.m_FramebufferResized = false;
                                                                                            //     m_App.recreateSwapChain();
                                                                                            // } else if (result != VK_SUCCESS) {
                                                                                            //     throw std::runtime_error("Failed to present swap chain image");
                                                                                            // }

                                                                                            // m_App.m_CurrentFrame = (m_App.m_CurrentFrame + 1) % m_App.m_MaxFramesInFlight;

    // Copying image to CPU ==========================================================================================================

                                                                                                                                                                        // bool screenshotSaved = false;
                                                                                                                                                                        // bool supportsBlit = true;

                                                                                                                                                                        // // Check blit support for source and destination
                                                                                                                                                                        // VkFormatProperties formatProps;

                                                                                                                                                                        // // Check if the device supports blitting from optimal images (the swapchain images are in optimal format)
                                                                                                                                                                        // vkGetPhysicalDeviceFormatProperties(m_App.m_PhysicalDevice, VK_FORMAT_R8G8B8A8_UNORM, &formatProps); // TODO color format could be wrong
                                                                                                                                                                        // if (!(formatProps.optimalTilingFeatures & VK_FORMAT_FEATURE_BLIT_SRC_BIT)) {
                                                                                                                                                                        //     std::cerr << "Device does not support blitting from optimal tiled images, using copy instead of blit!" << std::endl;
                                                                                                                                                                        //     supportsBlit = false;
                                                                                                                                                                        // }

                                                                                                                                                                        // // Check if the device supports blitting to linear images
                                                                                                                                                                        // vkGetPhysicalDeviceFormatProperties(m_App.m_PhysicalDevice, VK_FORMAT_R8G8B8A8_UNORM, &formatProps);
                                                                                                                                                                        // if (!(formatProps.linearTilingFeatures & VK_FORMAT_FEATURE_BLIT_DST_BIT)) {
                                                                                                                                                                        //     std::cerr << "Device does not support blitting to linear tiled images, using copy instead of blit!" << std::endl;
                                                                                                                                                                        //     supportsBlit = false;
                                                                                                                                                                        // }

                                                                                                                                                                        // // Source for the copy is the last rendered swapchain image
                                                                                                                                                                        // VkImage srcImage = m_App.m_SwapChainImages[m_App.m_CurrentFrame];

                                                                                                                                                                        // std::cout << "d" << std::endl;

                                                                                                                                                                        // // Create the linear tiled destination image to copy to and to read the memory from
                                                                                                                                                                        // VkImageCreateInfo imageCreateCI{};
                                                                                                                                                                        // imageCreateCI.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
                                                                                                                                                                        // imageCreateCI.imageType = VK_IMAGE_TYPE_2D;
                                                                                                                                                                        // // Note that vkCmdBlitImage (if supported) will also do format conversions if the swapchain color format would differ
                                                                                                                                                                        // imageCreateCI.format = VK_FORMAT_R8G8B8A8_UNORM;
                                                                                                                                                                        // imageCreateCI.extent.width = m_App.m_SwapChainExtent.width;
                                                                                                                                                                        // imageCreateCI.extent.height = m_App.m_SwapChainExtent.height;
                                                                                                                                                                        // imageCreateCI.extent.depth = 1;
                                                                                                                                                                        // imageCreateCI.arrayLayers = 1;
                                                                                                                                                                        // imageCreateCI.mipLevels = 1;
                                                                                                                                                                        // imageCreateCI.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
                                                                                                                                                                        // imageCreateCI.samples = VK_SAMPLE_COUNT_1_BIT;
                                                                                                                                                                        // imageCreateCI.tiling = VK_IMAGE_TILING_LINEAR;
                                                                                                                                                                        // imageCreateCI.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT;

                                                                                                                                                                        // // Create the image
                                                                                                                                                                        // VkImage dstImage;
                                                                                                                                                                        // if(vkCreateImage(m_App.m_Device, &imageCreateCI, nullptr, &dstImage) != VK_SUCCESS) {
                                                                                                                                                                        //     std::cout << "Could not create destination image" << std::endl;
                                                                                                                                                                        // }

                                                                                                                                                                        // // Create memory to back up the image
                                                                                                                                                                        // VkMemoryRequirements memRequirements{};
                                                                                                                                                                        // VkMemoryAllocateInfo memAllocInfo{};
                                                                                                                                                                        // memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
                                                                                                                                                                        // VkDeviceMemory dstImageMemory;
                                                                                                                                                                        // vkGetImageMemoryRequirements(m_App.m_Device, dstImage, &memRequirements);
                                                                                                                                                                        // memAllocInfo.allocationSize = memRequirements.size;
                                                                                                                                                                        // std::cout << "memRequirements.size: " << memRequirements.size << std::endl;
                                                                                                                                                                        // // Memory must be host visible to copy from
                                                                                                                                                                        // memAllocInfo.memoryTypeIndex = m_App.findMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

                                                                                                                                                                        // if (vkAllocateMemory(m_App.m_Device, &memAllocInfo, nullptr, &dstImageMemory) != VK_SUCCESS) {
                                                                                                                                                                        //     std::cout << "Failed to allocate memory" << std::endl;
                                                                                                                                                                        // }

                                                                                                                                                                        // if (vkBindImageMemory(m_App.m_Device, dstImage, dstImageMemory, 0) != VK_SUCCESS) {
                                                                                                                                                                        //     std::cout << "Failed to bind image memory" << std::endl;
                                                                                                                                                                        // }

                                                                                                                                                                        // std::cout << "e" << std::endl;

                                                                                                                                                                        // // Do the actual blit from the swapchain image to our host visible destination image
                                                                                                                                                                        // VkCommandBuffer gpuToCpuCommandBuffer;

                                                                                                                                                                        // VkCommandBufferAllocateInfo commandBufferAllocInfo{};
                                                                                                                                                                        // commandBufferAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
                                                                                                                                                                        // commandBufferAllocInfo.commandPool = m_App.m_CommandPool;
                                                                                                                                                                        // commandBufferAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
                                                                                                                                                                        // commandBufferAllocInfo.commandBufferCount = 1;

                                                                                                                                                                        // if (vkAllocateCommandBuffers(m_App.m_Device, &commandBufferAllocInfo, &gpuToCpuCommandBuffer) != VK_SUCCESS) {
                                                                                                                                                                        //     throw std::runtime_error("Failed to allocate command buffers");
                                                                                                                                                                        // }

                                                                                                                                                                        // VkCommandBufferBeginInfo cmdBufferInfoForScreenshot{};
                                                                                                                                                                        // cmdBufferInfoForScreenshot.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
                                                                                                                                                                        // cmdBufferInfoForScreenshot.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

                                                                                                                                                                        // if (vkBeginCommandBuffer(gpuToCpuCommandBuffer, &cmdBufferInfoForScreenshot) != VK_SUCCESS) {
                                                                                                                                                                        //     std::cout << "Failed to start command buffer" << std::endl;
                                                                                                                                                                        // }

                                                                                                                                                                        // // Transition destination image to transfer destination layout
                                                                                                                                                                        // insertImageMemoryBarrier(
                                                                                                                                                                        //     gpuToCpuCommandBuffer,
                                                                                                                                                                        //     dstImage,
                                                                                                                                                                        //     0,
                                                                                                                                                                        //     VK_ACCESS_TRANSFER_WRITE_BIT,
                                                                                                                                                                        //     VK_IMAGE_LAYOUT_UNDEFINED,
                                                                                                                                                                        //     VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                                                                                                                                                        //     VK_PIPELINE_STAGE_TRANSFER_BIT,
                                                                                                                                                                        //     VK_PIPELINE_STAGE_TRANSFER_BIT,
                                                                                                                                                                        //     VkImageSubresourceRange{ VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 }
                                                                                                                                                                        // );

                                                                                                                                                                        // std::cout << "f" << std::endl;

                                                                                                                                                                        // // Transition swapchain image from present to transfer source layout
                                                                                                                                                                        // insertImageMemoryBarrier(
                                                                                                                                                                        //     gpuToCpuCommandBuffer,
                                                                                                                                                                        //     srcImage,
                                                                                                                                                                        //     VK_ACCESS_MEMORY_READ_BIT,
                                                                                                                                                                        //     VK_ACCESS_TRANSFER_READ_BIT,
                                                                                                                                                                        //     VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                                                                                                                                                                        //     VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                                                                                                                                                                        //     VK_PIPELINE_STAGE_TRANSFER_BIT,
                                                                                                                                                                        //     VK_PIPELINE_STAGE_TRANSFER_BIT,
                                                                                                                                                                        //     VkImageSubresourceRange{ VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 }
                                                                                                                                                                        // );

                                                                                                                                                                        // // If source and destination support blit we'll blit as this also does automatic format conversion (e.g. from BGR to RGB)
                                                                                                                                                                        // if (supportsBlit)
                                                                                                                                                                        // {
                                                                                                                                                                        //     // Define the region to blit (we will blit the whole swapchain image)
                                                                                                                                                                        //     VkOffset3D blitSize;
                                                                                                                                                                        //     blitSize.x = m_App.m_SwapChainExtent.width;
                                                                                                                                                                        //     blitSize.y = m_App.m_SwapChainExtent.height;
                                                                                                                                                                        //     blitSize.z = 1;

                                                                                                                                                                        //     VkImageBlit imageBlitRegion{};
                                                                                                                                                                        //     imageBlitRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                                                                                                                                                                        //     imageBlitRegion.srcSubresource.layerCount = 1;
                                                                                                                                                                        //     imageBlitRegion.srcOffsets[1] = blitSize;
                                                                                                                                                                        //     imageBlitRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                                                                                                                                                                        //     imageBlitRegion.dstSubresource.layerCount = 1;
                                                                                                                                                                        //     imageBlitRegion.dstOffsets[1] = blitSize;

                                                                                                                                                                        //     // Issue the blit command
                                                                                                                                                                        //     vkCmdBlitImage(
                                                                                                                                                                        //         gpuToCpuCommandBuffer,
                                                                                                                                                                        //         srcImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                                                                                                                                                                        //         dstImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                                                                                                                                                        //         1,
                                                                                                                                                                        //         &imageBlitRegion,
                                                                                                                                                                        //         VK_FILTER_NEAREST
                                                                                                                                                                        //     );
                                                                                                                                                                        // }
                                                                                                                                                                        // else
                                                                                                                                                                        // {
                                                                                                                                                                        //     // Otherwise use image copy (requires us to manually flip components)
                                                                                                                                                                        //     VkImageCopy imageCopyRegion{};
                                                                                                                                                                        //     imageCopyRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                                                                                                                                                                        //     imageCopyRegion.srcSubresource.layerCount = 1;
                                                                                                                                                                        //     imageCopyRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                                                                                                                                                                        //     imageCopyRegion.dstSubresource.layerCount = 1;
                                                                                                                                                                        //     imageCopyRegion.extent.width = m_App.m_SwapChainExtent.width;
                                                                                                                                                                        //     imageCopyRegion.extent.height = m_App.m_SwapChainExtent.height;
                                                                                                                                                                        //     imageCopyRegion.extent.depth = 1;

                                                                                                                                                                        //     // Issue the copy command
                                                                                                                                                                        //     vkCmdCopyImage(
                                                                                                                                                                        //         gpuToCpuCommandBuffer,
                                                                                                                                                                        //         srcImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                                                                                                                                                                        //         dstImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                                                                                                                                                        //         1,
                                                                                                                                                                        //         &imageCopyRegion
                                                                                                                                                                        //     );
                                                                                                                                                                        // }

                                                                                                                                                                        // // Transition destination image to general layout, which is the required layout for mapping the image memory later on
                                                                                                                                                                        // insertImageMemoryBarrier(
                                                                                                                                                                        //     gpuToCpuCommandBuffer,
                                                                                                                                                                        //     dstImage,
                                                                                                                                                                        //     VK_ACCESS_TRANSFER_WRITE_BIT,
                                                                                                                                                                        //     VK_ACCESS_MEMORY_READ_BIT,
                                                                                                                                                                        //     VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                                                                                                                                                        //     VK_IMAGE_LAYOUT_GENERAL,
                                                                                                                                                                        //     VK_PIPELINE_STAGE_TRANSFER_BIT,
                                                                                                                                                                        //     VK_PIPELINE_STAGE_TRANSFER_BIT,
                                                                                                                                                                        //     VkImageSubresourceRange{ VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 }
                                                                                                                                                                        // );

                                                                                                                                                                        // std::cout << "g" << std::endl;

                                                                                                                                                                        // // Transition back the swap chain image after the blit is done
                                                                                                                                                                        // insertImageMemoryBarrier(
                                                                                                                                                                        //     gpuToCpuCommandBuffer,
                                                                                                                                                                        //     srcImage,
                                                                                                                                                                        //     VK_ACCESS_TRANSFER_READ_BIT,
                                                                                                                                                                        //     VK_ACCESS_MEMORY_READ_BIT,
                                                                                                                                                                        //     VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                                                                                                                                                                        //     VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                                                                                                                                                                        //     VK_PIPELINE_STAGE_TRANSFER_BIT,
                                                                                                                                                                        //     VK_PIPELINE_STAGE_TRANSFER_BIT,
                                                                                                                                                                        //     VkImageSubresourceRange{ VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 }
                                                                                                                                                                        // );

                                                                                                                                                                        // std::cout << "g.1" << std::endl;

                                                                                                                                                                        // flushCommandBuffer(gpuToCpuCommandBuffer, m_App.m_GraphicsQueue, m_App.m_CommandPool, true, m_App);
                                                                                                                                                                        // std::cout << "g.2" << std::endl;

                                                                                                                                                                        // // Get layout of the image (including row pitch)
                                                                                                                                                                        // VkImageSubresource subResource { VK_IMAGE_ASPECT_COLOR_BIT, 0, 0 };
                                                                                                                                                                        // VkSubresourceLayout subResourceLayout;
                                                                                                                                                                        // vkGetImageSubresourceLayout(m_App.m_Device, dstImage, &subResource, &subResourceLayout);
                                                                                                                                                                        // std::cout << "g.3" << std::endl;

                                                                                                                                                                        // std::cout << "About to map" << std::endl;
                                                                                                                                                                        // // Map image memory so we can start copying from it
                                                                                                                                                                        // const char* data;
                                                                                                                                                                        // vkMapMemory(m_App.m_Device, dstImageMemory, 0, VK_WHOLE_SIZE, 0, (void**)&data);
                                                                                                                                                                        // std::cout << "g.4" << std::endl;

                                                                                                                                                                        // std::cout << "Just mapped" << std::endl;
                                                                                                                                                                        // data += subResourceLayout.offset;
                                                                                                                                                                        // std::cout << "g.5" << std::endl;

                                                                                                                                                                        // std::cout << "===================== Printing =====================" << std::endl;
                                                                                                                                                                        // for (int i = 0; i < 4; ++i) {
                                                                                                                                                                        //     std::cout << (int)(char)data[i] << std::endl;
                                                                                                                                                                        // }
                                                                                                                                                                        // std::cout << "===================== Done Printing =====================" << std::endl;
                                                                                                                                                                        // std::cout << "g.6" << std::endl;

                                                                                                                                                                        // std::ofstream file("testScreenshot.txt", std::ios::out);

                                                                                                                                                                        // unsigned int width = m_App.m_SwapChainExtent.width;
                                                                                                                                                                        // unsigned int height = m_App.m_SwapChainExtent.height;

                                                                                                                                                                        // std::cout << "WIDTH: " << width << std::endl;
                                                                                                                                                                        // std::cout << "HEIGHT: " << height << std::endl;

                                                                                                                                                                        // // ppm header
                                                                                                                                                                        // file << "P6\n" << m_App.m_SwapChainExtent.width << "\n" << m_App.m_SwapChainExtent.height << "\n" << 255 << "\n";

                                                                                                                                                                        // // If source is BGR (destination is always RGB) and we can't use blit (which does automatic conversion), we'll have to manually swizzle color components
                                                                                                                                                                        // bool colorSwizzle = false;
                                                                                                                                                                        // // Check if source is BGR
                                                                                                                                                                        // // Note: Not complete, only contains most common and basic BGR surface formats for demonstration purposes
                                                                                                                                                                        // if (!supportsBlit)
                                                                                                                                                                        // {
                                                                                                                                                                        //     std::vector<VkFormat> formatsBGR = { VK_FORMAT_B8G8R8A8_SRGB, VK_FORMAT_B8G8R8A8_UNORM, VK_FORMAT_B8G8R8A8_SNORM };
                                                                                                                                                                        //     colorSwizzle = (std::find(formatsBGR.begin(), formatsBGR.end(), VK_FORMAT_R8G8B8A8_UNORM) != formatsBGR.end());
                                                                                                                                                                        // }

                                                                                                                                                                        // // ppm binary pixel data
                                                                                                                                                                        // for (uint32_t y = 0; y < height; y++)
                                                                                                                                                                        // {
                                                                                                                                                                        //     // std::cout << "height" << std::endl;
                                                                                                                                                                        //     unsigned int *row = (unsigned int*)data;
                                                                                                                                                                        //     for (uint32_t x = 0; x < width; x++)
                                                                                                                                                                        //     {
                                                                                                                                                                        //         if (colorSwizzle)
                                                                                                                                                                        //         {
                                                                                                                                                                        //             file.write((char*)row+2, 1);
                                                                                                                                                                        //             file.write((char*)row+1, 1);
                                                                                                                                                                        //             file.write((char*)row, 1);
                                                                                                                                                                        //         }
                                                                                                                                                                        //         else
                                                                                                                                                                        //         {
                                                                                                                                                                        //             file.write((char*)row, 3);
                                                                                                                                                                        //         }
                                                                                                                                                                        //         row++;
                                                                                                                                                                        //     }
                                                                                                                                                                        //     data += subResourceLayout.rowPitch;
                                                                                                                                                                        // }
                                                                                                                                                                        // file.close();

                                                                                                                                                                        // std::cout << "Screenshot saved to disk" << std::endl;

                                                                                                                                                                        // //Clean up resources


                                                                                                                                                                        // screenshotSaved = true;

                                                                                                                                                                        // std::cout << "h" << std::endl;
                                                                            // VkBuffer gpuToCpuStagingBuffer;
                                                                            // VkDeviceMemory gpuToCpuStagingBufferMemory;

                                                                            // VkDeviceSize imageSize = m_App.m_Wdith * m_App.m_Height * 4; // TODO 4 may be wrong

                                                                            // m_App.createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, gpuToCpuStagingBuffer, gpuToCpuStagingBufferMemory);
                                                                            
                                                                            // VkCommandBufferAllocateInfo allocInfo{};
                                                                            // allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
                                                                            // allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
                                                                            // allocInfo.commandPool = m_App.m_CommandPool;
                                                                            // allocInfo.commandBufferCount = 1;

                                                                            // VkCommandBuffer commandBuffer;
                                                                            // if (vkAllocateCommandBuffers(m_App.m_Device, &allocInfo, &commandBuffer) != VK_SUCCESS) {
                                                                            //     std::cout << "Failed to create command buffer used to copy info out of image" << std::endl;
                                                                            // }

                                                                            // // vkResetCommandBuffer(commandBuffer, 0);

                                                                            // VkCommandBufferBeginInfo beginInfo{};
                                                                            // beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
                                                                            // beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
                                                                            // beginInfo.pInheritanceInfo = nullptr;

                                                                            // if(vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
                                                                            //     std::cout << "Failed to start commandBuffer" << std::endl;
                                                                            // }

                                                                            // VkImageMemoryBarrier barrier{};
                                                                            // barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
                                                                            // barrier.oldLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
                                                                            // barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
                                                                            // barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                                                                            // barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                                                                            // barrier.image = m_App.m_SwapChainImages[imageIndex];
                                                                            // barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                                                                            // barrier.subresourceRange.baseMipLevel = 0;
                                                                            // barrier.subresourceRange.levelCount = 1;
                                                                            // barrier.subresourceRange.baseArrayLayer = 0;
                                                                            // barrier.subresourceRange.layerCount = 1;

                                                                            // barrier.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
                                                                            // barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

                                                                            // VkPipelineStageFlags  sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
                                                                            // VkPipelineStageFlags  destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;

                                                                            // vkCmdPipelineBarrier(
                                                                            //     commandBuffer, 
                                                                            //     sourceStage, 
                                                                            //     destinationStage,
                                                                            //     0,
                                                                            //     0,
                                                                            //     nullptr,
                                                                            //     0,
                                                                            //     nullptr,
                                                                            //     1,
                                                                            //     &barrier 
                                                                            // );

                                                                            // VkBufferImageCopy copyRegion{};
                                                                            // copyRegion.bufferOffset = 0;
                                                                            // copyRegion.bufferRowLength = 0;
                                                                            // copyRegion.bufferImageHeight = 0;
                                                                            // copyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                                                                            // copyRegion.imageSubresource.mipLevel = 0;
                                                                            // copyRegion.imageSubresource.baseArrayLayer = 0;
                                                                            // copyRegion.imageSubresource.layerCount = 1;
                                                                            // copyRegion.imageOffset = { 0, 0, 0 };
                                                                            // copyRegion.imageExtent = { m_App.m_Wdith, m_App.m_Height, 1 };

                                                                            // vkCmdCopyImageToBuffer(commandBuffer, m_App.m_SwapChainImages[imageIndex], VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, gpuToCpuStagingBuffer, 1, &copyRegion);

                                                                            // barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
                                                                            // barrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

                                                                            // barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
                                                                            // barrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;

                                                                            // sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
                                                                            // destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;

                                                                            // vkCmdPipelineBarrier(
                                                                            //     commandBuffer, 
                                                                            //     sourceStage, 
                                                                            //     destinationStage,
                                                                            //     0,
                                                                            //     0,
                                                                            //     nullptr,
                                                                            //     0,
                                                                            //     nullptr,
                                                                            //     1,
                                                                            //     &barrier 
                                                                            // );

                                                                            // result = vkEndCommandBuffer(commandBuffer);
                                                                            // std::cout << "vkEndCommandBuffer Result: " << result << std::endl;

                                                                            // VkSubmitInfo submitInfo2{};
                                                                            // submitInfo2.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
                                                                            // submitInfo2.commandBufferCount = 1;
                                                                            // submitInfo2.pCommandBuffers = &commandBuffer;
                                                                            // // std::cout << "Copy Command buffer (custom one): " << commandBuffer << std::endl;

                                                                            // // std::cout << "App command buffers:" << std::endl;
                                                                            // // int i = 0;
                                                                            // // for (auto buffer : m_App.m_CommandBuffers) {
                                                                            // //     std::cout << "  Buffer " << i << ": " << buffer << std::endl;

                                                                            // //     i++;
                                                                            // // }

                                                                            // vkWaitForFences(m_App.m_Device, 1, &m_App.m_InFlightFences[m_App.m_CurrentFrame], VK_TRUE, UINT64_MAX);

                                                                            // vkResetFences(m_App.m_Device, 1, &m_App.m_InFlightFences[m_App.m_CurrentFrame]);

                                                                            // result = vkQueueSubmit(m_App.m_GraphicsQueue, 1, &submitInfo2, VK_NULL_HANDLE);
                                                                            // std::cout << "vkQueueSubmit Result: " << result << std::endl;

                                                                            // vkQueueWaitIdle(m_App.m_GraphicsQueue);

                                                                            // vkFreeCommandBuffers(m_App.m_Device, m_App.m_CommandPool, 1, &commandBuffer);

                                                                            // void* data;
                                                                            // std::vector<uint8_t> imageData(imageSize);
                                                                            // vkMapMemory(m_App.m_Device, gpuToCpuStagingBufferMemory, 0, imageSize, 0, &data);
                                                                            //     // memcpy(data, imageData.data(), imageSize);
                                                                            //     int y = 0;
                                                                            //     for (int i = 0; i < imageSize; ++i) {
                                                                            //         if (((float*)data)[i] > 0) {
                                                                            //             y++;
                                                                            //         }
                                                                            //     }

                                                                            //     std::cout << "y: " << y << std::endl;

                                                                            //     std::cout << "First color:" << std::endl;
                                                                            //     std::cout << "  R: " << ((float*)data)[0] << std::endl;
                                                                            //     std::cout << "  G: " << ((float*)data)[1] << std::endl;
                                                                            //     std::cout << "  B: " << ((float*)data)[2] << std::endl;
                                                                            //     std::cout << "  A: " << ((float*)data)[3] << std::endl;

                                                                            // vkUnmapMemory(m_App.m_Device, gpuToCpuStagingBufferMemory);

                                                                            // std::cout << "Image size: " << imageSize << std::endl;

                                                                            // int s = 0;
                                                                            // for (auto val : imageData) {
                                                                            //     if (val != 0) {
                                                                            //         std::cout << val << std::endl;
                                                                            //         s++;
                                                                            //     }
                                                                            // }

                                                                            // std::cout << "==================Number of s's:" << s << "=================" << std::endl;

                                                                            // std::cout << imageData[0] << std::endl;

                                                                            // // for (int i = 0; i < 100; ++i) {
                                                                            // //     std::cout << imageData[i] << ", ";
                                                                            // // }

                                                                            // std::cout << std::endl;

    // Copying image to CPU ========================]

    // =======================================================================

                                                                                                // vkDeviceWaitIdle(m_App.m_Device);

    std::cout << "i" << std::endl;

    const int size = request->width() * request->height() * 4;

    std::cout << "request size: " << size << std::endl;

    std::cout << "i.1" << std::endl;
    // GLubyte* array = nullptr;

    GLubyte* tempArray = new GLubyte[size];
    std::cout << "i.2" << std::endl;
    // std::memcpy(tempArray, data, size);

    int n = 100;

    // for (int i = 0; i < n; ++i) {

    //     tempArray[i] = data[i];

    //     // std::cout << "Finished: " << i << std::endl;
    // }

    // for (int i = n; i < size; ++i) {
    //     tempArray[i] = 0;
    // }

    for (int i = 0; i < size; i += 4) {
        tempArray[i + 0] = 0;
        tempArray[i + 1] = 0;
        tempArray[i + 2] = 1;
        tempArray[i + 3] = 1;
    }

    std::cout << "i.3" << std::endl;
    // GLubyte* newArray = new GLubyte[size];

    // for (size_t i = 0; i < (size_t)size; i += 4) {
    //     newArray[i] = tempArray[i + 2];
    //     newArray[i + 1] = tempArray[i + 1];
    //     newArray[i + 2] = tempArray[i];
    //     newArray[i + 3] = tempArray[i + 3];
    // }

    QImage image{ tempArray, request->width(), request->height(), QImage::Format_ARGB32 };
    std::cout << "i.4" << std::endl;
    // image.mirror(false, true); // TODO reimplement flipping

    request->page()->setPixmap(request->observer(), new QPixmap(QPixmap::fromImage(image)));
    std::cout << "i.5" << std::endl;
    // delete[] newArray;
    delete[] tempArray;
    std::cout << "i.6" << std::endl;
    std::cout << "j" << std::endl;


    signalPixmapRequestDone(request);

    // m_App.m_Drawn = true;

                                                                                                    // vkUnmapMemory(m_App.m_Device, dstImageMemory);
                                                                                                    // vkFreeMemory(m_App.m_Device, dstImageMemory, nullptr);
                                                                                                    // vkDestroyImage(m_App.m_Device, dstImage, nullptr);

    std::cout << "k" << std::endl;
}

int VertexDataGenerator::m_VertexDataGeneratorCount{ 0 };

#include "vertex_data_generator.moc"
