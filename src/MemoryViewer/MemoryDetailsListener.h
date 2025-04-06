#pragma once
#include <QVariant>

struct MemoryDetailsListener {
    virtual ~MemoryDetailsListener() = default;
    virtual void showDetails(QVariant object) = 0;
};
