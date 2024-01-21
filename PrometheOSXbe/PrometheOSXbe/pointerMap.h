#pragma once

#include "xboxinternals.h"
#include "pointerVector.h"

class pointerMap
{
public:
    pointerMap(bool useDelete);
    ~pointerMap();
    void add(const char* key, void* value);
    void add(uint32_t key, void* value);
    void* get(const char* key);
    void* get(uint32_t key);
    void removeKey(const char* key);
    void removeKey(uint32_t key);
	uint32_t count();
	pointerVector* keys();
private:

    typedef struct keyValuePair {
        char* key;
        void* value;
    } keyValuePair;

    keyValuePair* mKeyValuePairs;
	bool mUseDelete;
    uint32_t mSize;
    uint32_t mMaxSize;
};

