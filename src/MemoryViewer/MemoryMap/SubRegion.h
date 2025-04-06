#pragma once
#include <QDebug>

#include "MemoryMap/RegionData.h"
#include <QDebug>
#include <vector>

class SubRegion {
    // Attributes
    QString name;
    std::vector<RegionData> datas;

    // Attributes - protection
    bool isSetName = false;

public:
    auto operator<=>(const SubRegion&) const = default;

    // Constructors
    SubRegion(QString name, std::vector<RegionData> data)
        : datas{data} {
        setName(name);
    }

    SubRegion() = default;

    // Getters
    QString getName() const { return name; }
    const auto& getNumberOfRegionDatas() const& { return datas; }
    int getNumberOfRegionDataSize() const { return datas.size(); }
    RegionData getRegionData(int regionData) const { return datas.at(regionData); }

    // Setters
    void setName(QString newName) {
        if(!isSetName) {
            name = newName;
            isSetName = true;
        } else
            qCritical("The name of the subregion can only be set once!");
    }

    void addData(RegionData data) {
        for(int i = 0; i < datas.size(); i++) { // FIXME
            if(datas.at(i).getAddress() > data.getAddress()) {
                datas.emplace(datas.begin() + i, std::move(data));
                return;
            }
        }
        datas.emplace_back(std::move(data));
    }
};
