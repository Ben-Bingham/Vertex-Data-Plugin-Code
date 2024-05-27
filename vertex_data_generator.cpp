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

#include "renderheadless.h"

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
    std::cout << "============================ Starting flush ============================" << std::endl;
    std::cout << "g.1.1" << std::endl;
    if (commandBuffer == VK_NULL_HANDLE) {
        return;
    }
    std::cout << "g.1.2" << std::endl;

    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
        std::cout << "Failed to end command buffer" << std::endl;
    }
    std::cout << "g.1.3" << std::endl;

    VkSubmitInfo submitInfo;
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;
    submitInfo.waitSemaphoreCount = 0;
    submitInfo.pNext = nullptr;
    submitInfo.pSignalSemaphores = nullptr;
    submitInfo.signalSemaphoreCount = 0;
    std::cout << "g.1.4" << std::endl;

    // Create fence to ensure that the command buffer has finished executing
    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    VkFence fence;
    std::cout << "g.1.5" << std::endl;
    if(vkCreateFence(m_App.m_Device, &fenceInfo, nullptr, &fence) != VK_SUCCESS) {
        std::cout << "Failed to create Fence" << std::endl;
    }
    std::cout << "g.1.6" << std::endl;

    // Submit to the queue
    if(vkQueueSubmit(queue, 1, &submitInfo, fence) != VK_SUCCESS) {
        std::cout << "Failed to submit queue" << std::endl;
    }

    std::cout << "g.1.7" << std::endl;
    // Wait for the fence to signal that command buffer has finished executing
    if (vkWaitForFences(m_App.m_Device, 1, &fence, VK_TRUE, UINT64_MAX) != VK_SUCCESS) {
        std::cout << "Couldent wait for fence" << std::endl;
    }
    std::cout << "g.1.8" << std::endl;
    vkDestroyFence(m_App.m_Device, fence, nullptr);
    std::cout << "g.1.9" << std::endl;
    if (free) {
        vkFreeCommandBuffers(m_App.m_Device, pool, 1, &commandBuffer);
    }
    std::cout << "g.1.10" << std::endl;
    std::cout << "============================ Ending flush ============================" << std::endl;
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

    {
        VulkanExample vulkanExample{"/home/benjaminb/kde/src/okular/generators/Vertex-Data-Plugin-Code/Headless-Vulkan-Renderer/shaders/"};

        size_t imageDataSize;
        VkSubresourceLayout imageDataInfo;

        unsigned char* imageData = vulkanExample.render(&imageDataSize, request->width(), request->height(), &imageDataInfo);

        int width = request->width();
        int height = request->height();

	    auto imgDatatmp = imageData;

        size_t finalImageSize = width * height * 4;

        std::vector<GLubyte> vectorData;
        vectorData.reserve(finalImageSize);

        int x = 0;
        unsigned int* oldRow;
        bool done = false;
        for (int32_t y = 0; y < height; y++) {
			unsigned int *row = (unsigned int*)imgDatatmp;
			for (int32_t x = 0; x < width; x++) {
                unsigned char* charRow = (unsigned char*)row;
                vectorData.push_back(charRow[2]);
                vectorData.push_back(charRow[1]);
                vectorData.push_back(charRow[0]);
                vectorData.push_back(charRow[3]);

				row++;
			}
			imgDatatmp += imageDataInfo.rowPitch;
		}

        QImage image{ vectorData.data(), request->width(), request->height(), QImage::Format_ARGB32 };


        // image.mirror(false, true); // TODO reimplement flipping

        request->page()->setPixmap(request->observer(), new QPixmap(QPixmap::fromImage(image)));
    }

    signalPixmapRequestDone(request);
}

int VertexDataGenerator::m_VertexDataGeneratorCount{ 0 };

#include "vertex_data_generator.moc"
