/*
    Orignal Source: txt plugin for Okular:
    SPDX-FileCopyrightText: 2013 Azat Khuzhin <a3at.mail@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later

    Code has been modified by Benjamin Bingham to become: vertex_data_generator
*/
#ifndef _VERTEX_DATA_GENERATOR_TXT_H_
#define _VERTEX_DATA_GENERATOR_TXT_H_

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <core/textdocumentgenerator.h>

class VertexDataGenerator : public Okular::Generator {

    Q_OBJECT
    Q_INTERFACES(Okular::Generator)

public:
    VertexDataGenerator(QObject *parent, const QVariantList &args);
    ~VertexDataGenerator() override;

    void generatePixmap(Okular::PixmapRequest* request) override;
    bool loadDocument(const QString &fileName, QVector<Okular::Page*> &pages) override;
    bool doCloseDocument() override;

private:
    GLFWwindow* m_Window;

    static int m_VertexDataGeneratorCount;

    unsigned int m_ShaderProgram;
};

#endif
