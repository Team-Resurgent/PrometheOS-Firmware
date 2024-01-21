#include "pointerVector.h"
#include <string.h>

#define VECTOR_SIZE_INCREMENT 100

pointerVector::pointerVector(bool useDelete)
{
	mUseDelete = useDelete;
    mSize = 0;
    mValueContainers = (valueContainer*)malloc(sizeof(valueContainer) * VECTOR_SIZE_INCREMENT);
    if (mValueContainers == NULL)
    {
        mMaxSize = 0;
        return;
    }
    mMaxSize = VECTOR_SIZE_INCREMENT;
}

pointerVector::~pointerVector()
{
    for (uint32_t i = 0; i < mSize; i++)
    {
		if (mUseDelete == true)
		{
			delete(mValueContainers[i].value);
		}
		else
		{
			free(mValueContainers[i].value);
		}
    }
    free(mValueContainers);
}

void pointerVector::add(void* value)
{
    if (mSize == mMaxSize)
    {
        valueContainer* reallocedValueContainers = (valueContainer*)realloc(mValueContainers, sizeof(valueContainer) * (mMaxSize + VECTOR_SIZE_INCREMENT));
        if (reallocedValueContainers == NULL)
        { 
            return;
        }
        mMaxSize += VECTOR_SIZE_INCREMENT;
        mValueContainers = reallocedValueContainers;
    }

    mValueContainers[mSize].value = value;
    mSize++;
}

void* pointerVector::get(uint32_t index)
{
    return mValueContainers[index].value;
}

void pointerVector::remove(uint32_t index)
{
	if (mUseDelete == true)
	{
		delete(mValueContainers[index].value);
	}
	else
	{
		free(mValueContainers[index].value);
	}
    for (uint32_t j = index; j < mSize - 1; j++)
    {
        mValueContainers[j].value = mValueContainers[j + 1].value;
    }
    mSize--; 
}

void pointerVector::swap(uint32_t index1, uint32_t index2)
{
	void* temp = mValueContainers[index1].value;
	mValueContainers[index1].value = mValueContainers[index2].value;
	mValueContainers[index2].value = temp;
}

uint32_t pointerVector::count()
{
    return mSize;
}

void pointerVector::clear()
{    
	for (uint32_t i = 0; i < mSize; i++)
    {
		if (mUseDelete == true)
		{
			delete(mValueContainers[i].value);
		}
		else
		{
			free(mValueContainers[i].value);
		}
    }
    mSize = 0;
}
