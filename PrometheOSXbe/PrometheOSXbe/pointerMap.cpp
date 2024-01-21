#include "pointerMap.h"
#include "stringUtility.h"
#include <string.h>

#define MAP_SIZE_INCREMENT 100

pointerMap::pointerMap(bool useDelete)
{
	mUseDelete = useDelete;
    mSize = 0;
    mKeyValuePairs = (keyValuePair*)malloc(sizeof(keyValuePair) * MAP_SIZE_INCREMENT);
    if (mKeyValuePairs == NULL)
    {
        mMaxSize = 0;
        return;
    }
    mMaxSize = MAP_SIZE_INCREMENT;
}

pointerMap::~pointerMap()
{
    for (uint32_t i = 0; i < mSize; i++)
    {
        free(mKeyValuePairs[i].key);
		if (mUseDelete == true)
		{
			delete(mKeyValuePairs[i].value);
		}
		else
		{
			free(mKeyValuePairs[i].value);
		}
    }
    free(mKeyValuePairs);
}


void pointerMap::add(const char* key, void* value) 
{
    if (mSize == mMaxSize)
    {
        keyValuePair* reallocedKeyValuePairs = (keyValuePair*)realloc(mKeyValuePairs, sizeof(keyValuePair) * (mMaxSize + MAP_SIZE_INCREMENT));
        if (reallocedKeyValuePairs == NULL)
        { 
            return;
        }
        mMaxSize += MAP_SIZE_INCREMENT;
        mKeyValuePairs = reallocedKeyValuePairs;
    }

    mKeyValuePairs[mSize].key = strdup(key);
    mKeyValuePairs[mSize].value = value;
    mSize++;
}

void pointerMap::add(uint32_t key, void* value) 
{
	char* keyValue = stringUtility::formatString("%ui", key);
	add(keyValue, value);
	free(keyValue);
}

void* pointerMap::get(const char* key) 
{
    for (uint32_t i = 0; i < mSize; i++)
    {
		if (stringUtility::equals(mKeyValuePairs[i].key, key, false) == true) 
        {
            return mKeyValuePairs[i].value;
        }
    }
    return NULL;
}

void* pointerMap::get(uint32_t key) 
{
	char* keyValue = stringUtility::formatString("%ui", key);
	void* result = get(keyValue);
	free(keyValue);
	return result;
}

void pointerMap::removeKey(const char* key) 
{
    for (uint32_t i = 0; i < mSize; i++)
    {
        if (stringUtility::equals(mKeyValuePairs[i].key, key, false) == true) 
        {
            free(mKeyValuePairs[i].key);
			if (mUseDelete == true)
			{
				delete(mKeyValuePairs[i].value);
			}
			else
			{
				free(mKeyValuePairs[i].value);
			}
            for (uint32_t j = i; j < mSize - 1; j++)
            {
                mKeyValuePairs[j].key = mKeyValuePairs[j + 1].key;
                mKeyValuePairs[j].value = mKeyValuePairs[j + 1].value;
            }
            mSize--;
        }
    }
}

void pointerMap::removeKey(uint32_t key) 
{
	char* keyValue = stringUtility::formatString("%ui", key);
	removeKey(keyValue);
	free(keyValue);
}

uint32_t pointerMap::count()
{
    return mSize;
}

pointerVector* pointerMap::keys()
{
	pointerVector* keys = new pointerVector(false);
    for (uint32_t i = 0; i < mSize; i++)
    {
        keys->add(strdup(mKeyValuePairs[i].key));
    }
	return keys;
}