#pragma once
#include "MemoryMap/MemoryMap.h"
#include <QDebug>

class MemoryMapFilter {
public:
    static MemoryMap filter(MemoryMap map) {
        MemoryMap filteredMap;
        for(Region filteredRegion: map.getNumberOfRegions()) {
            filteredRegion = filter(filteredRegion);
            if(filteredRegion.getNumberOfSubRegionSize() > 0)
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
        for(int i = 0; i < region.getNumberOfSubRegionSize(); i++) {
            SubRegion filteredSubRegion = filter(region.getSubRegion(i));
            if(filteredSubRegion.getNumberOfRegionDataSize() > 0)
                filteredRegion.addSubRegion(filteredSubRegion);
        }
        return filteredRegion;
    }

    static SubRegion filter(SubRegion subRegion) {
        SubRegion filteredSubRegion;
        filteredSubRegion.setName(subRegion.getName());
        for(int i = 0; i < subRegion.getNumberOfRegionDataSize(); i++) {
            RegionData filteredRegionData = subRegion.getRegionData(i);
            if(passesFilter(filteredRegionData))
                filteredSubRegion.addData(filteredRegionData);
        }
        return filteredSubRegion;
    }

    static bool passesFilter(RegionData regionData) {
        QString info = regionData.getInfo();
        bool validInfo = !(info.contains(u" = "_s) || info.contains(u"PROVIDE"_s) || info.contains(u"ASSERT"_s) || info.contains(u"linker stubs"_s));
        bool validAddress = (regionData.getAddress() != RegionData::DEFAULT_ADDRESS);

        return validInfo && validAddress;
    }
};
