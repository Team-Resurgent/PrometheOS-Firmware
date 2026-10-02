#pragma once

#include "math.h"
#include "utils.h"
#include "vertexContainer.h"

class meshUtility {
  public:
    static void appendQuadXY(vertexContainer* vertices, const math::vec3F& position, const math::sizeF& size,
        const math::rectF& uvRect, uint32_t color);
    static void appendNinePatchXY(vertexContainer* vertices, const math::vec3F& position, const math::sizeF& size,
        const math::rectF& uvRect, uint32_t color);
};
