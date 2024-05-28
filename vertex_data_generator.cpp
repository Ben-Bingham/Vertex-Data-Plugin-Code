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
        m_HeadlessRenderer = new HeadlessRenderer{ "/home/benjaminb/kde/src/okular/generators/Vertex-Data-Plugin-Code/Headless-Vulkan-Renderer/shaders/" };
        // m_HeadlessRenderer->setupRenderer();
    }

    m_VertexDataGeneratorCount++;
}

VertexDataGenerator::~VertexDataGenerator() {
    if (m_VertexDataGeneratorCount == 1) {
        // m_HeadlessRenderer->shutdownRenderer();
        delete m_HeadlessRenderer;
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

void VertexDataGenerator::generatePixmap(Okular::PixmapRequest* request) {
    std::string content = request->page()->text().toStdString();

    std::string delimeter = "\n";
    std::string currentToken;
    std::vector<float> vertices;

    for (auto c : content) {
        if (c == ' ') {
            if (currentToken != "" && currentToken != " " && currentToken != "  ") {
                vertices.push_back(std::stof(currentToken));
                currentToken = "";
            }
        }
        else {
            currentToken += c;
        }
    }

    // VulkanExample vulkanExample{"/home/benjaminb/kde/src/okular/generators/Vertex-Data-Plugin-Code/Headless-Vulkan-Renderer/shaders/"};

    int width = request->width();
    int height = request->height();
    VkSubresourceLayout imageSubresourceLayout;

    unsigned char* imageData = m_HeadlessRenderer->render(width, height, &imageSubresourceLayout, vertices);

    auto imgDatatmp = imageData;

    size_t finalImageSize = width * height * 4;

    std::vector<unsigned char> vectorData;
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
        imgDatatmp += imageSubresourceLayout.rowPitch;
    }

    QImage image{ vectorData.data(), request->width(), request->height(), QImage::Format_ARGB32 };

    image = image.mirrored(false, true); // TODO reimplement flipping

    request->page()->setPixmap(request->observer(), new QPixmap(QPixmap::fromImage(image)));

    signalPixmapRequestDone(request);
}

int VertexDataGenerator::m_VertexDataGeneratorCount{ 0 };

#include "vertex_data_generator.moc"
