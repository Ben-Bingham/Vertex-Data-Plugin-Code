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
        if (!glfwInit()) {
            std::cout << "GLFW failed to load" << std::endl;
        }

        glfwSetErrorCallback(error_callback);
    }

    m_VertexDataGeneratorCount++;

    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

    m_Window = glfwCreateWindow(640, 480, "Window", NULL, NULL);
    if (!m_Window) {
        std::cout << "Failed to create GLFW window" << std::endl;
    }

    glfwMakeContextCurrent(m_Window);

    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cout << "Failed to init GLEW" << std::endl;
    }

    const char* fragmentShaderSource = "#version 330 core\n"
        "out vec4 FragColour;\n"
        "void main() {\n"
        "   FragColour = vec4(1.0f, 0.0f, 0.0f, 1.0f);\n"
        "}\n\0";

    const char* vertexShaderSource = "#version 330 core\n"
        "layout (location = 0) in vec3 position;\n"
        "void main() {\n"
        "   gl_Position = vec4(position.x, position.y, position.z, 1.0f);"
        "}\n\0";

    // Vertex Shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        std::cout << "Vertex shader failed to compile\n" << infoLog << std::endl;
    }

    // Fragment Shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        std::cout << "Fragment shader failed to compile\n" << infoLog << std::endl;
    }

    // Shader Program
    m_ShaderProgram = glCreateProgram();
    glAttachShader(m_ShaderProgram, vertexShader);
    glAttachShader(m_ShaderProgram, fragmentShader);
    glLinkProgram(m_ShaderProgram);

    glGetProgramiv(m_ShaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(m_ShaderProgram, 512, nullptr, infoLog);
        std::cout << "Shader Program failed to link\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

VertexDataGenerator::~VertexDataGenerator() {
    if (m_VertexDataGeneratorCount == 1) {
        glfwDestroyWindow(m_Window);
        glfwTerminate();
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

    float* vertices = new float[tokens.size()];

    size_t i = 0;
    for (auto t : tokens) {
        vertices[i] = t;
        i++;
    }

    unsigned int VAO;
    unsigned int VBO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, tokens.size() * sizeof(float), vertices, GL_STATIC_DRAW);

    delete[] vertices;

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    unsigned int fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, request->width(), request->height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "Framebuffer failed" << std::endl;
    }

    glClearColor(0.0, 0.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    glViewport(0, 0, request->width(), request->height());

    glUseProgram(m_ShaderProgram);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, tokens.size() / 3);

    const int size = request->width() * request->height() * 4;

    unsigned int pbo;
    glGenBuffers(1, &pbo);
    glBindBuffer(GL_PIXEL_PACK_BUFFER, pbo);
    glBufferData(GL_PIXEL_PACK_BUFFER, size, nullptr, GL_DYNAMIC_READ);

    glReadBuffer(GL_COLOR_ATTACHMENT0);
    glBindBuffer(GL_PIXEL_PACK_BUFFER, pbo);
    glReadPixels(0, 0, request->width(), request->height(), GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    GLubyte* array = (GLubyte*)glMapBufferRange(GL_PIXEL_PACK_BUFFER, 0, size, GL_MAP_READ_BIT);

    GLubyte* tempArray = new GLubyte[size];
    std::memcpy(tempArray, array, size);

    glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
    glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);

    GLubyte* newArray = new GLubyte[size];

    for (size_t i = 0; i < (size_t)size; i += 4) {
        newArray[i] = tempArray[i + 2];
        newArray[i + 1] = tempArray[i + 1];
        newArray[i + 2] = tempArray[i];
        newArray[i + 3] = tempArray[i + 3];
    }

    QImage image{ newArray, request->width(), request->height(), QImage::Format_ARGB32 };

//    image.mirror(false, true); // TODO reimplement flipping

    request->page()->setPixmap(request->observer(), new QPixmap(QPixmap::fromImage(image)));

    delete[] newArray;
    delete[] tempArray;

    glfwSwapBuffers(m_Window);
    glfwPollEvents();

    signalPixmapRequestDone(request);
}

int VertexDataGenerator::m_VertexDataGeneratorCount{ 0 };

#include "vertex_data_generator.moc"
