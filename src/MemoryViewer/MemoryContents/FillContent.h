#pragma once
#include "MemoryContents/MemoryContent.h"
#include <QDebug>

class FillContent final : public MemoryContent {
public:
    // Constants
    static constexpr int DEFAULT_FILL = -1;

private:
    // Attributes
    int fill{DEFAULT_FILL};
    // Attributes - protection
    bool isSetFill = false;

public:
    // Constructors
    FillContent(long address, long size, int fill)
        : MemoryContent{address, size} {
        setFill(fill);
    }

    FillContent() = default;

    // Getters
    int getFill() { return fill; }

    // Setters
    void setFill(int newFill) {
        if(!isSetFill) {
            fill = newFill;
            isSetFill = true;
        } else
            qCritical("The fill of the fill content can only be set once!");
    }
};
