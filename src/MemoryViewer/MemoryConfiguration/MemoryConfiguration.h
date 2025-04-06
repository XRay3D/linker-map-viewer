#pragma once
#include "MemoryConfiguration/Space.h"
#include <QDebug>
#include <vector>

class MemoryConfiguration {
    // Attributes
    long totalSize{-1};
    std::vector<Space> spaces;

    // Attributes - protection
    bool isSetTotalSize = false;

public:
    // Constructors
    MemoryConfiguration(long totalSize, std::vector<Space> spaces)
        : spaces{spaces} {
        setTotalSize(totalSize);
    }

    MemoryConfiguration() = default;

    // Getters
    long getTotalSize() const { return totalSize; }
    int getNumberOfSpaces() const { return spaces.size(); }
    Space getSpace(int space) const { return spaces.at(space); }

    // Setters
    void setTotalSize(long newTotalSize) {
        if(!isSetTotalSize) {
            totalSize = newTotalSize;
            isSetTotalSize = true;
        } else
            qCritical("The total size of the memory configuration can only be set once!");
    }

    void addSpace(Space space) {
        for(int i = 0; i < spaces.size(); i++) {
            if(spaces.at(i).getOrigin() > space.getOrigin()) {
                spaces.emplace(spaces.begin() + i, space);
                return;
            }
        }
        spaces.emplace_back(space);
    }
};
