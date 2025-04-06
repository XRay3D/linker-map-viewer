#pragma once
#include "MemoryMap/Region.h"
#include <QDebug>
#include <vector>

class MemoryMap {
    // Attributes
    std::vector<Region> regions;

    // Constructors
public:
    MemoryMap(std::vector<Region> regions)
        : regions{regions} { }

    MemoryMap() = default;

    // Getters
    const auto& getNumberOfRegions() const& { return regions; }
    int getNumberOfRegionSize() const { return regions.size(); }
    Region getRegion(int region) const { return regions.at(region); }

    // Setters
    void addRegion(Region region) {
        for(int i = 0; i < regions.size(); i++) {
            if(regions.at(i).getData().getAddress() > region.getData().getAddress()) {
                regions.emplace(regions.begin() + i, std::move(region));
                return;
            }
        }
        regions.emplace_back(std::move(region));
    }
};
