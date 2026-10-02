#include "meshUtility.h"
#include "vertexContainer.h"
#include "vertex.h"

void meshUtility::appendQuadXY(vertexContainer* vertices, const math::vec3F& position, const math::sizeF& size,
    const math::rectF& uvRect, uint32_t color) {
    vertex v1 = vertex(position.x, position.y, position.z, uvRect.x, uvRect.y + uvRect.height, color);
    vertex v2 = vertex(
        position.x + size.width, position.y, position.z, uvRect.x + uvRect.width, uvRect.y + uvRect.height, color);
    vertex v3 =
        vertex(position.x + size.width, position.y + size.height, position.z, uvRect.x + uvRect.width, uvRect.y, color);
    vertex v4 = vertex(position.x, position.y + size.height, position.z, uvRect.x, uvRect.y, color);

    vertices->add(v3);
    vertices->add(v2);
    vertices->add(v1);

    vertices->add(v3);
    vertices->add(v1);
    vertices->add(v4);
}

void meshUtility::appendNinePatchXY(vertexContainer* vertices, const math::vec3F& position, const math::sizeF& size,
    const math::rectF& uvRect, uint32_t color) {
    vertexContainer* tempVertices = new vertexContainer();

    uint32_t tempVertexIndex = 0;
    for (int y = 0; y < 4; y++) {
        float piecePosY = math::calcNinePatchPosition(y, size.height) + position.y;
        float pieceUVY = math::calcNinePatchUV(y, uvRect.height) + uvRect.y;
        for (int x = 0; x < 4; x++) {
            float piecePosX = math::calcNinePatchPosition(x, size.width) + position.x;
            float pieceUVX = math::calcNinePatchUV(x, uvRect.width) + uvRect.x;
            vertex v = vertex(piecePosX, piecePosY, 0, pieceUVX, pieceUVY, color);
            tempVertices->add(v);
            tempVertexIndex++;
        }
    }

    uint32_t vertexIndex = 0;
    for (int yy = 0; yy < 3; yy++) {
        for (int xx = 0; xx < 3; xx++) {
            int offset = xx + (yy * 4);
            vertices->add(tempVertices->get(5 + offset));
            vertices->add(tempVertices->get(1 + offset));
            vertices->add(tempVertices->get(0 + offset));
            vertices->add(tempVertices->get(4 + offset));
            vertices->add(tempVertices->get(5 + offset));
            vertices->add(tempVertices->get(0 + offset));
        }
    }

    delete (tempVertices);
}
