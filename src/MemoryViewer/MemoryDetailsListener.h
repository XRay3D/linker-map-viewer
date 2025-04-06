#pragma once
#include <QDebug>

struct MemoryDetailsListener {
    virtual ~MemoryDetailsListener() = default;
    virtual void showDetails(QVariant object) = 0;
};
