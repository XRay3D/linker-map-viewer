#pragma once
#include "MemoryMap/MemoryMap.h"
#include <QDebug>

class MemoryMapFilter {
public:
    static MemoryMap filter(MemoryMap map) {
        MemoryMap filteredMap;
        for(int i = 0; i < map.getNumberOfRegions(); i++) {
            Region filteredRegion = filter(map.getRegion(i));
            if(filteredRegion.getNumberOfSubRegions() > 0)
                filteredMap.addRegion(filteredRegion);
        }
        return filteredMap;
    }

private:
    static Region filter(Region region) {
        Region filteredRegion;
        filteredRegion.setName(region.getName());
        filteredRegion.setData(region.getData());
        filteredRegion.setFill(region.getFill());
        for(int i = 0; i < region.getNumberOfSubRegions(); i++) {
            SubRegion filteredSubRegion = filter(region.getSubRegion(i));
            if(filteredSubRegion.getNumberOfRegionData() > 0)
                filteredRegion.addSubRegion(filteredSubRegion);
        }
        return filteredRegion;
    }

    static SubRegion filter(SubRegion subRegion) {
        SubRegion filteredSubRegion;
        filteredSubRegion.setName(subRegion.getName());
        for(int i = 0; i < subRegion.getNumberOfRegionData(); i++) {
            RegionData filteredRegionData = subRegion.getRegionData(i);
            if(passesFilter(filteredRegionData))
                filteredSubRegion.addData(filteredRegionData);
        }
        return filteredSubRegion;
    }

    static bool passesFilter(RegionData regionData) {
        QString info = regionData.getInfo();
        bool validInfo = !(info.contains(" = ") || info.contains("PROVIDE") || info.contains("ASSERT") || info.contains("linker stubs"));
        bool validAddress = (regionData.getAddress() != RegionData::DEFAULT_ADDRESS);

        return validInfo && validAddress;
    }
};
