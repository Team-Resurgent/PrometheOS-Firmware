#pragma once

#include "xboxinternals.h"
#include "vertex.h"

#define VECTOR_SIZE_INCREMENT 100

class vertexContainer {
  public:
    vertexContainer() {
        mSize = 0;
        mValueContainers = (uint8_t*)malloc(sizeof(vertex) * VECTOR_SIZE_INCREMENT);
        if (mValueContainers == NULL) {
            mMaxSize = 0;
            return;
        }
        mMaxSize = VECTOR_SIZE_INCREMENT;
    }

    ~vertexContainer() {
        free(mValueContainers);
    }

    void add(vertex value) {
        if (mSize == mMaxSize) {
            uint8_t* reallocedValueContainers =
                (uint8_t*)realloc(mValueContainers, sizeof(vertex) * (mMaxSize + VECTOR_SIZE_INCREMENT));
            if (reallocedValueContainers == NULL) {
                return;
            }
            mMaxSize += VECTOR_SIZE_INCREMENT;
            mValueContainers = reallocedValueContainers;
        }

        memcpy(mValueContainers + (mSize * sizeof(vertex)), &value, sizeof(vertex));
        mSize++;
    }

    uint8_t* get() {
        return mValueContainers;
    }

    vertex get(uint32_t index) {
        vertex result;
        memcpy(&result, mValueContainers + (index * sizeof(vertex)), sizeof(vertex));
        return result;
    }

    uint32_t size() {
        return mSize * sizeof(vertex);
    }

  private:
    uint8_t* mValueContainers;
    uint32_t mSize;
    uint32_t mMaxSize;
};
