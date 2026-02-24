#pragma once
#include "config.h"

class Material {
public:
    Material(const std::string& filename);
    ~Material();
    void use();
private:
    unsigned int texture;
};