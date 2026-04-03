#pragma once
#include "config.h"

class Camera {
    public:
    float radius;
    float theta; // rotation verticale
    float phi; // rotation horizontale

    float fov;
    float aspectRatio;
    float nearPlane;
    float farPlane;

    bool dragging;
    double lastX, lastY;
    float sensitivity;

    bool autoRotate;
    float autoRotateSpeed;

    Camera(float aspect):
        radius(1.5f), theta(1.2f), phi(0.5f),
        fov(45.0f), aspectRatio(aspect),
        nearPlane(0.01f), farPlane(500.0f),
        dragging(false), lastX(0), lastY(0),
        sensitivity(0.005f),
        autoRotate(false), autoRotateSpeed(0.0002f)
    {}

    glm::vec3 getPosition() {
        return glm::vec3(
            radius * sin(theta) * cos(phi),
            radius * cos(theta),
            radius * sin(theta) * sin(phi)
        );
    }

    glm::mat4 getView() {
        return glm::lookAt(
            getPosition(),
            glm::vec3(0.0f),        // regarde toujours l'origine
            glm::vec3(0.0f, 1.0f, 0.0f)
        );
    }

    glm::mat4 getProjection() {
        return glm::perspective(
            glm::radians(fov),
            aspectRatio,
            nearPlane,
            farPlane
        );
    }

    void update(){
        if(autoRotate) {
            phi += autoRotateSpeed;
            if(phi > 2.0f * glm::pi<float>())
                phi -= 2.0f * glm::pi<float>();
        }
    }

    void onMouseButton(int button, int action){
        if(button == GLFW_MOUSE_BUTTON_LEFT)
            dragging = (action == GLFW_PRESS);
    }

    void onMouseMove(double xpos, double ypos){
        if(dragging){
            float dx = (float)(xpos - lastX);
            float dy = (float)(ypos - lastY);
            phi   -= dx * sensitivity;
            theta -= dy * sensitivity;
            theta = glm::clamp(theta, 0.05f, glm::pi<float>() - 0.05f); // pour éviter le gimbal lock
        }
        lastX = xpos;
        lastY = ypos;
    }

    void onScroll(double yoffset){
        radius -= (float)yoffset * 0.3f;
        radius  = glm::clamp(radius, 0.5f, 200.0f);
    }
};