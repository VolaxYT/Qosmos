#pragma once
#include "config.h"

// Nuage de points ; 3 VBOs séparés pour positions, couleurs et alphas
// Les données sont régénérées à chaque changement d'orbitale 
class PointCloud {
public:

    PointCloud(){
        glGenVertexArrays(1, &VAO);
        glGenBuffers(3, VBOs);
    }

    // Envoie les données CPU -> GPU et configure les attributs
    void upload(const std::vector<glm::vec3>& positions, const std::vector<glm::vec3> colors, const std::vector<float>& alphas)
    {
        count = (int)positions.size();

        glBindVertexArray(VAO);

        // location 0 — positions
        glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
        glBufferData(GL_ARRAY_BUFFER, count * sizeof(glm::vec3), positions.data(), GL_DYNAMIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glEnableVertexAttribArray(0);

        // location 1 — RGB
        glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);
        glBufferData(GL_ARRAY_BUFFER, count * sizeof(glm::vec3), colors.data(), GL_DYNAMIC_DRAW);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glEnableVertexAttribArray(1);

        // location 2 — alpha
        glBindBuffer(GL_ARRAY_BUFFER, VBOs[2]);
        glBufferData(GL_ARRAY_BUFFER, count * sizeof(float), alphas.data(), GL_DYNAMIC_DRAW);
        glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glEnableVertexAttribArray(2);

        glBindVertexArray(0);
    }  

    void draw(){
        if(count == 0) return;
        glBindVertexArray(VAO);
        glDrawArrays(GL_POINTS, 0, count);
        glBindVertexArray(0);
    }

    ~PointCloud(){
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(3, VBOs);
    }

private:
    unsigned int VAO, VBOs[3];
    int count = 0;
};