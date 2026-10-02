#pragma once

#include "math.h"

typedef struct vertex {
    math::vec3F position;
    uint32_t diffuse;
    math::vec2F texcoord;

    vertex() {
        position = math::vec3F();
        texcoord = math::vec2F();
    }

    vertex(const float& positionX, const float& positionY, const float& positionZ, const float& texcoordX,
        const float& texcoordY, uint32_t color) {
        position = math::vec3F(positionX, positionY, positionZ);
        diffuse = color;
        texcoord = math::vec2F(texcoordX, texcoordY);
    }

    vertex(const math::vec3F& position, const math::vec2F& texcoord) : position(position), texcoord(texcoord) {
    }

} vertex;
