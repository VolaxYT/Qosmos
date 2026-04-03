#pragma once
#include "config.h"

class PointCloud {
public:

    PointCloud(){
        glGenVertexArrays(1, &VAO);
        glGenBuffers(3, VBOs);
    }

    void upload(const std::vector<glm::vec3>& positions, const std::vector<glm::vec3> colors, const std::vector<float>& alphas)
    {
        count = (int)positions.size();

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
        glBufferData(GL_ARRAY_BUFFER,
            count * sizeof(glm::vec3),
            positions.data(), GL_DYNAMIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);
        glBufferData(GL_ARRAY_BUFFER,
            count * sizeof(glm::vec3),
            colors.data(), GL_DYNAMIC_DRAW);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, VBOs[2]);
        glBufferData(GL_ARRAY_BUFFER,
            count * sizeof(float),
            alphas.data(), GL_DYNAMIC_DRAW);
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