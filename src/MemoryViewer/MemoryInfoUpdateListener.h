#pragma once
#include <QDebug>

public: interface MemoryInfoUpdateListener
{
    public: void update (MemoryInfoFactory memoryInfoFactory);
};
