#pragma once
#include "config.h"

// Axes cartésiens X (rouge), Y (vert), Z (bleu)
// chaque axe = segment de l'origine jusqu'à 10 unités
class Axes {
public:
    Axes(){
        // format : x,y,z, r,g,b, a 
        float data[] = {
            // X
            0.0f, 0.0f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f,
            10.0f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f,   1.0f,
            // Y
            0.0f, 0.0f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f,
            0.0f, 10.0f, 0.0f,  0.0f, 1.0f, 0.0f,   1.0f,
            // Z
            0.0f, 0.0f, 0.0f,   0.0f, 0.5f, 1.0f,   1.0f,
            0.0f, 0.0f, 10.0f,  0.0f, 0.5f, 1.0f,   1.0f,
        };

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);

        // location 0 — position
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        // location 1 — RGB
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        // location 2 — alpha
        glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(2);

        glBindVertexArray(0);
    }

    void draw(){
    glLineWidth(2.0f);
    glBindVertexArray(VAO);
    glDrawArrays(GL_LINES, 0, 6);
    glBindVertexArray(0);
}
    ~Axes(){
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
    }

private:
    unsigned int VAO, VBO;
};