#pragma once

#include "xboxinternals.h"

class pointerVector
{
public:
    pointerVector(bool useDelete);
    ~pointerVector();
    void add(void* value);
    void* get(uint32_t index);
    void remove(uint32_t index);
	void swap(uint32_t index1, uint32_t index2);
    uint32_t count();
	void clear();
private:

    typedef struct valueContainer {
        void* value;
    } valueContainer;

    valueContainer* mValueContainers;
	bool mUseDelete;
    uint32_t mSize;
    uint32_t mMaxSize;
};

