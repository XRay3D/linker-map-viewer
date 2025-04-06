#pragma once
#include "MemoryMap/RegionData.h"
#include "MemoryMap/SubRegion.h"
#include <QDebug>
#include <vector>

class Region {
public:
    // Constants
    static constexpr QStringView DEFAULT_NAME = u"";
    static inline const RegionData DEFAULT_DATA;
    static constexpr int DEFAULT_FILL = -1;

private:
    // Attributes
    QString name{DEFAULT_NAME};
    RegionData data{};
    int fill{DEFAULT_FILL};
    std::vector<SubRegion> subRegions{};

    // Attributes - protection
    bool isSetName = false;
    bool isSetData = false;
    bool isSetFill = false;

    // Constructors
public:
    Region(QString name, RegionData data, int fill, std::vector<SubRegion> subRegions)
        : subRegions{subRegions} {
        setName(name);
        setData(data);
        setFill(fill);
    }

    Region() = default;

    // Getters
    QString getName() { return name; }
    RegionData getData() { return data; }
    int getFill() { return fill; }
    int getNumberOfSubRegions() { return subRegions.size(); }
    SubRegion getSubRegion(int subRegion) { return subRegions.at(subRegion); }

    // Setters
    void setName(QString newName) {
        if(!isSetName) {
            name = newName;
            isSetName = true;
        } else
            qCritical("The name of the region can only be set once!");
    }

    void setData(RegionData newData) {
        if(!isSetData) {
            data = newData;
            isSetData = true;
        } else
            qCritical("The data of the region can only be set once!");
    }

    void setFill(int newFill) {
        if(!isSetFill) {
            fill = newFill;
            isSetFill = true;
        } else
            qCritical("The fill of the region can only be set once!");
    }

    void addSubRegion(SubRegion subRegion) {
        for(int i = 0; i < subRegions.size(); i++) {
            if(subRegions.at(i).getNumberOfRegionData() > 0 && subRegion.getNumberOfRegionData() > 0) {
                if(subRegions.at(i).getRegionData(0).getAddress() > subRegion.getRegionData(0).getAddress()) {
                    subRegions.emplace(subRegions.begin() + i, std::move(subRegion));
                    return;
                }
            }
        }
        subRegions.emplace_back(std::move(subRegion));
    }
};
