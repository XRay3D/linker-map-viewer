#pragma once
#include "MemoryInfoFactory.h"

struct MemoryInfoUpdateListener {
    virtual ~MemoryInfoUpdateListener() = default;
    virtual void update(MemoryInfoFactory* memoryInfoFactory) = 0;
};
